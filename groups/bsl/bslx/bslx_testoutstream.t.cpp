// bslx_testoutstream.t.cpp                                           -*-C++-*-

#include <bslx_testoutstream.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cctype.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a "test" implementation of the BDEX 'OutStream' protocol.
// This implementation inserts additional "informational data" into the output
// stream to identify the type (and size, for arrays) of the externalized
// values.
//
// For each output method, we must verify the placement and alignment of the
// byte values in the output stream, as in the "normal" implementation".  In
// addition, we must verify the correctness of the "informational data"
// associated with the output method.  We accomplish these goals by
// externalizing chosen values, and verify the resulting output stream contents
// against the expected contents.
//
// Note that byte alignment testing is automatically performed by the property
// that each output method inserts an odd number of "informational" bytes into
// the stream.
// ----------------------------------------------------------------------------
// [ 2] TestOutStream(int sV, *ba = 0);
// [ 2] TestOutStream(int sV, bsl::size_t initialCapacity, *ba = 0);
// [ 2] ~TestOutStream();
// [ 4] void invalidate();
// [ 2] makeNextInvalid();
// [25] putLength(int length);
// [25] putVersion(int version);
// [ 2] reserveCapacity(bsl::size_t newCapacity);
// [ 2] reset();
// [12] putInt64(bsls::Types::Int64 value);
// [12] putUint64(bsls::Types::Uint64 value);
// [11] putInt56(bsls::Types::Int64 value);
// [11] putUint56(bsls::Types::Uint64 value);
// [10] putInt48(bsls::Types::Int64 value);
// [10] putUint48(bsls::Types::Uint64 value);
// [ 9] putInt40(bsls::Types::Int64 value);
// [ 9] putUint40(bsls::Types::Uint64 value);
// [ 8] putInt32(int value);
// [ 8] putUint32(unsigned int value);
// [ 7] putInt24(int value);
// [ 7] putUint24(unsigned int value);
// [ 6] putInt16(int value);
// [ 6] putUint16(unsigned int value);
// [ 2] putInt8(int value);
// [ 2] putUint8(unsigned int value);
// [14] putFloat64(double value);
// [13] putFloat32(float value);
// [26] putString(const bsl::string& value);
// [22] putArrayInt64(const bsls::Types::Int64 *values, int numValues);
// [22] putArrayUint64(const bsls::Types::Uint64 *values, int numValues);
// [21] putArrayInt56(const bsls::Types::Int64 *values, int numValues);
// [21] putArrayUint56(const bsls::Types::Uint64 *values, int numValues);
// [20] putArrayInt48(const bsls::Types::Int64 *values, int numValues);
// [20] putArrayUint48(const bsls::Types::Uint64 *values, int numValues);
// [19] putArrayInt40(const bsls::Types::Int64 *values, int numValues);
// [19] putArrayUint40(const bsls::Types::Uint64 *values, int numValues);
// [18] putArrayInt32(const int *values, int numValues);
// [18] putArrayUint32(const unsigned int *values, int numValues);
// [17] putArrayInt24(const int *values, int numValues);
// [17] putArrayUint24(const unsigned int *values, int numValues);
// [16] putArrayInt16(const short *values, int numValues);
// [16] putArrayUint16(const unsigned short *values, int numValues);
// [15] putArrayInt8(const char *values, int numValues);
// [15] putArrayInt8(const signed char *values, int numValues);
// [15] putArrayUint8(const char *values, int numValues);
// [15] putArrayUint8(const unsigned char *values, int numValues);
// [24] putArrayFloat64(const double *values, int numValues);
// [23] putArrayFloat32(const float *values, int numValues);
// [ 4] operator const void *() const;
// [ 3] int bdexVersionSelector() const;
// [ 3] const char *data() const;
// [ 4] bool isValid() const;
// [ 3] bsl::size_t length() const;
//
// [ 5] ostream& operator<<(ostream& stream, const TestOutStream&);
// [27] TestOutStream& operator<<(TestOutStream&, const TYPE& value);
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

namespace BloombergLP {
namespace bslx {

void debugprint(const TestOutStream& object)
{
    bsl::cout << object;
}

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// Type codes from 'bslx::TypeCode' as bytes in string representation
#define INT8_TC    "\xe0"
#define UINT8_TC   "\xe1"
#define INT16_TC   "\xe2"
#define UINT16_TC  "\xe3"
#define INT24_TC   "\xe4"
#define UINT24_TC  "\xe5"
#define INT32_TC   "\xe6"
#define UINT32_TC  "\xe7"
#define INT40_TC   "\xe8"
#define UINT40_TC  "\xe9"
#define INT48_TC   "\xea"
#define UINT48_TC  "\xeb"
#define INT56_TC   "\xec"
#define UINT56_TC  "\xed"
#define INT64_TC   "\xee"
#define UINT64_TC  "\xef"
#define FLOAT32_TC "\xf0"
#define FLOAT64_TC "\xf1"
#define INVALID_TC "\xf2"
// bslx::TypeCode::e_INT8 as bytes in string representation
#define INT8_STR   "e0"
// length 0 to 3 as bytes in string representation
#define LEN_0      "\x00\x00\x00\x00"
#define LEN_1      "\x00\x00\x00\x01"
#define LEN_2      "\x00\x00\x00\x02"
#define LEN_3      "\x00\x00\x00\x03"

typedef TestOutStream Obj;

// size in bytes of each fundamental type
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
const int SIZEOF_CODE    = SIZEOF_INT8;
const int SIZEOF_ARRLEN  = SIZEOF_INT32;

// ============================================================================
//                                MAIN PROGRAM
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Externalization
///- - - - - - - - - - - - - - - -
// A 'bslx::TestOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::TestOutStream', compares the contents of this stream
// to the expected value, and then writes the contents of this stream's buffer
// to 'stdout'.
//
// First, we create a 'bslx::TestOutStream' with an arbitrary value for its
// 'versionSelector' and externalize some values:
//..
    bslx::TestOutStream outStream(20131127);
    outStream.putInt32(1);
    outStream.putInt32(2);
    outStream.putInt8('c');
    outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the stream to the expected value:
//..
    const char  *theChars = outStream.data();
    bsl::size_t  length   = outStream.length();
    ASSERT(24 == length);
    ASSERT( 0 == bsl::memcmp(theChars,
                             "\xE6\x00\x00\x00\x01\xE6\x00\x00\x00\x02\xE0"
                                        "c\xE0\x05\xE1\x00\x00\x00\x05""hello",
                             length));
//..
// Finally, we print the stream's contents to 'bsl::cout'.
//..
    if (veryVerbose)
    for (bsl::size_t i = 0; i < length; ++i) {
        if(bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
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
//  nextByte (int): -26
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): -26
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (int): -32
//  nextByte (char): c
//  nextByte (int): -32
//  nextByte (int): 5
//  nextByte (int): -31
//  nextByte (char): h
//  nextByte (char): e
//  nextByte (char): l
//  nextByte (char): l
//  nextByte (char): o
//..
// Note the negative numeric values indicate the "type" of the data that
// follows (see 'bslx_typecode').
//
// See the 'bslx_testinstream' component usage example for a more practical
// example of using this test output stream.

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
        //   TestOutStream& operator<<(TestOutStream&, const TYPE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXTERNALIZATION FREE OPERATOR" << endl
                          << "=============================" << endl;

        {
            char value = 'a';

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            Obj expected(VERSION_SELECTOR);
            mX << value;
            OutStreamFunctions::bdexStreamOut(expected, value);
            ASSERT(X.length() == expected.length());
            ASSERT(0 == memcmp(X.data(), expected.data(), expected.length()));
        }
        {
            double value = 7.0;

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            Obj expected(VERSION_SELECTOR);
            mX << value;
            OutStreamFunctions::bdexStreamOut(expected, value);
            ASSERT(X.length() == expected.length());
            ASSERT(0 == memcmp(X.data(), expected.data(), expected.length()));
        }
        {
            bsl::vector<int> value;
            for (int i = 0; i < 5; ++i) {
                Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

                Obj expected(VERSION_SELECTOR);
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
            float       value1 = 3.0;
            bsl::string value2 = "hello";
            short       value3 = 2;

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            Obj expected(VERSION_SELECTOR);
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
        // PUT STRING TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with this method.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking this
        //:   method.  (C-3)
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

            const int SIZE = 3 * SIZEOF_INT8 + SIZEOF_INT32 + 5 * SIZEOF_INT8;

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putString(DATA);     mX.putInt8(0xff);
            mX.putString(DATA);     mX.putInt8(0xfe);
            mX.putString(DATA);     mX.putInt8(0xfd);
            mX.putString(DATA);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * SIZEOF_INT8 + 4 * SIZEOF_INT8 + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xff"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfe"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfd"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfc",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putString(DATA);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xff"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfe"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfd"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfc",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putString(DATA));
        }
        if (verbose) cout << "\nTesting 'putString' with 'makeNextInvalid'."
                          << endl;
        {
            const bsl::string DATA = "hello";

            const int SIZE = 3 * SIZEOF_INT8 + SIZEOF_INT32 + 5 * SIZEOF_INT8;

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putString(DATA);     mX.putInt8(0xff);
            mX.makeNextInvalid();
            mX.putString(DATA);     mX.putInt8(0xfe);
            mX.putString(DATA);     mX.putInt8(0xfd);
            mX.makeNextInvalid();
            mX.putString(DATA);     mX.putInt8(0xfc);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * SIZEOF_INT8 + 6 + 4 * SIZEOF_INT8
                                                                    + 4 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xff"
             INVALID_TC "\x00\x00\x00\x05"
                            UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfe"
             INT8_TC "\x05" UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfd"
             INVALID_TC "\x00\x00\x00\x05"
                            UINT8_TC "\x00\x00\x00\x05" "hello" INT8_TC "\xfc",
                               NUM_BYTES));
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // PUT LENGTH AND VERSION TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putLength(int length);
        //   putVersion(int version);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT LENGTH AND VERSION TEST" << endl
                 << "===========================" << endl;
        }
        {
            if (verbose) {
                cout << "\nTesting 'putLength'." << endl;
            }
            {
                Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

                mX.putLength(1);
                mX.putLength(128);
                mX.putLength(3);
                mX.putLength(256);
                if (veryVerbose) { P(X); }
                const bsl::size_t NUM_BYTES =
                          4 * SIZEOF_CODE + 2 * SIZEOF_INT32 + 2 * SIZEOF_INT8;
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   INT8_TC "\x01"
                                   INT32_TC "\x80\x00\x00\x80"
                                   INT8_TC "\x03"
                                   INT32_TC "\x80\x00\x01\x00",
                                   NUM_BYTES));

