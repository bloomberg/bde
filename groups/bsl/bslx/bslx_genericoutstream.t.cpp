// bslx_genericoutstream.t.cpp                                        -*-C++-*-

#include <bslx_genericoutstream.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cctype.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// For all output methods in 'GenericOutStream', the primary concerns are the
// formatting of the input value to its correct byte representation and the
// proper placement and alignment of bytes in the output stream.  We verify
// these properties by inserting chosen "marker" bytes between each output
// method call, and ensure that the new output bytes are properly interleaved
// between the "marker" bytes.
//
// We have chosen the primary black-box manipulator for 'GenericOutStream' to
// be 'putInt8'.
// ----------------------------------------------------------------------------
// [ 2] GenericOutStream(STREAMBUF *streamBuf, int sV);
// [ 2] ~GenericOutStream();
// [26] GenericOutStream& flush();
// [ 4] void invalidate();
// [24] putLength(int length);
// [24] putVersion(int version);
// [11] putInt64(bsls::Types::Int64 value);
// [11] putUint64(bsls::Types::Uint64 value);
// [10] putInt56(bsls::Types::Int64 value);
// [10] putUint56(bsls::Types::Uint64 value);
// [ 9] putInt48(bsls::Types::Int64 value);
// [ 9] putUint48(bsls::Types::Uint64 value);
// [ 8] putInt40(bsls::Types::Int64 value);
// [ 8] putUint40(bsls::Types::Uint64 value);
// [ 7] putInt32(int value);
// [ 7] putUint32(unsigned int value);
// [ 6] putInt24(int value);
// [ 6] putUint24(unsigned int value);
// [ 5] putInt16(int value);
// [ 5] putUint16(unsigned int value);
// [ 2] putInt8(int value);
// [ 2] putUint8(unsigned int value);
// [13] putFloat64(double value);
// [12] putFloat32(float value);
// [25] putString(const bsl::string& value);
// [21] putArrayInt64(const bsls::Types::Int64 *array, int count);
// [21] putArrayUint64(const bsls::Types::Uint64 *array, int count);
// [20] putArrayInt56(const bsls::Types::Int64 *array, int count);
// [20] putArrayUint56(const bsls::Types::Uint64 *array, int count);
// [19] putArrayInt48(const bsls::Types::Int64 *array, int count);
// [19] putArrayUint48(const bsls::Types::Uint64 *array, int count);
// [18] putArrayInt40(const bsls::Types::Int64 *array, int count);
// [18] putArrayUint40(const bsls::Types::Uint64 *array, int count);
// [17] putArrayInt32(const int *array, int count);
// [17] putArrayUint32(const unsigned int *array, int count);
// [16] putArrayInt24(const int *array, int count);
// [16] putArrayUint24(const unsigned int *array, int count);
// [15] putArrayInt16(const short *array, int count);
// [15] putArrayUint16(const unsigned short *array, int count);
// [14] putArrayInt8(const char *array, int count);
// [14] putArrayInt8(const signed char *array, int count);
// [14] putArrayUint8(const char *array, int count);
// [14] putArrayUint8(const unsigned char *array, int count);
// [23] putArrayFloat64(const double *array, int count);
// [22] putArrayFloat32(const float *array, int count);
// [ 4] operator const void *() const;
// [ 3] int bdexVersionSelector() const;
// [ 4] bool isValid() const;
//
// [27] GenericOutStream& operator<<(GenericOutStream&, value);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [28] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                      HELPER CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

class TestOutStreamBuf {
    // This class implements a very basic stream buffer suitable for use in
    // 'bslx::GenericOutStream'.

    // DATA
    bsl::stringbuf      d_buffer;      // output buffer

    mutable bsl::string d_cache;       // caches the data for the 'data' method

    int                 d_flushCount;  // number of calls to 'pubsync'

    bool                d_flushFail;   // cause all 'pubsync' to fail

    int                 d_limit;       // number of bytes to write before
                                       // failure; -1 implies will never fail

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestOutStreamBuf&);

  public:
    // TYPES
    struct traits_type {
        static int eof() {  return -1;  }
    };

    // CREATORS
    TestOutStreamBuf();
        // Create an empty stream buffer.

    ~TestOutStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    int pubsync();
        // Increments the flush count.

    void setFlushFail();
        // Set all 'pubsync' methods to fail.

    void setLimit(int limit);
        // Set the input limit to the specified 'limit'.

    int sputc(char c);
        // Write the specified character 'c' to this buffer.  If the write is
        // successful, return the value 'c'; otherwise, 'EOF'.

    bsl::streamsize sputn(const char *s, bsl::streamsize length);
        // Write the specified 'length' characters at the specified address 's'
        // to this buffer and return the number of characters written.

    // ACCESSORS
    const char *data() const;
        // Return the address of the non-modifiable character buffer held by
        // this stream buffer.

    int flushCount() const;
        // Return the number of calls to 'pubsync' since the creation of this
        // stream buffer.

    bsl::streamsize length() const;
        // Return the number of characters from the beginning of the buffer to
        // the current write position.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const TestOutStreamBuf& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

// CREATORS
TestOutStreamBuf::TestOutStreamBuf()
: d_buffer()
, d_flushCount(0)
, d_flushFail(false)
, d_limit(-1)
{
}

TestOutStreamBuf::~TestOutStreamBuf()
{
}

// MANIPULATORS
int TestOutStreamBuf::pubsync()
{
    if (d_flushFail) return -1;                                       // RETURN
    ++d_flushCount;
    return d_buffer.pubsync();
}

void TestOutStreamBuf::setFlushFail()
{
    d_flushFail = true;
}

void TestOutStreamBuf::setLimit(int limit)
{
    d_limit = limit;
}

