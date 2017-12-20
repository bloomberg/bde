// bslx_marshallingutil.t.cpp                                         -*-C++-*-

#include <bslx_marshallingutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a suite of functions, each of which is independent (except
// for those that are inline-forwarded).  We should make sure that we fully
// understand the nature of a 'double' on this platform -- i.e., that it
// conforms to the IEEE standard.  After that, it is a simple matter of
// verifying that each function produces the expected encoding.  Once encoded,
// we should be able to decode it back to the identical value.  Therefore,
// pairing corresponding put/get functions seems to make sense.  In each case,
// endianness will be tested only if we explicitly verify the encoded results.
// Two general routines (one to encode byte strings from a bit string, and one
// to compare byte strings) will be needed.  A table-driven approach can then
// be used throughout.
//
// Note also that since most 64-bit platforms treat 'short' as 16 bits and
// 'int' as 32 bits, we either need to assert this as fact or make sure the
// test case verifies sign- and zero-extensions for get functions appropriately
// (we have opted to do the latter).
// ----------------------------------------------------------------------------
// [ 4] putInt64(char *buf, Int64 val);
// [ 5] putInt56(char *buf, Int64 val);
// [ 6] putInt48(char *buf, Int64 val);
// [ 7] putInt40(char *buf, Int64 val);
// [ 8] putInt32(char *buf, int val);
// [ 9] putInt24(char *buf, int val);
// [10] putInt16(char *buf, int val);
// [11] putInt8(char *buf, int val);
// [12] putFloat64(char *buf, double val);
// [13] putFloat32(char *buf, float val);
//
// [ 4] getInt64(Int64 *var, const char *buf);
// [ 4] getUint64(Uint64 *var, const char *buf);
// [ 5] getInt56(Int64 *var, const char *buf);
// [ 5] getUint56(Uint64 *var, const char *buf);
// [ 6] getInt48(Int64 *var, const char *buf);
// [ 6] getUint48(Uint64 *var, const char *buf);
// [ 7] getInt40(Int64 *var, const char *buf);
// [ 7] getUint40(Uint64 *var, const char *buf);
// [ 8] getInt32(int *var, const char *buf);
// [ 8] getUint32(unsigned int *var, const char *buf);
// [ 9] getInt24(int *var, const char *buf);
// [ 9] getUint24(unsigned int *var, const char *buf);
// [10] getInt16(short *var, const char *buf);
// [10] getUint16(unsigned short *var, const char *buf);
// [11] getInt8(char *var, const char *buf);
// [11] getInt8(signed char *var, const char *buf);
// [11] getInt8(unsigned char *var, const char *buf);
// [12] getFloat64(double *var, const char *buf);
// [13] getFloat32(float *var, const char *buf);
//
// [14] putArrayInt64(char *buf, const Int64 *ary, int count);
// [14] putArrayInt64(char *buf, const Uint64 *ary, int count);
// [15] putArrayInt56(char *buf, const Int64 *ary, int count);
// [15] putArrayInt56(char *buf, const Uint64 *ary, int count);
// [16] putArrayInt48(char *buf, const Int64 *ary, int count);
// [16] putArrayInt48(char *buf, const Uint64 *ary, int count);
// [17] putArrayInt40(char *buf, const Int64 *ary, int count);
// [17] putArrayInt40(char *buf, const Uint64 *ary, int count);
// [18] putArrayInt32(char *buf, const int *ary, int count);
// [18] putArrayInt32(char *buf, const unsigned int *ary, int count);
// [19] putArrayInt24(char *buf, const int *ary, int count);
// [19] putArrayInt24(char *buf, const unsigned int *ary, int count);
// [20] putArrayInt16(char *buf, const short *ary, int count);
// [20] putArrayInt16(char *buf, const unsigned short *ary, int count);
// [21] putArrayInt8(char *buf, const char *ary, int count);
// [21] putArrayInt8(char *buf, const signed char *ary, int count);
// [21] putArrayInt8(char *buf, const unsigned char *ary, int count);
// [22] putArrayFloat64(char *buf, const double *ary, int count);
// [23] putArrayFloat32(char *buf, const float *ary, int count);
//
// [14] getArrayInt64(Int64 *var, const char *buf, int count);
// [14] getArrayUint64(Uint64 *var, const char *buf, int count);
// [15] getArrayInt56(Int64 *var, const char *buf, int count);
// [15] getArrayUint56(Uint64 *var, const char *buf, int count);
// [16] getArrayInt48(Int64 *var, const char *buf, int count);
// [16] getArrayUint48(Uint64 *var, const char *buf, int count);
// [17] getArrayInt40(Int64 *var, const char *buf, int count);
// [17] getArrayUint40(Uint64 *var, const char *buf, int count);
// [18] getArrayInt32(int *var, const char *buf, int count);
// [18] getArrayUint32(unsigned int *var, const char *buf, int count);
// [19] getArrayInt24(int *var, const char *buf, int count);
// [19] getArrayUint24(unsigned int *var, const char *buf, int count);
// [20] getArrayInt16(short *var, const char *buf, int count);
// [20] getArrayUint16(unsigned short *var, const char *buf, int count);
// [21] getArrayInt8(char *var, const char *buf, int count);
// [21] getArrayInt8(signed char *var, const char *buf, int count);
// [21] getArrayInt8(unsigned char *var, const char *buf, int count);
// [22] getArrayFloat64(double *var, const char *buf, int count);
// [23] getArrayFloat32(float *var, const char *buf, int count);
// ----------------------------------------------------------------------------
// [ 1] SWAP FUNCTION: static inline void swap(T *x, T *y)
// [ 1] REVERSE FUNCTION: void reverse(T *array, int numElements)
// [ 2] EXPLORE DOUBLE FORMAT -- make sure format is IEEE-COMPLIANT
// [ 3] EXPLORE FLOAT FORMAT -- make sure format is IEEE-COMPLIANT
// [24] STRESS TEST - Used to determine performance characteristics.
// [25] USAGE EXAMPLE
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
//                      SUPPLEMENTARY TEST FUNCTIONALITY
// ----------------------------------------------------------------------------

static ostream& pBytes(const char *c, int n)
    // Print a sequence of bytes, the specified 'c' containing the specified
    // 'n' bytes, as space-separated 8-bit words in hexadecimal.
{
    bsl::ios_base::fmtflags flags = cout.flags(std::ios::hex);
    for (int i = 0; i < n; ++i) {
        if (i) {
            cout << ' ';
        }
        cout << bsl::setw(2)
             << bsl::setfill('0')
             << static_cast<int>(static_cast<unsigned char>(c[i]));
    }
    cout.flags(flags);
    return cout;
}

template <class TYPE>
inline
void swap(TYPE *a, TYPE *b)
    // Swap the values of the specified objects 'a' and 'b'.  Note that 'TYPE'
    // must have value-semantic operators COPY CTOR and OP=.
{
    TYPE t = *a;
    *a = *b;
    *b = t;
}

template <class TYPE>
inline
void reverse(TYPE *array, int numElements)
    // Reverse the locations of the objects in the specified 'array' containing
    // the specified 'numElements'.
{
    TYPE *top = array + numElements - 1;
    int   middle = numElements/2;  // if odd, middle is reversed already
    for (int i = 0; i < middle; ++i) {
        swap(array + i, top - i);
    }
}

// ============================================================================
//                      FUNCTIONS TO MANIPULATE DOUBLES
// ----------------------------------------------------------------------------
//  sign bit    11-bit exponent             52-bit mantissa
//    /        /                           /
//  +-+-----------+----------------------------------------------------+
//  |s|e10......e0|m51...............................................m0|
//  +-+-----------+----------------------------------------------------+
//  LSB                                                              MSB
// ----------------------------------------------------------------------------

static ostream& printDoubleBits(ostream& stream, double number)
    // Format the bits in the specified 'number' to the specified 'stream' from
    // least- to most-significant (space-separated) byte, printing
    // most-significant bits in each byte first.  Return a reference to the
    // modifiable 'stream'.
{
    const int SIZE = static_cast<int>(sizeof number);
    ASSERT(8 <= SIZE);

    double         tmp = number;
    unsigned char *bytes = reinterpret_cast<unsigned char *>(&tmp);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    swap(bytes + SIZE - 1, bytes + 0);
    swap(bytes + SIZE - 2, bytes + 1);
    swap(bytes + SIZE - 3, bytes + 2);
    swap(bytes + SIZE - 4, bytes + 3);
#endif

    for (int i = 0; i < SIZE; ++i) {
        if (i) {
            stream << ' ';
        }
        stream << !!(bytes[i] & 0x80)
               << !!(bytes[i] & 0x40)
               << !!(bytes[i] & 0x20)
               << !!(bytes[i] & 0x10)
               << !!(bytes[i] & 0x08)
               << !!(bytes[i] & 0x04)
               << !!(bytes[i] & 0x02)
               << !!(bytes[i] & 0x01);
    }

    return stream;
}

static void printDouble(ostream& stream, double number)
    // Format the bits in the specified 'number' to the specified 'stream' from
    // least- to most-significant (space-separated) byte, printing
    // most-significant bits in each byte first; followed by the a colon (:),
    // the number, and a newline.
{
    printDoubleBits(stream, number) << ": " << number << endl;
}

// ============================================================================
//                      FUNCTIONS TO MANIPULATE FLOATS
// ----------------------------------------------------------------------------
//  sign bit    8-bit exponent        23-bit mantissa
//     /       /                     /
//    +-+--------+-----------------------+
//    |s|e7....e0|m22..................m0|
//    +-+--------+-----------------------+
//    LSB                              MSB
// ----------------------------------------------------------------------------

static ostream& printFloatBits(ostream& stream, float number)
    // Format the bits in the specified 'number' to the specified 'stream' from
    // least- to most-significant (space-separated) byte, printing
    // most-significant bits in each byte first.  Return a reference to the
    // modifiable 'stream'.
{
    const int SIZE = static_cast<int>(sizeof number);
    ASSERT(4 <= SIZE);

    float          tmp = number;
    unsigned char *bytes = reinterpret_cast<unsigned char *>(&tmp);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    swap(bytes + SIZE - 1, bytes + 0);
    swap(bytes + SIZE - 2, bytes + 1);
    swap(bytes + SIZE - 3, bytes + 2);
    swap(bytes + SIZE - 4, bytes + 3);
#endif

    for (int i = 0; i < SIZE; ++i) {
        if (i) {
            stream << ' ';
        }
        stream << !!(bytes[i] & 0x80)
               << !!(bytes[i] & 0x40)
               << !!(bytes[i] & 0x20)
               << !!(bytes[i] & 0x10)
               << !!(bytes[i] & 0x08)
               << !!(bytes[i] & 0x04)
               << !!(bytes[i] & 0x02)
               << !!(bytes[i] & 0x01);
    }

    return stream;
}

