// bslx_byteoutstream.t.cpp                                           -*-C++-*-

#include <bslx_byteoutstream.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cctype.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// For all output methods in 'ByteOutStream', the formatting of the input
// value to its correct byte representation is delegated to another component.
// We assume that this formatting has been rigorously tested and verified.
// Therefore, we are concerned only with the proper placement and alignment of
// bytes in the output stream.  We verify these properties by inserting chosen
// "marker" bytes between each output method call, and ensure that the new
// output bytes are properly interleaved between the "marker" bytes.
//
// We have chosen the primary black-box manipulator for 'ByteOutStream'
// to be 'putInt8'.
//-----------------------------------------------------------------------------
// [ 2] ByteOutStream(int sV, *ba = 0);
// [ 2] ByteOutStream(int sV, int initialCapacity, *ba = 0);
// [ 2] ~ByteOutStream();
// [ 4] void invalidate();
// [25] putLength(int length);
// [25] putVersion(int version);
// [ 2] void reserveCapacity(int newCapacity);
// [ 2] void reset();
// [12] putInt64(bsls::Types::Int64 value);
// [12] putUint64(bsls::Types::Int64 value);
// [11] putInt56(bsls::Types::Int64 value);
// [11] putUint56(bsls::Types::Int64 value);
// [10] putInt48(bsls::Types::Int64 value);
// [10] putUint48(bsls::Types::Int64 value);
// [ 9] putInt40(bsls::Types::Int64 value);
// [ 9] putUint40(bsls::Types::Int64 value);
// [ 8] putInt32(int value);
// [ 8] putUint32(int value);
// [ 7] putInt24(int value);
// [ 7] putUint24(int value);
// [ 6] putInt16(int value);
// [ 6] putUint16(int value);
// [ 2] putInt8(int value);
// [ 2] putUint8(int value);
// [14] putFloat64(double value);
// [13] putFloat32(float value);
// [26] putString(const bsl::string& value);
// [22] putArrayInt64(const bsls::Types::Int64 *array, int count);
// [22] putArrayUint64(const bsls::Types::Uint64 *array, int count);
// [21] putArrayInt56(const bsls::Types::Int64 *array, int count);
// [21] putArrayUint56(const bsls::Types::Uint64 *array, int count);
// [20] putArrayInt48(const bsls::Types::Int64 *array, int count);
// [20] putArrayUint48(const bsls::Types::Uint64 *array, int count);
// [19] putArrayInt40(const bsls::Types::Int64 *array, int count);
// [19] putArrayUint40(const bsls::Types::Uint64 *array, int count);
// [18] putArrayInt32(const int *array, int count);
// [18] putArrayUint32(const unsigned int *array, int count);
// [17] putArrayInt24(const int *array, int count);
// [17] putArrayUint24(const unsigned int *array, int count);
// [16] putArrayInt16(const short *array, int count);
// [16] putArrayUint16(const unsigned short *array, int count);
// [15] putArrayInt8(const char *array, int count);
// [15] putArrayInt8(const signed char *array, int count);
// [15] putArrayUint8(const char *array, int count);
// [15] putArrayUint8(const unsigned char *array, int count);
// [24] putArrayFloat64(const double *array,int count);
// [23] putArrayFloat32(const float *array, int count);
// [ 4] operator const void *() const;
// [ 3] int bdexSerializationVersion() const;
// [ 3] const char *data() const;
// [ 4] bool isValid() const;
// [ 3] int length() const;
//
// [ 5] ostream& operator<<(ostream& stream, const ByteOutStream&);
// [27] ByteOutStream& operator<<(ByteOutStream&, const TYPE& value);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [28] USAGE
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ByteOutStream Obj;

const int SERIALIZATION_VERSION = 20131127;
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

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

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

    bslma::TestAllocator ta(veryVeryVeryVerbose);

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

        if (verbose) cout << endl << "Testing Usage Examples" << endl
                                  << "======================" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Externalization