int TestOutStreamBuf::sputc(char c)
{
    if (-1 == d_limit) {
        return d_buffer.sputc(c);                                     // RETURN
    }
    if (0 < d_limit) {
        --d_limit;
        return d_buffer.sputc(c);                                     // RETURN
    }
    return traits_type::eof();
}

bsl::streamsize TestOutStreamBuf::sputn(const char      *s,
                                        bsl::streamsize  length)
{
    if (-1 == d_limit) {
        return d_buffer.sputn(s, length);                             // RETURN
    }
    if (length <= d_limit) {
        d_limit -= static_cast<int>(length);
        return d_buffer.sputn(s, length);                             // RETURN
    }
    d_limit = 0;
    return 0;
}

// ACCESSORS
const char *TestOutStreamBuf::data() const
{
    d_cache = d_buffer.str();
    return d_cache.data();
}

int TestOutStreamBuf::flushCount() const
{
    return d_flushCount;
}

bsl::streamsize TestOutStreamBuf::length() const
{
    return d_buffer.str().size();
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestOutStreamBuf& object)
{
    bsl::string         buffer = object.d_buffer.str();
    const int           len    = static_cast<int>(buffer.size());
    const char         *data   = buffer.data();
    bsl::ios::fmtflags  flags  = stream.flags();

    stream << bsl::hex;

    for (int i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) {
            stream << ' ';
        }
        if (0 == i % 8) { // output newline character and address every 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }

        stream << bsl::setw(2)
               << bsl::setfill('0')
               << static_cast<int>(static_cast<unsigned char>(data[i]));
    }

    stream.flags(flags);  // reset stream format flags

    return stream;
}

void debugprint(const TestOutStreamBuf& object)
{
    bsl::cout << object;
}

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef TestOutStreamBuf                   Buf;
typedef GenericOutStream<TestOutStreamBuf> Obj;

const int VERSION_SELECTOR = 20131127;
const int SIZEOF_INT64   = 8;
const int SIZEOF_INT56   = 7;
const int SIZEOF_INT48   = 6;
const int SIZEOF_INT40   = 5;
const int SIZEOF_INT32   = 4;
const int SIZEOF_INT24   = 3;
const int SIZEOF_INT16   = 2;
const int SIZEOF_INT8    = 1;
const int SIZEOF_FLOAT64 = 8;
const int SIZEOF_FLOAT32 = 4;