                // Verify method has no effect if the stream is invalid.
                mX.invalidate();
                mX.putLength(1);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   INT8_TC "\x01"
                                   INT32_TC "\x80\x00\x00\x80"
                                   INT8_TC "\x03"
                                   INT32_TC "\x80\x00\x01\x00",
                                   NUM_BYTES));

                // Verify the return value.
                mX.reset();
                ASSERT(&mX == &mX.putLength(1));
            }

            if (verbose)
                cout << "\nTesting 'putLength' w/ 'makeNextInvalid'." << endl;
            {
                Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
                mX.makeNextInvalid();
                mX.putLength(1);
                mX.putLength(128);
                mX.makeNextInvalid();
                mX.putLength(3);
                mX.putLength(256);
                if (veryVerbose) { P(X); }
                const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_INT32);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   INVALID_TC "\x00\x00\x00\x01"
                                   INT32_TC "\x80\x00\x00\x80"
                                   INVALID_TC "\x00\x00\x00\x03"
                                   INT32_TC "\x80\x00\x01\x00",
                                   NUM_BYTES));
            }
        }

        {
            const int SIZE = SIZEOF_INT8;

            if (verbose) {
                cout << "\nTesting 'putVersion'." << endl;
            }
            {
                Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
                ASSERT(0 == X.length());

                mX.putVersion(1);
                mX.putVersion(2);
                mX.putVersion(3);
                mX.putVersion(4);
                if (veryVerbose) { P(X); }
                const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   UINT8_TC "\x01"
                                   UINT8_TC "\x02"
                                   UINT8_TC "\x03"
                                   UINT8_TC "\x04",
                                   NUM_BYTES));

                // Verify method has no effect if the stream is invalid.
                mX.invalidate();
                mX.putVersion(1);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   UINT8_TC "\x01"
                                   UINT8_TC "\x02"
                                   UINT8_TC "\x03"
                                   UINT8_TC "\x04",
                                   NUM_BYTES));

                // Verify the return value.
                mX.reset();
                ASSERT(&mX == &mX.putVersion(1));
            }

            if (verbose)
                cout << "\nTesting 'putVersion' w/ 'makeNextInvalid'." << endl;
            {
                Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
                ASSERT(0 == X.length());
                mX.makeNextInvalid();
                mX.putVersion(1);
                mX.putVersion(2);
                mX.makeNextInvalid();
                mX.putVersion(3);
                mX.putVersion(4);
                if (veryVerbose) { P(X); }
                const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
                ASSERT(NUM_BYTES == X.length());
                ASSERT(0 == memcmp(X.data(),
                                   INVALID_TC "\x01"
                                   UINT8_TC "\x02"
                                   INVALID_TC "\x03"
                                   UINT8_TC "\x04",
                                   NUM_BYTES));
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putLength(-1));
            ASSERT_PASS(mX.putLength(0));
            ASSERT_PASS(mX.putLength(1));
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT ARRAY TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with this method.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayFloat64(const double *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 64-BIT FLOAT ARRAY TEST" << endl
                 << "===========================" << endl;
        }

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) {
            cout << "\nTesting 'putArrayFloat64'." << endl;
        }
        {
            const double DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayFloat64(DATA, 0);
            mX.putArrayFloat64(DATA, 1);
            mX.putArrayFloat64(DATA, 2);
            mX.putArrayFloat64(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                           FLOAT64_TC LEN_0 ""
                           FLOAT64_TC LEN_1 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           FLOAT64_TC LEN_2 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                           FLOAT64_TC LEN_3 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                                            "\x40\x08\x00\x00\x00\x00\x00\x00",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayFloat64(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                           FLOAT64_TC LEN_0 ""
                           FLOAT64_TC LEN_1 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           FLOAT64_TC LEN_2 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                           FLOAT64_TC LEN_3 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                                            "\x40\x08\x00\x00\x00\x00\x00\x00",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayFloat64(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayFloat64' w/ 'makeNextInvalid'." <<endl;
        }
        {
            const double DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayFloat64(DATA, 0);
            mX.putArrayFloat64(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayFloat64(DATA, 2);
            mX.putArrayFloat64(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                           INVALID_TC LEN_0 ""
                           FLOAT64_TC LEN_1 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           INVALID_TC LEN_2 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                           FLOAT64_TC LEN_3 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                                            "\x40\x08\x00\x00\x00\x00\x00\x00",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const double DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayFloat64(0, 0));
            ASSERT_FAIL(mX.putArrayFloat64(DATA, -1));
            ASSERT_PASS(mX.putArrayFloat64(DATA, 0));
            ASSERT_PASS(mX.putArrayFloat64(DATA, 1));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT ARRAY TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with this method.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayFloat32(const float *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 32-BIT FLOAT ARRAY TEST" << endl
                 << "===========================" << endl;
        }

        const int SIZE = SIZEOF_FLOAT32;

        if (verbose) {
            cout << "\nTesting 'putArrayFloat32'." << endl;
        }
        {
            const float DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayFloat32(DATA, 0);
            mX.putArrayFloat32(DATA, 1);
            mX.putArrayFloat32(DATA, 2);
            mX.putArrayFloat32(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               FLOAT32_TC LEN_0 ""
                               FLOAT32_TC LEN_1 "\x3f\x80\x00\x00"
                               FLOAT32_TC LEN_2 "\x3f\x80\x00\x00"
                                                "\x40\x00\x00\x00"
                               FLOAT32_TC LEN_3 "\x3f\x80\x00\x00"
                                                "\x40\x00\x00\x00"
                                                "\x40\x40\x00\x00",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayFloat32(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               FLOAT32_TC LEN_0 ""
                               FLOAT32_TC LEN_1 "\x3f\x80\x00\x00"
                               FLOAT32_TC LEN_2 "\x3f\x80\x00\x00"
                                                "\x40\x00\x00\x00"
                               FLOAT32_TC LEN_3 "\x3f\x80\x00\x00"
                                                "\x40\x00\x00\x00"
                                                "\x40\x40\x00\x00",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayFloat32(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayFloat32' w/ 'makeNextInvalid'." <<endl;
        }
        {
            const float DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayFloat32(DATA, 0);
            mX.putArrayFloat32(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayFloat32(DATA, 2);
            mX.putArrayFloat32(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                               FLOAT32_TC LEN_1 "\x3f\x80\x00\x00"
                               INVALID_TC LEN_2 "\x3f\x80\x00\x00"
                                                "\x40\x00\x00\x00"
                               FLOAT32_TC LEN_3 "\x3f\x80\x00\x00"
                                                "\x40\x00\x00\x00"
                                                "\x40\x40\x00\x00",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const float DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayFloat32(0, 0));
            ASSERT_FAIL(mX.putArrayFloat32(DATA, -1));
            ASSERT_PASS(mX.putArrayFloat32(DATA, 0));
            ASSERT_PASS(mX.putArrayFloat32(DATA, 1));
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt64(const bsls::Types::Int64 *values, int numValues);
        //   putArrayUint64(const bsls::Types::Uint64 *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 64-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT64;

        if (verbose) {
            cout << "\nTesting 'putArrayInt64'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt64(DATA, 0);
            mX.putArrayInt64(DATA, 1);
            mX.putArrayInt64(DATA, 2);
            mX.putArrayInt64(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                             INT64_TC LEN_0 ""
                             INT64_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                             INT64_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                             INT64_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                                            "\x00\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt64(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                             INT64_TC LEN_0 ""
                             INT64_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                             INT64_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                             INT64_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                                            "\x00\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt64(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt64' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt64(DATA, 0);
            mX.putArrayInt64(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt64(DATA, 2);
            mX.putArrayInt64(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                           INVALID_TC LEN_0 ""
                             INT64_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                           INVALID_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                             INT64_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                                            "\x00\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint64'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint64(DATA, 0);
            mX.putArrayUint64(DATA, 1);
            mX.putArrayUint64(DATA, 2);
            mX.putArrayUint64(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                            UINT64_TC LEN_0 ""
                            UINT64_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                            UINT64_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                            UINT64_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                                            "\x00\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint64(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                            UINT64_TC LEN_0 ""
                            UINT64_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                            UINT64_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                            UINT64_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                                            "\x00\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint64(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint64' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint64(DATA, 0);
            mX.putArrayUint64(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint64(DATA, 2);
            mX.putArrayUint64(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                           INVALID_TC LEN_0 ""
                            UINT64_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                           INVALID_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                            UINT64_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                            "\x00\x00\x00\x00\x00\x00\x00\x02"
                                            "\x00\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt64(0, 0));
            ASSERT_FAIL(mX.putArrayInt64(DATA, -1));
            ASSERT_PASS(mX.putArrayInt64(DATA, 0));
            ASSERT_PASS(mX.putArrayInt64(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint64(0, 0));
            ASSERT_FAIL(mX.putArrayUint64(DATA, -1));
            ASSERT_PASS(mX.putArrayUint64(DATA, 0));
            ASSERT_PASS(mX.putArrayUint64(DATA, 1));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt56(const bsls::Types::Int64 *values, int numValues);
        //   putArrayUint56(const bsls::Types::Uint64 *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 56-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT56;

        if (verbose) {
            cout << "\nTesting 'putArrayInt56'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt56(DATA, 0);
            mX.putArrayInt56(DATA, 1);
            mX.putArrayInt56(DATA, 2);
            mX.putArrayInt56(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT56_TC LEN_0 ""
                               INT56_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                               INT56_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x00\x02"
                               INT56_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x00\x02"
                                              "\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt56(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT56_TC LEN_0 ""
                               INT56_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                               INT56_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x00\x02"
                               INT56_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x00\x02"
                                              "\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt56(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt56' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt56(DATA, 0);
            mX.putArrayInt56(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt56(DATA, 2);
            mX.putArrayInt56(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 INT56_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x00\x02"
                                 INT56_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x00\x02"
                                                "\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint56'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint56(DATA, 0);
            mX.putArrayUint56(DATA, 1);
            mX.putArrayUint56(DATA, 2);
            mX.putArrayUint56(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT56_TC LEN_0 ""
                               UINT56_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                               UINT56_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x00\x02"
                               UINT56_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x00\x02"
                                               "\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint56(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT56_TC LEN_0 ""
                               UINT56_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                               UINT56_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x00\x02"
                               UINT56_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x00\x02"
                                               "\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint56(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint56' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint56(DATA, 0);
            mX.putArrayUint56(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint56(DATA, 2);
            mX.putArrayUint56(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                UINT56_TC LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x00\x02"
                                UINT56_TC LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x00\x02"
                                                "\x00\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt56(0, 0));
            ASSERT_FAIL(mX.putArrayInt56(DATA, -1));
            ASSERT_PASS(mX.putArrayInt56(DATA, 0));
            ASSERT_PASS(mX.putArrayInt56(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint56(0, 0));
            ASSERT_FAIL(mX.putArrayUint56(DATA, -1));
            ASSERT_PASS(mX.putArrayUint56(DATA, 0));
            ASSERT_PASS(mX.putArrayUint56(DATA, 1));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt48(const bsls::Types::Int64 *values, int numValues);
        //   putArrayUint48(const bsls::Types::Uint64 *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 48-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT48;

        if (verbose) {
            cout << "\nTesting 'putArrayInt48'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt48(DATA, 0);
            mX.putArrayInt48(DATA, 1);
            mX.putArrayInt48(DATA, 2);
            mX.putArrayInt48(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT48_TC LEN_0 ""
                               INT48_TC LEN_1 "\x00\x00\x00\x00\x00\x01"
                               INT48_TC LEN_2 "\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x02"
                               INT48_TC LEN_3 "\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x02"
                                              "\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt48(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT48_TC LEN_0 ""
                               INT48_TC LEN_1 "\x00\x00\x00\x00\x00\x01"
                               INT48_TC LEN_2 "\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x02"
                               INT48_TC LEN_3 "\x00\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x00\x02"
                                              "\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt48(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt48' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt48(DATA, 0);
            mX.putArrayInt48(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt48(DATA, 2);
            mX.putArrayInt48(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 INT48_TC LEN_1 "\x00\x00\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x02"
                                 INT48_TC LEN_3 "\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x02"
                                                "\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint48'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint48(DATA, 0);
            mX.putArrayUint48(DATA, 1);
            mX.putArrayUint48(DATA, 2);
            mX.putArrayUint48(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT48_TC LEN_0 ""
                               UINT48_TC LEN_1 "\x00\x00\x00\x00\x00\x01"
                               UINT48_TC LEN_2 "\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x02"
                               UINT48_TC LEN_3 "\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x02"
                                               "\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint48(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT48_TC LEN_0 ""
                               UINT48_TC LEN_1 "\x00\x00\x00\x00\x00\x01"
                               UINT48_TC LEN_2 "\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x02"
                               UINT48_TC LEN_3 "\x00\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x00\x02"
                                               "\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint48(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint48' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint48(DATA, 0);
            mX.putArrayUint48(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint48(DATA, 2);
            mX.putArrayUint48(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                UINT48_TC LEN_1 "\x00\x00\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x02"
                                UINT48_TC LEN_3 "\x00\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x00\x02"
                                                "\x00\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt48(0, 0));
            ASSERT_FAIL(mX.putArrayInt48(DATA, -1));
            ASSERT_PASS(mX.putArrayInt48(DATA, 0));
            ASSERT_PASS(mX.putArrayInt48(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint48(0, 0));
            ASSERT_FAIL(mX.putArrayUint48(DATA, -1));
            ASSERT_PASS(mX.putArrayUint48(DATA, 0));
            ASSERT_PASS(mX.putArrayUint48(DATA, 1));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt40(const bsls::Types::Int64 *values, int numValues);
        //   putArrayUint40(const bsls::Types::Uint64 *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 40-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT40;

        if (verbose) {
            cout << "\nTesting 'putArrayInt40'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt40(DATA, 0);
            mX.putArrayInt40(DATA, 1);
            mX.putArrayInt40(DATA, 2);
            mX.putArrayInt40(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT40_TC LEN_0 ""
                               INT40_TC LEN_1 "\x00\x00\x00\x00\x01"
                               INT40_TC LEN_2 "\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x02"
                               INT40_TC LEN_3 "\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x02"
                                              "\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt40(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT40_TC LEN_0 ""
                               INT40_TC LEN_1 "\x00\x00\x00\x00\x01"
                               INT40_TC LEN_2 "\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x02"
                               INT40_TC LEN_3 "\x00\x00\x00\x00\x01"
                                              "\x00\x00\x00\x00\x02"
                                              "\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt40(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt40' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt40(DATA, 0);
            mX.putArrayInt40(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt40(DATA, 2);
            mX.putArrayInt40(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 INT40_TC LEN_1 "\x00\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x02"
                                 INT40_TC LEN_3 "\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x02"
                                                "\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint40'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint40(DATA, 0);
            mX.putArrayUint40(DATA, 1);
            mX.putArrayUint40(DATA, 2);
            mX.putArrayUint40(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT40_TC LEN_0 ""
                               UINT40_TC LEN_1 "\x00\x00\x00\x00\x01"
                               UINT40_TC LEN_2 "\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x02"
                               UINT40_TC LEN_3 "\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x02"
                                               "\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint40(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT40_TC LEN_0 ""
                               UINT40_TC LEN_1 "\x00\x00\x00\x00\x01"
                               UINT40_TC LEN_2 "\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x02"
                               UINT40_TC LEN_3 "\x00\x00\x00\x00\x01"
                                               "\x00\x00\x00\x00\x02"
                                               "\x00\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint40(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint40' w/ 'makeNextInvalid'." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint40(DATA, 0);
            mX.putArrayUint40(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint40(DATA, 2);
            mX.putArrayUint40(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                UINT40_TC LEN_1 "\x00\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x02"
                                UINT40_TC LEN_3 "\x00\x00\x00\x00\x01"
                                                "\x00\x00\x00\x00\x02"
                                                "\x00\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const bsls::Types::Int64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt40(0, 0));
            ASSERT_FAIL(mX.putArrayInt40(DATA, -1));
            ASSERT_PASS(mX.putArrayInt40(DATA, 0));
            ASSERT_PASS(mX.putArrayInt40(DATA, 1));
        }
        {
            const bsls::Types::Uint64 DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint40(0, 0));
            ASSERT_FAIL(mX.putArrayUint40(DATA, -1));
            ASSERT_PASS(mX.putArrayUint40(DATA, 0));
            ASSERT_PASS(mX.putArrayUint40(DATA, 1));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt32(const int *values, int numValues);
        //   putArrayUint32(const unsigned int *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 32-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT32;

        if (verbose) {
            cout << "\nTesting 'putArrayInt32'." << endl;
        }
        {
            const int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt32(DATA, 0);
            mX.putArrayInt32(DATA, 1);
            mX.putArrayInt32(DATA, 2);
            mX.putArrayInt32(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT32_TC LEN_0 ""
                               INT32_TC LEN_1 "\x00\x00\x00\x01"
                               INT32_TC LEN_2 "\x00\x00\x00\x01"
                                              "\x00\x00\x00\x02"
                               INT32_TC LEN_3 "\x00\x00\x00\x01"
                                              "\x00\x00\x00\x02"
                                              "\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt32(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT32_TC LEN_0 ""
                               INT32_TC LEN_1 "\x00\x00\x00\x01"
                               INT32_TC LEN_2 "\x00\x00\x00\x01"
                                              "\x00\x00\x00\x02"
                               INT32_TC LEN_3 "\x00\x00\x00\x01"
                                              "\x00\x00\x00\x02"
                                              "\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt32(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt32' w/ 'makeNextInvalid'." << endl;
        }
        {
            const int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt32(DATA, 0);
            mX.putArrayInt32(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt32(DATA, 2);
            mX.putArrayInt32(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 INT32_TC LEN_1 "\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x01"
                                                "\x00\x00\x00\x02"
                                 INT32_TC LEN_3 "\x00\x00\x00\x01"
                                                "\x00\x00\x00\x02"
                                               "\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint32'." << endl;
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint32(DATA, 0);
            mX.putArrayUint32(DATA, 1);
            mX.putArrayUint32(DATA, 2);
            mX.putArrayUint32(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT32_TC LEN_0 ""
                               UINT32_TC LEN_1 "\x00\x00\x00\x01"
                               UINT32_TC LEN_2 "\x00\x00\x00\x01"
                                               "\x00\x00\x00\x02"
                               UINT32_TC LEN_3 "\x00\x00\x00\x01"
                                               "\x00\x00\x00\x02"
                                               "\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint32(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT32_TC LEN_0 ""
                               UINT32_TC LEN_1 "\x00\x00\x00\x01"
                               UINT32_TC LEN_2 "\x00\x00\x00\x01"
                                               "\x00\x00\x00\x02"
                               UINT32_TC LEN_3 "\x00\x00\x00\x01"
                                               "\x00\x00\x00\x02"
                                               "\x00\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint32(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint32' w/ 'makeNextInvalid'." << endl;
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint32(DATA, 0);
            mX.putArrayUint32(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint32(DATA, 2);
            mX.putArrayUint32(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                UINT32_TC LEN_1 "\x00\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x00\x01"
                                                "\x00\x00\x00\x02"
                                UINT32_TC LEN_3 "\x00\x00\x00\x01"
                                                "\x00\x00\x00\x02"
                                                "\x00\x00\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const int DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt32(0, 0));
            ASSERT_FAIL(mX.putArrayInt32(DATA, -1));
            ASSERT_PASS(mX.putArrayInt32(DATA, 0));
            ASSERT_PASS(mX.putArrayInt32(DATA, 1));
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint32(0, 0));
            ASSERT_FAIL(mX.putArrayUint32(DATA, -1));
            ASSERT_PASS(mX.putArrayUint32(DATA, 0));
            ASSERT_PASS(mX.putArrayUint32(DATA, 1));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt24(const int *values, int numValues);
        //   putArrayUint24(const unsigned int *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 24-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT24;

        if (verbose) {
            cout << "\nTesting 'putArrayInt24'." << endl;
        }
        {
            const int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt24(DATA, 0);
            mX.putArrayInt24(DATA, 1);
            mX.putArrayInt24(DATA, 2);
            mX.putArrayInt24(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT24_TC LEN_0 ""
                               INT24_TC LEN_1 "\x00\x00\x01"
                               INT24_TC LEN_2 "\x00\x00\x01"
                                              "\x00\x00\x02"
                               INT24_TC LEN_3 "\x00\x00\x01"
                                              "\x00\x00\x02"
                                              "\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt24(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT24_TC LEN_0 ""
                               INT24_TC LEN_1 "\x00\x00\x01"
                               INT24_TC LEN_2 "\x00\x00\x01"
                                              "\x00\x00\x02"
                               INT24_TC LEN_3 "\x00\x00\x01"
                                              "\x00\x00\x02"
                                              "\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt24(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt24' w/ 'makeNextInvalid'." << endl;
        }
        {
            const int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt24(DATA, 0);
            mX.putArrayInt24(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt24(DATA, 2);
            mX.putArrayInt24(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 INT24_TC LEN_1 "\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x01"
                                                "\x00\x00\x02"
                                 INT24_TC LEN_3 "\x00\x00\x01"
                                                "\x00\x00\x02"
                                               "\x00\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint24'." << endl;
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint24(DATA, 0);
            mX.putArrayUint24(DATA, 1);
            mX.putArrayUint24(DATA, 2);
            mX.putArrayUint24(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT24_TC LEN_0 ""
                               UINT24_TC LEN_1 "\x00\x00\x01"
                               UINT24_TC LEN_2 "\x00\x00\x01"
                                               "\x00\x00\x02"
                               UINT24_TC LEN_3 "\x00\x00\x01"
                                               "\x00\x00\x02"
                                               "\x00\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint24(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT24_TC LEN_0 ""
                               UINT24_TC LEN_1 "\x00\x00\x01"
                               UINT24_TC LEN_2 "\x00\x00\x01"
                                               "\x00\x00\x02"
                               UINT24_TC LEN_3 "\x00\x00\x01"
                                               "\x00\x00\x02"
                                               "\x00\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint24(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint24' w/ 'makeNextInvalid'." << endl;
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint24(DATA, 0);
            mX.putArrayUint24(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint24(DATA, 2);
            mX.putArrayUint24(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                UINT24_TC LEN_1 "\x00\x00\x01"
                               INVALID_TC LEN_2 "\x00\x00\x01"
                                                "\x00\x00\x02"
                                UINT24_TC LEN_3 "\x00\x00\x01"
                                                "\x00\x00\x02"
                                                "\x00\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const int DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt24(0, 0));
            ASSERT_FAIL(mX.putArrayInt24(DATA, -1));
            ASSERT_PASS(mX.putArrayInt24(DATA, 0));
            ASSERT_PASS(mX.putArrayInt24(DATA, 1));
        }
        {
            const unsigned int DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint24(0, 0));
            ASSERT_FAIL(mX.putArrayUint24(DATA, -1));
            ASSERT_PASS(mX.putArrayUint24(DATA, 0));
            ASSERT_PASS(mX.putArrayUint24(DATA, 1));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt16(const short *values, int numValues);
        //   putArrayUint16(const unsigned short *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 16-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        const int SIZE = SIZEOF_INT16;

        if (verbose) {
            cout << "\nTesting 'putArrayInt16'." << endl;
        }
        {
            const short DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt16(DATA, 0);
            mX.putArrayInt16(DATA, 1);
            mX.putArrayInt16(DATA, 2);
            mX.putArrayInt16(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT16_TC LEN_0 ""
                               INT16_TC LEN_1 "\x00\x01"
                               INT16_TC LEN_2 "\x00\x01" "\x00\x02"
                               INT16_TC LEN_3 "\x00\x01" "\x00\x02" "\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt16(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT16_TC LEN_0 ""
                               INT16_TC LEN_1 "\x00\x01"
                               INT16_TC LEN_2 "\x00\x01" "\x00\x02"
                               INT16_TC LEN_3 "\x00\x01" "\x00\x02" "\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt16(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt16' w/ 'makeNextInvalid'." << endl;
        }
        {
            const short DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt16(DATA, 0);
            mX.putArrayInt16(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt16(DATA, 2);
            mX.putArrayInt16(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                             INVALID_TC LEN_0 ""
                               INT16_TC LEN_1 "\x00\x01"
                             INVALID_TC LEN_2 "\x00\x01" "\x00\x02"
                               INT16_TC LEN_3 "\x00\x01" "\x00\x02" "\x00\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint16'." << endl;
        }
        {
            const unsigned short DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint16(DATA, 0);
            mX.putArrayUint16(DATA, 1);
            mX.putArrayUint16(DATA, 2);
            mX.putArrayUint16(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                              UINT16_TC LEN_0 ""
                              UINT16_TC LEN_1 "\x00\x01"
                              UINT16_TC LEN_2 "\x00\x01" "\x00\x02"
                              UINT16_TC LEN_3 "\x00\x01" "\x00\x02" "\x00\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint16(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                              UINT16_TC LEN_0 ""
                              UINT16_TC LEN_1 "\x00\x01"
                              UINT16_TC LEN_2 "\x00\x01" "\x00\x02"
                              UINT16_TC LEN_3 "\x00\x01" "\x00\x02" "\x00\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint16(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint16' w/ 'makeNextInvalid'." << endl;
        }
        {
            const unsigned short DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint16(DATA, 0);
            mX.putArrayUint16(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint16(DATA, 2);
            mX.putArrayUint16(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                             INVALID_TC LEN_0 ""
                              UINT16_TC LEN_1 "\x00\x01"
                             INVALID_TC LEN_2 "\x00\x01" "\x00\x02"
                              UINT16_TC LEN_3 "\x00\x01" "\x00\x02" "\x00\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const short DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt16(0, 0));
            ASSERT_FAIL(mX.putArrayInt16(DATA, -1));
            ASSERT_PASS(mX.putArrayInt16(DATA, 0));
            ASSERT_PASS(mX.putArrayInt16(DATA, 1));
        }
        {
            const unsigned short DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint16(0, 0));
            ASSERT_FAIL(mX.putArrayUint16(DATA, -1));
            ASSERT_PASS(mX.putArrayUint16(DATA, 0));
            ASSERT_PASS(mX.putArrayUint16(DATA, 1));
        }
       } break;
      case 15: {
        // --------------------------------------------------------------------
        // PUT 8-BIT INTEGER ARRAYS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putArrayInt8(const char *values, int numValues);
        //   putArrayInt8(const signed char *values, int numValues);
        //   putArrayUint8(const char *values, int numValues);
        //   putArrayUint8(const unsigned char *values, int numValues);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 8-BIT INTEGER ARRAYS TEST" << endl
                 << "=============================" << endl;
        }

        const int SIZE = SIZEOF_INT8;

        if (verbose) {
            cout << "\nTesting 'putArrayInt8(char*)'." << endl;
        }
        {
            const char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt8(DATA, 0);
            mX.putArrayInt8(DATA, 1);
            mX.putArrayInt8(DATA, 2);
            mX.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC LEN_0 ""
                               INT8_TC LEN_1 "\x01"
                               INT8_TC LEN_2 "\x01" "\x02"
                               INT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC LEN_0 ""
                               INT8_TC LEN_1 "\x01"
                               INT8_TC LEN_2 "\x01" "\x02"
                               INT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt8(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt8(char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        }
        {
            const char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt8(DATA, 0);
            mX.putArrayInt8(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt8(DATA, 2);
            mX.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                  INT8_TC LEN_1 "\x01"
                               INVALID_TC LEN_2 "\x01" "\x02"
                                  INT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayInt8(signed char*)'." << endl;
        }
        {
            const signed char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayInt8(DATA, 0);
            mX.putArrayInt8(DATA, 1);
            mX.putArrayInt8(DATA, 2);
            mX.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC LEN_0 ""
                               INT8_TC LEN_1 "\x01"
                               INT8_TC LEN_2 "\x01" "\x02"
                               INT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayInt8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC LEN_0 ""
                               INT8_TC LEN_1 "\x01"
                               INT8_TC LEN_2 "\x01" "\x02"
                               INT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayInt8(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayInt8(signed char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        }
        {
            const signed char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayInt8(DATA, 0);
            mX.putArrayInt8(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayInt8(DATA, 2);
            mX.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                  INT8_TC LEN_1 "\x01"
                               INVALID_TC LEN_2 "\x01" "\x02"
                                  INT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint8(char*)'." << endl;
        }
        {
            const char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint8(DATA, 0);
            mX.putArrayUint8(DATA, 1);
            mX.putArrayUint8(DATA, 2);
            mX.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT8_TC LEN_0 ""
                               UINT8_TC LEN_1 "\x01"
                               UINT8_TC LEN_2 "\x01" "\x02"
                               UINT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT8_TC LEN_0 ""
                               UINT8_TC LEN_1 "\x01"
                               UINT8_TC LEN_2 "\x01" "\x02"
                               UINT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint8(DATA, 3));
        }

        if (verbose) {
            cout << "\nTesting 'putArrayUint8(char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        }
        {
            const char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint8(DATA, 0);
            mX.putArrayUint8(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint8(DATA, 2);
            mX.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 UINT8_TC LEN_1 "\x01"
                               INVALID_TC LEN_2 "\x01" "\x02"
                                 UINT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putArrayUint8(unsigned char*)'."
                          << endl;
        }
        {
            const unsigned char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putArrayUint8(DATA, 0);
            mX.putArrayUint8(DATA, 1);
            mX.putArrayUint8(DATA, 2);
            mX.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT8_TC LEN_0 ""
                               UINT8_TC LEN_1 "\x01"
                               UINT8_TC LEN_2 "\x01" "\x02"
                               UINT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putArrayUint8(DATA, 3);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT8_TC LEN_0 ""
                               UINT8_TC LEN_1 "\x01"
                               UINT8_TC LEN_2 "\x01" "\x02"
                               UINT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putArrayUint8(DATA, 3));
        }
        if (verbose) {
            cout << "\nTesting 'putArrayUint8(unsigned char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        }
        {
            const unsigned char DATA[] = {1, 2, 3};

            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putArrayUint8(DATA, 0);
            mX.putArrayUint8(DATA, 1);
            mX.makeNextInvalid();
            mX.putArrayUint8(DATA, 2);
            mX.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES =
                                  4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC LEN_0 ""
                                 UINT8_TC LEN_1 "\x01"
                               INVALID_TC LEN_2 "\x01" "\x02"
                                 UINT8_TC LEN_3 "\x01" "\x02" "\x03",
                               NUM_BYTES));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            const char DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt8((char *)0, 0));
            ASSERT_FAIL(mX.putArrayInt8(DATA, -1));
            ASSERT_PASS(mX.putArrayInt8(DATA, 0));
            ASSERT_PASS(mX.putArrayInt8(DATA, 1));
        }
        {
            const signed char DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayInt8((signed char *)0, 0));
            ASSERT_FAIL(mX.putArrayInt8(DATA, -1));
            ASSERT_PASS(mX.putArrayInt8(DATA, 0));
            ASSERT_PASS(mX.putArrayInt8(DATA, 1));
        }
        {
            const char DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint8((char *)0, 0));
            ASSERT_FAIL(mX.putArrayUint8(DATA, -1));
            ASSERT_PASS(mX.putArrayUint8(DATA, 0));
            ASSERT_PASS(mX.putArrayUint8(DATA, 1));
        }
        {
            const unsigned char DATA[] = {1, 2, 3};

            bsls::AssertTestHandlerGuard guard;

            Obj mX(VERSION_SELECTOR);
            ASSERT_FAIL(mX.putArrayUint8((unsigned char *)0, 0));
            ASSERT_FAIL(mX.putArrayUint8(DATA, -1));
            ASSERT_PASS(mX.putArrayUint8(DATA, 0));
            ASSERT_PASS(mX.putArrayUint8(DATA, 1));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with this method.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putFloat64(double value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 64-BIT FLOAT TEST" << endl
                 << "=====================" << endl;
        }

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) {
            cout << "\nTesting 'putFloat64'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putFloat64(1);
            mX.putFloat64(2);
            mX.putFloat64(3);
            mX.putFloat64(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               FLOAT64_TC "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x00\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x08\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x10\x00\x00\x00\x00\x00\x00",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putFloat64(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               FLOAT64_TC "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x00\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x08\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x10\x00\x00\x00\x00\x00\x00",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putFloat64(1));
        }

        if (verbose) {
            cout << "\nTesting 'putFloat64' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putFloat64(1);
            mX.putFloat64(2);
            mX.makeNextInvalid();
            mX.putFloat64(3);
            mX.putFloat64(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x00\x00\x00\x00\x00\x00\x00"
                               INVALID_TC "\x40\x08\x00\x00\x00\x00\x00\x00"
                               FLOAT64_TC "\x40\x10\x00\x00\x00\x00\x00\x00",
                               NUM_BYTES));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT TEST
        //   Verify the method externalizes the expected bytes.
        //
        // Concerns:
        //: 1 The method externalizes the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with this method.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putFloat32(float value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 32-BIT FLOAT TEST" << endl
                 << "=====================" << endl;
        }

        const int SIZE = SIZEOF_FLOAT32;

        if (verbose) {
            cout << "\nTesting 'putFloat32'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putFloat32(1);
            mX.putFloat32(2);
            mX.putFloat32(3);
            mX.putFloat32(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               FLOAT32_TC "\x3f\x80\x00\x00"
                               FLOAT32_TC "\x40\x00\x00\x00"
                               FLOAT32_TC "\x40\x40\x00\x00"
                               FLOAT32_TC "\x40\x80\x00\x00",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putFloat32(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               FLOAT32_TC "\x3f\x80\x00\x00"
                               FLOAT32_TC "\x40\x00\x00\x00"
                               FLOAT32_TC "\x40\x40\x00\x00"
                               FLOAT32_TC "\x40\x80\x00\x00",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putFloat32(1));
        }

        if (verbose) {
            cout << "\nTesting 'putFloat32' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putFloat32(1);
            mX.putFloat32(2);
            mX.makeNextInvalid();
            mX.putFloat32(3);
            mX.putFloat32(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x3f\x80\x00\x00"
                               FLOAT32_TC "\x40\x00\x00\x00"
                               INVALID_TC "\x40\x40\x00\x00"
                               FLOAT32_TC "\x40\x80\x00\x00",
                               NUM_BYTES));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt64(bsls::Types::Int64 value);
        //   putUint64(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 64-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT64;

        if (verbose) {
            cout << "\nTesting 'putInt64'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt64(1);
            mX.putInt64(2);
            mX.putInt64(3);
            mX.putInt64(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x01"
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x02"
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x03"
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt64(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x01"
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x02"
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x03"
                               INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt64(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt64' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt64(1);
            mX.putInt64(2);
            mX.makeNextInvalid();
            mX.putInt64(3);
            mX.putInt64(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x00\x01"
                                 INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x00\x03"
                                 INT64_TC "\x00\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint64'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint64(1);
            mX.putUint64(2);
            mX.putUint64(3);
            mX.putUint64(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x01"
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x02"
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x03"
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint64(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x01"
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x02"
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x03"
                               UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint64(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint64' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint64(1);
            mX.putUint64(2);
            mX.makeNextInvalid();
            mX.putUint64(3);
            mX.putUint64(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x00\x01"
                                UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x00\x03"
                                UINT64_TC "\x00\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt56(bsls::Types::Int64 value);
        //   putUint56(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 56-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT56;

        if (verbose) {
            cout << "\nTesting 'putInt56'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt56(1);
            mX.putInt56(2);
            mX.putInt56(3);
            mX.putInt56(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x01"
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x02"
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x03"
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt56(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x01"
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x02"
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x03"
                               INT56_TC "\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt56(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt56' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt56(1);
            mX.putInt56(2);
            mX.makeNextInvalid();
            mX.putInt56(3);
            mX.putInt56(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x01"
                                 INT56_TC "\x00\x00\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x03"
                                 INT56_TC "\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint56'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint56(1);
            mX.putUint56(2);
            mX.putUint56(3);
            mX.putUint56(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x01"
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x02"
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x03"
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint56(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x01"
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x02"
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x03"
                               UINT56_TC "\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint56(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint56' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint56(1);
            mX.putUint56(2);
            mX.makeNextInvalid();
            mX.putUint56(3);
            mX.putUint56(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x01"
                                UINT56_TC "\x00\x00\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x00\x00\x03"
                                UINT56_TC "\x00\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt48(bsls::Types::Int64 value);
        //   putUint48(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 48-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT48;

        if (verbose) {
            cout << "\nTesting 'putInt48'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt48(1);
            mX.putInt48(2);
            mX.putInt48(3);
            mX.putInt48(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT48_TC "\x00\x00\x00\x00\x00\x01"
                               INT48_TC "\x00\x00\x00\x00\x00\x02"
                               INT48_TC "\x00\x00\x00\x00\x00\x03"
                               INT48_TC "\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt48(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT48_TC "\x00\x00\x00\x00\x00\x01"
                               INT48_TC "\x00\x00\x00\x00\x00\x02"
                               INT48_TC "\x00\x00\x00\x00\x00\x03"
                               INT48_TC "\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt48(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt48' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt48(1);
            mX.putInt48(2);
            mX.makeNextInvalid();
            mX.putInt48(3);
            mX.putInt48(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x00\x01"
                                 INT48_TC "\x00\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x00\x03"
                                 INT48_TC "\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint48'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint48(1);
            mX.putUint48(2);
            mX.putUint48(3);
            mX.putUint48(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT48_TC "\x00\x00\x00\x00\x00\x01"
                               UINT48_TC "\x00\x00\x00\x00\x00\x02"
                               UINT48_TC "\x00\x00\x00\x00\x00\x03"
                               UINT48_TC "\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint48(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT48_TC "\x00\x00\x00\x00\x00\x01"
                               UINT48_TC "\x00\x00\x00\x00\x00\x02"
                               UINT48_TC "\x00\x00\x00\x00\x00\x03"
                               UINT48_TC "\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint48(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint48' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint48(1);
            mX.putUint48(2);
            mX.makeNextInvalid();
            mX.putUint48(3);
            mX.putUint48(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x00\x01"
                                UINT48_TC "\x00\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x00\x03"
                                UINT48_TC "\x00\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt40(bsls::Types::Int64 value);
        //   putUint40(bsls::Types::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 40-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT40;

        if (verbose) {
            cout << "\nTesting 'putInt40'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt40(1);
            mX.putInt40(2);
            mX.putInt40(3);
            mX.putInt40(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT40_TC "\x00\x00\x00\x00\x01"
                               INT40_TC "\x00\x00\x00\x00\x02"
                               INT40_TC "\x00\x00\x00\x00\x03"
                               INT40_TC "\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt40(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT40_TC "\x00\x00\x00\x00\x01"
                               INT40_TC "\x00\x00\x00\x00\x02"
                               INT40_TC "\x00\x00\x00\x00\x03"
                               INT40_TC "\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt40(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt40' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt40(1);
            mX.putInt40(2);
            mX.makeNextInvalid();
            mX.putInt40(3);
            mX.putInt40(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x01"
                                 INT40_TC "\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x03"
                                 INT40_TC "\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint40'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint40(1);
            mX.putUint40(2);
            mX.putUint40(3);
            mX.putUint40(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT40_TC "\x00\x00\x00\x00\x01"
                               UINT40_TC "\x00\x00\x00\x00\x02"
                               UINT40_TC "\x00\x00\x00\x00\x03"
                               UINT40_TC "\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint40(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT40_TC "\x00\x00\x00\x00\x01"
                               UINT40_TC "\x00\x00\x00\x00\x02"
                               UINT40_TC "\x00\x00\x00\x00\x03"
                               UINT40_TC "\x00\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint40(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint40' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint40(1);
            mX.putUint40(2);
            mX.makeNextInvalid();
            mX.putUint40(3);
            mX.putUint40(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x00\x01"
                                UINT40_TC "\x00\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x00\x03"
                                UINT40_TC "\x00\x00\x00\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt32(int value);
        //   putUint32(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 32-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT32;

        if (verbose) {
            cout << "\nTesting 'putInt32'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt32(1);
            mX.putInt32(2);
            mX.putInt32(3);
            mX.putInt32(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT32_TC "\x00\x00\x00\x01"
                               INT32_TC "\x00\x00\x00\x02"
                               INT32_TC "\x00\x00\x00\x03"
                               INT32_TC "\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt32(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT32_TC "\x00\x00\x00\x01"
                               INT32_TC "\x00\x00\x00\x02"
                               INT32_TC "\x00\x00\x00\x03"
                               INT32_TC "\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt32(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt32' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt32(1);
            mX.putInt32(2);
            mX.makeNextInvalid();
            mX.putInt32(3);
            mX.putInt32(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x01"
                                 INT32_TC "\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x03"
                                 INT32_TC "\x00\x00\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint32'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint32(1);
            mX.putUint32(2);
            mX.putUint32(3);
            mX.putUint32(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT32_TC "\x00\x00\x00\x01"
                               UINT32_TC "\x00\x00\x00\x02"
                               UINT32_TC "\x00\x00\x00\x03"
                               UINT32_TC "\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint32(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT32_TC "\x00\x00\x00\x01"
                               UINT32_TC "\x00\x00\x00\x02"
                               UINT32_TC "\x00\x00\x00\x03"
                               UINT32_TC "\x00\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint32(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint32' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint32(1);
            mX.putUint32(2);
            mX.makeNextInvalid();
            mX.putUint32(3);
            mX.putUint32(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x00\x01"
                                UINT32_TC "\x00\x00\x00\x02"
                               INVALID_TC "\x00\x00\x00\x03"
                                UINT32_TC "\x00\x00\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt24(int value);
        //   putUint24(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 24-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT24;

        if (verbose) {
            cout << "\nTesting 'putInt24'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt24(1);
            mX.putInt24(2);
            mX.putInt24(3);
            mX.putInt24(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT24_TC "\x00\x00\x01"
                               INT24_TC "\x00\x00\x02"
                               INT24_TC "\x00\x00\x03"
                               INT24_TC "\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt24(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT24_TC "\x00\x00\x01"
                               INT24_TC "\x00\x00\x02"
                               INT24_TC "\x00\x00\x03"
                               INT24_TC "\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt24(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt24' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt24(1);
            mX.putInt24(2);
            mX.makeNextInvalid();
            mX.putInt24(3);
            mX.putInt24(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x01"
                                 INT24_TC "\x00\x00\x02"
                               INVALID_TC "\x00\x00\x03"
                                 INT24_TC "\x00\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint24'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint24(1);
            mX.putUint24(2);
            mX.putUint24(3);
            mX.putUint24(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT24_TC "\x00\x00\x01"
                               UINT24_TC "\x00\x00\x02"
                               UINT24_TC "\x00\x00\x03"
                               UINT24_TC "\x00\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint24(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT24_TC "\x00\x00\x01"
                               UINT24_TC "\x00\x00\x02"
                               UINT24_TC "\x00\x00\x03"
                               UINT24_TC "\x00\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint24(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint24' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint24(1);
            mX.putUint24(2);
            mX.makeNextInvalid();
            mX.putUint24(3);
            mX.putUint24(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x00\x01"
                                UINT24_TC "\x00\x00\x02"
                               INVALID_TC "\x00\x00\x03"
                                UINT24_TC "\x00\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGERS TEST
        //   Verify the methods externalize the expected bytes.
        //
        // Concerns:
        //: 1 The methods externalize the expected bytes.
        //:
        //: 2 The externalization position does not affect the output.
        //:
        //: 3 'makeNextInvalid' works correctly with these methods.
        //
        // Plan:
        //: 1 Externalize values at different offsets and verify the bytes.
        //:   (C-1..2)
        //:
        //: 2 Verify results when using 'makeNextInvalid' before invoking these
        //:   methods.  (C-3)
        //
        // Testing:
        //   putInt16(int value);
        //   putUint16(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PUT 16-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        const int SIZE = SIZEOF_INT16;

        if (verbose) {
            cout << "\nTesting 'putInt16'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putInt16(1);
            mX.putInt16(2);
            mX.putInt16(3);
            mX.putInt16(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT16_TC "\x00\x01"
                               INT16_TC "\x00\x02"
                               INT16_TC "\x00\x03"
                               INT16_TC "\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt16(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT16_TC "\x00\x01"
                               INT16_TC "\x00\x02"
                               INT16_TC "\x00\x03"
                               INT16_TC "\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt16(1));
        }

        if (verbose) {
            cout << "\nTesting 'putInt16' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putInt16(1);
            mX.putInt16(2);
            mX.makeNextInvalid();
            mX.putInt16(3);
            mX.putInt16(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x01"
                                 INT16_TC "\x00\x02"
                               INVALID_TC "\x00\x03"
                                 INT16_TC "\x00\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'putUint16'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            mX.putUint16(1);
            mX.putUint16(2);
            mX.putUint16(3);
            mX.putUint16(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT16_TC "\x00\x01"
                               UINT16_TC "\x00\x02"
                               UINT16_TC "\x00\x03"
                               UINT16_TC "\x00\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint16(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT16_TC "\x00\x01"
                               UINT16_TC "\x00\x02"
                               UINT16_TC "\x00\x03"
                               UINT16_TC "\x00\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint16(1));
        }

        if (verbose) {
            cout << "\nTesting 'putUint16' w/ 'makeNextInvalid'." << endl;
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.makeNextInvalid();
            mX.putUint16(1);
            mX.putUint16(2);
            mX.makeNextInvalid();
            mX.putUint16(3);
            mX.putUint16(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x00\x01"
                                UINT16_TC "\x00\x02"
                               INVALID_TC "\x00\x03"
                                UINT16_TC "\x00\x04",
                               NUM_BYTES));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT OPERATOR TEST
        //   Verify the method produces the expected output format.
        //
        // Concerns:
        //: 1 The method produces expected output format.
        //
        // Plan:
        //: 1 For a small set of objects, use 'ostringstream' to write the
        //:   object's value to a string buffer and then compare to expected
        //:   output format.  (C-1)
        //
        // Testing:
        //   ostream& operator<<(ostream& stream, const TestOutStream&);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PRINT OPERATOR TEST" << endl
                 << "===================" << endl;
        }

        if (verbose) {
            cout << "\nTesting print operator." << endl;
        }

        const int   SIZE = 1000;  // Must be big enough to hold output string.
        const char  XX = static_cast<char>(0xFF);  // Value that represents an
                                                   // unset char.
        char        ctrl[SIZE];    memset(ctrl, XX, SIZE);
        const char *CTRL = ctrl;

        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

            const char *EXPECTED = "";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt8(0);  mX.putInt8(1);  mX.putInt8(2);  mX.putInt8(3);
            mX.putInt8(4);  mX.putInt8(5);  mX.putInt8(6);  mX.putInt8(7);
            mX.putInt8(8);  mX.putInt8(9);  mX.putInt8(10); mX.putInt8(11);

            const char *EXPECTED =
                "\n0000\t" INT8_STR " 00 " INT8_STR " 01"
                       " " INT8_STR " 02 " INT8_STR " 03"
                "\n0008\t" INT8_STR " 04 " INT8_STR " 05"
                       " " INT8_STR " 06 " INT8_STR " 07"
                "\n0010\t" INT8_STR " 08 " INT8_STR " 09"
                       " " INT8_STR " 0a " INT8_STR " 0b";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            mX.putInt8(  0);  mX.putInt8( 1);  mX.putInt8( 2);  mX.putInt8( 3);
            mX.putInt8(  4);  mX.putInt8( 5);  mX.putInt8( 6);  mX.putInt8( 7);
            mX.putInt8(  8);  mX.putInt8( 9);  mX.putInt8(10);  mX.putInt8(11);
            mX.putInt8(127);  mX.putInt8(-1);  mX.putInt8(-2);  mX.putInt8(-3);

            const char *EXPECTED =
                "\n0000\t" INT8_STR " 00 " INT8_STR " 01"
                       " " INT8_STR " 02 " INT8_STR " 03"
                "\n0008\t" INT8_STR " 04 " INT8_STR " 05"
                       " " INT8_STR " 06 " INT8_STR " 07"
                "\n0010\t" INT8_STR " 08 " INT8_STR " 09"
                       " " INT8_STR " 0a " INT8_STR " 0b"
                "\n0018\t" INT8_STR " 7f " INT8_STR " ff"
                       " " INT8_STR " fe " INT8_STR " fd";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
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
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            ASSERT( X && X.isValid());

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
        //: 1 'length' and 'data' methods return correct values.
        //:
        //: 2 'bdexVersionSelector' returns correct value.
        //
        // Plan:
        //: 1 Create an empty object, use 'putInt8' to modify state, and
        //:   verify the expected values for the methods.  (C-1)
        //:
        //: 2 Create empty objects with different 'versionSelector' constructor
        //:   values and verify the 'bdexVersionSelector' method's return
        //:   value.  (C-2)
        //
        // Testing:
        //   int bdexVersionSelector() const;
        //   const char *data() const;
        //   bsl::size_t length() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "BASIC ACCESSORS TEST" << endl
                 << "====================" << endl;
        }

        if (verbose) {
            cout << "\nTesting 'length' and 'data'." << endl;
        }

        const char *DATA[] = {
            "",
            INT8_TC "\x00",
            INT8_TC "\x00" INT8_TC "\x01",
            INT8_TC "\x00" INT8_TC "\x01" INT8_TC "\x02",
            INT8_TC "\x00" INT8_TC "\x01" INT8_TC "\x02" INT8_TC "\x03"
        };
        const int   NUM_TEST = static_cast<int>(sizeof DATA / sizeof *DATA);
        for (int iLen = 0; iLen < NUM_TEST; iLen++) {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            for (int j = 0; j < iLen; j++) {
                mX.putInt8(j);
            }

            if (veryVerbose) { P_(iLen); P(X); }
            const bsl::size_t bytes = iLen * (SIZEOF_CODE + SIZEOF_INT8);
            // verify length()
            LOOP_ASSERT(iLen, X.length() == bytes);
            // verify data()
            LOOP_ASSERT(iLen, 0 == memcmp(X.data(), DATA[iLen], bytes));
        }

        if (verbose) cout << "\nTesting bdexVersionSelector()." << endl;

        for (int i = 0; i < 5; ++i) {
            Obj x(VERSION_SELECTOR + i);
            LOOP_ASSERT(i, VERSION_SELECTOR + i == x.bdexVersionSelector());
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
        //: 2 The initial capacity constructor allocates if needed.
        //:
        //: 3 'reserveCapacity' allocates if needed.
        //:
        //: 4 'makeNextInvalid' works correctly with the primary manipulators.
        //:
        //: 5 'reset' validates and removes all data from the object.
        //:
        //: 6 The destructor functions properly.
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
        //: 5 Verify results when using 'makeNextInvalid' before invoking the
        //:   primary manipulators.  (C-4)
        //:
        //: 6 Execute 'reset' on objects with varying length and verify
        //:   the object is emptied.  Also execute 'reset' on invalid objects.
        //:   (C-5)
        //:
        //: 7 Verify the functionality of the destructor using test allocators.
        //:   (C-6)
        //
        // Testing:
        //   TestOutStream(int sV, *ba = 0);
        //   TestOutStream(int sV, bsl::size_t initialCapacity, *ba = 0);
        //   ~TestOutStream();
        //   makeNextInvalid();
        //   putInt8(int value);
        //   putUint8(unsigned int value);
        //   reset();
        //   reserveCapacity(bsl::size_t newCapacity);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PRIMARY MANIPULATORS TEST" << endl
                 << "=========================" << endl;
        }

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting putInt8, reserveCapacity, and "
                          << "constructor w/o allocator." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX(VERSION_SELECTOR, 100);  const Obj& X = mX;
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            mX.putInt8(1);
            mX.putInt8(2);
            mX.putInt8(3);
            mX.putInt8(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC "\x01"
                               INT8_TC "\x02"
                               INT8_TC "\x03"
                               INT8_TC "\x04",
                               NUM_BYTES));
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt8(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC "\x01"
                               INT8_TC "\x02"
                               INT8_TC "\x03"
                               INT8_TC "\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt8(1));
        }
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

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
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC "\x05"
                               INT8_TC "\x06"
                               INT8_TC "\x07"
                               INT8_TC "\x08",
                               NUM_BYTES));
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putInt8, reserveCapacity, and "
                          << "constructor w/ allocator." << endl;
        {
            bsls::Types::Int64 allocations = ta.numAllocations();

            Obj mX(VERSION_SELECTOR, 100, &ta);  const Obj& X = mX;
            ASSERT(0 == X.length());
            ASSERT(allocations + 1 == ta.numAllocations());
            mX.putInt8(1);
            mX.putInt8(2);
            mX.putInt8(3);
            mX.putInt8(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC "\x01"
                               INT8_TC "\x02"
                               INT8_TC "\x03"
                               INT8_TC "\x04",
                               NUM_BYTES));
            ASSERT(allocations + 1 == ta.numAllocations());

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putInt8(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC "\x01"
                               INT8_TC "\x02"
                               INT8_TC "\x03"
                               INT8_TC "\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putInt8(1));
        }
        {
            Obj mX(VERSION_SELECTOR, &ta);  const Obj& X = mX;

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
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INT8_TC "\x05"
                               INT8_TC "\x06"
                               INT8_TC "\x07"
                               INT8_TC "\x08",
                               NUM_BYTES));
            ASSERT(allocations + 1 == ta.numAllocations());
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'putInt8' w/ 'makeNextInvalid'." << endl;
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            ASSERT(0 == X.length());
            mX.makeNextInvalid();
            mX.putInt8(1);
            mX.putInt8(2);
            mX.makeNextInvalid();
            mX.putInt8(3);
            mX.putInt8(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x01"
                                  INT8_TC "\x02"
                               INVALID_TC "\x03"
                                  INT8_TC "\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint8." << endl;
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            ASSERT(0 == X.length());
            mX.putUint8(1);
            mX.putUint8(2);
            mX.putUint8(3);
            mX.putUint8(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT8_TC "\x01"
                               UINT8_TC "\x02"
                               UINT8_TC "\x03"
                               UINT8_TC "\x04",
                               NUM_BYTES));

            // Verify method has no effect if the stream is invalid.
            mX.invalidate();
            mX.putUint8(1);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               UINT8_TC "\x01"
                               UINT8_TC "\x02"
                               UINT8_TC "\x03"
                               UINT8_TC "\x04",
                               NUM_BYTES));

            // Verify the return value.
            mX.reset();
            ASSERT(&mX == &mX.putUint8(1));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'putUint8' w/ 'makeNextInvalid'." << endl;
        {
            Obj mX(VERSION_SELECTOR);  const Obj& X = mX;
            ASSERT(0 == X.length());
            mX.makeNextInvalid();
            mX.putUint8(1);
            mX.putUint8(2);
            mX.makeNextInvalid();
            mX.putUint8(3);
            mX.putUint8(4);
            if (veryVerbose) { P(X); }
            const bsl::size_t NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            ASSERT(NUM_BYTES == X.length());
            ASSERT(0 == memcmp(X.data(),
                               INVALID_TC "\x01"
                                 UINT8_TC "\x02"
                               INVALID_TC "\x03"
                                 UINT8_TC "\x04",
                               NUM_BYTES));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting reset()." << endl;
        {
            const int NUM_TEST = 5;
            for (int iLen = 1; iLen < NUM_TEST; iLen++) {
                Obj mX(VERSION_SELECTOR);  const Obj& X = mX;

                for (int j = 0; j < iLen; j++) {
                    mX.putInt8(j);
                }
                mX.reset();
                if (veryVerbose) { P_(iLen); P(X); }
                LOOP_ASSERT(iLen, 0 == X.length());

                for (int j = 0; j < iLen; j++) {
                    mX.putInt8(j);
                }
                mX.invalidate();
                LOOP_ASSERT(iLen, false == X.isValid());
                mX.reset();
                if (veryVerbose) { P_(iLen); P(X); }
                LOOP_ASSERT(iLen, X.isValid());
                LOOP_ASSERT(iLen, 0 == X.length());
            }
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
        //: 1 Create 'TestOutStream' objects using the default constructor.
        //:
        //: 2 Exercise these objects using various methods.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        if (verbose) {
            cout << "\nCreate object x1 using default ctor." << endl;
        }
        Obj x1(VERSION_SELECTOR);
        ASSERT(0 == x1.length());

        if (verbose) {
            cout << "\nTry putInt32 with x1." << endl;
        }
        x1.putInt32(1);
        if (veryVerbose) { P(x1); }
        ASSERT(SIZEOF_CODE + SIZEOF_INT32 == x1.length());
        ASSERT(0 == memcmp(INT32_TC "\x00\x00\x00\x01",
                           x1.data(),
                           SIZEOF_INT32));

        if (verbose) {
            cout << "\nClear x1 using reset method." << endl;
        }
        x1.reset();
        ASSERT(0 == x1.length());

        if (verbose) {
            cout << "\nTry putArrayInt8 with x1." << endl;
        }
        const char DATA[] = {0x01, 0x02, 0x03, 0x04};
        const int  SIZE = static_cast<int>(sizeof DATA / sizeof *DATA);
        x1.putArrayInt8(DATA, SIZE);
        if (veryVerbose) { P(x1); }
        const bsl::size_t NUM_BYTES =
                              SIZEOF_CODE + SIZEOF_ARRLEN + SIZEOF_INT8 * SIZE;
        ASSERT(NUM_BYTES == x1.length());
        ASSERT(0 == memcmp(INT8_TC "\x00\x00\x00\x04" "\x01\x02\x03\x04",
                           x1.data(),
                           NUM_BYTES));
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