///- - - - - - - - - - - - - - - -
// A 'bslx::ByteOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::ByteOutStream', compares the contents of this stream
// to the expected value, and then writes the contents of this stream's buffer
// to 'stdout'.
//
// First, we create a 'bslx::ByteOutStream' with an arbitrary value for its
// 'serializationVersion' and externalize some values:
//..
    bslx::ByteOutStream outStream(20131127);
    outStream.putInt32(1);
    outStream.putInt32(2);
    outStream.putInt8('c');
    outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the stream to the expected value:
//..
    const char *theChars = outStream.data();
    int length = outStream.length();
    ASSERT(15 == length);
    ASSERT( 0 == bsl::memcmp(theChars,
                             "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
                             length));
//..
// Finally, we print the stream's contents to 'bsl::cout'.
//..
    if (veryVerbose)
    for (int i = 0; i < length; ++i) {
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
// See the 'bslx_byteinstream' component usage example for a more practical
// example of using 'bslx' streams.

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
        //   ByteOutStream& operator<<(ByteOutStream&, const TYPE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXTERNALIZATION FREE OPERATOR" << endl
                          << "=============================" << endl;

        {
            char value = 'a';
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            Obj expected(SERIALIZATION_VERSION);
            mX << value;
            OutStreamFunctions::bdexStreamOut(expected, value);
            ASSERT(X.length() == expected.length());
            ASSERT(0 == memcmp(X.data(), expected.data(), expected.length()));
        }
        {
            double value = 7.0;
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            Obj expected(SERIALIZATION_VERSION);
            mX << value;
            OutStreamFunctions::bdexStreamOut(expected, value);
            ASSERT(X.length() == expected.length());
            ASSERT(0 == memcmp(X.data(), expected.data(), expected.length()));
        }
        {
            bsl::vector<int> value;
            for (int i = 0; i < 5; ++i) {
                Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                Obj expected(SERIALIZATION_VERSION);
                mX << value;
                OutStreamFunctions::bdexStreamOut(expected, value);
                LOOP_ASSERT(i, X.length() == expected.length());
                LOOP_ASSERT(i, 0 == memcmp(X.data(),
                                           expected.data(),
                                           expected.length()));
                value.push_back(i);
            }
        }
        {
            float value1 = 3.0;
            bsl::string value2 = "hello";
            short value3 = 2;
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            Obj expected(SERIALIZATION_VERSION);
            mX << value1 << value2 << value3;
            OutStreamFunctions::bdexStreamOut(expected, value1);
            OutStreamFunctions::bdexStreamOut(expected, value2);
            OutStreamFunctions::bdexStreamOut(expected, value3);
            ASSERT(X.length() == expected.length());
            ASSERT(0 == memcmp(X.data(), expected.data(), expected.length()));
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // PUT STRING TEST:
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
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
            const int SIZE = SIZEOF_INT8 + 5 * SIZEOF_INT8;
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putString(DATA);     mX.putInt8(0xff);
            mX.putString(DATA);     mX.putInt8(0xfe);
            mX.putString(DATA);     mX.putInt8(0xfd);
            mX.putString(DATA);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               "\x05hello" "\xff"
                               "\x05hello" "\xfe"
                               "\x05hello" "\xfd"
                               "\x05hello" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putString(DATA);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               "\x05hello" "\xff"
                               "\x05hello" "\xfe"
                               "\x05hello" "\xfd"
                               "\x05hello" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putString(DATA));
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // PUT LENGTH AND VERSION TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putLength(int length);
        //   putVersion(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT LENGTH AND VERSION TEST" << endl
                          << "===========================" << endl;
        {
            if (verbose) cout << "\nTesting putLength." << endl;
            {
                Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                       mX.putInt8(0xff);
                mX.putLength(  1);     mX.putInt8(0xfe);
                mX.putLength(128);     mX.putInt8(0xfd);
                mX.putLength(127);     mX.putInt8(0xfc);
                mX.putLength(256);     mX.putInt8(0xfb);
                if (veryVerbose) { P(X); }
                const int NUM_BYTES = 7 * SIZEOF_INT8 + 2 * SIZEOF_INT32;
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   "\xff" "\x01" "\xfe"
                                   "\x80\x00\x00\x80" "\xfd"
                                   "\x7f" "\xfc"
                                   "\x80\x00\x01\x00" "\xfb",
                                   NUM_BYTES));

                // Verify method has no effect if the stream is invalid.
                mX.invalidate();
                mX.putLength(7);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   "\xff" "\x01" "\xfe"
                                   "\x80\x00\x00\x80" "\xfd"
                                   "\x7f" "\xfc"
                                   "\x80\x00\x01\x00" "\xfb",
                                   NUM_BYTES));

                // Verify the return value.
                mX.reset();
                ASSERT(&mX == &mX.putLength(7));
            }
        }

        {
            const int SIZE = SIZEOF_INT8;

            if (verbose) cout << "\nTesting putVersion." << endl;
            {
                Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                ASSERT(0 == X.length());
                mX.putVersion(1);
                mX.putVersion(2);
                mX.putVersion(3);
                mX.putVersion(4);
                if (veryVerbose) { P(X); }
                const int NUM_BYTES = 4 * SIZE;
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   "\x01\x02\x03\x04",
                                   NUM_BYTES));

                // Verify method has no effect if the stream is invalid.
                mX.invalidate();
                mX.putVersion(7);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   "\x01\x02\x03\x04",
                                   NUM_BYTES));

                // Verify the return value.
                mX.reset();
                ASSERT(&mX == &mX.putVersion(7));
            }
            {
                Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                ASSERT(0 == X.length());
                mX.putVersion(252);
                mX.putVersion(253);
                mX.putVersion(254);
                mX.putVersion(255);
                if (veryVerbose) { P(X); }
                const int NUM_BYTES = 4 * SIZE;
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   "\xfc\xfd\xfe\xff",
                                   NUM_BYTES));
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putLength(-1));
            ASSERT_SAFE_PASS(mX.putLength(0));
            ASSERT_SAFE_PASS(mX.putLength(1));
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT ARRAY TEST:
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayFloat64(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayFloat64(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayFloat64(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayFloat64(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\x40\x00\x00\x00\x00\x00\x00\x00"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayFloat64(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const double DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayFloat64(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayFloat64(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayFloat64(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayFloat64(DATA, 1));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT ARRAY TEST:
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayFloat32(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayFloat32(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayFloat32(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayFloat32(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x3f\x80\x00\x00"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x3f\x80\x00\x00"
                               "\x40\x00\x00\x00"
                               "\x40\x40\x00\x00" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayFloat32(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const float DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayFloat32(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayFloat32(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayFloat32(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayFloat32(DATA, 1));
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt64(const int *array, int count);
        //   putArrayUint64(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT64;

        if (verbose) cout << "\nTesting putArrayInt64." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt64(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt64(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt64(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt64(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt64(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint64." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint64(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint64(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint64(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint64(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint64(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt64(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt64(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt64(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt64(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint64(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint64(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint64(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint64(DATA, 1));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt56(const int *array, int count);
        //   putArrayUint56(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 56-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT56;

        if (verbose) cout << "\nTesting putArrayInt56." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt56(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt56(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt56(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt56(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt56(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint56." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint56(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint56(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint56(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint56(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint56(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt56(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt56(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt56(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt56(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint56(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint56(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint56(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint56(DATA, 1));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt48(const int *array, int count);
        //   putArrayUint48(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 48-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT48;

        if (verbose) cout << "\nTesting putArrayInt48." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt48(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt48(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt48(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt48(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt48(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint48." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint48(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint48(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint48(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint48(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint48(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt48(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt48(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt48(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt48(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint48(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint48(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint48(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint48(DATA, 1));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt40(const int *array, int count);
        //   putArrayUint40(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 40-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT40;

        if (verbose) cout << "\nTesting putArrayInt40." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt40(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt40(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt40(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt40(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt40(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint40." << endl;
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint40(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint40(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint40(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint40(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x01"
                               "\x00\x00\x00\x00\x02"
                               "\x00\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint40(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt40(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt40(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt40(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt40(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint40(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint40(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint40(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint40(DATA, 1));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt32(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt32(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt32(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt32(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02"
                               "\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt32(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint32." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint32(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint32(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint32(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint32(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x01"
                               "\x00\x00\x00\x02"
                               "\x00\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint32(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const int DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt32(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt32(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt32(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt32(DATA, 1));
        }
        {
            const unsigned int DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint32(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint32(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint32(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint32(DATA, 1));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt24(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt24(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt24(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt24(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x01"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x01"
                               "\x00\x00\x02"
                               "\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt24(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint24." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint24(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint24(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint24(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint24(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
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
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x01"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x01"
                               "\x00\x00\x02"
                               "\x00\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint24(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const int DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt24(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt24(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt24(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt24(DATA, 1));
        }
        {
            const unsigned int DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint24(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint24(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint24(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint24(DATA, 1));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt16(const int *array, int count);
        //   putArrayUint16(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 16-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT16;

        if (verbose) cout << "\nTesting putArrayInt16." << endl;
        {
            const short DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt16(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt16(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt16(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt16(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt16(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt16(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint16." << endl;
        {
            const unsigned short DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint16(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint16(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint16(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint16(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint16(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                               "\xff"
                               "\x00\x01"                       "\xfe"
                               "\x00\x01" "\x00\x02"            "\xfd"
                               "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint16(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const short DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt16(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt16(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt16(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt16(DATA, 1));
        }
        {
            const unsigned short DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint16(0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint16(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint16(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint16(DATA, 1));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // PUT 8-BIT INTEGER ARRAYS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   putArrayInt8(const char *array, int count);
        //   putArrayInt8(const signed char *array, int count);
        //   putArrayUint8(const char *array, int count);
        //   putArrayUint8(const unsigned char *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 8-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting putArrayInt8(char *)." << endl;
        {
            const char DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayInt8(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayInt8(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayInt8(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayInt8(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt8(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint8(char *)." << endl;
        {
            const char DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint8(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint8(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint8(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint8(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint8(DATA, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint8(unsigned char *)."
                          << endl;
        {
            const unsigned char DATA[] = {1, 2, 3};
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putArrayUint8(DATA, 0);     mX.putInt8(0xff);
            mX.putArrayUint8(DATA, 1);     mX.putInt8(0xfe);
            mX.putArrayUint8(DATA, 2);     mX.putInt8(0xfd);
            mX.putArrayUint8(DATA, 3);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                   "\xff"
                               "\x01"               "\xfe"
                               "\x01" "\x02"        "\xfd"
                               "\x01" "\x02" "\x03" "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint8(DATA, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const char DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt8((char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 1));
        }
        {
            const signed char DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayInt8((signed char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayInt8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayInt8(DATA, 1));
        }
        {
            const char DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint8((char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 1));
        }
        {
            const unsigned char DATA[] = {1, 2, 3};
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.putArrayUint8((unsigned char *)0, 0));
            ASSERT_SAFE_FAIL(mX.putArrayUint8(DATA, -1));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 0));
            ASSERT_SAFE_PASS(mX.putArrayUint8(DATA, 1));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT TEST:
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //
        // Testing:
        //   putFloat64(float value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT FLOAT TEST" << endl
                          << "=====================" << endl;

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) cout << "\nTesting putFloat64." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                  mX.putInt8(0xff);
            mX.putFloat64(1);     mX.putInt8(0xfe);
            mX.putFloat64(2);     mX.putInt8(0xfd);
            mX.putFloat64(3);     mX.putInt8(0xfc);
            mX.putFloat64(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc"
                               "\x40\x10\x00\x00\x00\x00\x00\x00" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putFloat64(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                               "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                               "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc"
                               "\x40\x10\x00\x00\x00\x00\x00\x00" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putFloat64(1));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT TEST:
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                  mX.putInt8(0xff);
            mX.putFloat32(1);     mX.putInt8(0xfe);
            mX.putFloat32(2);     mX.putInt8(0xfd);
            mX.putFloat32(3);     mX.putInt8(0xfc);
            mX.putFloat32(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x40\x40\x00\x00" "\xfc"
                               "\x40\x80\x00\x00" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putFloat32(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x3f\x80\x00\x00" "\xfe"
                               "\x40\x00\x00\x00" "\xfd"
                               "\x40\x40\x00\x00" "\xfc"
                               "\x40\x80\x00\x00" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putFloat32(1));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt64(1);     mX.putInt8(0xfe);
            mX.putInt64(2);     mX.putInt8(0xfd);
            mX.putInt64(3);     mX.putInt8(0xfc);
            mX.putInt64(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt64(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt64(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint64." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint64(1);     mX.putInt8(0xfe);
            mX.putUint64(2);     mX.putInt8(0xfd);
            mX.putUint64(3);     mX.putInt8(0xfc);
            mX.putUint64(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint64(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                                 "\xff"
                               "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint64(1));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //    (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt56(1);     mX.putInt8(0xfe);
            mX.putInt56(2);     mX.putInt8(0xfd);
            mX.putInt56(3);     mX.putInt8(0xfc);
            mX.putInt56(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt56(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt56(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint56." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint56(1);     mX.putInt8(0xfe);
            mX.putUint56(2);     mX.putInt8(0xfd);
            mX.putUint56(3);     mX.putInt8(0xfc);
            mX.putUint56(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint56(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                             "\xff"
                               "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint56(1));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt48(1);     mX.putInt8(0xfe);
            mX.putInt48(2);     mX.putInt8(0xfd);
            mX.putInt48(3);     mX.putInt8(0xfc);
            mX.putInt48(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt48(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt48(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint48." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint48(1);     mX.putInt8(0xfe);
            mX.putUint48(2);     mX.putInt8(0xfd);
            mX.putUint48(3);     mX.putInt8(0xfc);
            mX.putUint48(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint48(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                         "\xff"
                               "\x00\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint48(1));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt40(1);     mX.putInt8(0xfe);
            mX.putInt40(2);     mX.putInt8(0xfd);
            mX.putInt40(3);     mX.putInt8(0xfc);
            mX.putInt40(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt40(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt40(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint40." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint40(1);     mX.putInt8(0xfe);
            mX.putUint40(2);     mX.putInt8(0xfd);
            mX.putUint40(3);     mX.putInt8(0xfc);
            mX.putUint40(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint40(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                     "\xff"
                               "\x00\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint40(1));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //    (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt32(1);     mX.putInt8(0xfe);
            mX.putInt32(2);     mX.putInt8(0xfd);
            mX.putInt32(3);     mX.putInt8(0xfc);
            mX.putInt32(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt32(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt32(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint32." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint32(1);     mX.putInt8(0xfe);
            mX.putUint32(2);     mX.putInt8(0xfd);
            mX.putUint32(3);     mX.putInt8(0xfc);
            mX.putUint32(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint32(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""                 "\xff"
                               "\x00\x00\x00\x01" "\xfe"
                               "\x00\x00\x00\x02" "\xfd"
                               "\x00\x00\x00\x03" "\xfc"
                               "\x00\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint32(1));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
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
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt24(1);     mX.putInt8(0xfe);
            mX.putInt24(2);     mX.putInt8(0xfd);
            mX.putInt24(3);     mX.putInt8(0xfc);
            mX.putInt24(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt24(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt24(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint24." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint24(1);     mX.putInt8(0xfe);
            mX.putUint24(2);     mX.putInt8(0xfd);
            mX.putUint24(3);     mX.putInt8(0xfc);
            mX.putUint24(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint24(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""             "\xff"
                               "\x00\x00\x01" "\xfe"
                               "\x00\x00\x02" "\xfd"
                               "\x00\x00\x03" "\xfc"
                               "\x00\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint24(1));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGERS TEST:
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //
        // Testing:
        //   putInt16(short value);
        //   putUint16(unsigned short value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 16-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT16;

        if (verbose) cout << "\nTesting putInt16." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                mX.putInt8(0xff);
            mX.putInt16(1);     mX.putInt8(0xfe);
            mX.putInt16(2);     mX.putInt8(0xfd);
            mX.putInt16(3);     mX.putInt8(0xfc);
            mX.putInt16(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt16(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt16(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint16." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                                 mX.putInt8(0xff);
            mX.putUint16(1);     mX.putInt8(0xfe);
            mX.putUint16(2);     mX.putInt8(0xfd);
            mX.putUint16(3);     mX.putInt8(0xfc);
            mX.putUint16(4);     mX.putInt8(0xfb);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint16(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               ""         "\xff"
                               "\x00\x01" "\xfe"
                               "\x00\x02" "\xfd"
                               "\x00\x03" "\xfc"
                               "\x00\x04" "\xfb",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint16(1));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT OPERATOR TEST:
        //   Verify the method produces the expected output format.
        //
        // Concerns:
        //: 1 The method produces the expected output format.
        //
        // Plan:
        //: 1 For a small set of objects, use 'ostrstream' to write the
        //:   object's value to a string buffer and then compare to expected
        //:   output format.  (C-1)
        //
        // Testing:
        //   ostream& operator<<(ostream&, const ByteOutStream&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT OPERATOR TEST" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting print operator." << endl;

        const int SIZE = 1000;     // Must be big enough to hold output string.
        const char XX = (char) 0xFF;  // Value that represents an unset char.
        char ctrl[SIZE];    memset(ctrl, XX, SIZE);
        const char *CTRL = ctrl;

        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            const char *EXPECTED = "";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << X << ends;
            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putInt8(0);  mX.putInt8(1);  mX.putInt8(2);  mX.putInt8(3);
            const char *EXPECTED =
                "\n0000\t00 01 02 03";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << X << ends;
            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putInt8(0);  mX.putInt8(1);  mX.putInt8(2);  mX.putInt8(3);
            mX.putInt8(4);  mX.putInt8(5);  mX.putInt8(6);  mX.putInt8(7);
            mX.putInt8(8);  mX.putInt8(9);  mX.putInt8(10); mX.putInt8(11);
            const char *EXPECTED =
                "\n0000\t00 01 02 03 04 05 06 07"
                "\n0008\t08 09 0a 0b";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << X << ends;
            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            mX.putInt8(  0);  mX.putInt8( 1);  mX.putInt8( 2);  mX.putInt8( 3);
            mX.putInt8(  4);  mX.putInt8( 5);  mX.putInt8( 6);  mX.putInt8( 7);
            mX.putInt8(  8);  mX.putInt8( 9);  mX.putInt8(10);  mX.putInt8(11);
            mX.putInt8(127);  mX.putInt8(-1);  mX.putInt8(-2);  mX.putInt8(-3);
            const char *EXPECTED =
                "\n0000\t00 01 02 03 04 05 06 07"
                "\n0008\t08 09 0a 0b 7f ff fe fd";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << X << ends;
            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
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
                          << "INVALIDATE" << endl
                          << "==========" << endl;

        if (verbose) cout << "\nTesting invalidate." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
                              ASSERT( X && X.isValid());

            mX.invalidate();  ASSERT(!X && !X.isValid());
            mX.invalidate();  ASSERT(!X && !X.isValid());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST:
        //   Verify functionality of the basic accessors.
        //
        // Concerns:
        //: 1 'length' and 'data' methods return correct values.
        //:
        //: 2 'bdexSerializationVersion' returns correct value.
        //
        // Plan:
        //: 1 Create an empty object, use 'putInt8' to modify state, and
        //:   verify the expected values for the methods.  (C-1)
        //:
        //: 2 Create empty objects with different 'serializationVersion'
        //:   constructor values and verify the 'bdexSerializationVersion'
        //:   method's return value.  (C-2)
        //
        // Testing:
        //   int bdexSerializationVersion() const;
        //   const char *data() const;
        //   int length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS TEST" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting length() and data()." << endl;

        const char *DATA[] = {
            "",
            "\x00",
            "\x00\x01",
            "\x00\x01\x02",
            "\x00\x01\x02\x03"
        };
        const int NUM_TEST = static_cast<int>(sizeof DATA / sizeof *DATA);
        for (int iLen = 0; iLen < NUM_TEST; iLen++) {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            for (int j = 0; j < iLen; j++) mX.putInt8(j);

            if (veryVerbose) { P_(iLen); P(X); }
            const int bytes = iLen * SIZEOF_INT8;
            // verify length()
            LOOP_ASSERT(iLen, X.length() == bytes);
            // verify data()
            LOOP_ASSERT(iLen, 0 == memcmp(X.data(), DATA[iLen], bytes));
        }

        if (verbose) cout << "\nTesting bdexSerializationVersion()." << endl;

        for (int i = 0; i < 5; ++i) {
            Obj x(SERIALIZATION_VERSION + i);
            LOOP_ASSERT(i, SERIALIZATION_VERSION + i ==
                                                 x.bdexSerializationVersion());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST:
        //   Verify functionality of primary manipulators.
        //
        // Concerns:
        //: 1 'putInt8' and 'putUint8' produce the expected results.
        //:
        //: 2 The initial capacity constructor allocates if needed.
        //:
        //: 3 'reserveCapacity' allocates if needed.
        //:
        //: 4 'reset' validates and removes all data from the object.
        //:
        //: 5 The destructor functions properly.
        //:
        //: 6 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify allocation occurrences by using a test allocator.
        //:
        //: 2 Externalize data with the primary manipulator and verify the
        //:   produced output using the basic accessors 'length' and 'data'.
        //:   (C-1)
        //:
        //: 3 Construct with the initial capacity constructor and verify
        //:   allocation occurred.  (C-2)
        //:
        //: 4 Use 'reserveCapacity' to set a larger required capacity, verify
        //:   allocation occurred, use the method again to request a smaller
        //:   capacity, and verify no allocation occurs.  (C-3)
        //:
        //: 5 Execute 'reset' on objects with varying length and verify
        //:   the object is emptied.  Also execute 'reset' on invalid objects.
        //:   (C-4)
        //:
        //: 6 Verify the functionality of the destructor using test allocators.
        //:   (C-5)
        //:
        //: 7 Verify defensive checks are triggered for invalid values.  (C-6)
        //
        // Testing:
        //   ByteOutStream(int sV, *ba = 0);
        //   ByteOutStream(int sV, int initialCapacity, *ba = 0);
        //   ~ByteOutStream();
        //   putInt8(int value);
        //   putUint8(int value);
        //   reset();
        //   reserveCapacity(int newCapacity);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting putInt8, reserveCapacity, and "
                          << "constructor w/o allocator." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            Obj mX(SERIALIZATION_VERSION, 100);  const Obj& X = mX;
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            mX.putInt8(1);
            mX.putInt8(2);
            mX.putInt8(3);
            mX.putInt8(4);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x01\x02\x03\x04", NUM_BYTES));
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt8(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x01\x02\x03\x04", NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt8(1));
        }
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            mX.reserveCapacity(300);     // Make larger than default
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            mX.reserveCapacity(100);
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            mX.putInt8(5);
            mX.putInt8(6);
            mX.putInt8(7);
            mX.putInt8(8);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x05\x06\x07\x08", NUM_BYTES));
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putInt8, reserveCapacity, and "
                          << "constructor w/ allocator." << endl;
        {
            bsls::Types::Int64 allocations = ta.numAllocations();
            Obj mX(SERIALIZATION_VERSION, 100, &ta);  const Obj& X = mX;
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == ta.numAllocations());
            mX.putInt8(1);
            mX.putInt8(2);
            mX.putInt8(3);
            mX.putInt8(4);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x01\x02\x03\x04", NUM_BYTES));
            ASSERT(allocations + 1 == ta.numAllocations());

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt8(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x01\x02\x03\x04", NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt8(1));
        }
        {
            Obj mX(SERIALIZATION_VERSION, &ta);  const Obj& X = mX;
            bsls::Types::Int64 allocations = ta.numAllocations();
            mX.reserveCapacity(300);     // Make larger than default
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == ta.numAllocations());
            mX.reserveCapacity(100);
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == ta.numAllocations());
            mX.putInt8(5);
            mX.putInt8(6);
            mX.putInt8(7);
            mX.putInt8(8);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x05\x06\x07\x08", NUM_BYTES));
            ASSERT(allocations + 1 == ta.numAllocations());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint8." << endl;
        {
            Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;
            ASSERT(0 == X.length());
            mX.putUint8(1);
            mX.putUint8(2);
            mX.putUint8(3);
            mX.putUint8(4);
            if (veryVerbose) { P(X); }
            const int NUM_BYTES = 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x01\x02\x03\x04", NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint8(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(), "\x01\x02\x03\x04", NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint8(1));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting reset()." << endl;
        {
            const int NUM_TEST = 5;
            for (int iLen = 1; iLen < NUM_TEST; iLen++) {
                Obj mX(SERIALIZATION_VERSION);  const Obj& X = mX;

                for (int j = 0; j < iLen; j++) mX.putInt8(j);
                mX.reset();
                if (veryVerbose) { P_(iLen); P(X); }
                LOOP_ASSERT(iLen, 0 == X.length());

                for (int j = 0; j < iLen; j++) mX.putInt8(j);
                mX.invalidate();
                LOOP_ASSERT(iLen, false == X.isValid());
                mX.reset();
                if (veryVerbose) { P_(iLen); P(X); }
                LOOP_ASSERT(iLen, X.isValid());
                LOOP_ASSERT(iLen, 0 == X.length());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj x(SERIALIZATION_VERSION, -1));
            ASSERT_SAFE_PASS(Obj x(SERIALIZATION_VERSION, 0));
            ASSERT_SAFE_PASS(Obj x(SERIALIZATION_VERSION, 1));

            Obj mX(SERIALIZATION_VERSION);
            ASSERT_SAFE_FAIL(mX.reserveCapacity(-1));
            ASSERT_SAFE_PASS(mX.reserveCapacity(0));
            ASSERT_SAFE_PASS(mX.reserveCapacity(1));
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
        //: 1 Create 'ByteOutStream' objects using the default constructor.
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

        if (verbose) cout << "\nCreate object x1 using default ctor." << endl;
        Obj x1(SERIALIZATION_VERSION);
        ASSERT(0 == x1.length());

        if (verbose) cout << "\nTry putInt32 with x1." << endl;
        x1.putInt32(1);
        if (veryVerbose) { P(x1); }
        ASSERT(SIZEOF_INT32 == x1.length());
        ASSERT(0 == memcmp("\x00\x00\x00\x01", x1.data(), SIZEOF_INT32));

        if (verbose) cout << "\nClear x1 using reset method." << endl;
        x1.reset();
        ASSERT(0 == x1.length());

        if (verbose) cout << "\nTry putArrayInt8 with x1." << endl;
        const char data[] = {0x01, 0x02, 0x03, 0x04};
        const int size = static_cast<int>(sizeof data / sizeof *data);
        x1.putArrayInt8(data, size);
        if (veryVerbose) { P(x1); }
        ASSERT(SIZEOF_INT8 * size == x1.length());
        ASSERT(0 == memcmp("\x01\x02\x03\x04", x1.data(), size));
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
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