// ============================================================================
//                                 USAGE EXAMPLE
// ----------------------------------------------------------------------------

    class MyOutStreamBuf {
        // This class implements a very basic stream buffer suitable for use in
        // 'bslx::GenericOutStream'.

        // DATA
        bsl::string d_buffer;  // output buffer

      private:
        // NOT IMPLEMENTED
        MyOutStreamBuf(const MyOutStreamBuf&);
        MyOutStreamBuf& operator=(const MyOutStreamBuf&);

      public:
        // TYPES
        struct traits_type {
            static int eof() { return -1; }
        };

        // CREATORS
        MyOutStreamBuf();
            // Create an empty stream buffer.

        ~MyOutStreamBuf();
            // Destroy this stream buffer.

        // MANIPULATORS
        int pubsync();
            // Return 0.

        int sputc(char c);
            // Write the specified character 'c' to this buffer.  Return 'c' on
            // success, and 'traits_type::eof()' otherwise.

        bsl::streamsize sputn(const char *s, bsl::streamsize length);
            // Write the specified 'length' characters at the specified address
            // 's' to this buffer, and return the number of characters written.

        // ACCESSORS
        const char *data() const;
            // Return the address of the non-modifiable character buffer held
            // by this stream buffer.

        bsl::streamsize size() const;
            // Return the number of characters from the beginning of the buffer
            // to the current write position.
    };

    // ========================================================================
    //                  INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CREATORS
    MyOutStreamBuf::MyOutStreamBuf()
    : d_buffer()
    {
    }

    MyOutStreamBuf::~MyOutStreamBuf()
    {
    }

    // MANIPULATORS
    int MyOutStreamBuf::pubsync()
    {
        // In this implementation, there is nothing to be done except return
        // success.

        return 0;
    }

    int MyOutStreamBuf::sputc(char c)
    {
        d_buffer += c;
        return static_cast<int>(c);
    }

    bsl::streamsize MyOutStreamBuf::sputn(const char      *s,
                                          bsl::streamsize  length)
    {
        d_buffer.append(s, length);
        return length;
    }

    // ACCESSORS
    const char *MyOutStreamBuf::data() const
    {
        return d_buffer.data();
    }

    bsl::streamsize MyOutStreamBuf::size() const
    {
        return d_buffer.size();
    }

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 28: {
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.  The first example
// depicts usage with a 'bsl::stringbuf'.  The second example replaces the
// 'bsl::stringbuf' with a user-defined 'STREAMBUF'.
//
///Example 1: Basic Externalization
///- - - - - - - - - - - - - - - -
// A 'bslx::GenericOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::GenericOutStream', compares the contents of this
// stream to the expected value, and then writes the contents of this stream's
// buffer to 'stdout'.
//
// First, we create a 'bslx::GenericOutStream', with an arbitrary value for its
// 'versionSelector', and externalize some values:
//..
    bsl::stringbuf                         buffer1;
    bslx::GenericOutStream<bsl::stringbuf> outStream1(&buffer1, 20131127);
    outStream1.putInt32(1);
    outStream1.putInt32(2);
    outStream1.putInt8('c');
    outStream1.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the buffer to the expected value:
//..
    bsl::string  theChars = buffer1.str();
    ASSERT(15 == theChars.size());
    ASSERT( 0 == bsl::memcmp(theChars.data(),
                             "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
                             15));
//..
// Finally, we print the buffer's contents to 'bsl::cout'.
//..
    if (veryVerbose)
    for (bsl::size_t i = 0; i < theChars.size(); ++i) {
        if (bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
            bsl::cout << "nextByte (char): " << theChars[i] << bsl::endl;
        }
        else {
            bsl::cout << "nextByte (int): "
                      << static_cast<int>(theChars[i])
                      << bsl::endl;
        }
    }
//..
// Executing the above code results in the following output:
//..
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (char): c
//  nextByte (int): 5
//  nextByte (char): h
//  nextByte (char): e
//  nextByte (char): l
//  nextByte (char): l
//  nextByte (char): o
//..
// See the 'bslx_genericinstream' component usage example for a more practical
// example of using 'bslx' streams.
// example of using 'bslx' streams.
//
///Example 2: Sample 'STREAMBUF' Implementation
///- - - - - - - - - - - - - - - - - - - - - -
// For this example, we will implement 'MyOutStreamBuf', a minimal 'STREAMBUF'
// to be used with 'bslx::GenericOutStream'.  The implementation will consist
// of only what is required of the type and two accessors to verify correct
// functionality ('data' and 'length').
//
// First, we implement 'MyOutStreamBuf' (which, for brevity, simply uses the
// default allocator):
//..
//..
// Then, we create 'buffer2', an instance of 'MyOutStreamBuf', and a
// 'bslx::GenericOutStream' using 'buffer2', with an arbitrary value for its
// 'versionSelector', and externalize some values:
//..
    MyOutStreamBuf                         buffer2;
    bslx::GenericOutStream<MyOutStreamBuf> outStream2(&buffer2, 20131127);
    outStream2.putInt32(1);
    outStream2.putInt32(2);
    outStream2.putInt8('c');
    outStream2.putString(bsl::string("hello"));
//..
// Finally, we compare the contents of the buffer to the expected value:
//..
    ASSERT(15 == buffer2.size());
    ASSERT( 0 == bsl::memcmp(buffer2.data(),
                             "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
                             15));
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // EXTERNALIZATION FREE OPERATOR
        //   Verify 'operator<<' works correctly.
        //
        // Concerns:
        //: 1 The method inline-forwards to the implementation correctly.
        //:
        //: 2 Invocations of the method can be chained.
        //
        // Plan:
        //: 1 Externalize a set of values and ensure the resultant buffer
        //:   matches the one produced by the oracle implementation (the one
        //:   forwarded to).  (C-1)
        //:
        //: 2 Externalize a set of values to the stream in one code line.
        //:   (C-2)
        //
        // Testing:
        //   GenericOutStream& operator<<(GenericOutStream&, value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXTERNALIZATION FREE OPERATOR" << endl
                          << "=============================" << endl;

        {
            char value = 'a';

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);

            Buf expected;
            Obj mY(&expected, VERSION_SELECTOR);

            mX << value;
            OutStreamFunctions::bdexStreamOut(mY, value);
            ASSERT(B.length() == expected.length());
            ASSERT(0 == memcmp(B.data(), expected.data(), expected.length()));
        }
        {
            double value = 7.0;

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);

            Buf expected;
            Obj mY(&expected, VERSION_SELECTOR);

            mX << value;
            OutStreamFunctions::bdexStreamOut(mY, value);
            ASSERT(B.length() == expected.length());
            ASSERT(0 == memcmp(B.data(), expected.data(), expected.length()));
        }
        {
            bsl::vector<int> value;
            for (int i = 0; i < 5; ++i) {
                if (veryVerbose) { P(i); }
                Buf mB;  const Buf& B = mB;

                Obj mX(&mB, VERSION_SELECTOR);

                Buf expected;
                Obj mY(&expected, VERSION_SELECTOR);

                mX << value;
                OutStreamFunctions::bdexStreamOut(mY, value);
                LOOP_ASSERT(i, B.length() == expected.length());
                LOOP_ASSERT(i, 0 == memcmp(B.data(),
                                           expected.data(),
                                           expected.length()));
                value.push_back(i);
            }
        }
        {
            float       value1 = 3.0;
            bsl::string value2 = "hello";
            short       value3 = 2;

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);

            Buf expected;
            Obj mY(&expected, VERSION_SELECTOR);

            mX << value1 << value2 << value3;
            OutStreamFunctions::bdexStreamOut(mY, value1);
            OutStreamFunctions::bdexStreamOut(mY, value2);
            OutStreamFunctions::bdexStreamOut(mY, value3);
            ASSERT(B.length() == expected.length());
            ASSERT(0 == memcmp(B.data(), expected.data(), expected.length()));
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // FLUSH TEST
        //   Verify the method forwards correctly.
        //
        // Concerns:
        //: 1 The method forwards correctly.
        //:
        //: 2 If there is a failure, the stream is invalidated.
        //
        // Plan:
        //: 1 Directly test the method using the testing methods of
        //:   'TestOutStreamBuf'.  (C-1)
        //:
        //: 2 Use the 'setFlushFail' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-2)
        //
        // Testing:
        //   GenericOutStream& flush();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FLUSH TEST" << endl
                          << "==========" << endl;


        if (verbose) cout << "\nTesting 'flush'." << endl;
        {
            Buf mB;  const Buf& B = mB;
            ASSERT(0 == B.flushCount());

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;

            mX.flush();
            ASSERT(1 == B.flushCount());
            ASSERT(X.isValid());

            mX.invalidate();

            mX.flush();
            ASSERT(1 == B.flushCount());
        }
        {
            // Verify error handling.
            Buf mB;  const Buf& B = mB;
            ASSERT(0 == B.flushCount());

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;

            mB.setFlushFail();
            mX.flush();
            ASSERT(0 == B.flushCount());
            ASSERT(false == X.isValid());
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // PUT STRING TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putString(const bsl::string& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT STRING TEST" << endl
                          << "===============" << endl;


        if (verbose) cout << "\nTesting 'putString'." << endl;
        {
            const bsl::string DATA = "hello";
            const int         SIZE = SIZEOF_INT8 + 5 * SIZEOF_INT8;

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putString(DATA);     mX.putInt8(0xff);
            mX.putString(DATA);     mX.putInt8(0xfe);
            mX.putString(DATA);     mX.putInt8(0xfd);
            mX.putString(DATA);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               "\x05hello" "\xff"
                               "\x05hello" "\xfe"
                               "\x05hello" "\xfd"
                               "\x05hello" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putString(DATA);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               "\x05hello" "\xff"
                               "\x05hello" "\xfe"
                               "\x05hello" "\xfd"
                               "\x05hello" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsl::string DATA = "hello";

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putString(DATA));
        }
        {
            // Verify error handling.
            const bsl::string DATA = "hello";
            const int         SIZE = SIZEOF_INT8 + 5 * SIZEOF_INT8;

            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putString(DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putString(DATA);
            ASSERT(X.isValid());
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // PUT LENGTH AND VERSION TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putLength(int length);
        //   putVersion(int version);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT LENGTH AND VERSION TEST" << endl
                          << "===========================" << endl;
        {
            if (verbose) cout << "\nTesting putLength." << endl;
            {
                Buf mB;  const Buf& B = mB;

                Obj mX(&mB, VERSION_SELECTOR);
                                       mX.putInt8(0xff);
                mX.putLength(  1);     mX.putInt8(0xfe);
                mX.putLength(128);     mX.putInt8(0xfd);
                mX.putLength(127);     mX.putInt8(0xfc);
                mX.putLength(256);     mX.putInt8(0xfb);
                if (veryVerbose) { P(B); }

                const bsl::size_t NUM_BYTES =
                                            7 * SIZEOF_INT8 + 2 * SIZEOF_INT32;
                ASSERT(NUM_BYTES == B.length());
                ASSERT(0 == memcmp(B.data(),
                                   "\xff" "\x01" "\xfe"
                                   "\x80\x00\x00\x80" "\xfd"
                                   "\x7f" "\xfc"
                                   "\x80\x00\x01\x00" "\xfb",
                                   NUM_BYTES));

                // Verify method has no effect if the stream is invalid.
                mX.invalidate();
                mX.putLength(7);
                ASSERT(NUM_BYTES == B.length());
                ASSERT(0 == memcmp(B.data(),
                                   "\xff" "\x01" "\xfe"
                                   "\x80\x00\x00\x80" "\xfd"
                                   "\x7f" "\xfc"
                                   "\x80\x00\x01\x00" "\xfb",
                                   NUM_BYTES));
            }
            {
                // Verify the return value.
                Buf mB;
                Obj mX(&mB, VERSION_SELECTOR);
                ASSERT(&mX == &mX.putLength(7));
            }
            {
                // Verify error handling; short length.

                const int SIZE = SIZEOF_INT8;

                Buf mB;

                for (int i = 0; i < SIZE; ++i) {
                    mB.setLimit(i);

                    Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                    mX.putLength(1);
                    ASSERT(false == X.isValid());
                }

                mB.setLimit(SIZE);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putLength(1);
                ASSERT(X.isValid());
            }
            {
                // Verify error handling; large length.

                const int SIZE = SIZEOF_INT32;

                Buf mB;

                for (int i = 0; i < SIZE; ++i) {
                    mB.setLimit(i);

                    Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                    mX.putLength(300);
                    ASSERT(false == X.isValid());
                }

                mB.setLimit(SIZE);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putLength(300);
                ASSERT(X.isValid());
            }
        }

        {
            const int SIZE = SIZEOF_INT8;

            if (verbose) cout << "\nTesting putVersion." << endl;
            {
                Buf mB;  const Buf& B = mB;

                Obj mX(&mB, VERSION_SELECTOR);
                ASSERT(0 == B.length());
                mX.putVersion(1);
                mX.putVersion(2);
                mX.putVersion(3);
                mX.putVersion(4);
                if (veryVerbose) { P(B); }

                const bsl::size_t NUM_BYTES = 4 * SIZE;
                ASSERT(NUM_BYTES == B.length());
                ASSERT(0 == memcmp(B.data(),
                                   "\x01\x02\x03\x04",
                                   NUM_BYTES));

                // Verify method has no effect if the stream is invalid.
                mX.invalidate();
                mX.putVersion(7);
                ASSERT(NUM_BYTES == B.length());
                ASSERT(0 == memcmp(B.data(),
                                   "\x01\x02\x03\x04",
                                   NUM_BYTES));
            }
            {
                Buf mB;  const Buf& B = mB;

                Obj mX(&mB, VERSION_SELECTOR);
                ASSERT(0 == B.length());
                mX.putVersion(252);
                mX.putVersion(253);
                mX.putVersion(254);
                mX.putVersion(255);
                if (veryVerbose) { P(B); }

                const bsl::size_t NUM_BYTES = 4 * SIZE;
                ASSERT(NUM_BYTES == B.length());
                ASSERT(0 == memcmp(B.data(),
                                   "\xfc\xfd\xfe\xff",
                                   NUM_BYTES));
            }
            {
                // Verify the return value.
                Buf mB;
                Obj mX(&mB, VERSION_SELECTOR);
                ASSERT(&mX == &mX.putVersion(7));
            }
            {
                // Verify error handling.
                Buf mB;

                for (int i = 0; i < SIZE; ++i) {
                    mB.setLimit(i);

                    Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                    mX.putVersion(1);
                    ASSERT(false == X.isValid());
                }

                mB.setLimit(SIZE);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putVersion(1);
                ASSERT(X.isValid());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_SAFE_FAIL(mX.putLength(-1));
            ASSERT_SAFE_PASS(mX.putLength(0));
            ASSERT_SAFE_PASS(mX.putLength(1));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT ARRAY TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayFloat64(const double *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT FLOAT ARRAY TEST" << endl
                          << "===========================" << endl;

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) cout << "\nTesting putArrayFloat64." << endl;
        {
            const double DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayFloat64(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayFloat64(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayFloat64(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayFloat64(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\x40\x00\x00\x00\x00\x00\x00\x00"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayFloat64(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\x40\x00\x00\x00\x00\x00\x00\x00"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const double DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayFloat64(DATA, 3));
        }
        {
            // Verify error handling.
            const double DATA[] = {1, 2, 3};
            const int    NUM_DATA = static_cast<int>(sizeof DATA
                                                     / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayFloat64(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayFloat64(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const double DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayFloat64(0, 0));
            ASSERT_FAIL(mX.putArrayFloat64(DATA, -1));
            ASSERT_PASS(mX.putArrayFloat64(DATA, 0));
            ASSERT_PASS(mX.putArrayFloat64(DATA, 1));
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT ARRAY TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayFloat32(const float *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT FLOAT ARRAY TEST" << endl
                          << "===========================" << endl;

        const int SIZE = SIZEOF_FLOAT32;

        if (verbose) cout << "\nTesting putArrayFloat32." << endl;
        {
            const float DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayFloat32(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayFloat32(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayFloat32(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayFloat32(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x3f\x80\x00\x00"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x3f\x80\x00\x00"
                               "\x40\x00\x00\x00"
                               "\x40\x40\x00\x00" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayFloat32(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x3f\x80\x00\x00"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x3f\x80\x00\x00"
                               "\x40\x00\x00\x00"
                               "\x40\x40\x00\x00" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const float DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayFloat32(DATA, 3));
        }
        {
            // Verify error handling.
            const float DATA[] = {1, 2, 3};
            const int   NUM_DATA = static_cast<int>(sizeof DATA
                                                    / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayFloat32(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayFloat32(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const float DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayFloat32(0, 0));
            ASSERT_FAIL(mX.putArrayFloat32(DATA, -1));
            ASSERT_PASS(mX.putArrayFloat32(DATA, 0));
            ASSERT_PASS(mX.putArrayFloat32(DATA, 1));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt64(const bsls::Types::Int64 *array, int count);
        //   putArrayUint64(const bsls::Types::Uint64 *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT64;

        if (verbose) cout << "\nTesting putArrayInt64." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt64(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt64(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt64(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt64(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt64(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt64(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt64(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt64(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint64." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint64(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint64(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint64(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint64(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint64(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint64(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint64(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint64(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt64(0, 0));
            ASSERT_FAIL(mX.putArrayInt64(DATA, -1));
            ASSERT_PASS(mX.putArrayInt64(DATA, 0));
            ASSERT_PASS(mX.putArrayInt64(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint64(0, 0));
            ASSERT_FAIL(mX.putArrayUint64(DATA, -1));
            ASSERT_PASS(mX.putArrayUint64(DATA, 0));
            ASSERT_PASS(mX.putArrayUint64(DATA, 1));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt56(const bsls::Types::Int64 *array, int count);
        //   putArrayUint56(const bsls::Types::Uint64 *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 56-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT56;

        if (verbose) cout << "\nTesting putArrayInt56." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt56(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt56(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt56(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt56(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt56(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt56(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt56(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt56(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint56." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint56(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint56(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint56(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint56(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint56(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint56(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint56(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint56(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt56(0, 0));
            ASSERT_FAIL(mX.putArrayInt56(DATA, -1));
            ASSERT_PASS(mX.putArrayInt56(DATA, 0));
            ASSERT_PASS(mX.putArrayInt56(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint56(0, 0));
            ASSERT_FAIL(mX.putArrayUint56(DATA, -1));
            ASSERT_PASS(mX.putArrayUint56(DATA, 0));
            ASSERT_PASS(mX.putArrayUint56(DATA, 1));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt48(const bsls::Types::Int64 *array, int count);
        //   putArrayUint48(const bsls::Types::Uint64 *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 48-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT48;

        if (verbose) cout << "\nTesting putArrayInt48." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt48(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt48(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt48(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt48(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt48(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt48(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt48(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt48(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint48." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint48(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint48(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint48(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint48(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint48(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint48(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint48(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint48(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt48(0, 0));
            ASSERT_FAIL(mX.putArrayInt48(DATA, -1));
            ASSERT_PASS(mX.putArrayInt48(DATA, 0));
            ASSERT_PASS(mX.putArrayInt48(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint48(0, 0));
            ASSERT_FAIL(mX.putArrayUint48(DATA, -1));
            ASSERT_PASS(mX.putArrayUint48(DATA, 0));
            ASSERT_PASS(mX.putArrayUint48(DATA, 1));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt40(const bsls::Types::Int64 *array, int count);
        //   putArrayUint40(const bsls::Types::Uint64 *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 40-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT40;

        if (verbose) cout << "\nTesting putArrayInt40." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt40(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt40(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt40(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt40(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt40(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt40(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt40(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt40(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint40." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint40(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint40(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint40(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint40(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint40(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint40(DATA, 3));
        }
        {
            // Verify error handling.
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint40(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint40(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt40(0, 0));
            ASSERT_FAIL(mX.putArrayInt40(DATA, -1));
            ASSERT_PASS(mX.putArrayInt40(DATA, 0));
            ASSERT_PASS(mX.putArrayInt40(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint40(0, 0));
            ASSERT_FAIL(mX.putArrayUint40(DATA, -1));
            ASSERT_PASS(mX.putArrayUint40(DATA, 0));
            ASSERT_PASS(mX.putArrayUint40(DATA, 1));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt32(const int *array, int count);
        //   putArrayUint32(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT32;

        if (verbose) cout << "\nTesting putArrayInt32." << endl;
        {
            const int DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt32(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt32(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt32(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt32(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02"
                               "\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt32(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02"
                               "\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const int DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt32(DATA, 3));
        }
        {
            // Verify error handling.
            const int DATA[] = {1, 2, 3};
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt32(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt32(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint32." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint32(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint32(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint32(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint32(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02"
                               "\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint32(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02"
                               "\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const unsigned int DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint32(DATA, 3));
        }
        {
            // Verify error handling.
            const unsigned int DATA[] = {1, 2, 3};
            const int          NUM_DATA = static_cast<int>(sizeof DATA
                                                           / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint32(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint32(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const int DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt32(0, 0));
            ASSERT_FAIL(mX.putArrayInt32(DATA, -1));
            ASSERT_PASS(mX.putArrayInt32(DATA, 0));
            ASSERT_PASS(mX.putArrayInt32(DATA, 1));
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint32(0, 0));
            ASSERT_FAIL(mX.putArrayUint32(DATA, -1));
            ASSERT_PASS(mX.putArrayUint32(DATA, 0));
            ASSERT_PASS(mX.putArrayUint32(DATA, 1));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt24(const int *array, int count);
        //   putArrayUint24(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 24-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT24;

        if (verbose) cout << "\nTesting putArrayInt24." << endl;
        {
            const int DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt24(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt24(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt24(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt24(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x01"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x01"
                               "\x00\x00\x02"
                               "\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt24(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x01"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x01"
                               "\x00\x00\x02"
                               "\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const int DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt24(DATA, 3));
        }
        {
            // Verify error handling.
            const int DATA[] = {1, 2, 3};
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt24(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt24(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint24." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint24(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint24(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint24(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint24(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x01"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x01"
                               "\x00\x00\x02"
                               "\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint24(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x01"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x01"
                               "\x00\x00\x02"
                               "\x00\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const unsigned int DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint24(DATA, 3));
        }
        {
            // Verify error handling.
            const unsigned int DATA[] = {1, 2, 3};
            const int          NUM_DATA = static_cast<int>(sizeof DATA
                                                           / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint24(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint24(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const int DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt24(0, 0));
            ASSERT_FAIL(mX.putArrayInt24(DATA, -1));
            ASSERT_PASS(mX.putArrayInt24(DATA, 0));
            ASSERT_PASS(mX.putArrayInt24(DATA, 1));
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint24(0, 0));
            ASSERT_FAIL(mX.putArrayUint24(DATA, -1));
            ASSERT_PASS(mX.putArrayUint24(DATA, 0));
            ASSERT_PASS(mX.putArrayUint24(DATA, 1));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt16(const short *array, int count);
        //   putArrayUint16(const unsigned short *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 16-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT16;

        if (verbose) cout << "\nTesting putArrayInt16." << endl;
        {
            const short DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt16(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt16(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt16(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt16(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt16(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const short DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt16(DATA, 3));
        }
        {
            // Verify error handling.
            const short DATA[] = {1, 2, 3};
            const int   NUM_DATA = static_cast<int>(sizeof DATA
                                                    / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt16(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt16(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint16." << endl;
        {
            const unsigned short DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint16(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint16(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint16(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint16(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint16(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const unsigned short DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint16(DATA, 3));
        }
        {
            // Verify error handling.
            const unsigned short DATA[] = {1, 2, 3};
            const int            NUM_DATA = static_cast<int>(sizeof DATA
                                                             / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint16(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint16(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const short DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt16(0, 0));
            ASSERT_FAIL(mX.putArrayInt16(DATA, -1));
            ASSERT_PASS(mX.putArrayInt16(DATA, 0));
            ASSERT_PASS(mX.putArrayInt16(DATA, 1));
        }
        {
            const unsigned short DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint16(0, 0));
            ASSERT_FAIL(mX.putArrayUint16(DATA, -1));
            ASSERT_PASS(mX.putArrayUint16(DATA, 0));
            ASSERT_PASS(mX.putArrayUint16(DATA, 1));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PUT 8-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt8(const char *array, int count);
        //   putArrayInt8(const signed char *array, int count);
        //   putArrayUint8(const char *array, int count);
        //   putArrayUint8(const unsigned char *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 8-BIT INTEGER ARRAYS TEST" << endl
                          << "=============================" << endl;

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting putArrayInt8(char *)." << endl;
        {
            const char DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayInt8(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt8(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt8(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt8(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt8(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const char DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayInt8(DATA, 3));
        }
        {
            // Verify error handling.
            const char DATA[] = {1, 2, 3};
            const int  NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayInt8(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt8(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint8(char *)." << endl;
        {
            const char DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint8(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint8(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint8(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint8(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint8(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const char DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint8(DATA, 3));
        }
        {
            // Verify error handling.
            const char DATA[] = {1, 2, 3};
            const int  NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint8(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint8(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint8(unsigned char *)."
                          << endl;
        {
            const unsigned char DATA[] = {1, 2, 3};

            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            mX.putArrayUint8(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint8(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint8(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint8(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(B); }

            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint8(DATA, 3);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            const unsigned char DATA[] = {1, 2, 3};

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putArrayUint8(DATA, 3));
        }
        {
            // Verify error handling.
            const unsigned char DATA[] = {1, 2, 3};
            const int           NUM_DATA = static_cast<int>(sizeof DATA
                                                            / sizeof *DATA);

            Buf mB;

            for (int i = 0; i < NUM_DATA * SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putArrayUint8(DATA, NUM_DATA);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(NUM_DATA * SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint8(DATA, NUM_DATA);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const char DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_SAFE_FAIL(mX.putArrayInt8((char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 1));
        }
        {
            const signed char DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_SAFE_FAIL(mX.putArrayInt8((signed char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 1));
        }
        {
            const char DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_SAFE_FAIL(mX.putArrayUint8((char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 1));
        }
        {
            const unsigned char DATA[] = {1, 2, 3};

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT_SAFE_FAIL(mX.putArrayUint8((unsigned char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 1));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putFloat64(double value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT FLOAT TEST" << endl
                          << "=====================" << endl;

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) cout << "\nTesting putFloat64." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                  mX.putInt8(0xff);
            mX.putFloat64(1);     mX.putInt8(0xfe);
            mX.putFloat64(2);     mX.putInt8(0xfd);
            mX.putFloat64(3);     mX.putInt8(0xfc);
            mX.putFloat64(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc"
                               "\x40\x10\x00\x00\x00\x00\x00\x00" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putFloat64(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc"
                               "\x40\x10\x00\x00\x00\x00\x00\x00" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putFloat64(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putFloat64(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putFloat64(1);
            ASSERT(X.isValid());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putFloat32(float value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT FLOAT TEST" << endl
                          << "=====================" << endl;

        const int SIZE = SIZEOF_FLOAT32;

        if (verbose) cout << "\nTesting putFloat32." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                  mX.putInt8(0xff);
            mX.putFloat32(1);     mX.putInt8(0xfe);
            mX.putFloat32(2);     mX.putInt8(0xfd);
            mX.putFloat32(3);     mX.putInt8(0xfc);
            mX.putFloat32(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x40\x40\x00\x00" "\xfc"
                               "\x40\x80\x00\x00" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putFloat32(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x40\x40\x00\x00" "\xfc"
                               "\x40\x80\x00\x00" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putFloat32(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putFloat32(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putFloat32(1);
            ASSERT(X.isValid());
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt64(bsls::Types::Int64 value);
        //   putUint64(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT64;

        if (verbose) cout << "\nTesting putInt64." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt64(1);     mX.putInt8(0xfe);
            mX.putInt64(2);     mX.putInt8(0xfd);
            mX.putInt64(3);     mX.putInt8(0xfc);
            mX.putInt64(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt64(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt64(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt64(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt64(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint64." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint64(1);     mX.putInt8(0xfe);
            mX.putUint64(2);     mX.putInt8(0xfd);
            mX.putUint64(3);     mX.putInt8(0xfc);
            mX.putUint64(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint64(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint64(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint64(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint64(1);
            ASSERT(X.isValid());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt56(bsls::Types::Int64 value);
        //   putUint56(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 56-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT56;

        if (verbose) cout << "\nTesting putInt56." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt56(1);     mX.putInt8(0xfe);
            mX.putInt56(2);     mX.putInt8(0xfd);
            mX.putInt56(3);     mX.putInt8(0xfc);
            mX.putInt56(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt56(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt56(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt56(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt56(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint56." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint56(1);     mX.putInt8(0xfe);
            mX.putUint56(2);     mX.putInt8(0xfd);
            mX.putUint56(3);     mX.putInt8(0xfc);
            mX.putUint56(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint56(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint56(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint56(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint56(1);
            ASSERT(X.isValid());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt48(bsls::Types::Int64 value);
        //   putUint48(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 48-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT48;

        if (verbose) cout << "\nTesting putInt48." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt48(1);     mX.putInt8(0xfe);
            mX.putInt48(2);     mX.putInt8(0xfd);
            mX.putInt48(3);     mX.putInt8(0xfc);
            mX.putInt48(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt48(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt48(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt48(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt48(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint48." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint48(1);     mX.putInt8(0xfe);
            mX.putUint48(2);     mX.putInt8(0xfd);
            mX.putUint48(3);     mX.putInt8(0xfc);
            mX.putUint48(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint48(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint48(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint48(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint48(1);
            ASSERT(X.isValid());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt40(bsls::Types::Int64 value);
        //   putUint40(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 40-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT40;

        if (verbose) cout << "\nTesting putInt40." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt40(1);     mX.putInt8(0xfe);
            mX.putInt40(2);     mX.putInt8(0xfd);
            mX.putInt40(3);     mX.putInt8(0xfc);
            mX.putInt40(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt40(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt40(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt40(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt40(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint40." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint40(1);     mX.putInt8(0xfe);
            mX.putUint40(2);     mX.putInt8(0xfd);
            mX.putUint40(3);     mX.putInt8(0xfc);
            mX.putUint40(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint40(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint40(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint40(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint40(1);
            ASSERT(X.isValid());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt32(int value);
        //   putUint32(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT32;

        if (verbose) cout << "\nTesting putInt32." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt32(1);     mX.putInt8(0xfe);
            mX.putInt32(2);     mX.putInt8(0xfd);
            mX.putInt32(3);     mX.putInt8(0xfc);
            mX.putInt32(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt32(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt32(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt32(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt32(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint32." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint32(1);     mX.putInt8(0xfe);
            mX.putUint32(2);     mX.putInt8(0xfd);
            mX.putUint32(3);     mX.putInt8(0xfc);
            mX.putUint32(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint32(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint32(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint32(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint32(1);
            ASSERT(X.isValid());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt24(int value);
        //   putUint24(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 24-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT24;

        if (verbose) cout << "\nTesting putInt24." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt24(1);     mX.putInt8(0xfe);
            mX.putInt24(2);     mX.putInt8(0xfd);
            mX.putInt24(3);     mX.putInt8(0xfc);
            mX.putInt24(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt24(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt24(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt24(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt24(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint24." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint24(1);     mX.putInt8(0xfe);
            mX.putUint24(2);     mX.putInt8(0xfd);
            mX.putUint24(3);     mX.putInt8(0xfc);
            mX.putUint24(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint24(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint24(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint24(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint24(1);
            ASSERT(X.isValid());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-3)
        //
        // Testing:
        //   putInt16(int value);
        //   putUint16(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 16-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT16;

        if (verbose) cout << "\nTesting putInt16." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                mX.putInt8(0xff);
            mX.putInt16(1);     mX.putInt8(0xfe);
            mX.putInt16(2);     mX.putInt8(0xfd);
            mX.putInt16(3);     mX.putInt8(0xfc);
            mX.putInt16(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt16(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt16(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt16(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt16(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint16." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
                                 mX.putInt8(0xff);
            mX.putUint16(1);     mX.putInt8(0xfe);
            mX.putUint16(2);     mX.putInt8(0xfd);
            mX.putUint16(3);     mX.putInt8(0xfc);
            mX.putUint16(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint16(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint16(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint16(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint16(1);
            ASSERT(X.isValid());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // STREAM VALIDITY METHODS
        //   Verify that the validity methods work correctly.
        //
        // Concerns:
        //: 1 The stream initializes valid.
        //:
        //: 2 'invalidate' marks the stream invalid.
        //:
        //: 3 Both validity accessors return the correct validity state.
        //
        // Plan:
        //: 1 Initialize a stream and verify the accessors' values.  (C-1)
        //:
        //: 2 'invalidate' the stream and verify the values.  (C-2..3)
        //
        // Testing:
        //   void invalidate();
        //   operator const void *() const;
        //   bool isValid() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STREAM VALIDITY METHODS" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting invalidate." << endl;
        {
            Buf mB;

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            ASSERT( X &&  X.isValid());

            mX.invalidate();
            ASSERT(!X && !X.isValid());

            mX.invalidate();
            ASSERT(!X && !X.isValid());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST
        //   Verify functionality of the basic accessors.
        //
        // Concerns:
        //: 1 'bdexVersionSelector' returns correct value.
        //
        // Plan:
        //: 1 Create empty objects with different 'versionSelector' constructor
        //:   values and verify the 'bdexVersionSelector' method's return
        //:   value.  (C-1)
        //
        // Testing:
        //   int bdexVersionSelector() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS TEST" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting bdexVersionSelector()." << endl;

        for (int i = 0; i < 5; ++i) {
            if (veryVerbose) { P(i); }

            Buf mB;

            Obj mX(&mB, VERSION_SELECTOR + i);  const Obj& X = mX;
            LOOP_ASSERT(i, VERSION_SELECTOR + i == X.bdexVersionSelector());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   Verify functionality of primary manipulators.
        //
        // Concerns:
        //: 1 'putInt8' and 'putUint8' produce the expected results.
        //:
        //: 2 If there is a failure writing to the underlying buffer, the
        //:   stream is invalidated.
        //:
        //: 3 The destructor functions properly.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize data with the primary manipulator and verify the
        //:   produced output.  (C-1)
        //:
        //: 2 Use the 'setLimit' method of the 'TestOutStreamBuf' to verify
        //:   errors are handled correctly.  (C-2)
        //:
        //: 3 Since the destructor for this object is empty, the concern
        //:   regarding the destructor is trivially satisfied.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   GenericOutStream(STREAMBUF *streamBuf, int sV);
        //   ~GenericOutStream();
        //   putInt8(int value);
        //   putUint8(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting putInt8 and constructor." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(0 == B.length());

            mX.putInt8(1);
            mX.putInt8(2);
            mX.putInt8(3);
            mX.putInt8(4);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(), "\x01\x02\x03\x04", NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt8(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(), "\x01\x02\x03\x04", NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putInt8(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putInt8(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt8(1);
            ASSERT(X.isValid());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint8." << endl;
        {
            Buf mB;  const Buf& B = mB;

            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(0 == B.length());

            mX.putUint8(1);
            mX.putUint8(2);
            mX.putUint8(3);
            mX.putUint8(4);
            if (veryVerbose) { P(B); }
            const bsl::size_t NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(), "\x01\x02\x03\x04", NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint8(1);
            ASSERT(NUM_BYTES == B.length());
            ASSERT(0 == memcmp(B.data(), "\x01\x02\x03\x04", NUM_BYTES));
        }
        {
            // Verify the return value.
            Buf mB;
            Obj mX(&mB, VERSION_SELECTOR);
            ASSERT(&mX == &mX.putUint8(1));
        }
        {
            // Verify error handling.
            Buf mB;

            for (int i = 0; i < SIZE; ++i) {
                mB.setLimit(i);

                Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
                mX.putUint8(1);
                ASSERT(false == X.isValid());
            }

            mB.setLimit(SIZE);

            Obj mX(&mB, VERSION_SELECTOR);  const Obj& X = mX;
            mX.putUint8(1);
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            ASSERT_SAFE_PASS(Obj mX(&mB, VERSION_SELECTOR));
            ASSERT_SAFE_FAIL(Obj mX(0, VERSION_SELECTOR));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create 'GenericOutStream' objects.
        //:
        //: 2 Exercise these objects using various methods.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nCreate object x1." << endl;
        Buf b1;
        Obj x1(&b1, VERSION_SELECTOR);
        ASSERT(0 == b1.length());

        if (verbose) cout << "\nTry putInt32 with x1." << endl;
        x1.putInt32(1);
        if (veryVerbose) { P(b1); }
        ASSERT(SIZEOF_INT32 == b1.length());
        ASSERT(0 == memcmp("\x00\x00\x00\x01", b1.data(), SIZEOF_INT32));

        if (verbose) cout << "\nCreate object x2." << endl;
        Buf b2;
        Obj x2(&b2, VERSION_SELECTOR);
        ASSERT(0 == b2.length());

        if (verbose) cout << "\nTry putArrayInt8 with x2." << endl;
        const char data[] = {0x01, 0x02, 0x03, 0x04};
        const int  size = static_cast<int>(sizeof data / sizeof *data);
        x2.putArrayInt8(data, size);
        if (veryVerbose) { P(b2); }
        ASSERT(SIZEOF_INT8 * size == b2.length());
        ASSERT(0 == memcmp("\x01\x02\x03\x04", b2.data(), SIZEOF_INT8 * size));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
