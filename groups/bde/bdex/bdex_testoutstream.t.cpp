// bdex_testoutstream.t.cpp             -*-C++-*-

#include <bdex_testoutstream.h>

#include <bsls_platformutil.h>                  // for testing only

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy(), memcmp(), strlen()

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
//                              Overview
//                              --------
// We are testing a "test" implementation of the 'bdex_OutStream' protocol.
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
//-----------------------------------------------------------------------------
// [ 2] bdex_TestOutStream();
// [ 2] ~bdex_TestOutStream();
// [24] putLength(int length);
// [24] putVersion(int version);
// [11] putInt64(bsls_PlatformUtil::Int64 value);
// [11] putUint64(bsls_PlatformUtil::Int64 value);
// [10] putInt56(bsls_PlatformUtil::Int64 value);
// [10] putUint56(bsls_PlatformUtil::Int64 value);
// [ 9] putInt48(bsls_PlatformUtil::Int64 value);
// [ 9] putUint48(bsls_PlatformUtil::Int64 value);
// [ 8] putInt40(bsls_PlatformUtil::Int64 value);
// [ 8] putUint40(bsls_PlatformUtil::Int64 value);
// [ 7] putInt32(int value);
// [ 7] putUint32(int value);
// [ 6] putInt24(int value);
// [ 6] putUint24(int value);
// [ 5] putInt16(int value);
// [ 5] putUint16(int value);
// [ 2] putInt8(int value);
// [ 2] putUint8(int value);
// [13] putFloat64(double value);
// [12] putFloat32(float value);
// [21] putArrayInt64(const bsls_PlatformUtil::Int64 *array, int count);
// [21] putArrayUint64(const bsls_PlatformUtil::Uint64 *array, int count);
// [20] putArrayInt56(const bsls_PlatformUtil::Int64 *array, int count);
// [20] putArrayUint56(const bsls_PlatformUtil::Uint64 *array, int count);
// [19] putArrayInt48(const bsls_PlatformUtil::Int64 *array, int count);
// [19] putArrayUint48(const bsls_PlatformUtil::Uint64 *array, int count);
// [18] putArrayInt40(const bsls_PlatformUtil::Int64 *array, int count);
// [18] putArrayUint40(const bsls_PlatformUtil::Uint64 *array, int count);
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
// [23] putArrayFloat64(const double *array,int count);
// [22] putArrayFloat32(const float *array, int count);
// [ 2] void removeAll();
// [ 3] const char* data();
// [ 3] int length();
//
// [ 4] ostream& operator<<(ostream& stream, const bdex_TestOutStream&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// Type codes from bdex_FieldCode as bytes in string representation
#define INT8_FL    "\xe0"
#define UINT8_FL   "\xe1"
#define INT16_FL   "\xe2"
#define UINT16_FL  "\xe3"
#define INT24_FL   "\xe4"
#define UINT24_FL  "\xe5"
#define INT32_FL   "\xe6"
#define UINT32_FL  "\xe7"
#define INT40_FL   "\xe8"
#define UINT40_FL  "\xe9"
#define INT48_FL   "\xea"
#define UINT48_FL  "\xeb"
#define INT56_FL   "\xec"
#define UINT56_FL  "\xed"
#define INT64_FL   "\xee"
#define UINT64_FL  "\xef"
#define FLOAT32_FL "\xf0"
#define FLOAT64_FL "\xf1"
#define INVALID_FL "\xf2"
// bdex_FieldCode::INT8 as bits in string representation
#define INT8_BITS  "11100000"
// length 0 to 3 as bytes in string representation
#define LEN_0      "\x00\x00\x00\x00"
#define LEN_1      "\x00\x00\x00\x01"
#define LEN_2      "\x00\x00\x00\x02"
#define LEN_3      "\x00\x00\x00\x03"

typedef bdex_TestOutStream Obj;

// size in bytes of each fundamental type
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

//=============================================================================
//                      SUPPLEMENTARY TEST FUNCTIONALITY
//-----------------------------------------------------------------------------