static void printFloat(ostream& stream, float number)
    // Format the bits in the specified 'number' to the specified 'stream' from
    // least- to most-significant (space-separated) byte, printing
    // most-significant bits in each byte first; followed by the a colon (:),
    // the number, and a newline.
{
    printFloatBits(stream, number) << ": " << number << endl;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 25: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 the usage example provided in the component header file compiles,
        //:   links, and runs as shown
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

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Round-Trip Marshalling
///- - - - - - - - - - - - - - - - -
// The 'bslx::MarshallingUtil' component can be used stand-alone to marshal a
// platform-neutral representation of fundamental data and arrays of
// fundamental data to and from a buffer.  In this example, the round-trip
// marshalling of an 'int' and an array of 'int' values will be demonstrated.
// First, declare the buffer and the data to be marshalled:
//..
    char buffer[32];
    int  value = 17;
    int  values[] = { 1, 2, 3 };
//..
// Then, marshal all data into the 'buffer':
//..
    bslx::MarshallingUtil::putInt32(buffer + 0, value);
    bslx::MarshallingUtil::putArrayInt32(buffer + 4, values, 3);
//..
// Next, declare variables to hold the values to be extracted from the
// 'buffer':
//..
    int newValue = 0;
    int newValues[] = { 0, 0, 0 };
//..
// Finally, marshal the data from the 'buffer' to these variables and confirm
// the round-trip marshalling was successful:
//..
    bslx::MarshallingUtil::getInt32(&newValue, buffer + 0);
    bslx::MarshallingUtil::getArrayInt32(newValues, buffer + 4, 3);

    ASSERT(newValue     == value);
    ASSERT(newValues[0] == values[0]);
    ASSERT(newValues[1] == values[1]);
    ASSERT(newValues[2] == values[2]);
//..

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //   Provide mechanism to determine performance characteristics.
        //
        // Concerns:
        //: 1 Operations have reasonable run-time characteristics.
        //
        // Plan:
        //: 1 Put/get either an individual double or a double array a specified
        //:   number of times.  (C-1)
        //
        // Testing:
        //   STRESS TEST - Used to determine performance characteristics.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRESS TEST" << endl
                          << "===========" << endl;

        const int DEFAULT = argc > 2 ? 1000 : 1; // don't repeat in production.

        int mSize = argc > 2 ? (atoi(argv[2]) ? atoi(argv[2]) : DEFAULT)
                             : DEFAULT; // A zero value implies the default.

        int testIndividualDoubles = argc > 3;

        const int COUNT = 10000;
        if (testIndividualDoubles) {
            if (verbose) cout << "put/getFloat64" << endl;
            mSize *= COUNT;
        }
        else {
            if (verbose) cout << "put/getArrayFloat64" << endl;
        }
        const int SIZE = mSize;
        if (verbose) cout << "\nSIZE = " << SIZE << endl;

        const int NUM_FEEDBACKS = 50;
        int       feedback = SIZE / NUM_FEEDBACKS <= 0
                             ? 1
                             : SIZE / NUM_FEEDBACKS;

        if (verbose) cerr << "     "
            "----+----+----+----+----+----+----+----+----+----+" << endl;
        if (verbose) cerr << "BEGIN";

        int    i;
        double x = 123;
        double y = 0;

        double xa[COUNT];
        double ya[COUNT];
        char   b[sizeof x];
        char   buffer[sizeof xa];

        for (i = 0; i < COUNT; ++i) {
            xa[i] = i;
            ya[i] = -1;
        }

        ASSERT (x != y);
        for (i = 0; i < COUNT; ++i) {
            LOOP_ASSERT(i, xa[i] != ya[i]);
        }

        if (testIndividualDoubles) {
            for (i = SIZE - 1; i >= 0; --i)  {
                if (verbose && 0 == i % feedback) cerr << '.';

                MarshallingUtil::putFloat64(b, x);
                MarshallingUtil::getFloat64(&y, b);
            }
            ASSERT (x == y);
            for (i = 0; i < COUNT; ++i) {
                LOOP_ASSERT(i, xa[i] != ya[i]);
            }
        }
        else {
            for (i = SIZE - 1; i >= 0; --i)  {
                if (verbose && 0 == i % feedback) cerr << '.';

                MarshallingUtil::putArrayFloat64(buffer, xa, COUNT);
                MarshallingUtil::getArrayFloat64(ya, buffer, COUNT);
            }

            ASSERT (x != y);
            for (i = 0; i < COUNT; ++i) {
                LOOP_ASSERT(i, xa[i] == ya[i]);
            }
        }

        if (verbose) cerr << "END" << endl;

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PUT/GET 32-BIT FLOAT ARRAYS
        //   Verify put/get operations for 32-bit float arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayFloat32(char *buf, const float *ary, int count);
        //   getArrayFloat32(float *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 32-BIT FLOAT ARRAYS" << endl
                          << "===========================" << endl;

        typedef float T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20

        // 2^23 + 1 = 8388609 sets the LSB

        const T A = 8 * M + 1 * 1
                          + 2 * Z
                          + 3 * Z * Z;

        const char *const A_SPEC = "\x4B\x03\x02\x01";

        const T B = -8 * M - 14 * 1
                           - 13 * Z
                           - 12 * Z * Z;

        const char *const B_SPEC = "\xCB\x0C\x0D\x0E";

        const T C = 8 * M + 4 * 1
                          + 5 * Z
                          + 6 * Z * Z;

        const char *const C_SPEC = "\x4B\x06\x05\x04";

        const T D = -8 * M - 11 * 1
                           - 10 * Z
                           -  9 * Z * Z;

        const char *const D_SPEC = "\xCB\x09\x0A\x0B";


        const T W = 4 * M - 1;          // 2^22 - 1
        const T E = 8 * M + W + W + 1 ; // 2^24 - 1

        const char *const E_SPEC = "\x4B\x7F\xFF\xFF";


        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 4;             // size (in bytes) of data in buffer

        int i, k;
        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putFloat32(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x00';
        const char YY = '\xff';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayFloat32(
                                        buffer1 + align, input, length);
                MarshallingUtil::putArrayFloat32(
                                        buffer2 + align, input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                T       result2[NUM_TRIALS];
                const T POS = +2.0F;     // mantissa is 0000 0000 0000 ...
                const T NEG = -1.6F;     // mantissa is 1001 1001 1001 ...
                {
                    if (veryVerbose) {
                        P_(POS); P(NEG);
                        printFloat(cout, POS);
                        printFloat(cout, NEG);
                    }
                }

                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // mantissa should be zero filled
                    result2[i] = NEG;   // mantissa should be zero filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayFloat32(result1,
                                                 buffer1 + align,
                                                 length);
                MarshallingUtil::getArrayFloat32(result2,
                                                 buffer2 + align,
                                                 length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || input[i] != result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayFloat32(ZCHARPTR, VALUES, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayFloat32(VALUES, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayFloat32(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayFloat32(ZTPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayFloat32(BUFFER, VALUES, -1));
            ASSERT_PASS(MarshallingUtil::putArrayFloat32(BUFFER, VALUES, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayFloat32(VALUES, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayFloat32(VALUES, BUFFER, 0));
        }

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // PUT/GET 64-BIT FLOAT ARRAYS
        //   Verify put/get operations for 64-bit float arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayFloat64(char *buf, const double *ary, int count);
        //   getArrayFloat64(double *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 64-BIT FLOAT ARRAYS" << endl
                          << "===========================" << endl;

        typedef double T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20

        // 2^52 + 1 = 4503599627370497 sets the LSB

        const T A = 4 * K * M * M + 1 * 1
                                  + 2 * Z
                                  + 3 * Z * Z
                                  + 4 * Z * Z * Z
                                  + 5 * Z * Z * Z * Z
                                  + 6 * Z * Z * Z * Z * Z
                                  + 7 * Z * Z * Z * Z * Z * Z;

        const char *const A_SPEC = "\x43\x37\x06\x05\x04\x03\x02\x01";

        const T B = -4 * K * M * M - 14 * 1
                                   - 13 * Z
                                   - 12 * Z * Z
                                   - 11 * Z * Z * Z
                                   - 10 * Z * Z * Z * Z
                                   -  9 * Z * Z * Z * Z * Z
                                   -  8 * Z * Z * Z * Z * Z * Z;

        const char *const B_SPEC = "\xC3\x38\x09\x0A\x0B\x0C\x0D\x0E";

        const T C = 4 * K * M * M + 7 * 1
                                  + 6 * Z
                                  + 5 * Z * Z
                                  + 4 * Z * Z * Z
                                  + 3 * Z * Z * Z * Z
                                  + 2 * Z * Z * Z * Z * Z
                                  + 1 * Z * Z * Z * Z * Z * Z;

        const char *const C_SPEC = "\x43\x31\x02\x03\x04\x05\x06\x07";

        const T D = -4 * K * M * M - 8 * 1
                                   - 9 * Z
                                   - 10 * Z * Z
                                   - 11 * Z * Z * Z
                                   - 12 * Z * Z * Z * Z
                                   - 13 * Z * Z * Z * Z * Z
                                   - 14 * Z * Z * Z * Z * Z * Z;

        const char *const D_SPEC = "\xC3\x3E\x0D\x0C\x0B\x0A\x09\x08";

        const T W = 2 * K * M * M - 1;                  // 2^51 - 1
        const T E = 4 * K * M * M + W + W + 1 ;         // 2^53 - 1

        const char *const E_SPEC = "\x43\x3F\xFF\xFF\xFF\xFF\xFF\xFF";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 8;             // size (in bytes) of data in buffer

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putFloat64(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x00';
        const char YY = '\xff';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayFloat64(
                                        buffer1 + align, input, length);
                MarshallingUtil::putArrayFloat64(
                                        buffer2 + align, input, length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                T       result2[NUM_TRIALS];
                const T POS = +2.0;     // mantissa is 0000 0000 0000 ...
                const T NEG = -1.6;     // mantissa is 1001 1001 1001 ...
                {
                    if (veryVerbose) {
                        P_(POS); P(NEG);
                        printDouble(cout, POS);
                        printDouble(cout, NEG);
                    }
                }

                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // mantissa should be zero filled
                    result2[i] = NEG;   // mantissa should be zero filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayFloat64(result1,
                                                 buffer1 + align,
                                                 length);
                MarshallingUtil::getArrayFloat64(result2,
                                                 buffer2 + align,
                                                 length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || input[i] != result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayFloat64(ZCHARPTR, VALUES, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayFloat64(VALUES, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayFloat64(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayFloat64(ZTPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayFloat64(BUFFER, VALUES, -1));
            ASSERT_PASS(MarshallingUtil::putArrayFloat64(BUFFER, VALUES, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayFloat64(VALUES, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayFloat64(VALUES, BUFFER, 0));
        }

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // PUT/GET 8-BIT INTEGER ARRAYS
        //   Verify put/get operations for 8-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt8(char *buf, const char *ary, int count);
        //   putArrayInt8(char *buf, const signed char *ary, int count);
        //   putArrayInt8(char *buf, const unsigned char *ary, int count);
        //   getArrayInt8(char *var, const char *buf, int count);
        //   getArrayInt8(signed char *var, const char *buf, int count);
        //   getArrayInt8(unsigned char *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 8-BIT INTEGER ARRAYS" << endl
                          << "============================" << endl;

        typedef char          T;
        typedef signed char   S;
        typedef unsigned char U;

        const T     A = static_cast<T>(0x05);  // POSITIVE NUMBER
        const char *A_SPEC = "\x05";

        const T     B = static_cast<T>(0x80);  // NEGATIVE NUMBER
        const char *B_SPEC = "\x80";

        const T     C = static_cast<T>(0x10);  // POSITIVE NUMBER
        const char *C_SPEC = "\x10";

        const T     D = static_cast<T>(0x08);  // POSITIVE NUMBER
        const char *D_SPEC = "\x08";

        const T     E = static_cast<T>(0xfd);  // NEGATIVE NUMBER
        const char *E_SPEC = "\xFD";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 1;             // size (in bytes) of data in buffer

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt8(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i] & 0xFF)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];  // char
        char buffer2[SIZE * (1 + NUM_TRIALS)];  // unsigned char
        char buffer3[SIZE * (1 + NUM_TRIALS)];  // signed char

        const char XX = '\x69';                 // char
        const char YY = '\xA5';                 // unsigned char
        const char ZZ = '\x58';                 // signed char

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);
                memset(buffer3, ZZ, sizeof buffer3);

                MarshallingUtil::putArrayInt8(buffer1 + align,
                                              reinterpret_cast<T *>(input),
                                              length);
                MarshallingUtil::putArrayInt8(buffer2 + align,
                                              reinterpret_cast<U *>(input),
                                              length);
                MarshallingUtil::putArrayInt8(buffer3 + align,
                                              reinterpret_cast<S *>(input),
                                              length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i] & 0xFF)
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i] & 0xFF)
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer3 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i] & 0xff)
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer3 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                    LOOP3_ASSERT(length, align, k, ZZ == buffer3[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                    LOOP3_ASSERT(length, align, k, ZZ == buffer3[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                S       result3[NUM_TRIALS];
                const S POS = +1;
                const U NEG = static_cast<U>(static_cast<S>(-1));

                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // could be signed or unsigned
                    result2[i] = NEG;   // no issue of filling here
                    result3[i] = POS;   // no issue of filling here
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) != result2[i]);
                    LOOP3_ASSERT(length, align, i, S(input[i]) != result3[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt8(result1,
                                              buffer1 + align,
                                              length);
                MarshallingUtil::getArrayInt8(result2,
                                              buffer2 + align,
                                              length);
                MarshallingUtil::getArrayInt8(result3,
                                              buffer3 + align,
                                              length);
                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || U(input[i]) != result2[i]
                                    || S(input[i]) != result3[i]) {
                        P(i);    P(input[i] & 0xff);    P(result1[i] & 0xff);
                                 P(result2[i] & 0xff);  P(result3[i] & 0xff);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) == result2[i]);
                    LOOP3_ASSERT(length, align, i, S(input[i]) == result3[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, T(POS) == result1[i]);
                    LOOP3_ASSERT(length, align, i, U(NEG) == result2[i]);
                    LOOP3_ASSERT(length, align, i, S(POS) == result3[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            S     SV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            S    *ZSPTR    = static_cast<S *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(ZCHARPTR, TV, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(TV, ZCHARPTR, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(ZCHARPTR, SV, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(SV, ZCHARPTR, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(ZCHARPTR, UV, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(BUFFER, ZTPTR, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(ZTPTR, BUFFER, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(BUFFER, ZSPTR, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(ZSPTR, BUFFER, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(BUFFER, ZUPTR, 3));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(BUFFER, TV, -1));
            ASSERT_SAFE_PASS(MarshallingUtil::putArrayInt8(BUFFER, TV, 0));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(TV, BUFFER, -1));
            ASSERT_SAFE_PASS(MarshallingUtil::getArrayInt8(TV, BUFFER, 0));
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(BUFFER, SV, -1));
            ASSERT_SAFE_PASS(MarshallingUtil::putArrayInt8(BUFFER, SV, 0));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(SV, BUFFER, -1));
            ASSERT_SAFE_PASS(MarshallingUtil::getArrayInt8(SV, BUFFER, 0));
            ASSERT_SAFE_FAIL(MarshallingUtil::putArrayInt8(BUFFER, UV, -1));
            ASSERT_SAFE_PASS(MarshallingUtil::putArrayInt8(BUFFER, UV, 0));
            ASSERT_SAFE_FAIL(MarshallingUtil::getArrayInt8(UV, BUFFER, -1));
            ASSERT_SAFE_PASS(MarshallingUtil::getArrayInt8(UV, BUFFER, 0));
        }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // PUT/GET 16-BIT INTEGER ARRAYS
        //   Verify put/get operations for 16-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt16(char *buf, const short *ary, int count);
        //   putArrayInt16(char *buf, const unsigned short *ary, int count);
        //   getArrayInt16(short *var, const char *buf, int count);
        //   getArrayUint16(unsigned short *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 16-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef short          T;
        typedef unsigned short U;

        const T     A = static_cast<T>((0x01 << 8) + 0x02);  // POSITIVE NUMBER
        const char *A_SPEC = "\x01\x02";

        const T     B = static_cast<T>((0x80 << 8) + 0x70);  // NEGATIVE NUMBER
        const char *B_SPEC = "\x80\x70";

        const T     C = static_cast<T>((0x10 << 8) + 0x20);  // POSITIVE NUMBER
        const char *C_SPEC = "\x10\x20";

        const T     D = static_cast<T>((0x08 << 8) + 0x07);  // POSITIVE NUMBER
        const char *D_SPEC = "\x08\x07";

        const T     E = static_cast<T>((0xFF << 8) + 0xFE);  // NEGATIVE NUMBER
        const char *E_SPEC = "\xFF\xFE";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 2;             // size (in bytes) of data in buffer

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt16(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (int k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt16(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt16(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(static_cast<T>(-1));
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) != result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt16(result1,
                                               buffer1 + align,
                                               length);
                MarshallingUtil::getArrayUint16(result2,
                                                buffer2 + align,
                                                length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose || input[i] != result1[i]
                                    || U(input[i]) != result2[i]) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, U(input[i]) == result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt16(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt16(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt16(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint16(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt16(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt16(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt16(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint16(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt16(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt16(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt16(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt16(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt16(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt16(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint16(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint16(UV, BUFFER, 0));
        }

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // PUT/GET 24-BIT INTEGER ARRAYS
        //   Verify put/get operations for 24-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt24(char *buf, const int *ary, int count);
        //   putArrayInt24(char *buf, const unsigned int *ary, int count);
        //   getArrayInt24(int *var, const char *buf, int count);
        //   getArrayUint24(unsigned int *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 24-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef int T;
        typedef unsigned int U;

        const T A = ((((( static_cast<T>  // POSITIVE NUMBER
                          (0x00) << 8) + 0x02) << 8) + 0x03) << 8) + 0x04;

        const char *A_SPEC = "\x02\x03\x04";

        const T B = ((((( static_cast<T>  // NEGATIVE NUMBER
                          (0xFF) << 8) + 0x80) << 8) + 0x70) << 8) + 0x60;

        const char *B_SPEC = "\x80\x70\x60";

        const T C = ((((( static_cast<T>  // POSITIVE NUMBER
                          (0x00) << 8) + 0x20) << 8) + 0x30) << 8) + 0x40;

        const char *C_SPEC = "\x20\x30\x40";

        const T D = ((((( static_cast<T>  // POSITIVE NUMBER
                          (0x00) << 8) + 0x07) << 8) + 0x06) << 8) + 0x05;

        const char *D_SPEC = "\x07\x06\x05";

        const T E = ((((( static_cast<T>  // NEGATIVE NUMBER
                          (0xff) << 8) + 0xfe) << 8) + 0xfd) << 8) + 0xfc;

        const char *E_SPEC = "\xFE\xFD\xFC";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 3;

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt24(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (int k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xA5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt24(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt24(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(-1);
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt24(result1,
                                               buffer1 + align, length);
                MarshallingUtil::getArrayUint24(result2,
                                                buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose
                        || input[i] != result1[i]
                        || ((input[i] != static_cast<T>(result2[i]))
                            && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0) {
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == (T) result2[i]);
                    }
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt24(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt24(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt24(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint24(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt24(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt24(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt24(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint24(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt24(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt24(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt24(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt24(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt24(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt24(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint24(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint24(UV, BUFFER, 0));
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // PUT/GET 32-BIT INTEGER ARRAYS
        //   Verify put/get operations for 32-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt32(char *buf, const int *ary, int count);
        //   putArrayInt32(char *buf, const unsigned int *ary, int count);
        //   getArrayInt32(int *var, const char *buf, int count);
        //   getArrayUint32(unsigned int *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 32-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef int T;
        typedef unsigned int U;

        const T A = ((((( static_cast<T>  // POSITIVE NUMBER
                          (0x01) << 8) + 0x02) << 8) + 0x03) << 8) + 0x04;

        const char *A_SPEC = "\x01\x02\x03\x04";

        const T B = ((((( static_cast<T>  // NEGATIVE NUMBER
                          (0x80) << 8) + 0x70) << 8) + 0x60) << 8) + 0x50;

        const char *B_SPEC = "\x80\x70\x60\x50";

        const T C = ((((( static_cast<T>  // POSITIVE NUMBER
                          (0x10) << 8) + 0x20) << 8) + 0x30) << 8) + 0x40;

        const char *C_SPEC = "\x10\x20\x30\x40";

        const T D = ((((( static_cast<T>  // POSITIVE NUMBER
                          (0x08) << 8) + 0x07) << 8) + 0x06) << 8) + 0x05;

        const char *D_SPEC = "\x08\x07\x06\x05";

        const T E = ((((( static_cast<T>  // NEGATIVE NUMBER
                          (0xff) << 8) + 0xfe) << 8) + 0xfd) << 8) + 0xfc;

        const char *E_SPEC = "\xFF\xFE\xFD\xFC";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 4;             // size (in bytes) of data in buffer

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt32(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (int k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xA5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt32(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt32(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(-1);
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt32(result1,
                                               buffer1 + align, length);
                MarshallingUtil::getArrayUint32(result2,
                                                buffer2 + align, length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose
                        || input[i] != result1[i]
                        || input[i] != static_cast<T>(result2[i])) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt32(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt32(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt32(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint32(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt32(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt32(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt32(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint32(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt32(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt32(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt32(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt32(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt32(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt32(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint32(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint32(UV, BUFFER, 0));
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PUT/GET 40-BIT INTEGER ARRAYS
        //   Verify put/get operations for 40-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt40(char *buf, const Int64 *ary, int count);
        //   putArrayInt40(char *buf, const Uint64 *ary, int count);
        //   getArrayInt40(Int64 *var, const char *buf, int count);
        //   getArrayUint40(Uint64 *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 40-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef bsls::Types::Int64 T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x00) << 8) + 0x04) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x04\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xff) << 8) + 0xff) << 8) + 0x80) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x40\x30\x20\x10";

        const T C = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x00) << 8) + 0x40) << 8)
                        + 0x50) << 8) + 0x60) << 8) + 0x70) << 8) + 0x80;

        const char *C_SPEC = "\x40\x50\x60\x70\x80";

        const T D = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x00) << 8) + 0x05) << 8)
                        + 0x04) << 8) + 0x03) << 8) + 0x02) << 8) + 0x01;

        const char *D_SPEC = "\x05\x04\x03\x02\x01";

        const T E = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xff) << 8) + 0xff) << 8) + 0xfc) << 8)
                        + 0xfb) << 8) + 0xfa) << 8) + 0xf9) << 8) + 0xf8;

        const char *E_SPEC = "\xFC\xFB\xFA\xF9\xF8";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 5;

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt40(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt40(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt40(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(static_cast<T>(-1));
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt40(result1,
                                               buffer1 + align,
                                               length);
                MarshallingUtil::getArrayUint40(result2,
                                                buffer2 + align,
                                                length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose
                        || input[i] != result1[i]
                        || ((input[i] != static_cast<T>(result2[i]))
                            && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0) {
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == (T) result2[i]);
                    }
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt40(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt40(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt40(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint40(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt40(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt40(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt40(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint40(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt40(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt40(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt40(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt40(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt40(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt40(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint40(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint40(UV, BUFFER, 0));
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // PUT/GET 48-BIT INTEGER ARRAYS
        //   Verify put/get operations for 48-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt48(char *buf, const Int64 *ary, int count);
        //   putArrayInt48(char *buf, const Uint64 *ary, int count);
        //   getArrayInt48(Int64 *var, const char *buf, int count);
        //   getArrayUint48(Uint64 *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 48-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef bsls::Types::Int64 T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x03) << 8) + 0x04) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x03\x04\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xff) << 8) + 0x80) << 8) + 0x50) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x50\x40\x30\x20\x10";

        const T C = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x30) << 8) + 0x40) << 8)
                        + 0x50) << 8) + 0x60) << 8) + 0x70) << 8) + 0x80;

        const char *C_SPEC = "\x30\x40\x50\x60\x70\x80";

        const T D = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x06) << 8) + 0x05) << 8)
                        + 0x04) << 8) + 0x03) << 8) + 0x02) << 8) + 0x01;

        const char *D_SPEC = "\x06\x05\x04\x03\x02\x01";

        const T E = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xff) << 8) + 0xfd) << 8) + 0xfc) << 8)
                        + 0xfb) << 8) + 0xfa) << 8) + 0xf9) << 8) + 0xf8;

        const char *E_SPEC = "\xFD\xFC\xFB\xFA\xF9\xF8";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 6;

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt48(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt48(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt48(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(static_cast<T>(-1));
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt48(result1,
                                               buffer1 + align,
                                               length);
                MarshallingUtil::getArrayUint48(result2,
                                                buffer2 + align,
                                                length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose
                        || input[i] != result1[i]
                        || ((input[i] != static_cast<T>(result2[i]))
                            && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0) {
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == static_cast<T>(result2[i]));
                    }
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt48(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt48(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt48(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint48(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt48(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt48(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt48(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint48(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt48(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt48(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt48(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt48(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt48(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt48(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint48(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint48(UV, BUFFER, 0));
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // PUT/GET 56-BIT INTEGER ARRAYS
        //   Verify put/get operations for 56-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt56(char *buf, const Int64 *ary, int count);
        //   putArrayInt56(char *buf, const Uint64 *ary, int count);
        //   getArrayInt56(Int64 *var, const char *buf, int count);
        //   getArrayUint56(Uint64 *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 56-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef bsls::Types::Int64  T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x02) << 8) + 0x03) << 8) + 0x04) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x02\x03\x04\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0x80) << 8) + 0x60) << 8) + 0x50) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x60\x50\x40\x30\x20\x10";

        const T C = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x20) << 8) + 0x30) << 8) + 0x40) << 8)
                        + 0x50) << 8) + 0x60) << 8) + 0x70) << 8) + 0x80;

        const char *C_SPEC = "\x20\x30\x40\x50\x60\x70\x80";

        const T D = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x07) << 8) + 0x06) << 8) + 0x05) << 8)
                        + 0x04) << 8) + 0x03) << 8) + 0x02) << 8) + 0x01;

        const char *D_SPEC = "\x07\x06\x05\x04\x03\x02\x01";

        const T E = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xfe) << 8) + 0xfd) << 8) + 0xfc) << 8)
                        + 0xfb) << 8) + 0xfa) << 8) + 0xf9) << 8) + 0xf8;

        const char *E_SPEC = "\xFE\xFD\xFC\xFB\xFA\xF9\xF8";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 7;

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt56(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt56(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt56(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(static_cast<T>(-1));
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt56(result1,
                                               buffer1 + align,
                                               length);
                MarshallingUtil::getArrayUint56(result2,
                                                buffer2 + align,
                                                length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose
                        || input[i] != result1[i]
                        || ((input[i] != static_cast<T>(result2[i]))
                            && (input[i] > 0))) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    if (input[i] > 0) {
                        LOOP3_ASSERT(length, align, i,
                                     input[i] == static_cast<T>(result2[i]));
                    }
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt56(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt56(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt56(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint56(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt56(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt56(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt56(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint56(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt56(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt56(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt56(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt56(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt56(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt56(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint56(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint56(UV, BUFFER, 0));
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PUT/GET 64-BIT INTEGER ARRAYS
        //   Verify put/get operations for 64-bit integer arrays.
        //
        // Concerns:
        //: 1 'put' produces the correct format.
        //:
        //: 2 'get' inverts the 'put' operation.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Assume individual elements can be written/read (previously
        //:   tested).
        //:
        //: 2 Tests arrays of length 0, 1, 2, ...  (C-1)
        //:
        //: 3 'get' from the result values and verify this data matches the
        //:   original data used in the 'put'.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   putArrayInt64(char *buf, const Int64 *ary, int count);
        //   putArrayInt64(char *buf, const Uint64 *ary, int count);
        //   getArrayInt64(Int64 *var, const char *buf, int count);
        //   getArrayUint64(Uint64 *var, const char *buf, int count);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 64-BIT INTEGER ARRAYS" << endl
                          << "=============================" << endl;

        typedef bsls::Types::Int64 T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x01) << 8) + 0x02) << 8) + 0x03) << 8) + 0x04) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x01\x02\x03\x04\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0x80) << 8) + 0x70) << 8) + 0x60) << 8) + 0x50) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x70\x60\x50\x40\x30\x20\x10";

        const T C = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x10) << 8) + 0x20) << 8) + 0x30) << 8) + 0x40) << 8)
                        + 0x50) << 8) + 0x60) << 8) + 0x70) << 8) + 0x80;

        const char *C_SPEC = "\x10\x20\x30\x40\x50\x60\x70\x80";

        const T D = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x08) << 8) + 0x07) << 8) + 0x06) << 8) + 0x05) << 8)
                        + 0x04) << 8) + 0x03) << 8) + 0x02) << 8) + 0x01;

        const char *D_SPEC = "\x08\x07\x06\x05\x04\x03\x02\x01";

        const T E = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xfe) << 8) + 0xfd) << 8) + 0xfc) << 8)
                        + 0xfb) << 8) + 0xfa) << 8) + 0xf9) << 8) + 0xf8;

        const char *E_SPEC = "\xFF\xFE\xFD\xFC\xFB\xFA\xF9\xF8";

        T           VALUES[] = { A, B, C, D, E };
        const char *SPECS[] = { A_SPEC, B_SPEC, C_SPEC, D_SPEC, E_SPEC };
        const int   NUM_VALUES = static_cast<int>(sizeof VALUES
                                                  / sizeof *VALUES);
        ASSERT(NUM_VALUES == sizeof SPECS / sizeof *SPECS);

        const int SIZE = 8;             // size (in bytes) of data in buffer

        int i, k;

        ASSERT(SIZE <= static_cast<int>(sizeof(T)));

        // Ensure expected pattern match actual buffer pattern for each value.
        {
            int wasError = 0;
            for (i = 0; i < NUM_VALUES; ++i) {
                const char *exp = SPECS[i];
                char        buffer[SIZE];
                MarshallingUtil::putInt64(buffer, VALUES[i]);
                bool isEq = 0 == memcmp(exp, buffer, SIZE);
                if (veryVerbose || !isEq) {
                    P(VALUES[i])
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer, SIZE) << endl;
                }
                LOOP_ASSERT(i, isEq);
                if (!isEq) wasError = 1;
            }
            if (wasError) break;        // no need to continue.
        }

        const int NUM_TRIALS = 10;
        T         input[NUM_TRIALS];
        {   // load repeating pattern: A, B, C, ..., A, B, C, ...
            for (i = 0; i < NUM_TRIALS; ++i) {
                input[i] = VALUES[i % NUM_VALUES];
            }
            if (veryVerbose) {
                cout << "inputs: " << endl;
                for (k = 0; k < NUM_TRIALS; ++k) {
                    cout << "\t" << k << ".\t" << input[k] << endl;
                }
            }
        }

        char buffer1[SIZE * (1 + NUM_TRIALS)];
        char buffer2[SIZE * (1 + NUM_TRIALS)];

        const char XX = '\x69';
        const char YY = '\xa5';

        for (int length = 0; length < NUM_TRIALS; ++length) {
            for (int align = 0; align < SIZE; ++align) {
                memset(buffer1, XX, sizeof buffer1);
                memset(buffer2, YY, sizeof buffer2);

                MarshallingUtil::putArrayInt64(buffer1 + align,
                                               reinterpret_cast<T *>(input),
                                               length);
                MarshallingUtil::putArrayInt64(buffer2 + align,
                                               reinterpret_cast<U *>(input),
                                               length);
                // check buffer data
                for (i = 0; i < length; ++i) {
                    const char *exp = SPECS[i % NUM_VALUES];
                    int         off = align + SIZE * i;

                    bool isEq = 0 == memcmp(exp, buffer1 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer1 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);

                    isEq = 0 == memcmp(exp, buffer2 + off, SIZE);
                    if (veryVerbose || !isEq) {
                        P(VALUES[i])
                        cout << "exp: "; pBytes(exp, SIZE) << endl;
                        cout << "act: "; pBytes(buffer2 + off, SIZE) << endl;
                    }
                    LOOP3_ASSERT(length, align, i, isEq);
                }

                // check leader for overwrite
                for (k = 0; k < align; ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // check trailer for overwrite
                for (int k = align + SIZE * length;
                     k < static_cast<int>(sizeof buffer1);
                     ++k) {
                    LOOP3_ASSERT(length, align, k, XX == buffer1[k]);
                    LOOP3_ASSERT(length, align, k, YY == buffer2[k]);
                }

                // initialize result arrays
                T       result1[NUM_TRIALS];
                U       result2[NUM_TRIALS];
                const T POS = +1;
                const U NEG = static_cast<U>(static_cast<T>(-1));
                for (i = 0; i < NUM_TRIALS; ++i) {
                    result1[i] = POS;   // signed should be sign-filled
                    result2[i] = NEG;   // unsigned should zero-filled
                    LOOP3_ASSERT(length, align, i, input[i] != result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] != (T) result2[i]);
                }

                // fetch data from arrays
                MarshallingUtil::getArrayInt64(result1,
                                               buffer1 + align,
                                               length);
                MarshallingUtil::getArrayUint64(result2,
                                                buffer2 + align,
                                                length);

                int i = 0;
                for (; i < length; ++i) {       // check values in range
                    if (veryVerbose
                        || input[i] != result1[i]
                        || input[i] != static_cast<T>(result2[i])) {
                        P(i); P(input[i]); P(result1[i]); P(result2[i]);
                    }
                    LOOP3_ASSERT(length, align, i, input[i] == result1[i]);
                    LOOP3_ASSERT(length, align, i, input[i] == (T) result2[i]);
                }
                for (; i < NUM_TRIALS; ++i) {   // check values beyond range
                    LOOP3_ASSERT(length, align, i, POS == result1[i]);
                    LOOP3_ASSERT(length, align, i, NEG == result2[i]);
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            T     TV[4];
            U     UV[4];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_FAIL(MarshallingUtil::putArrayInt64(ZCHARPTR, TV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt64(TV, ZCHARPTR, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt64(ZCHARPTR, UV, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint64(UV, ZCHARPTR, 3));

            // invalid 'values'
            ASSERT_FAIL(MarshallingUtil::putArrayInt64(BUFFER, ZTPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayInt64(ZTPTR, BUFFER, 3));
            ASSERT_FAIL(MarshallingUtil::putArrayInt64(BUFFER, ZUPTR, 3));
            ASSERT_FAIL(MarshallingUtil::getArrayUint64(ZUPTR, BUFFER, 3));

            // valid and invalid 'numValues'
            ASSERT_FAIL(MarshallingUtil::putArrayInt64(BUFFER, TV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt64(BUFFER, TV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayInt64(TV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayInt64(TV, BUFFER, 0));
            ASSERT_FAIL(MarshallingUtil::putArrayInt64(BUFFER, UV, -1));
            ASSERT_PASS(MarshallingUtil::putArrayInt64(BUFFER, UV, 0));
            ASSERT_FAIL(MarshallingUtil::getArrayUint64(UV, BUFFER, -1));
            ASSERT_PASS(MarshallingUtil::getArrayUint64(UV, BUFFER, 0));
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PUT/GET 32-BIT FLOATS
        //   Verify put/get operations for 32-bit floats.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putFloat32(char *buf, float val);
        //   getFloat32(float *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 32-BIT FLOATS" << endl
                          << "=====================" << endl;

        typedef float T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20
        const T B = K * M; // 2^30

        const T W = 4 * M - 1;                  // 2^22 - 1
        const T ONES = 8 * M + W + W + 1 ;      // 2^24 - 1

        const char *const ONES_SPEC = "\x4B\x7F\xFF\xFF";

        // 2^23 + 1 = 8388609 sets the LSB

        const T X = 8 * M + 1 * 1
                          + 2 * Z
                          + 3 * Z * Z;

        const char *const X_SPEC = "\x4B\x03\x02\x01";

        const T Y = -8 * M - 14 * 1
                           - 13 * Z
                           - 12 * Z * Z;

        const char *const Y_SPEC = "\xCB\x0C\x0D\x0E";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,   0.0,        "\x00\x00\x00\x00"                      },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_,  -0.0,        "\x80\x00\x00\x00"                      },
#else
            { L_,  -0.0,        "\x00\x00\x00\x00"                      },
#endif
            { L_,   ONES,       ONES_SPEC                               },

            // small integers
            { L_,   5.0,        "\x40\xA0\x00\x00"                      },
            { L_,   4.0,        "\x40\x80\x00\x00"                      },
            { L_,   3.75,       "\x40\x70\x00\x00"                      },
            { L_,   3.5,        "\x40\x60\x00\x00"                      },
            { L_,   3.25,       "\x40\x50\x00\x00"                      },
            { L_,   3.0,        "\x40\x40\x00\x00"                      },
            { L_,   2.75,       "\x40\x30\x00\x00"                      },
            { L_,   2.5,        "\x40\x20\x00\x00"                      },
            { L_,   2.0,        "\x40\x00\x00\x00"                      },
            { L_,   1.5,        "\x3F\xC0\x00\x00"                      },
            { L_,   1.0,        "\x3F\x80\x00\x00"                      },

            // base-2 fractions
            { L_,   0.75,       "\x3F\x40\x00\x00"                      },
            { L_,   0.625,      "\x3F\x20\x00\x00"                      },
            { L_,   0.5,        "\x3F\x00\x00\x00"                      },
            { L_,   0.375,      "\x3E\xC0\x00\x00"                      },
            { L_,   0.25,       "\x3E\x80\x00\x00"                      },
            { L_,   0.125,      "\x3E\x00\x00\x00"                      },

            // larger integers
            { L_,   1020,       "\x44\x7F\x00\x00"                      },
            { L_,   1021,       "\x44\x7F\x40\x00"                      },
            { L_,   1022,       "\x44\x7F\x80\x00"                      },
            { L_,   1023,       "\x44\x7F\xC0\x00"                      },
            { L_,   1024,       "\x44\x80\x00\x00"                      },
            { L_,   1025,       "\x44\x80\x20\x00"                      },
            { L_,   1026,       "\x44\x80\x40\x00"                      },
            { L_,   1027,       "\x44\x80\x60\x00"                      },

            // really big integers (powers of 2)
            { L_,   K,          "\x44\x80\x00\x00"                      },
            { L_,   M,          "\x49\x80\x00\x00"                      },
            { L_,   B,          "\x4E\x80\x00\x00"                      },
            { L_,   K * B,      "\x53\x80\x00\x00"                      },
            { L_,   M * B,      "\x58\x80\x00\x00"                      },
            { L_,   B * B,      "\x5D\x80\x00\x00"                      },
            { L_,   K * B * B,  "\x62\x80\x00\x00"                      },
            { L_,   M * B * B,  "\x67\x80\x00\x00"                      },
            { L_,   B * B * B,  "\x6C\x80\x00\x00"                      },

            // least significant bits of mantissa
            { L_,   1*M + 1,    "\x49\x80\x00\x08"                      },
            { L_,   2*M + 1,    "\x4A\x00\x00\x04"                      },
            { L_,   3*M + 1,    "\x4A\x40\x00\x04"                      },
            { L_,   4*M + 1,    "\x4A\x80\x00\x02"                      },
            { L_,   5*M + 1,    "\x4A\xA0\x00\x02"                      },
            { L_,   6*M + 1,    "\x4A\xC0\x00\x02"                      },
            { L_,   7*M + 1,    "\x4A\xE0\x00\x02"                      },
            { L_,   8*M + 1,    "\x4B\x00\x00\x01"                      },
            { L_,   9*M + 2,    "\x4B\x10\x00\x02"                      },
            { L_,  10*M + 5,    "\x4B\x20\x00\x05"                      },

            // test each byte independently
            { L_,   X,          X_SPEC                                  },
            { L_,   Y,          Y_SPEC                                  },

        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     number = DATA[di].d_number;
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 4;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putFloat32(buffer + i, number);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(number)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                // check negative
                memset(buffer, '\x32', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putFloat32(buffer + i, -number);
                buffer[i] = static_cast<char>(
                                  static_cast<unsigned int>(buffer[i]) ^ 0x80);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                // check positive again
                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putFloat32(buffer + i, number);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static T INITIAL_VALUES[6];
                INITIAL_VALUES[0] = 0.0000001F;
                INITIAL_VALUES[1] = -0.0000001F;
                INITIAL_VALUES[2] = 1.234567F;
                INITIAL_VALUES[3]  = -765432.1F;
                INITIAL_VALUES[4] = ONES-1;
                INITIAL_VALUES[5] = 1-ONES;

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        MarshallingUtil::getFloat32(&x, buffer + i);
                        if (number != x) {
                            P_(number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, number == x);
                    }
                    {   // negate buffer value
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        buffer[i] = static_cast<char>(
                                  static_cast<unsigned int>(buffer[i]) ^ 0x80);
                        MarshallingUtil::getFloat32(&x, buffer + i);
                        buffer[i] = static_cast<char>(
                                  static_cast<unsigned int>(buffer[i]) ^ 0x80);
                        if (-number != x) {
                            P_(-number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, -number == x);
                    }

                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            T     TV       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putFloat32(ZCHARPTR, TV));
            ASSERT_SAFE_FAIL(MarshallingUtil::getFloat32(&TV, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getFloat32(ZTPTR, BUFFER));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PUT/GET 64-BIT FLOATS
        //   Verify put/get operations for 64-bit floats.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putFloat64(char *buf, double val);
        //   getFloat64(double *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 64-BIT FLOATS" << endl
                          << "=====================" << endl;

        typedef double T;

        const T Z = 256;   // 2^8
        const T K = 1024;  // 2^10
        const T M = K * K; // 2^20
        const T B = K * M; // 2^30

        const T W = 2 * B * M - 1;              // 2^51 - 1
        const T ONES = 4 * B * M + W + W + 1 ;  // 2^53 - 1

        const char *const ONES_SPEC = "\x43\x3F\xFF\xFF\xFF\xFF\xFF\xFF";

        // 2^52 + 1 = 4503599627370497 sets the LSB

        const T X = 4 * M * B + 1 * 1
                              + 2 * Z
                              + 3 * Z * Z
                              + 4 * Z * Z * Z
                              + 5 * Z * Z * Z * Z
                              + 6 * Z * Z * Z * Z * Z
                              + 7 * Z * Z * Z * Z * Z * Z;

        const char *const X_SPEC = "\x43\x37\x06\x05\x04\x03\x02\x01";

        const T Y = -4 * M * B - 14 * 1
                               - 13 * Z
                               - 12 * Z * Z
                               - 11 * Z * Z * Z
                               - 10 * Z * Z * Z * Z
                                - 9 * Z * Z * Z * Z * Z
                                - 8 * Z * Z * Z * Z * Z * Z;

        const char *const Y_SPEC = "\xC3\x38\x09\x0A\x0B\x0C\x0D\x0E";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,   0.0,        "\x00\x00\x00\x00\x00\x00\x00\x00"      },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_,  -0.0,        "\x80\x00\x00\x00\x00\x00\x00\x00"      },
#else
            { L_,  -0.0,        "\x00\x00\x00\x00\x00\x00\x00\x00"      },
#endif
            { L_,   ONES,       ONES_SPEC                               },

            // small integers
            { L_,   5.00,       "\x40\x14\x00\x00\x00\x00\x00\x00"      },
            { L_,   4.00,       "\x40\x10\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.75,       "\x40\x0E\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.5,        "\x40\x0C\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.25,       "\x40\x0A\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.0 ,       "\x40\x08\x00\x00\x00\x00\x00\x00"      },
            { L_,   2.75,       "\x40\x06\x00\x00\x00\x00\x00\x00"      },
            { L_,   2.5,        "\x40\x04\x00\x00\x00\x00\x00\x00"      },
            { L_,   2.0,        "\x40\x00\x00\x00\x00\x00\x00\x00"      },
            { L_,   1.5,        "\x3F\xF8\x00\x00\x00\x00\x00\x00"      },
            { L_,   1.0,        "\x3F\xF0\x00\x00\x00\x00\x00\x00"      },

            // base-2 fractions
            { L_,   0.75,       "\x3F\xE8\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.625,      "\x3F\xE4\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.5,        "\x3F\xE0\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.375,      "\x3F\xD8\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.25,       "\x3F\xD0\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.125,      "\x3F\xC0\x00\x00\x00\x00\x00\x00"      },

            // larger integers
            { L_,   1020,       "\x40\x8F\xE0\x00\x00\x00\x00\x00"      },
            { L_,   1021,       "\x40\x8F\xE8\x00\x00\x00\x00\x00"      },
            { L_,   1022,       "\x40\x8F\xF0\x00\x00\x00\x00\x00"      },
            { L_,   1023,       "\x40\x8F\xF8\x00\x00\x00\x00\x00"      },
            { L_,   1024,       "\x40\x90\x00\x00\x00\x00\x00\x00"      },
            { L_,   1025,       "\x40\x90\x04\x00\x00\x00\x00\x00"      },
            { L_,   1026,       "\x40\x90\x08\x00\x00\x00\x00\x00"      },
            { L_,   1027,       "\x40\x90\x0C\x00\x00\x00\x00\x00"      },

            // really big integers (powers of 2)
            { L_,   K,          "\x40\x90\x00\x00\x00\x00\x00\x00"      },
            { L_,   M,          "\x41\x30\x00\x00\x00\x00\x00\x00"      },
            { L_,   B,          "\x41\xD0\x00\x00\x00\x00\x00\x00"      },
            { L_,   K * B,      "\x42\x70\x00\x00\x00\x00\x00\x00"      },
            { L_,   M * B,      "\x43\x10\x00\x00\x00\x00\x00\x00"      },
            { L_,   B * B,      "\x43\xB0\x00\x00\x00\x00\x00\x00"      },
            { L_,   K * B * B,  "\x44\x50\x00\x00\x00\x00\x00\x00"      },
            { L_,   M * B * B,  "\x44\xF0\x00\x00\x00\x00\x00\x00"      },
            { L_,   B * B * B,  "\x45\x90\x00\x00\x00\x00\x00\x00"      },

            // least significant bits of mantissa
            { L_,   1*M*B + 1,  "\x43\x10\x00\x00\x00\x00\x00\x04"      },
            { L_,   2*M*B + 1,  "\x43\x20\x00\x00\x00\x00\x00\x02"      },
            { L_,   3*M*B + 1,  "\x43\x28\x00\x00\x00\x00\x00\x02"      },
            { L_,   4*M*B + 1,  "\x43\x30\x00\x00\x00\x00\x00\x01"      },
            { L_,   5*M*B + 2,  "\x43\x34\x00\x00\x00\x00\x00\x02"      },
            { L_,   6*M*B + 3,  "\x43\x38\x00\x00\x00\x00\x00\x03"      },
            { L_,   7*M*B + 4,  "\x43\x3C\x00\x00\x00\x00\x00\x04"      },

            // test each byte independently
            { L_,   X,          X_SPEC                                  },
            { L_,   Y,          Y_SPEC                                  },

        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     number = DATA[di].d_number;
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 8;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putFloat64(buffer + i, number);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(number)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                // check negative
                memset(buffer, '\x64', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putFloat64(buffer + i, -number);
                buffer[i] = static_cast<char>(
                                  static_cast<unsigned int>(buffer[i]) ^ 0x80);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                // check positive again
                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putFloat64(buffer + i, number);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static T INITIAL_VALUES[6];
                INITIAL_VALUES[0] = 0.0000001,
                INITIAL_VALUES[1] = -0.0000001;
                INITIAL_VALUES[2] = 1.234567;
                INITIAL_VALUES[3] = -765432.1;
                INITIAL_VALUES[4] = ONES-1;
                INITIAL_VALUES[5] = 1-ONES;

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        MarshallingUtil::getFloat64(&x, buffer + i);
                        if (number != x) {
                            P_(number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, number == x);
                    }
                    {   // negate buffer value
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, number != x);
                        buffer[i] = static_cast<char>(
                                  static_cast<unsigned int>(buffer[i]) ^ 0x80);
                        MarshallingUtil::getFloat64(&x, buffer + i);
                        buffer[i] = static_cast<char>(
                                  static_cast<unsigned int>(buffer[i]) ^ 0x80);
                        if (-number != x) {
                            P_(-number) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, -number == x);
                    }

                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            T     TV       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putFloat64(ZCHARPTR, TV));
            ASSERT_SAFE_FAIL(MarshallingUtil::getFloat64(&TV, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getFloat64(ZTPTR, BUFFER));
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PUT/GET 8-BIT INTEGERS
        //   Verify put/get operations for 8-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 Only the least-significant byte is used by 'put'.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify results for multi-byte inputs to 'put' to ensure only
        //:   the least-significant byte is used.  (C-4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   putInt8(char *buf, int val);
        //   getInt8(char *var, const char *buf);
        //   getInt8(signed char *var, const char *buf);
        //   getInt8(unsigned char *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 8-BIT INTEGERS" << endl
                          << "======================" << endl;

        typedef char T;
        typedef unsigned char U;
        typedef signed char S;

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number                    Bit pattern
            //--  --------                  -------------------
            { L_,  0,                       "\x00"                      },
            { L_,  1,                       "\x01"                      },
            { L_, -1,                       "\xFF"                      },
            { L_, -2,                       "\xFE"                      },
            { L_,  0x71,                    "\x71"                      },
            { L_,  static_cast<T>(0x8C),    "\x8C"                      },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     xsignedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const S     signedNumber = static_cast<S>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 1;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt8(buffer + i, xsignedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(xsignedNumber & 0xff)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xa5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt8(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, xsignedNumber != x);
                        MarshallingUtil::getInt8(&x, buffer + i);
                        if (xsignedNumber != x) {
                            P_(xsignedNumber & 0xff) P(x & 0xff)
                        }
                        LOOP3_ASSERT(LINE, i, k, xsignedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        MarshallingUtil::getInt8(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber & 0xff) P(y & 0xff)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                    }
                    {
                        S z = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != z);
                        MarshallingUtil::getInt8(&z, buffer + i);
                        if (signedNumber != z) {
                            P_(signedNumber & 0xff) P(z & 0xff)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == z);
                   }
                }
            }
        }

        { // verify functionality for non-one-byte values
            const int SIZE = 1;
            char      buffer[SIZE + 1];

            buffer[0] = static_cast<char>(0xb3);
            buffer[SIZE] = static_cast<char>(0xa7);

            MarshallingUtil::putInt8(buffer, static_cast<int>(0));
            ASSERT(0 == memcmp("\x00\xa7", buffer, SIZE + 1));

            MarshallingUtil::putInt8(buffer, static_cast<int>(1));
            ASSERT(0 == memcmp("\x01\xa7", buffer, SIZE + 1));

            MarshallingUtil::putInt8(buffer, static_cast<int>(-1));
            ASSERT(0 == memcmp("\xff\xa7", buffer, SIZE + 1));

            MarshallingUtil::putInt8(buffer, static_cast<int>(0x010000));
            ASSERT(0 == memcmp("\x00\xa7", buffer, SIZE + 1));

            MarshallingUtil::putInt8(buffer, static_cast<int>(0x010001));
            ASSERT(0 == memcmp("\x01\xa7", buffer, SIZE + 1));

            MarshallingUtil::putInt8(buffer, static_cast<int>(0xfffffe00));
            ASSERT(0 == memcmp("\x00\xa7", buffer, SIZE + 1));

            MarshallingUtil::putInt8(buffer, static_cast<int>(0xfffffe01));
            ASSERT(0 == memcmp("\x01\xa7", buffer, SIZE + 1));
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            S    *ZSPTR    = static_cast<S *>(0);
            int   V        = 0;
            T     TV       = 0;
            U     TU       = 0;
            S     TS       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt8(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt8(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt8(&TU, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt8(&TS, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt8(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt8(ZUPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt8(ZSPTR, BUFFER));
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PUT/GET 16-BIT INTEGERS
        //   Verify put/get operations for 16-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt16(char *buf, int val);
        //   getInt16(short *var, const char *buf);
        //   getUint16(unsigned short *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 16-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef short          T;
        typedef unsigned short U;

        const T A = (0x01 << 8) + 0x02;                  // POSITIVE NUMBER

        const char *A_SPEC = "\x01\x02";

        const T B = static_cast<T>((0x80 << 8) + 0x70);  // NEGATIVE NUMBER

        const char *B_SPEC = "\x80\x70";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00"                              },
            { L_,  1,           "\x00\x01"                              },
            { L_, -1,           "\xFF\xFF"                              },
            { L_, -2,           "\xFF\xFE"                              },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 2;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {

                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt16(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt16(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt16(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        MarshallingUtil::getUint16(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber) P(y)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                    }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            int   V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt16(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt16(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint16(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt16(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint16(ZUPTR, BUFFER));
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PUT/GET 24-BIT INTEGERS
        //   Verify put/get operations for 24-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt24(char *buf, int val);
        //   getInt24(int *var, const char *buf);
        //   getUint24(unsigned int *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 24-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef int          T;
        typedef unsigned int U;
        const T A = ((((( // POSITIVE NUMBER
                         0x00 << 8) + 0x02) << 8) + 0x03) << 8) + 0x04;

        const char *A_SPEC = "\x02\x03\x04";

        const T B = ((((( // NEGATIVE NUMBER
                         0xff << 8) + 0x80) << 8) + 0x60) << 8) + 0x50;

        const char *B_SPEC = "\x80\x60\x50";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00\x00"                          },
            { L_,  1,           "\x00\x00\x01"                          },
            { L_, -1,           "\xFF\xFF\xFF"                          },
            { L_, -2,           "\xFF\xFF\xFE"                          },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 3;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt24(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt24(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp,
                                                  buffer + i,
                                                  SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt24(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            MarshallingUtil::getUint24(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                    }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            int   V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt24(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt24(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint24(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt24(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint24(ZUPTR, BUFFER));
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT/GET 32-BIT INTEGERS
        //   Verify put/get operations for 32-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt32(char *buf, int val);
        //   getInt32(int *var, const char *buf);
        //   getUint32(unsigned int *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 32-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef int          T;
        typedef unsigned int U;

        const T A = ((((( // POSITIVE NUMBER
                         0x01 << 8) + 0x02) << 8) + 0x03) << 8) + 0x04;

        const char *A_SPEC = "\x01\x02\x03\x04";

        const T B = ((((( // NEGATIVE NUMBER
                         0x80 << 8) + 0x70) << 8) + 0x60) << 8) + 0x50;

        const char *B_SPEC = "\x80\x70\x60\x50";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00\x00\x00"                      },
            { L_,  1,           "\x00\x00\x00\x01"                      },
            { L_, -1,           "\xFF\xFF\xFF\xFF"                      },
            { L_, -2,           "\xFF\xFF\xFF\xFE"                      },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 4;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt32(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt32(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt32(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        MarshallingUtil::getUint32(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber) P(y)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                    }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            int   V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt32(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt32(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint32(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt32(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint32(ZUPTR, BUFFER));
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PUT/GET 40-BIT INTEGERS
        //   Verify put/get operations for 40-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt40(char *buf, Int64 val);
        //   getInt40(Int64 *var, const char *buf);
        //   getUint40(Uint64 *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 40-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef bsls::Types::Int64  T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x00) << 8) + 0x01) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x01\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xff) << 8) + 0xff) << 8) + 0x80) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x40\x30\x20\x10";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00\x00\x00\x00"                  },
            { L_,  1,           "\x00\x00\x00\x00\x01"                  },
            { L_, -1,           "\xFF\xFF\xFF\xFF\xFF"                  },
            { L_, -2,           "\xFF\xFF\xFF\xFF\xFE"                  },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 5;


            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt40(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt40(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt40(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            MarshallingUtil::getUint40(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                    }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            T     V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt40(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt40(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint40(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt40(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint40(ZUPTR, BUFFER));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PUT/GET 48-BIT INTEGERS
        //   Verify put/get operations for 48-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt48(char *buf, Int64 val);
        //   getInt48(Int64 *var, const char *buf);
        //   getUint48(Uint64 *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 48-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef bsls::Types::Int64 T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x00) << 8) + 0x01) << 8) + 0x02) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x01\x02\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0xff) << 8) + 0x80) << 8) + 0x70) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x70\x40\x30\x20\x10";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00\x00\x00\x00\x00"              },
            { L_,  1,           "\x00\x00\x00\x00\x00\x01"              },
            { L_, -1,           "\xFF\xFF\xFF\xFF\xFF\xFF"              },
            { L_, -2,           "\xFF\xFF\xFF\xFF\xFF\xFE"              },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 6;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt48(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt48(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp,
                                                  buffer + i,
                                                  SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt48(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            MarshallingUtil::getUint48(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                    }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            T     V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt48(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt48(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint48(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt48(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint48(ZUPTR, BUFFER));
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PUT/GET 56-BIT INTEGERS
        //   Verify put/get operations for 56-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt56(char *buf, Int64 val);
        //   getInt56(Int64 *var, const char *buf);
        //   getUint56(Uint64 *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 56-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef bsls::Types::Int64  T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x00) << 8) + 0x02) << 8) + 0x03) << 8) + 0x04) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x02\x03\x04\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0xff) << 8) + 0x80) << 8) + 0x60) << 8) + 0x50) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x60\x50\x40\x30\x20\x10";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00\x00\x00\x00\x00\x00"          },
            { L_,  1,           "\x00\x00\x00\x00\x00\x00\x01"          },
            { L_, -1,           "\xFF\xFF\xFF\xFF\xFF\xFF\xFF"          },
            { L_, -2,           "\xFF\xFF\xFF\xFF\xFF\xFF\xFE"          },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 7;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt56(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt56(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp,
                                                  buffer + i,
                                                  SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt56(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        if (signedNumber > 0) {
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                            MarshallingUtil::getUint56(&y, buffer + i);
                            if (unsignedNumber != y) {
                                P_(unsignedNumber) P(y)
                            }
                            LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                        }
                   }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            T     V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt56(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt56(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint56(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt56(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint56(ZUPTR, BUFFER));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PUT/GET 64-BIT INTEGERS
        //   Verify put/get operations for 64-bit integers.
        //
        // Concerns:
        //: 1 Endianness of the platform does not affect 'put' encoding.
        //:
        //: 2 Alignment in the stream does not affect 'put' encoding.
        //:
        //: 3 'get' inverts the 'put'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of test vectors that explore key values.
        //:
        //: 2 Iterate over the table and compare the results to the
        //:   expected value and also invert the 'put' with 'get' and
        //:   verify the initial value is recovered.  (C-1, C-3)
        //:
        //: 3 Iterate over the iteration at various byte offsets.  (C-2)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   putInt64(char *buf, Int64 val);
        //   getInt64(Int64 *var, const char *buf);
        //   getUint64(Uint64 *var, const char *buf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT/GET 64-BIT INTEGERS" << endl
                          << "=======================" << endl;

        typedef bsls::Types::Int64  T;
        typedef bsls::Types::Uint64 U;

        const T A = ((((((((((((( static_cast<T>  // POSITIVE NUMBER
                         (0x01) << 8) + 0x02) << 8) + 0x03) << 8) + 0x04) << 8)
                        + 0x05) << 8) + 0x06) << 8) + 0x07) << 8) + 0x08;

        const char *A_SPEC = "\x01\x02\x03\x04\x05\x06\x07\x08";

        const T B = ((((((((((((( static_cast<T>  // NEGATIVE NUMBER
                         (0x80) << 8) + 0x70) << 8) + 0x60) << 8) + 0x50) << 8)
                        + 0x40) << 8) + 0x30) << 8) + 0x20) << 8) + 0x10;

        const char *B_SPEC = "\x80\x70\x60\x50\x40\x30\x20\x10";

        static const struct {
            int         d_lineNum;              // line number
            T           d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            { L_,  0,           "\x00\x00\x00\x00\x00\x00\x00\x00"      },
            { L_,  1,           "\x00\x00\x00\x00\x00\x00\x00\x01"      },
            { L_, -1,           "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"      },
            { L_, -2,           "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE"      },
            { L_,  A,           A_SPEC                                  },
            { L_,  B,           B_SPEC                                  },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int   LINE = DATA[di].d_lineNum;
            const T     signedNumber = DATA[di].d_number;
            const U     unsignedNumber = static_cast<U>(DATA[di].d_number);
            const char *exp = DATA[di].d_spec;
            const int   SIZE = 8;

            // Repeat tests at every alignment.

            char buffer[2 * SIZE];
            for (int i = 0; i < SIZE; ++i) {
                memset(buffer, '\x69', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt64(buffer + i, signedNumber);
                bool isEq = 0 == memcmp(exp, buffer + i, SIZE);
                if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                    const char *e = "little-endian ";
#else
                    const char *e = "BIG-ENDIAN ";
#endif
                    cout << e; P(signedNumber)
                    cout << "exp: "; pBytes(exp, SIZE) << endl;
                    cout << "act: "; pBytes(buffer + i, SIZE) << endl;
                }
                LOOP2_ASSERT(LINE, i, isEq);
                if (!isEq) break;               // no need to continue.

                memset(buffer, '\xA5', sizeof buffer);
                LOOP2_ASSERT(LINE, i, memcmp(exp, buffer + i, SIZE));

                MarshallingUtil::putInt64(buffer + i, unsignedNumber);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(exp, buffer + i, SIZE));

                static const T INITIAL_VALUES[] = { -99, +99 };

                const int NUM_VALUES = static_cast<int>(sizeof INITIAL_VALUES
                                                     / sizeof *INITIAL_VALUES);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    {
                        T x = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, signedNumber != x);
                        MarshallingUtil::getInt64(&x, buffer + i);
                        if (signedNumber != x) {
                            P_(signedNumber) P(x)
                        }
                        LOOP3_ASSERT(LINE, i, k, signedNumber == x);
                    }
                    {
                        U y = INITIAL_VALUES[k];
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber != y);
                        MarshallingUtil::getUint64(&y, buffer + i);
                        if (unsignedNumber != y) {
                            P_(unsignedNumber) P(y)
                        }
                        LOOP3_ASSERT(LINE, i, k, unsignedNumber == y);
                   }
                }
            }
        }

        { // negative testing
            char  BUFFER[32];
            char *ZCHARPTR = static_cast<char *>(0);
            T    *ZTPTR    = static_cast<T *>(0);
            U    *ZUPTR    = static_cast<U *>(0);
            T     V        = 0;
            T     TV       = 0;
            U     TU       = 0;

            bsls::AssertTestHandlerGuard guard;

            // invalid 'buffer'
            ASSERT_SAFE_FAIL(MarshallingUtil::putInt64(ZCHARPTR, V));
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt64(&TV, ZCHARPTR));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint64(&TU, ZCHARPTR));

            // invalid 'value'
            ASSERT_SAFE_FAIL(MarshallingUtil::getInt64(ZTPTR, BUFFER));
            ASSERT_SAFE_FAIL(MarshallingUtil::getUint64(ZUPTR, BUFFER));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VERIFY SINGLE-PRECISION FORMAT
        //   Ensure generated format for 32-bit floats is IEEE-compliant.
        //
        // Concerns:
        //: 1 Generated data for a floating-point value is IEEE-compliant.
        //
        // Plan:
        //: 1 Enumerate a sequence of test vectors and verify the result
        //:   matches the IEEE specification.
        //:
        //: 2 If the machine is little-endian, the bytes will be swapped
        //:   before comparison.  (C-1)
        //
        // Testing:
        //   EXPLORE FLOAT FORMAT -- make sure format is IEEE-COMPLIANT
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VERIFY SINGLE-PRECISION FORMAT" << endl
                          << "==============================" << endl;

        const double K = 1024;
        const double M = K * K;
        const double B = M * K;

        typedef float T;

        static const struct {
            int         d_lineNum;              // line number
            float       d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,  0.0,             "\x00\x00\x00\x00"                 },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_, -0.0,             "\x80\x00\x00\x00"                 },
#else
            { L_, -0.0,             "\x00\x00\x00\x00"                 },
#endif

            // small integers
            { L_,  5.0,             "\x40\xA0\x00\x00"                 },
            { L_,  4.0,             "\x40\x80\x00\x00"                 },
            { L_,  3.75,            "\x40\x70\x00\x00"                 },
            { L_,  3.5,             "\x40\x60\x00\x00"                 },
            { L_,  3.25,            "\x40\x50\x00\x00"                 },
            { L_,  3.0,             "\x40\x40\x00\x00"                 },
            { L_,  2.75,            "\x40\x30\x00\x00"                 },
            { L_,  2.5,             "\x40\x20\x00\x00"                 },
            { L_,  2.0,             "\x40\x00\x00\x00"                 },
            { L_,  1.5,             "\x3F\xC0\x00\x00"                 },
            { L_,  1.0,             "\x3F\x80\x00\x00"                 },

            // base-2 fractions
            { L_,  0.75,            "\x3F\x40\x00\x00"                 },
            { L_,  0.625,           "\x3F\x20\x00\x00"                 },
            { L_,  0.5,             "\x3F\x00\x00\x00"                 },
            { L_,  0.375,           "\x3E\xC0\x00\x00"                 },
            { L_,  0.25,            "\x3E\x80\x00\x00"                 },
            { L_,  0.125,           "\x3E\x00\x00\x00"                 },

            // larger integers
            { L_,  1020,            "\x44\x7F\x00\x00"                 },
            { L_,  1021,            "\x44\x7F\x40\x00"                 },
            { L_,  1022,            "\x44\x7F\x80\x00"                 },
            { L_,  1023,            "\x44\x7F\xC0\x00"                 },
            { L_,  1024,            "\x44\x80\x00\x00"                 },
            { L_,  1025,            "\x44\x80\x20\x00"                 },
            { L_,  1026,            "\x44\x80\x40\x00"                 },
            { L_,  1027,            "\x44\x80\x60\x00"                 },

            // really big integers (powers of 2)
            { L_, static_cast<T>(K),         "\x44\x80\x00\x00"        },
            { L_, static_cast<T>(M),         "\x49\x80\x00\x00"        },
            { L_, static_cast<T>(B),         "\x4E\x80\x00\x00"        },
            { L_, static_cast<T>(K * B),     "\x53\x80\x00\x00"        },
            { L_, static_cast<T>(M * B),     "\x58\x80\x00\x00"        },
            { L_, static_cast<T>(B * B),     "\x5D\x80\x00\x00"        },
            { L_, static_cast<T>(K * B * B), "\x62\x80\x00\x00"        },
            { L_, static_cast<T>(M * B * B), "\x67\x80\x00\x00"        },
            { L_, static_cast<T>(B * B * B), "\x6C\x80\x00\x00"        },

            // least significant bits of mantissa
            { L_, static_cast<T>( 1*M + 1),   "\x49\x80\x00\x08"       },
            { L_, static_cast<T>( 2*M + 1),   "\x4A\x00\x00\x04"       },
            { L_, static_cast<T>( 3*M + 1),   "\x4A\x40\x00\x04"       },
            { L_, static_cast<T>( 4*M + 1),   "\x4A\x80\x00\x02"       },
            { L_, static_cast<T>( 5*M + 1),   "\x4A\xA0\x00\x02"       },
            { L_, static_cast<T>( 6*M + 1),   "\x4A\xC0\x00\x02"       },
            { L_, static_cast<T>( 7*M + 1),   "\x4A\xE0\x00\x02"       },
            { L_, static_cast<T>( 8*M + 1),   "\x4B\x00\x00\x01"       },
            { L_, static_cast<T>( 9*M + 2),   "\x4B\x10\x00\x02"       },
            { L_, static_cast<T>(10*M + 5),   "\x4B\x20\x00\x05"       },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int    LINE   = DATA[di].d_lineNum;
            const float  number = DATA[di].d_number;
            const char  *exp    = DATA[di].d_spec;
            const int    SIZE   = static_cast<int>(sizeof number);

            float  tmp   = number;
            char  *bytes = reinterpret_cast<char *>(&tmp);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
            reverse(bytes, SIZE);
#endif

            bool isEq = 0 == memcmp(bytes, exp, SIZE);

            if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                const char *e = "little-endian ";
#else
                const char *e = "BIG-ENDIAN ";
#endif
                cout << e; P(number)
                cout << "exp: "; pBytes(exp, SIZE) << endl;
                cout << "act: "; pBytes(bytes, SIZE) << endl;
            }
            LOOP_ASSERT(LINE, isEq);

            // To avoid having to explicitly repeat negative numbers, if we
            // negate the number and toggle bit seven in the LSB the results
            // should be compatible.

            if (isEq) {         // If the original test failed skip this one.
                tmp = -number;
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                reverse(bytes, SIZE);
#endif
                char EXP[SIZE];
                memcpy(EXP, exp, SIZE);
                EXP[0] =
                   static_cast<char>(static_cast<unsigned int>(EXP[0]) ^ 0x80);
                LOOP_ASSERT(LINE, 0 == memcmp(bytes, EXP, SIZE));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VERIFY DOUBLE-PRECISION FORMAT
        //   Ensure generated format for 64-bit floats is IEEE-compliant.
        //
        // Concerns:
        //: 1 Generated data for a floating-point value is IEEE-compliant.
        //
        // Plan:
        //: 1 Enumerate a sequence of test vectors and verify the result
        //:   matches the IEEE specification.
        //:
        //: 2 If the machine is little-endian, the bytes will be swapped
        //:   before comparison.  (C-1)
        //
        // Testing:
        //   EXPLORE DOUBLE FORMAT -- make sure format is IEEE-COMPLIANT
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VERIFY DOUBLE-PRECISION FORMAT" << endl
                          << "==============================" << endl;

        const double K = 1024;
        const double M = K * K;
        const double B = M * K;

        static const struct {
            int         d_lineNum;              // line number
            double      d_number;               // literal number
            const char *d_spec;                 // expected bit pattern
        } DATA[] = {
            //L#  number        Bit pattern
            //--  --------      -------------------
            // test zero pattern and sign bit.
            { L_,   0.0,        "\x00\x00\x00\x00\x00\x00\x00\x00"      },
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VER_MAJOR >= 1400
    // Microsoft 2003 compiler does not support -0.0
            { L_,  -0.0,        "\x80\x00\x00\x00\x00\x00\x00\x00"      },
#else
            { L_,  -0.0,        "\x00\x00\x00\x00\x00\x00\x00\x00"      },
#endif

            // small integers
            { L_,   5.00,       "\x40\x14\x00\x00\x00\x00\x00\x00"      },
            { L_,   4.00,       "\x40\x10\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.75,       "\x40\x0E\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.5,        "\x40\x0C\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.25,       "\x40\x0A\x00\x00\x00\x00\x00\x00"      },
            { L_,   3.0 ,       "\x40\x08\x00\x00\x00\x00\x00\x00"      },
            { L_,   2.75,       "\x40\x06\x00\x00\x00\x00\x00\x00"      },
            { L_,   2.5,        "\x40\x04\x00\x00\x00\x00\x00\x00"      },
            { L_,   2.0,        "\x40\x00\x00\x00\x00\x00\x00\x00"      },
            { L_,   1.5,        "\x3F\xF8\x00\x00\x00\x00\x00\x00"      },
            { L_,   1.0,        "\x3F\xF0\x00\x00\x00\x00\x00\x00"      },

            // base-2 fractions
            { L_,   0.75,       "\x3F\xE8\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.625,      "\x3F\xE4\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.5,        "\x3F\xE0\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.375,      "\x3F\xD8\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.25,       "\x3F\xD0\x00\x00\x00\x00\x00\x00"      },
            { L_,   0.125,      "\x3F\xC0\x00\x00\x00\x00\x00\x00"      },

            // larger integers
            { L_,   1020,       "\x40\x8F\xE0\x00\x00\x00\x00\x00"      },
            { L_,   1021,       "\x40\x8F\xE8\x00\x00\x00\x00\x00"      },
            { L_,   1022,       "\x40\x8F\xF0\x00\x00\x00\x00\x00"      },
            { L_,   1023,       "\x40\x8F\xF8\x00\x00\x00\x00\x00"      },
            { L_,   1024,       "\x40\x90\x00\x00\x00\x00\x00\x00"      },
            { L_,   1025,       "\x40\x90\x04\x00\x00\x00\x00\x00"      },
            { L_,   1026,       "\x40\x90\x08\x00\x00\x00\x00\x00"      },
            { L_,   1027,       "\x40\x90\x0C\x00\x00\x00\x00\x00"      },

            // really big integers (powers of 2)
            { L_,   K,          "\x40\x90\x00\x00\x00\x00\x00\x00"      },
            { L_,   M,          "\x41\x30\x00\x00\x00\x00\x00\x00"      },
            { L_,   B,          "\x41\xD0\x00\x00\x00\x00\x00\x00"      },
            { L_,   K * B,      "\x42\x70\x00\x00\x00\x00\x00\x00"      },
            { L_,   M * B,      "\x43\x10\x00\x00\x00\x00\x00\x00"      },
            { L_,   B * B,      "\x43\xB0\x00\x00\x00\x00\x00\x00"      },
            { L_,   K * B * B,  "\x44\x50\x00\x00\x00\x00\x00\x00"      },
            { L_,   M * B * B,  "\x44\xF0\x00\x00\x00\x00\x00\x00"      },
            { L_,   B * B * B,  "\x45\x90\x00\x00\x00\x00\x00\x00"      },

            // least significant bits of mantissa
            { L_,   1*M*B + 1,  "\x43\x10\x00\x00\x00\x00\x00\x04"      },
            { L_,   2*M*B + 1,  "\x43\x20\x00\x00\x00\x00\x00\x02"      },
            { L_,   3*M*B + 1,  "\x43\x28\x00\x00\x00\x00\x00\x02"      },
            { L_,   4*M*B + 1,  "\x43\x30\x00\x00\x00\x00\x00\x01"      },
            { L_,   5*M*B + 2,  "\x43\x34\x00\x00\x00\x00\x00\x02"      },
            { L_,   6*M*B + 3,  "\x43\x38\x00\x00\x00\x00\x00\x03"      },
            { L_,   7*M*B + 4,  "\x43\x3C\x00\x00\x00\x00\x00\x04"      },
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int     LINE = DATA[di].d_lineNum;
            const double  number = DATA[di].d_number;
            const char   *exp = DATA[di].d_spec;
            const int     SIZE = static_cast<int>(sizeof number);

            double  tmp   = number;
            char   *bytes = reinterpret_cast<char *>(&tmp);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
            reverse(bytes, SIZE);
#endif

            bool isEq = 0 == memcmp(bytes, exp, SIZE);

            if (veryVerbose || !isEq) {
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                const char *e = "little-endian ";
#else
                const char *e = "BIG-ENDIAN ";
#endif
                cout << e; P(number)
                cout << "exp: "; pBytes(exp, SIZE) << endl;
                cout << "act: "; pBytes(bytes, SIZE) << endl;
            }
            LOOP_ASSERT(LINE, isEq);

            // To avoid having to explicitly repeat negative numbers, if we
            // negate the number and toggle bit seven in the LSB the results
            // should be compatible.

            if (isEq) {         // If the original test failed skip this one.
                tmp = -number;
#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
                reverse(bytes, SIZE);
#endif
                char EXP[SIZE];
                memcpy(EXP, exp, SIZE);
                EXP[0] =
                   static_cast<char>(static_cast<unsigned int>(EXP[0]) ^ 0x80);
                LOOP_ASSERT(LINE, 0 == memcmp(bytes, EXP, SIZE));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VERIFY TESTING APPARATUS
        //   Before we get started, let's make sure that the basic supporting
        //   test functions work as expected.
        //
        // Concerns:
        //: 1 Testing apparatus methods work as expected.
        //
        // Plan:
        //: 1 Directly verify functionality on a set of test vectors.  (C-1)
        //
        // Testing:
        //   SWAP FUNCTION: static inline void swap(T *x, T *y)
        //   REVERSE FUNCTION: void reverse(T *array, int numElements)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VERIFY TESTING APPARATUS" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting swap(T*, T*)" << endl;
        {
            {
                const char A = 1, B = 2;        ASSERT(1 == A); ASSERT(2 == B);

                char a = A, b = B;              ASSERT(A == a); ASSERT(B == b);

                swap(&a, &b);                   ASSERT(B == a); ASSERT(A == b);
                swap(&a, &b);                   ASSERT(A == a); ASSERT(B == b);
            }
            {
                const int A = 1000, B = 2000;   ASSERT(A == 1000);

                int a = A, b = B;               ASSERT(A == a); ASSERT(B == b);

                swap(&a, &b);                   ASSERT(B == a); ASSERT(A == b);
                swap(&a, &b);                   ASSERT(A == a); ASSERT(B == b);
            }
            {
                double A = 1e-10, B = 2e-10;    ASSERT(B == 2e-10);

                double a = A, b = B;            ASSERT(A == a); ASSERT(B == b);

                swap(&a, &b);                   ASSERT(B == a); ASSERT(A == b);
                swap(&a, &b);                   ASSERT(A == a); ASSERT(B == b);
            }
        }

        if (verbose) cout << "\nTesting reverse(T*, numElements)" << endl;
        {
            {
                int a[] = { 1 };
                int S = static_cast<int>(sizeof a / sizeof *a);
                ASSERT(1 == a[0]);
                reverse(a, S);
                ASSERT(1 == a[0]);
                reverse(a, S);
                ASSERT(1 == a[0]);
            }
            {
                char a[] = { 1, 2 };
                int  S = static_cast<int>(sizeof a / sizeof *a);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]);
                reverse(a, S);
                ASSERT(2 == a[0]); ASSERT(1 == a[1]);
                reverse(a, S);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]);
            }
            {
                float a[] = { 1, 2, 3 };
                int   S = static_cast<int>(sizeof a / sizeof *a);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
                reverse(a, S);
                ASSERT(3 == a[0]); ASSERT(2 == a[1]); ASSERT(1 == a[2]);
                reverse(a, S);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
            }
            {
                double a[] = { 1, 2, 3, 4, 5, 6 };
                int    S = static_cast<int>(sizeof a / sizeof *a);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
                ASSERT(4 == a[3]); ASSERT(5 == a[4]); ASSERT(6 == a[5]);
                reverse(a, S);
                ASSERT(6 == a[0]); ASSERT(5 == a[1]); ASSERT(4 == a[2]);
                ASSERT(3 == a[3]); ASSERT(2 == a[4]); ASSERT(1 == a[5]);
                reverse(a, S);
                ASSERT(1 == a[0]); ASSERT(2 == a[1]); ASSERT(3 == a[2]);
                ASSERT(4 == a[3]); ASSERT(5 == a[4]); ASSERT(6 == a[5]);
            }

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