static int eq(const char *lhs, const char *rhs, int numBits)
    // Return 1 if the specified leading 'numBits' are the same
    // for the specified 'lhs' and 'rhs' byte arrays, and 0 otherwise.
    // Note that it is the most significant bits in a partial byte that
    // are compared.
{
    ASSERT(0 <= numBits);
    int wholeBytes = numBits / 8;
    int extraBits = numBits % 8;

    for (int i = 0; i < wholeBytes; ++i) {
        if (lhs[i] ^ rhs[i]) {
            return 0;   // different
        }
    }

    if (extraBits) {
        int diff = lhs[wholeBytes] ^ rhs[wholeBytes];
        int remaingBits = 8 - extraBits;
        int mask = 0xff >> remaingBits << remaingBits;
        diff &= mask;   // if 0 != diff they're not equal
        return 0 == diff;
    }

    return 1;   // same
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 24: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGERS TEST:
        //
        // Testing:
        //   putLength(int length);
        //   putVersion(int version);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT LENGTH AND VERSION TEST" << endl
                          << "===========================" << endl;

        {
            if (verbose) cout << "\nTesting 'putLength'." << endl;
            {
                Obj x;

                x.putLength(1);
                x.putLength(128);
                x.putLength(3);
                x.putLength(256);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 4 * SIZEOF_CODE + 2 * SIZEOF_INT32 +
                                                               2 * SIZEOF_INT8;
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(),
                               INT8_FL "\x01"
                               INT32_FL "\x80\x00\x00\x80"
                               INT8_FL "\x03"
                               INT32_FL "\x80\x00\x01\x00", NUM_BITS));
            }
            if (verbose)
              cout << "\nTesting 'putLength' w/ 'makeNextInvalid'." << endl;
            {
                Obj x;
                x.makeNextInvalid();
                x.putLength(1);
                x.putLength(128);
                x.makeNextInvalid();
                x.putLength(3);
                x.putLength(256);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_INT32);
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(),
                               INVALID_FL "\x00\x00\x00\x01"
                               INT32_FL "\x80\x00\x00\x80"
                               INVALID_FL "\x00\x00\x00\x03"
                               INT32_FL "\x80\x00\x01\x00", NUM_BITS));
            }
        }

        {
            const int SIZE = SIZEOF_INT8;

            if (verbose) cout << "\nTesting 'putVersion'." << endl;
            {
                Obj x;
                ASSERT(0 == x.length());

                x.putVersion(1);
                x.putVersion(2);
                x.putVersion(3);
                x.putVersion(4);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(),
                               UINT8_FL "\x01"
                               UINT8_FL "\x02"
                               UINT8_FL "\x03"
                               UINT8_FL "\x04", NUM_BITS));
            }
            if (verbose)
                cout << "\nTesting 'putVersion' w/ 'makeNextInvalid'." << endl;
            {
                Obj x;
                ASSERT(0 == x.length());
                x.makeNextInvalid();
                x.putVersion(1);
                x.putVersion(2);
                x.makeNextInvalid();
                x.putVersion(3);
                x.putVersion(4);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(),
                               INVALID_FL "\x01"
                               UINT8_FL "\x02"
                               INVALID_FL "\x03"
                               UINT8_FL "\x04", NUM_BITS));
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOAT ARRAYS TEST:
        //
        // Testing:
        //   putArrayFloat64(const double *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT FLOAT ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) cout << "\nTesting 'putArrayFloat64'." << endl;
        {
            const double DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayFloat64(DATA, 0);
            x.putArrayFloat64(DATA, 1);
            x.putArrayFloat64(DATA, 2);
            x.putArrayFloat64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT64_FL LEN_0 ""
                           FLOAT64_FL LEN_1 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL LEN_2 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL LEN_3 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                                            "\x40\x08\x00\x00\x00\x00\x00\x00",
                           NUM_BITS));
        }
        {
            const double DATA[] = {1.5, 2.5, 3.5};
            Obj x;
            x.putArrayFloat64(DATA, 0);
            x.putArrayFloat64(DATA, 1);
            x.putArrayFloat64(DATA, 2);
            x.putArrayFloat64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT64_FL LEN_0 ""
                           FLOAT64_FL LEN_1 "\x3f\xf8\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL LEN_2 "\x3f\xf8\x00\x00\x00\x00\x00\x00"
                                            "\x40\x04\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL LEN_3 "\x3f\xf8\x00\x00\x00\x00\x00\x00"
                                            "\x40\x04\x00\x00\x00\x00\x00\x00"
                                            "\x40\x0c\x00\x00\x00\x00\x00\x00",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayFloat64' w/ 'makeNextInvalid'." <<endl;
        {
            const double DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayFloat64(DATA, 0);
            x.putArrayFloat64(DATA, 1);
            x.makeNextInvalid();
            x.putArrayFloat64(DATA, 2);
            x.putArrayFloat64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INVALID_FL LEN_0 ""
                           FLOAT64_FL LEN_1 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           INVALID_FL LEN_2 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL LEN_3 "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                                            "\x40\x00\x00\x00\x00\x00\x00\x00"
                                            "\x40\x08\x00\x00\x00\x00\x00\x00",
                           NUM_BITS));
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOAT ARRAYS TEST:
        //
        // Testing:
        //   putArrayFloat32(const float *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT FLOAT ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_FLOAT32;

        if (verbose) cout << "\nTesting 'putArrayFloat32'." << endl;
        {
            const float DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayFloat32(DATA, 0);
            x.putArrayFloat32(DATA, 1);
            x.putArrayFloat32(DATA, 2);
            x.putArrayFloat32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT32_FL LEN_0 ""
                           FLOAT32_FL LEN_1 "\x3f\x80\x00\x00"
                           FLOAT32_FL LEN_2 "\x3f\x80\x00\x00"
                                            "\x40\x00\x00\x00"
                           FLOAT32_FL LEN_3 "\x3f\x80\x00\x00"
                                            "\x40\x00\x00\x00"
                                            "\x40\x40\x00\x00", NUM_BITS));
        }
        {
            const float DATA[] = {1.5, 2.5, 3.5};
            Obj x;
            x.putArrayFloat32(DATA, 0);
            x.putArrayFloat32(DATA, 1);
            x.putArrayFloat32(DATA, 2);
            x.putArrayFloat32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT32_FL LEN_0 ""
                           FLOAT32_FL LEN_1 "\x3f\xc0\x00\x00"
                           FLOAT32_FL LEN_2 "\x3f\xc0\x00\x00"
                                            "\x40\x20\x00\x00"
                           FLOAT32_FL LEN_3 "\x3f\xc0\x00\x00"
                                            "\x40\x20\x00\x00"
                                            "\x40\x60\x00\x00", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayFloat32' w/ 'makeNextInvalid'." <<endl;
        {
            const float DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayFloat32(DATA, 0);
            x.putArrayFloat32(DATA, 1);
            x.makeNextInvalid();
            x.putArrayFloat32(DATA, 2);
            x.putArrayFloat32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INVALID_FL LEN_0 ""
                           FLOAT32_FL LEN_1 "\x3f\x80\x00\x00"
                           INVALID_FL LEN_2 "\x3f\x80\x00\x00"
                                            "\x40\x00\x00\x00"
                           FLOAT32_FL LEN_3 "\x3f\x80\x00\x00"
                                            "\x40\x00\x00\x00"
                                            "\x40\x40\x00\x00", NUM_BITS));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt64(const int *array, int count);
        //   putArrayUint64(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT64;

        if (verbose) cout << "\nTesting 'putArrayInt64'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt64(DATA, 0);
            x.putArrayInt64(DATA, 1);
            x.putArrayInt64(DATA, 2);
            x.putArrayInt64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT64_FL LEN_0 ""
                           INT64_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                           INT64_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x00\x02"
                           INT64_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt64(DATA, 0);
            x.putArrayInt64(DATA, 1);
            x.putArrayInt64(DATA, 2);
            x.putArrayInt64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT64_FL LEN_0 ""
                           INT64_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x04"
                           INT64_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x00\x00\x00\x05"
                           INT64_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x00\x00\x00\x05"
                                          "\x00\x00\x00\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt64' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt64(DATA, 0);
            x.putArrayInt64(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt64(DATA, 2);
            x.putArrayInt64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL LEN_0 ""
                           INT64_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                         INVALID_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x00\x02"
                           INT64_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint64'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint64(DATA, 0);
            x.putArrayUint64(DATA, 1);
            x.putArrayUint64(DATA, 2);
            x.putArrayUint64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT64_FL LEN_0 ""
                           UINT64_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                           UINT64_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x00\x02"
                           UINT64_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint64(DATA, 0);
            x.putArrayUint64(DATA, 1);
            x.putArrayUint64(DATA, 2);
            x.putArrayUint64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT64_FL LEN_0 ""
                           UINT64_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x04"
                           UINT64_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x00\x00\x00\x05"
                           UINT64_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x00\x00\x00\x05"
                                           "\x00\x00\x00\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint64' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint64(DATA, 0);
            x.putArrayUint64(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint64(DATA, 2);
            x.putArrayUint64(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL LEN_0 ""
                           UINT64_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x00\x01"
                          INVALID_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x00\x02"
                           UINT64_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt56(const int *array, int count);
        //   putArrayUint56(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 56-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT56;

        if (verbose) cout << "\nTesting 'putArrayInt56'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt56(DATA, 0);
            x.putArrayInt56(DATA, 1);
            x.putArrayInt56(DATA, 2);
            x.putArrayInt56(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT56_FL LEN_0 ""
                           INT56_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                           INT56_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x02"
                           INT56_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt56(DATA, 0);
            x.putArrayInt56(DATA, 1);
            x.putArrayInt56(DATA, 2);
            x.putArrayInt56(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT56_FL LEN_0 ""
                           INT56_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x04"
                           INT56_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x00\x00\x05"
                           INT56_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x00\x00\x05"
                                          "\x00\x00\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt56' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt56(DATA, 0);
            x.putArrayInt56(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt56(DATA, 2);
            x.putArrayInt56(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL LEN_0 ""
                           INT56_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                         INVALID_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x02"
                           INT56_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint56'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint56(DATA, 0);
            x.putArrayUint56(DATA, 1);
            x.putArrayUint56(DATA, 2);
            x.putArrayUint56(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT56_FL LEN_0 ""
                           UINT56_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                           UINT56_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x02"
                           UINT56_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint56(DATA, 0);
            x.putArrayUint56(DATA, 1);
            x.putArrayUint56(DATA, 2);
            x.putArrayUint56(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT56_FL LEN_0 ""
                           UINT56_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x04"
                           UINT56_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x00\x00\x05"
                           UINT56_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x00\x00\x05"
                                           "\x00\x00\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint56' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint56(DATA, 0);
            x.putArrayUint56(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint56(DATA, 2);
            x.putArrayUint56(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL LEN_0 ""
                           UINT56_FL LEN_1 "\x00\x00\x00\x00\x00\x00\x01"
                          INVALID_FL LEN_2 "\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x02"
                           UINT56_FL LEN_3 "\x00\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt48(const int *array, int count);
        //   putArrayUint48(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 48-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT48;

        if (verbose) cout << "\nTesting 'putArrayInt48'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt48(DATA, 0);
            x.putArrayInt48(DATA, 1);
            x.putArrayInt48(DATA, 2);
            x.putArrayInt48(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT48_FL LEN_0 ""
                           INT48_FL LEN_1 "\x00\x00\x00\x00\x00\x01"
                           INT48_FL LEN_2 "\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x02"
                           INT48_FL LEN_3 "\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt48(DATA, 0);
            x.putArrayInt48(DATA, 1);
            x.putArrayInt48(DATA, 2);
            x.putArrayInt48(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT48_FL LEN_0 ""
                           INT48_FL LEN_1 "\x00\x00\x00\x00\x00\x04"
                           INT48_FL LEN_2 "\x00\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x00\x05"
                           INT48_FL LEN_3 "\x00\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x00\x05"
                                          "\x00\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt48' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt48(DATA, 0);
            x.putArrayInt48(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt48(DATA, 2);
            x.putArrayInt48(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL LEN_0 ""
                           INT48_FL LEN_1 "\x00\x00\x00\x00\x00\x01"
                         INVALID_FL LEN_2 "\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x02"
                           INT48_FL LEN_3 "\x00\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint48'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint48(DATA, 0);
            x.putArrayUint48(DATA, 1);
            x.putArrayUint48(DATA, 2);
            x.putArrayUint48(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT48_FL LEN_0 ""
                           UINT48_FL LEN_1 "\x00\x00\x00\x00\x00\x01"
                           UINT48_FL LEN_2 "\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x02"
                           UINT48_FL LEN_3 "\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint48(DATA, 0);
            x.putArrayUint48(DATA, 1);
            x.putArrayUint48(DATA, 2);
            x.putArrayUint48(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT48_FL LEN_0 ""
                           UINT48_FL LEN_1 "\x00\x00\x00\x00\x00\x04"
                           UINT48_FL LEN_2 "\x00\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x00\x05"
                           UINT48_FL LEN_3 "\x00\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x00\x05"
                                           "\x00\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint48' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint48(DATA, 0);
            x.putArrayUint48(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint48(DATA, 2);
            x.putArrayUint48(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL LEN_0 ""
                           UINT48_FL LEN_1 "\x00\x00\x00\x00\x00\x01"
                          INVALID_FL LEN_2 "\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x02"
                           UINT48_FL LEN_3 "\x00\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt40(const int *array, int count);
        //   putArrayUint40(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 40-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT40;

        if (verbose) cout << "\nTesting 'putArrayInt40'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt40(DATA, 0);
            x.putArrayInt40(DATA, 1);
            x.putArrayInt40(DATA, 2);
            x.putArrayInt40(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT40_FL LEN_0 ""
                           INT40_FL LEN_1 "\x00\x00\x00\x00\x01"
                           INT40_FL LEN_2 "\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x02"
                           INT40_FL LEN_3 "\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt40(DATA, 0);
            x.putArrayInt40(DATA, 1);
            x.putArrayInt40(DATA, 2);
            x.putArrayInt40(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT40_FL LEN_0 ""
                           INT40_FL LEN_1 "\x00\x00\x00\x00\x04"
                           INT40_FL LEN_2 "\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x05"
                           INT40_FL LEN_3 "\x00\x00\x00\x00\x04"
                                          "\x00\x00\x00\x00\x05"
                                          "\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt40' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt40(DATA, 0);
            x.putArrayInt40(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt40(DATA, 2);
            x.putArrayInt40(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL LEN_0 ""
                           INT40_FL LEN_1 "\x00\x00\x00\x00\x01"
                         INVALID_FL LEN_2 "\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x02"
                           INT40_FL LEN_3 "\x00\x00\x00\x00\x01"
                                          "\x00\x00\x00\x00\x02"
                                          "\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint40'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint40(DATA, 0);
            x.putArrayUint40(DATA, 1);
            x.putArrayUint40(DATA, 2);
            x.putArrayUint40(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT40_FL LEN_0 ""
                           UINT40_FL LEN_1 "\x00\x00\x00\x00\x01"
                           UINT40_FL LEN_2 "\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x02"
                           UINT40_FL LEN_3 "\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint40(DATA, 0);
            x.putArrayUint40(DATA, 1);
            x.putArrayUint40(DATA, 2);
            x.putArrayUint40(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT40_FL LEN_0 ""
                           UINT40_FL LEN_1 "\x00\x00\x00\x00\x04"
                           UINT40_FL LEN_2 "\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x05"
                           UINT40_FL LEN_3 "\x00\x00\x00\x00\x04"
                                           "\x00\x00\x00\x00\x05"
                                           "\x00\x00\x00\x00\x06",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint40' w/ 'makeNextInvalid'." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint40(DATA, 0);
            x.putArrayUint40(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint40(DATA, 2);
            x.putArrayUint40(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL LEN_0 ""
                           UINT40_FL LEN_1 "\x00\x00\x00\x00\x01"
                          INVALID_FL LEN_2 "\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x02"
                           UINT40_FL LEN_3 "\x00\x00\x00\x00\x01"
                                           "\x00\x00\x00\x00\x02"
                                           "\x00\x00\x00\x00\x03",
                           NUM_BITS));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt32(const int *array, int count);
        //   putArrayUint32(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT32;

        if (verbose) cout << "\nTesting 'putArrayInt32'." << endl;
        {
            const int DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt32(DATA, 0);
            x.putArrayInt32(DATA, 1);
            x.putArrayInt32(DATA, 2);
            x.putArrayInt32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT32_FL LEN_0 ""
                           INT32_FL LEN_1 "\x00\x00\x00\x01"
                           INT32_FL LEN_2 "\x00\x00\x00\x01"
                                          "\x00\x00\x00\x02"
                           INT32_FL LEN_3 "\x00\x00\x00\x01"
                                          "\x00\x00\x00\x02"
                                          "\x00\x00\x00\x03", NUM_BITS));
        }
        {
            const int DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt32(DATA, 0);
            x.putArrayInt32(DATA, 1);
            x.putArrayInt32(DATA, 2);
            x.putArrayInt32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT32_FL LEN_0 ""
                           INT32_FL LEN_1 "\x00\x00\x00\x04"
                           INT32_FL LEN_2 "\x00\x00\x00\x04"
                                          "\x00\x00\x00\x05"
                           INT32_FL LEN_3 "\x00\x00\x00\x04"
                                          "\x00\x00\x00\x05"
                                          "\x00\x00\x00\x06", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt32' w/ 'makeNextInvalid'." << endl;
        {
            const int DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt32(DATA, 0);
            x.putArrayInt32(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt32(DATA, 2);
            x.putArrayInt32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL LEN_0 ""
                           INT32_FL LEN_1 "\x00\x00\x00\x01"
                         INVALID_FL LEN_2 "\x00\x00\x00\x01"
                                          "\x00\x00\x00\x02"
                           INT32_FL LEN_3 "\x00\x00\x00\x01"
                                          "\x00\x00\x00\x02"
                                          "\x00\x00\x00\x03", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint32'." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint32(DATA, 0);
            x.putArrayUint32(DATA, 1);
            x.putArrayUint32(DATA, 2);
            x.putArrayUint32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT32_FL LEN_0 ""
                           UINT32_FL LEN_1 "\x00\x00\x00\x01"
                           UINT32_FL LEN_2 "\x00\x00\x00\x01"
                                           "\x00\x00\x00\x02"
                           UINT32_FL LEN_3 "\x00\x00\x00\x01"
                                           "\x00\x00\x00\x02"
                                           "\x00\x00\x00\x03", NUM_BITS));
        }
        {
            const unsigned int DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint32(DATA, 0);
            x.putArrayUint32(DATA, 1);
            x.putArrayUint32(DATA, 2);
            x.putArrayUint32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT32_FL LEN_0 ""
                           UINT32_FL LEN_1 "\x00\x00\x00\x04"
                           UINT32_FL LEN_2 "\x00\x00\x00\x04"
                                           "\x00\x00\x00\x05"
                           UINT32_FL LEN_3 "\x00\x00\x00\x04"
                                           "\x00\x00\x00\x05"
                                           "\x00\x00\x00\x06", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint32' w/ 'makeNextInvalid'." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint32(DATA, 0);
            x.putArrayUint32(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint32(DATA, 2);
            x.putArrayUint32(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL LEN_0 ""
                           UINT32_FL LEN_1 "\x00\x00\x00\x01"
                          INVALID_FL LEN_2 "\x00\x00\x00\x01"
                                           "\x00\x00\x00\x02"
                           UINT32_FL LEN_3 "\x00\x00\x00\x01"
                                           "\x00\x00\x00\x02"
                                           "\x00\x00\x00\x03", NUM_BITS));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt24(const int *array, int count);
        //   putArrayUint24(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 24-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT24;

        if (verbose) cout << "\nTesting 'putArrayInt24'." << endl;
        {
            const int DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt24(DATA, 0);
            x.putArrayInt24(DATA, 1);
            x.putArrayInt24(DATA, 2);
            x.putArrayInt24(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT24_FL LEN_0 ""
                           INT24_FL LEN_1 "\x00\x00\x01"
                           INT24_FL LEN_2 "\x00\x00\x01"
                                          "\x00\x00\x02"
                           INT24_FL LEN_3 "\x00\x00\x01"
                                          "\x00\x00\x02"
                                          "\x00\x00\x03", NUM_BITS));
        }
        {
            const int DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt24(DATA, 0);
            x.putArrayInt24(DATA, 1);
            x.putArrayInt24(DATA, 2);
            x.putArrayInt24(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT24_FL LEN_0 ""
                           INT24_FL LEN_1 "\x00\x00\x04"
                           INT24_FL LEN_2 "\x00\x00\x04"
                                          "\x00\x00\x05"
                           INT24_FL LEN_3 "\x00\x00\x04"
                                          "\x00\x00\x05"
                                          "\x00\x00\x06", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt24' w/ 'makeNextInvalid'." << endl;
        {
            const int DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt24(DATA, 0);
            x.putArrayInt24(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt24(DATA, 2);
            x.putArrayInt24(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL LEN_0 ""
                           INT24_FL LEN_1 "\x00\x00\x01"
                         INVALID_FL LEN_2 "\x00\x00\x01"
                                          "\x00\x00\x02"
                           INT24_FL LEN_3 "\x00\x00\x01"
                                          "\x00\x00\x02"
                                          "\x00\x00\x03", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint24'." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint24(DATA, 0);
            x.putArrayUint24(DATA, 1);
            x.putArrayUint24(DATA, 2);
            x.putArrayUint24(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT24_FL LEN_0 ""
                           UINT24_FL LEN_1 "\x00\x00\x01"
                           UINT24_FL LEN_2 "\x00\x00\x01"
                                           "\x00\x00\x02"
                           UINT24_FL LEN_3 "\x00\x00\x01"
                                           "\x00\x00\x02"
                                           "\x00\x00\x03", NUM_BITS));
        }
        {
            const unsigned int DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint24(DATA, 0);
            x.putArrayUint24(DATA, 1);
            x.putArrayUint24(DATA, 2);
            x.putArrayUint24(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT24_FL LEN_0 ""
                           UINT24_FL LEN_1 "\x00\x00\x04"
                           UINT24_FL LEN_2 "\x00\x00\x04"
                                           "\x00\x00\x05"
                           UINT24_FL LEN_3 "\x00\x00\x04"
                                           "\x00\x00\x05"
                                           "\x00\x00\x06", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint24' w/ 'makeNextInvalid'." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint24(DATA, 0);
            x.putArrayUint24(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint24(DATA, 2);
            x.putArrayUint24(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL LEN_0 ""
                           UINT24_FL LEN_1 "\x00\x00\x01"
                          INVALID_FL LEN_2 "\x00\x00\x01"
                                           "\x00\x00\x02"
                           UINT24_FL LEN_3 "\x00\x00\x01"
                                           "\x00\x00\x02"
                                           "\x00\x00\x03", NUM_BITS));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   putArrayInt16(const int *array, int count);
        //   putArrayUint16(const unsigned int *array, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 16-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        const int SIZE = SIZEOF_INT16;

        if (verbose) cout << "\nTesting 'putArrayInt16'." << endl;
        {
            const short DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt16(DATA, 0);
            x.putArrayInt16(DATA, 1);
            x.putArrayInt16(DATA, 2);
            x.putArrayInt16(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   INT16_FL LEN_0 ""
                   INT16_FL LEN_1 "\x00\x01"
                   INT16_FL LEN_2 "\x00\x01" "\x00\x02"
                   INT16_FL LEN_3 "\x00\x01" "\x00\x02" "\x00\x03", NUM_BITS));
        }
        {
            const short DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt16(DATA, 0);
            x.putArrayInt16(DATA, 1);
            x.putArrayInt16(DATA, 2);
            x.putArrayInt16(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   INT16_FL LEN_0 ""
                   INT16_FL LEN_1 "\x00\x04"
                   INT16_FL LEN_2 "\x00\x04" "\x00\x05"
                   INT16_FL LEN_3 "\x00\x04" "\x00\x05" "\x00\x06", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayInt16' w/ 'makeNextInvalid'." << endl;
        {
            const short DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt16(DATA, 0);
            x.putArrayInt16(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt16(DATA, 2);
            x.putArrayInt16(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                 INVALID_FL LEN_0 ""
                   INT16_FL LEN_1 "\x00\x01"
                 INVALID_FL LEN_2 "\x00\x01" "\x00\x02"
                   INT16_FL LEN_3 "\x00\x01" "\x00\x02" "\x00\x03", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint16'." << endl;
        {
            const unsigned short DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint16(DATA, 0);
            x.putArrayUint16(DATA, 1);
            x.putArrayUint16(DATA, 2);
            x.putArrayUint16(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                  UINT16_FL LEN_0 ""
                  UINT16_FL LEN_1 "\x00\x01"
                  UINT16_FL LEN_2 "\x00\x01" "\x00\x02"
                  UINT16_FL LEN_3 "\x00\x01" "\x00\x02" "\x00\x03", NUM_BITS));
        }
        {
            const unsigned short DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint16(DATA, 0);
            x.putArrayUint16(DATA, 1);
            x.putArrayUint16(DATA, 2);
            x.putArrayUint16(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                  UINT16_FL LEN_0 ""
                  UINT16_FL LEN_1 "\x00\x04"
                  UINT16_FL LEN_2 "\x00\x04" "\x00\x05"
                  UINT16_FL LEN_3 "\x00\x04" "\x00\x05" "\x00\x06", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putArrayUint16' w/ 'makeNextInvalid'." << endl;
        {
            const unsigned short DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint16(DATA, 0);
            x.putArrayUint16(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint16(DATA, 2);
            x.putArrayUint16(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                 INVALID_FL LEN_0 ""
                  UINT16_FL LEN_1 "\x00\x01"
                 INVALID_FL LEN_2 "\x00\x01" "\x00\x02"
                  UINT16_FL LEN_3 "\x00\x01" "\x00\x02" "\x00\x03", NUM_BITS));
        }
       } break;
      case 14: {
        // --------------------------------------------------------------------
        // PUT 8-BIT INTEGER ARRAYS TEST:
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

        if (verbose) cout << "\nTesting 'putArrayInt8(char*)'." << endl;
        {
            const char DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt8(DATA, 0);
            x.putArrayInt8(DATA, 1);
            x.putArrayInt8(DATA, 2);
            x.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   INT8_FL LEN_0 ""
                   INT8_FL LEN_1 "\x01"
                   INT8_FL LEN_2 "\x01" "\x02"
                   INT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }
        {
            const char DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt8(DATA, 0);
            x.putArrayInt8(DATA, 1);
            x.putArrayInt8(DATA, 2);
            x.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   INT8_FL LEN_0 ""
                   INT8_FL LEN_1 "\x04"
                   INT8_FL LEN_2 "\x04" "\x05"
                   INT8_FL LEN_3 "\x04" "\x05" "\x06", NUM_BITS));
        }
        if (verbose) cout << "\nTesting 'putArrayInt8(char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        {
            const char DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt8(DATA, 0);
            x.putArrayInt8(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt8(DATA, 2);
            x.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                INVALID_FL LEN_0 ""
                   INT8_FL LEN_1 "\x01"
                INVALID_FL LEN_2 "\x01" "\x02"
                   INT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayInt8(signed char*)'." << endl;
        {
            const signed char DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayInt8(DATA, 0);
            x.putArrayInt8(DATA, 1);
            x.putArrayInt8(DATA, 2);
            x.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   INT8_FL LEN_0 ""
                   INT8_FL LEN_1 "\x01"
                   INT8_FL LEN_2 "\x01" "\x02"
                   INT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }
        {
            const signed char DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayInt8(DATA, 0);
            x.putArrayInt8(DATA, 1);
            x.putArrayInt8(DATA, 2);
            x.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   INT8_FL LEN_0 ""
                   INT8_FL LEN_1 "\x04"
                   INT8_FL LEN_2 "\x04" "\x05"
                   INT8_FL LEN_3 "\x04" "\x05" "\x06", NUM_BITS));
        }
        if (verbose) cout << "\nTesting 'putArrayInt8(signed char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        {
            const signed char DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayInt8(DATA, 0);
            x.putArrayInt8(DATA, 1);
            x.makeNextInvalid();
            x.putArrayInt8(DATA, 2);
            x.putArrayInt8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                INVALID_FL LEN_0 ""
                   INT8_FL LEN_1 "\x01"
                INVALID_FL LEN_2 "\x01" "\x02"
                   INT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint8(char*)'." << endl;
        {
            const char DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint8(DATA, 0);
            x.putArrayUint8(DATA, 1);
            x.putArrayUint8(DATA, 2);
            x.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   UINT8_FL LEN_0 ""
                   UINT8_FL LEN_1 "\x01"
                   UINT8_FL LEN_2 "\x01" "\x02"
                   UINT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }
        {
            const char DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint8(DATA, 0);
            x.putArrayUint8(DATA, 1);
            x.putArrayUint8(DATA, 2);
            x.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   UINT8_FL LEN_0 ""
                   UINT8_FL LEN_1 "\x04"
                   UINT8_FL LEN_2 "\x04" "\x05"
                   UINT8_FL LEN_3 "\x04" "\x05" "\x06", NUM_BITS));
        }
        if (verbose) cout << "\nTesting 'putArrayUint8(char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        {
            const char DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint8(DATA, 0);
            x.putArrayUint8(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint8(DATA, 2);
            x.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                 INVALID_FL LEN_0 ""
                   UINT8_FL LEN_1 "\x01"
                 INVALID_FL LEN_2 "\x01" "\x02"
                   UINT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putArrayUint8(unsigned char*)'."
                          << endl;
        {
            const unsigned char DATA[] = {1, 2, 3};
            Obj x;
            x.putArrayUint8(DATA, 0);
            x.putArrayUint8(DATA, 1);
            x.putArrayUint8(DATA, 2);
            x.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   UINT8_FL LEN_0 ""
                   UINT8_FL LEN_1 "\x01"
                   UINT8_FL LEN_2 "\x01" "\x02"
                   UINT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }
        {
            const unsigned char DATA[] = {4, 5, 6};
            Obj x;
            x.putArrayUint8(DATA, 0);
            x.putArrayUint8(DATA, 1);
            x.putArrayUint8(DATA, 2);
            x.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   UINT8_FL LEN_0 ""
                   UINT8_FL LEN_1 "\x04"
                   UINT8_FL LEN_2 "\x04" "\x05"
                   UINT8_FL LEN_3 "\x04" "\x05" "\x06", NUM_BITS));
        }
        if (verbose) cout << "\nTesting 'putArrayUint8(unsigned char*)' w/ "
                          << "'makeNextInvalid'." << endl;
        {
            const unsigned char DATA[] = {1, 2, 3};
            Obj x;
            x.makeNextInvalid();
            x.putArrayUint8(DATA, 0);
            x.putArrayUint8(DATA, 1);
            x.makeNextInvalid();
            x.putArrayUint8(DATA, 2);
            x.putArrayUint8(DATA, 3);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZEOF_ARRLEN) + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                 INVALID_FL LEN_0 ""
                   UINT8_FL LEN_1 "\x01"
                 INVALID_FL LEN_2 "\x01" "\x02"
                   UINT8_FL LEN_3 "\x01" "\x02" "\x03", NUM_BITS));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PUT 64-BIT FLOATS TEST:
        //
        // Testing:
        //   putFloat64(float value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT FLOATS TEST" << endl
                          << "=======================" << endl;

        const int SIZE = SIZEOF_FLOAT64;

        if (verbose) cout << "\nTesting 'putFloat64'." << endl;
        {
            Obj x;

            x.putFloat64(1);
            x.putFloat64(2);
            x.putFloat64(3);
            x.putFloat64(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT64_FL "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x00\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x08\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x10\x00\x00\x00\x00\x00\x00",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putFloat64(1.5);
            x.putFloat64(2.5);
            x.putFloat64(3.5);
            x.putFloat64(5.0);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT64_FL "\x3f\xf8\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x04\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x0c\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x14\x00\x00\x00\x00\x00\x00",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putFloat64' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putFloat64(1);
            x.putFloat64(2);
            x.makeNextInvalid();
            x.putFloat64(3);
            x.putFloat64(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INVALID_FL "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x00\x00\x00\x00\x00\x00\x00"
                           INVALID_FL "\x40\x08\x00\x00\x00\x00\x00\x00"
                           FLOAT64_FL "\x40\x10\x00\x00\x00\x00\x00\x00",
                           NUM_BITS));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PUT 32-BIT FLOATS TEST:
        //
        // Testing:
        //   putFloat32(float value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT FLOATS TEST" << endl
                          << "=======================" << endl;

        const int SIZE = SIZEOF_FLOAT32;

        if (verbose) cout << "\nTesting 'putFloat32'." << endl;
        {
            Obj x;

            x.putFloat32(1);
            x.putFloat32(2);
            x.putFloat32(3);
            x.putFloat32(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT32_FL "\x3f\x80\x00\x00"
                           FLOAT32_FL "\x40\x00\x00\x00"
                           FLOAT32_FL "\x40\x40\x00\x00"
                           FLOAT32_FL "\x40\x80\x00\x00", NUM_BITS));
        }
        {
            Obj x;

            x.putFloat32(1.5);
            x.putFloat32(2.5);
            x.putFloat32(3.5);
            x.putFloat32(5.0);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           FLOAT32_FL "\x3f\xc0\x00\x00"
                           FLOAT32_FL "\x40\x20\x00\x00"
                           FLOAT32_FL "\x40\x60\x00\x00"
                           FLOAT32_FL "\x40\xa0\x00\x00", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putFloat32' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putFloat32(1);
            x.putFloat32(2);
            x.makeNextInvalid();
            x.putFloat32(3);
            x.putFloat32(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INVALID_FL "\x3f\x80\x00\x00"
                           FLOAT32_FL "\x40\x00\x00\x00"
                           INVALID_FL "\x40\x40\x00\x00"
                           FLOAT32_FL "\x40\x80\x00\x00", NUM_BITS));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PUT 64-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt64(bsls_PlatformUtil::Int64 value);
        //   putUint64(bsls_PlatformUtil::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 64-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT64;

        if (verbose) cout << "\nTesting 'putInt64'." << endl;
        {
            Obj x;

            x.putInt64(1);
            x.putInt64(2);
            x.putInt64(3);
            x.putInt64(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x01"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x02"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x03"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putInt64(5);
            x.putInt64(6);
            x.putInt64(7);
            x.putInt64(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x05"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x06"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x07"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt64' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt64(1);
            x.putInt64(2);
            x.makeNextInvalid();
            x.putInt64(3);
            x.putInt64(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x00\x00\x00\x00\x00\x00\x01"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x02"
                         INVALID_FL "\x00\x00\x00\x00\x00\x00\x00\x03"
                           INT64_FL "\x00\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint64'." << endl;
        {
            Obj x;

            x.putUint64(1);
            x.putUint64(2);
            x.putUint64(3);
            x.putUint64(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x01"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x02"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x03"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putUint64(5);
            x.putUint64(6);
            x.putUint64(7);
            x.putUint64(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x05"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x06"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x07"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint64' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint64(1);
            x.putUint64(2);
            x.makeNextInvalid();
            x.putUint64(3);
            x.putUint64(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x00\x00\x00\x00\x00\x00\x01"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x02"
                          INVALID_FL "\x00\x00\x00\x00\x00\x00\x00\x03"
                           UINT64_FL "\x00\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PUT 56-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt56(bsls_PlatformUtil::Int64 value);
        //   putUint56(bsls_PlatformUtil::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 56-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT56;

        if (verbose) cout << "\nTesting 'putInt56'." << endl;
        {
            Obj x;

            x.putInt56(1);
            x.putInt56(2);
            x.putInt56(3);
            x.putInt56(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x01"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x02"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x03"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putInt56(5);
            x.putInt56(6);
            x.putInt56(7);
            x.putInt56(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x05"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x06"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x07"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt56' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt56(1);
            x.putInt56(2);
            x.makeNextInvalid();
            x.putInt56(3);
            x.putInt56(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x00\x00\x00\x00\x00\x01"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x02"
                         INVALID_FL "\x00\x00\x00\x00\x00\x00\x03"
                           INT56_FL "\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint56'." << endl;
        {
            Obj x;

            x.putUint56(1);
            x.putUint56(2);
            x.putUint56(3);
            x.putUint56(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x01"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x02"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x03"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putUint56(5);
            x.putUint56(6);
            x.putUint56(7);
            x.putUint56(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x05"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x06"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x07"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint56' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint56(1);
            x.putUint56(2);
            x.makeNextInvalid();
            x.putUint56(3);
            x.putUint56(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x00\x00\x00\x00\x00\x01"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x02"
                          INVALID_FL "\x00\x00\x00\x00\x00\x00\x03"
                           UINT56_FL "\x00\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PUT 48-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt48(bsls_PlatformUtil::Int64 value);
        //   putUint48(bsls_PlatformUtil::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 48-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT48;

        if (verbose) cout << "\nTesting 'putInt48'." << endl;
        {
            Obj x;

            x.putInt48(1);
            x.putInt48(2);
            x.putInt48(3);
            x.putInt48(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT48_FL "\x00\x00\x00\x00\x00\x01"
                           INT48_FL "\x00\x00\x00\x00\x00\x02"
                           INT48_FL "\x00\x00\x00\x00\x00\x03"
                           INT48_FL "\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putInt48(5);
            x.putInt48(6);
            x.putInt48(7);
            x.putInt48(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT48_FL "\x00\x00\x00\x00\x00\x05"
                           INT48_FL "\x00\x00\x00\x00\x00\x06"
                           INT48_FL "\x00\x00\x00\x00\x00\x07"
                           INT48_FL "\x00\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt48' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt48(1);
            x.putInt48(2);
            x.makeNextInvalid();
            x.putInt48(3);
            x.putInt48(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x00\x00\x00\x00\x01"
                           INT48_FL "\x00\x00\x00\x00\x00\x02"
                         INVALID_FL "\x00\x00\x00\x00\x00\x03"
                           INT48_FL "\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint48'." << endl;
        {
            Obj x;

            x.putUint48(1);
            x.putUint48(2);
            x.putUint48(3);
            x.putUint48(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT48_FL "\x00\x00\x00\x00\x00\x01"
                           UINT48_FL "\x00\x00\x00\x00\x00\x02"
                           UINT48_FL "\x00\x00\x00\x00\x00\x03"
                           UINT48_FL "\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putUint48(5);
            x.putUint48(6);
            x.putUint48(7);
            x.putUint48(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT48_FL "\x00\x00\x00\x00\x00\x05"
                           UINT48_FL "\x00\x00\x00\x00\x00\x06"
                           UINT48_FL "\x00\x00\x00\x00\x00\x07"
                           UINT48_FL "\x00\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint48' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint48(1);
            x.putUint48(2);
            x.makeNextInvalid();
            x.putUint48(3);
            x.putUint48(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x00\x00\x00\x00\x01"
                           UINT48_FL "\x00\x00\x00\x00\x00\x02"
                          INVALID_FL "\x00\x00\x00\x00\x00\x03"
                           UINT48_FL "\x00\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT 40-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt40(bsls_PlatformUtil::Int64 value);
        //   putUint40(bsls_PlatformUtil::Uint64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 40-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT40;

        if (verbose) cout << "\nTesting 'putInt40'." << endl;
        {
            Obj x;

            x.putInt40(1);
            x.putInt40(2);
            x.putInt40(3);
            x.putInt40(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT40_FL "\x00\x00\x00\x00\x01"
                           INT40_FL "\x00\x00\x00\x00\x02"
                           INT40_FL "\x00\x00\x00\x00\x03"
                           INT40_FL "\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putInt40(5);
            x.putInt40(6);
            x.putInt40(7);
            x.putInt40(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT40_FL "\x00\x00\x00\x00\x05"
                           INT40_FL "\x00\x00\x00\x00\x06"
                           INT40_FL "\x00\x00\x00\x00\x07"
                           INT40_FL "\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt40' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt40(1);
            x.putInt40(2);
            x.makeNextInvalid();
            x.putInt40(3);
            x.putInt40(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x00\x00\x00\x01"
                           INT40_FL "\x00\x00\x00\x00\x02"
                         INVALID_FL "\x00\x00\x00\x00\x03"
                           INT40_FL "\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint40'." << endl;
        {
            Obj x;

            x.putUint40(1);
            x.putUint40(2);
            x.putUint40(3);
            x.putUint40(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT40_FL "\x00\x00\x00\x00\x01"
                           UINT40_FL "\x00\x00\x00\x00\x02"
                           UINT40_FL "\x00\x00\x00\x00\x03"
                           UINT40_FL "\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
        {
            Obj x;

            x.putUint40(5);
            x.putUint40(6);
            x.putUint40(7);
            x.putUint40(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT40_FL "\x00\x00\x00\x00\x05"
                           UINT40_FL "\x00\x00\x00\x00\x06"
                           UINT40_FL "\x00\x00\x00\x00\x07"
                           UINT40_FL "\x00\x00\x00\x00\x08",
                           NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint40' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint40(1);
            x.putUint40(2);
            x.makeNextInvalid();
            x.putUint40(3);
            x.putUint40(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x00\x00\x00\x01"
                           UINT40_FL "\x00\x00\x00\x00\x02"
                          INVALID_FL "\x00\x00\x00\x00\x03"
                           UINT40_FL "\x00\x00\x00\x00\x04",
                           NUM_BITS));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PUT 32-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt32(int value);
        //   putUint32(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 32-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT32;

        if (verbose) cout << "\nTesting 'putInt32'." << endl;
        {
            Obj x;

            x.putInt32(1);
            x.putInt32(2);
            x.putInt32(3);
            x.putInt32(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT32_FL "\x00\x00\x00\x01"
                           INT32_FL "\x00\x00\x00\x02"
                           INT32_FL "\x00\x00\x00\x03"
                           INT32_FL "\x00\x00\x00\x04", NUM_BITS));
        }
        {
            Obj x;

            x.putInt32(5);
            x.putInt32(6);
            x.putInt32(7);
            x.putInt32(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT32_FL "\x00\x00\x00\x05"
                           INT32_FL "\x00\x00\x00\x06"
                           INT32_FL "\x00\x00\x00\x07"
                           INT32_FL "\x00\x00\x00\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt32' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt32(1);
            x.putInt32(2);
            x.makeNextInvalid();
            x.putInt32(3);
            x.putInt32(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x00\x00\x01"
                           INT32_FL "\x00\x00\x00\x02"
                         INVALID_FL "\x00\x00\x00\x03"
                           INT32_FL "\x00\x00\x00\x04", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint32'." << endl;
        {
            Obj x;

            x.putUint32(1);
            x.putUint32(2);
            x.putUint32(3);
            x.putUint32(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT32_FL "\x00\x00\x00\x01"
                           UINT32_FL "\x00\x00\x00\x02"
                           UINT32_FL "\x00\x00\x00\x03"
                           UINT32_FL "\x00\x00\x00\x04", NUM_BITS));
        }
        {
            Obj x;

            x.putUint32(5);
            x.putUint32(6);
            x.putUint32(7);
            x.putUint32(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT32_FL "\x00\x00\x00\x05"
                           UINT32_FL "\x00\x00\x00\x06"
                           UINT32_FL "\x00\x00\x00\x07"
                           UINT32_FL "\x00\x00\x00\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint32' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint32(1);
            x.putUint32(2);
            x.makeNextInvalid();
            x.putUint32(3);
            x.putUint32(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x00\x00\x01"
                           UINT32_FL "\x00\x00\x00\x02"
                          INVALID_FL "\x00\x00\x00\x03"
                           UINT32_FL "\x00\x00\x00\x04", NUM_BITS));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PUT 24-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt24(int value);
        //   putUint24(unsigned int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 24-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT24;

        if (verbose) cout << "\nTesting 'putInt24'." << endl;
        {
            Obj x;

            x.putInt24(1);
            x.putInt24(2);
            x.putInt24(3);
            x.putInt24(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT24_FL "\x00\x00\x01"
                           INT24_FL "\x00\x00\x02"
                           INT24_FL "\x00\x00\x03"
                           INT24_FL "\x00\x00\x04", NUM_BITS));
        }
        {
            Obj x;

            x.putInt24(5);
            x.putInt24(6);
            x.putInt24(7);
            x.putInt24(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT24_FL "\x00\x00\x05"
                           INT24_FL "\x00\x00\x06"
                           INT24_FL "\x00\x00\x07"
                           INT24_FL "\x00\x00\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt24' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt24(1);
            x.putInt24(2);
            x.makeNextInvalid();
            x.putInt24(3);
            x.putInt24(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x00\x01"
                           INT24_FL "\x00\x00\x02"
                         INVALID_FL "\x00\x00\x03"
                           INT24_FL "\x00\x00\x04", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint24'." << endl;
        {
            Obj x;

            x.putUint24(1);
            x.putUint24(2);
            x.putUint24(3);
            x.putUint24(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT24_FL "\x00\x00\x01"
                           UINT24_FL "\x00\x00\x02"
                           UINT24_FL "\x00\x00\x03"
                           UINT24_FL "\x00\x00\x04", NUM_BITS));
        }
        {
            Obj x;

            x.putUint24(5);
            x.putUint24(6);
            x.putUint24(7);
            x.putUint24(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT24_FL "\x00\x00\x05"
                           UINT24_FL "\x00\x00\x06"
                           UINT24_FL "\x00\x00\x07"
                           UINT24_FL "\x00\x00\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint24' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint24(1);
            x.putUint24(2);
            x.makeNextInvalid();
            x.putUint24(3);
            x.putUint24(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x00\x01"
                           UINT24_FL "\x00\x00\x02"
                          INVALID_FL "\x00\x00\x03"
                           UINT24_FL "\x00\x00\x04", NUM_BITS));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PUT 16-BIT INTEGERS TEST:
        //
        // Testing:
        //   putInt16(short value);
        //   putUint16(unsigned short value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT 16-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        const int SIZE = SIZEOF_INT16;

        if (verbose) cout << "\nTesting 'putInt16'." << endl;
        {
            Obj x;

            x.putInt16(1);
            x.putInt16(2);
            x.putInt16(3);
            x.putInt16(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT16_FL "\x00\x01"
                           INT16_FL "\x00\x02"
                           INT16_FL "\x00\x03"
                           INT16_FL "\x00\x04", NUM_BITS));
        }
        {
            Obj x;

            x.putInt16(5);
            x.putInt16(6);
            x.putInt16(7);
            x.putInt16(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT16_FL "\x00\x05"
                           INT16_FL "\x00\x06"
                           INT16_FL "\x00\x07"
                           INT16_FL "\x00\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt16' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putInt16(1);
            x.putInt16(2);
            x.makeNextInvalid();
            x.putInt16(3);
            x.putInt16(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x00\x01"
                           INT16_FL "\x00\x02"
                         INVALID_FL "\x00\x03"
                           INT16_FL "\x00\x04", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint16'." << endl;
        {
            Obj x;

            x.putUint16(1);
            x.putUint16(2);
            x.putUint16(3);
            x.putUint16(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT16_FL "\x00\x01"
                           UINT16_FL "\x00\x02"
                           UINT16_FL "\x00\x03"
                           UINT16_FL "\x00\x04", NUM_BITS));
        }
        {
            Obj x;

            x.putUint16(5);
            x.putUint16(6);
            x.putUint16(7);
            x.putUint16(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT16_FL "\x00\x05"
                           UINT16_FL "\x00\x06"
                           UINT16_FL "\x00\x07"
                           UINT16_FL "\x00\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint16' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            x.makeNextInvalid();
            x.putUint16(1);
            x.putUint16(2);
            x.makeNextInvalid();
            x.putUint16(3);
            x.putUint16(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                          INVALID_FL "\x00\x01"
                           UINT16_FL "\x00\x02"
                          INVALID_FL "\x00\x03"
                           UINT16_FL "\x00\x04", NUM_BITS));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT OPERATOR TEST:
        //   For each of a small representative set of objects, use
        //   'ostrstream' to write that object's value to a string buffer and
        //   then compare this buffer with the expected output format.
        //
        // Testing:
        //   ostream& operator<<(ostream&, const bdex_TestOutStream&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT OPERATOR TEST" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting print operator." << endl;

        const int SIZE = 1000;     // Must be big enough to hold output string.
        const char XX = (char) 0xFF; // Value that represents an unset char.
        char ctrl[SIZE];    memset(ctrl, XX, SIZE);
        const char *CTRL = ctrl;

        {
            Obj x;
            const char *EXPECTED = "";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << x << ends;
            const int LEN = strlen(EXPECTED) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj x;  x.putInt8(0);  x.putInt8(1);  x.putInt8(2);  x.putInt8(3);
            const char *EXPECTED =
                "\n0000\t" INT8_BITS " 00000000 " INT8_BITS " 00000001"
                       " " INT8_BITS " 00000010 " INT8_BITS " 00000011";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << x << ends;
            const int LEN = strlen(EXPECTED) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Obj x;
            x.putInt8(0);  x.putInt8(1);  x.putInt8(2);  x.putInt8(3);
            x.putInt8(4);  x.putInt8(5);  x.putInt8(6);  x.putInt8(7);
            x.putInt8(8);  x.putInt8(9);  x.putInt8(10); x.putInt8(11);
            const char *EXPECTED =
                "\n0000\t" INT8_BITS " 00000000 " INT8_BITS " 00000001"
                       " " INT8_BITS " 00000010 " INT8_BITS " 00000011"
                "\n0008\t" INT8_BITS " 00000100 " INT8_BITS " 00000101"
                       " " INT8_BITS " 00000110 " INT8_BITS " 00000111"
                "\n0010\t" INT8_BITS " 00001000 " INT8_BITS " 00001001"
                       " " INT8_BITS " 00001010 " INT8_BITS " 00001011";
            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);
            ostrstream out(buf, SIZE);    out << x << ends;
            const int LEN = strlen(EXPECTED) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST:
        //   For each independent test, use the default ctor to create an empty
        //   object.  Use function 'putInt8' to change the object's state, then
        //   verify that each of the basic accessors returns the correct value.
        //
        // Testing:
        //   length();
        //   data();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS TEST" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting 'length' and 'data'." << endl;

        const char *DATA[] = {
            "",
            INT8_FL "\x00",
            INT8_FL "\x00" INT8_FL "\x01",
            INT8_FL "\x00" INT8_FL "\x01" INT8_FL "\x02",
            INT8_FL "\x00" INT8_FL "\x01" INT8_FL "\x02" INT8_FL "\x03"
        };
        const int NUM_TEST = sizeof DATA / sizeof *DATA;
        for (int iLen = 0; iLen < NUM_TEST; iLen++) {
            Obj x;
            for (int j = 0; j < iLen; j++) x.putInt8(j);

            if (veryVerbose) { P_(iLen); P(x); }
            const int bytes = iLen * (SIZEOF_CODE + SIZEOF_INT8);
            // verify length()
            LOOP_ASSERT(iLen, x.length() == bytes);
            // verify data()
            LOOP_ASSERT(iLen,
                        1 == eq(x.data(),
                                DATA[iLen], bytes * 8));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST:
        //    For each independent test, use the default ctor to create an
        //    empty object, then use the primary manipulator function 'putInt8'
        //    to set its state.  Verify the correctness of this function using
        //    the basic accessors 'length' and 'data'.  Note that the
        //    destructor is exercised on each configuration as the object being
        //    tested leaves scope.
        //
        // Testing:
        //   bdex_TestOutStream();
        //   ~bdex_TestOutStream();  // via purify
        //   putInt8(int value);
        //   putUint8(int value);
        //   removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting 'putInt8'." << endl;
        {
            Obj x;
            ASSERT(0 == x.length());

            x.putInt8(1);
            x.putInt8(2);
            x.putInt8(3);
            x.putInt8(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT8_FL "\x01"
                           INT8_FL "\x02"
                           INT8_FL "\x03"
                           INT8_FL "\x04", NUM_BITS));
        }
        {
            Obj x;
            ASSERT(0 == x.length());

            x.putInt8(5);
            x.putInt8(6);
            x.putInt8(7);
            x.putInt8(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           INT8_FL "\x05"
                           INT8_FL "\x06"
                           INT8_FL "\x07"
                           INT8_FL "\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putInt8' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            ASSERT(0 == x.length());
            x.makeNextInvalid();
            x.putInt8(1);
            x.putInt8(2);
            x.makeNextInvalid();
            x.putInt8(3);
            x.putInt8(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                        INVALID_FL "\x01"
                           INT8_FL "\x02"
                        INVALID_FL "\x03"
                           INT8_FL "\x04", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'putUint8'." << endl;
        {
            Obj x;
            ASSERT(0 == x.length());

            x.putUint8(1);
            x.putUint8(2);
            x.putUint8(3);
            x.putUint8(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT8_FL "\x01"
                           UINT8_FL "\x02"
                           UINT8_FL "\x03"
                           UINT8_FL "\x04", NUM_BITS));
        }
        {
            Obj x;
            ASSERT(0 == x.length());

            x.putUint8(5);
            x.putUint8(6);
            x.putUint8(7);
            x.putUint8(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                           UINT8_FL "\x05"
                           UINT8_FL "\x06"
                           UINT8_FL "\x07"
                           UINT8_FL "\x08", NUM_BITS));
        }
        if (verbose)
            cout << "\nTesting 'putUint8' w/ 'makeNextInvalid'." << endl;
        {
            Obj x;
            ASSERT(0 == x.length());
            x.makeNextInvalid();
            x.putUint8(1);
            x.putUint8(2);
            x.makeNextInvalid();
            x.putUint8(3);
            x.putUint8(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * (SIZEOF_CODE + SIZE);
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                         INVALID_FL "\x01"
                           UINT8_FL "\x02"
                         INVALID_FL "\x03"
                           UINT8_FL "\x04", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            const int NUM_TEST = 5;
            for (int iLen = 1; iLen < NUM_TEST; iLen++) {
                Obj x;
                for (int j = 0; j < iLen; j++) x.putInt8(j);

                x.removeAll();
                if (veryVerbose) { P_(iLen); P(x); }
                LOOP_ASSERT(iLen, 0 == x.length());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdex_TestOutStream' objects using the default constructor,
        //   and exercise these objects using some "put" methods.  Verify the
        //   contents of the resulting objects with basic accessors.  Display
        //   object values frequently in verbose mode.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nCreate object x1 using default ctor." << endl;
        Obj x1;
        ASSERT(0 == x1.length());

        if (verbose) cout << "\nTry putInt32 with x1." << endl;
        x1.putInt32(1);
        if (veryVerbose) { P(x1); }
        ASSERT(SIZEOF_CODE + SIZEOF_INT32 == x1.length());
        ASSERT(1 == eq(INT32_FL "\x00\x00\x00\x01",
                       x1.data(), SIZEOF_INT32 * 8));

        if (verbose) cout << "\nClear x1 using removeAll method." << endl;
        x1.removeAll();
        ASSERT(0 == x1.length());

        if (verbose) cout << "\nTry putArrayInt8 with x1." << endl;
        const char DATA[] = {0x01, 0x02, 0x03, 0x04};
        const int SIZE = sizeof DATA / sizeof *DATA;
        x1.putArrayInt8(DATA, SIZE);
        if (veryVerbose) { P(x1); }
        const int NUM_BYTES = SIZEOF_CODE + SIZEOF_ARRLEN + SIZEOF_INT8 * SIZE;
        ASSERT(NUM_BYTES == x1.length());
        ASSERT(1 == eq(INT8_FL "\x00\x00\x00\x04" "\x01\x02\x03\x04",
                       x1.data(), NUM_BYTES * 8));
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
