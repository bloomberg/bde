// bdex_byteoutstreamraw.t.cpp             -*-C++-*-

#include <bdex_byteoutstreamraw.h>

#include <bdex_outstreamfunctions.h>

#include <bsls_platformutil.h>             // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy(), memcmp(), strlen()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

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
// For all output methods in 'bdex_ByteOutStreamRaw', the formatting of the
// input value to its correct byte representation is delegated to another
// component.  We assume that this formatting has been rigorously tested and
// verified.  Therefore, we are concerned only with the proper placement and
// alignment of bytes in the output stream.  We verify these properties by
// inserting chosen "marker" bytes between each output method call, and ensure
// that the new output bytes are properly interleaved between the "marker"
// bytes.
//
// We have chosen the primary black-box manipulator for 'bdex_ByteOutStreamRaw'
// to be 'putInt8'.
//-----------------------------------------------------------------------------
// [ 2] bdex_ByteOutStreamRaw(char *buffer, int length);
// [ 2] ~bdex_ByteOutStreamRaw();
// [25] void invalidate();
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
// [ 2] void reserveCapacity(int newCapacity);
// [25] operator const void *() const;
// [ 3] const char* data();
// [ 3] int length();
//
// [ 4] ostream& operator<<(ostream& stream, const bdex_ByteOutStreamRaw&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [26] USAGE
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdex_ByteOutStreamRaw Obj;

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
//                                  MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 26: {
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

// int main(int argc, char **argv)
   {
//
//    // Create a stream and write out some values.
      char bigBuffer[10000];
      bdex_ByteOutStreamRaw outStream(bigBuffer, 10000);
      bdex_OutStreamFunctions::streamOut(outStream, 1, 0);
      bdex_OutStreamFunctions::streamOut(outStream, 2, 0);
      bdex_OutStreamFunctions::streamOut(outStream, 'c', 0);
      bdex_OutStreamFunctions::streamOut(outStream, bsl::string("hello"), 0);
//
//    // Verify the results on 'stdout'.
      const char *theChars = outStream.data();
      int length = outStream.length();
      if (verbose) {
          for(int i = 0; i < length; ++i) {
              if(isalnum(theChars[i]))
                  cout << "nextByte (char): " << theChars[i] << endl;
              else
                  cout << "nextByte (int): " << (int)theChars[i] << endl;
          }
      }
//     return 0;
   }

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // STREAM VALIDITY METHODS
        //
        // Testing:
        //   void invalidate();
        //   operator const void *() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INVALIDATE" << endl
                          << "==========" << endl;

        if (verbose) cout << "\nTesting invalidate." << endl;
        {
            char buffer[4096];
            Obj mX(buffer, sizeof buffer);  const Obj& X = mX;
                              ASSERT( X);

            mX.invalidate();  ASSERT(!X);
            mX.invalidate();  ASSERT(!X);
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // PUT LENGTH AND VERSION TEST:
        //
        // Testing:
        //   putLength(int value);
        //   putVersion(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT LENGTH AND VERSION TEST" << endl
                          << "===========================" << endl;
        {
            if (verbose) cout << "\nTesting putLength." << endl;
            {
                char buffer[4096];
                Obj x(buffer, sizeof buffer);
                                      x.putInt8(0xff);
                x.putLength(  1);     x.putInt8(0xfe);
                x.putLength(128);     x.putInt8(0xfd);
                x.putLength(127);     x.putInt8(0xfc);
                x.putLength(256);     x.putInt8(0xfb);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 7 * SIZEOF_INT8 + 2 * SIZEOF_INT32;
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(),
                               "\xff" "\x01" "\xfe"
                               "\x80\x00\x00\x80" "\xfd"
                               "\x7f" "\xfc"
                               "\x80\x00\x01\x00" "\xfb", NUM_BITS));
            }
        }

        {
            const int SIZE = SIZEOF_INT8;

            if (verbose) cout << "\nTesting putVersion." << endl;
            {
                char buffer[4096];
                Obj x(buffer, sizeof buffer);
                ASSERT(0 == x.length());
                x.putVersion(1);
                x.putVersion(2);
                x.putVersion(3);
                x.putVersion(4);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 4 * SIZE;
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(), "\x01\x02\x03\x04", NUM_BITS));
            }
            {
                char buffer[4096];
                Obj x(buffer, sizeof buffer);
                ASSERT(0 == x.length());
                x.putVersion(252);
                x.putVersion(253);
                x.putVersion(254);
                x.putVersion(255);
                if (veryVerbose) { P(x); }
                const int NUM_BYTES = 4 * SIZE;
                const int NUM_BITS = 8 * NUM_BYTES;
                ASSERT(NUM_BYTES == x.length());
                ASSERT(1 == eq(x.data(), "\xfc\xfd\xfe\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putArrayFloat64." << endl;
        {
            const double DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayFloat64(DATA, 0);     x.putInt8(0xff);
            x.putArrayFloat64(DATA, 1);     x.putInt8(0xfe);
            x.putArrayFloat64(DATA, 2);     x.putInt8(0xfd);
            x.putArrayFloat64(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                                 "\xff"
                   "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                   "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                   "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                   "\x3f\xf0\x00\x00\x00\x00\x00\x00"
                   "\x40\x00\x00\x00\x00\x00\x00\x00"
                   "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc", NUM_BITS));
        }
        {
            const double DATA[] = {1.5, 2.5, 3.5};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayFloat64(DATA, 0);     x.putInt8(0xfc);
            x.putArrayFloat64(DATA, 1);     x.putInt8(0xfd);
            x.putArrayFloat64(DATA, 2);     x.putInt8(0xfe);
            x.putArrayFloat64(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                                 "\xfc"
                   "\x3f\xf8\x00\x00\x00\x00\x00\x00" "\xfd"
                   "\x3f\xf8\x00\x00\x00\x00\x00\x00"
                   "\x40\x04\x00\x00\x00\x00\x00\x00" "\xfe"
                   "\x3f\xf8\x00\x00\x00\x00\x00\x00"
                   "\x40\x04\x00\x00\x00\x00\x00\x00"
                   "\x40\x0c\x00\x00\x00\x00\x00\x00" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putArrayFloat32." << endl;
        {
            const float DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayFloat32(DATA, 0);     x.putInt8(0xff);
            x.putArrayFloat32(DATA, 1);     x.putInt8(0xfe);
            x.putArrayFloat32(DATA, 2);     x.putInt8(0xfd);
            x.putArrayFloat32(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                 "\xff"
                   "\x3f\x80\x00\x00" "\xfe"
                   "\x3f\x80\x00\x00"
                   "\x40\x00\x00\x00" "\xfd"
                   "\x3f\x80\x00\x00"
                   "\x40\x00\x00\x00"
                   "\x40\x40\x00\x00" "\xfc", NUM_BITS));
        }
        {
            const float DATA[] = {1.5, 2.5, 3.5};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayFloat32(DATA, 0);     x.putInt8(0xfc);
            x.putArrayFloat32(DATA, 1);     x.putInt8(0xfd);
            x.putArrayFloat32(DATA, 2);     x.putInt8(0xfe);
            x.putArrayFloat32(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                 "\xfc"
                   "\x3f\xc0\x00\x00" "\xfd"
                   "\x3f\xc0\x00\x00"
                   "\x40\x20\x00\x00" "\xfe"
                   "\x3f\xc0\x00\x00"
                   "\x40\x20\x00\x00"
                   "\x40\x60\x00\x00" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putArrayInt64." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt64(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt64(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt64(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt64(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                                 "\xff"
                   "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt64(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt64(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt64(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt64(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                                 "\xfc"
                   "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x00\x00\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint64." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint64(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint64(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint64(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint64(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                                 "\xff"
                   "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint64(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint64(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint64(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint64(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                                 "\xfc"
                   "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x00\x00\x00\x06" "\xff",
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

        if (verbose) cout << "\nTesting putArrayInt56." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt56(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt56(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt56(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt56(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                             "\xff"
                   "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt56(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt56(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt56(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt56(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                             "\xfc"
                   "\x00\x00\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x00\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint56." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint56(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint56(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint56(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint56(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                             "\xff"
                   "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint56(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint56(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint56(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint56(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                             "\xfc"
                   "\x00\x00\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x00\x00\x06" "\xff",
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

        if (verbose) cout << "\nTesting putArrayInt48." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt48(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt48(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt48(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt48(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                         "\xff"
                   "\x00\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt48(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt48(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt48(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt48(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                         "\xfc"
                   "\x00\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint48." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint48(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint48(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint48(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint48(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                         "\xff"
                   "\x00\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint48(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint48(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint48(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint48(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                         "\xfc"
                   "\x00\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x00\x06" "\xff",
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

        if (verbose) cout << "\nTesting putArrayInt40." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt40(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt40(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt40(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt40(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                     "\xff"
                   "\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt40(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt40(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt40(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt40(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                     "\xfc"
                   "\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint40." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint40(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint40(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint40(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint40(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                     "\xff"
                   "\x00\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x00\x01"
                   "\x00\x00\x00\x00\x02"
                   "\x00\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint40(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint40(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint40(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint40(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                     "\xfc"
                   "\x00\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x00\x04"
                   "\x00\x00\x00\x00\x05"
                   "\x00\x00\x00\x00\x06" "\xff",
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

        if (verbose) cout << "\nTesting putArrayInt32." << endl;
        {
            const int DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt32(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt32(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt32(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt32(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                 "\xff"
                   "\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x01"
                   "\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x01"
                   "\x00\x00\x00\x02"
                   "\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const int DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt32(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt32(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt32(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt32(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                 "\xfc"
                   "\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x04"
                   "\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x04"
                   "\x00\x00\x00\x05"
                   "\x00\x00\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint32." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint32(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint32(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint32(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint32(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                 "\xff"
                   "\x00\x00\x00\x01" "\xfe"
                   "\x00\x00\x00\x01"
                   "\x00\x00\x00\x02" "\xfd"
                   "\x00\x00\x00\x01"
                   "\x00\x00\x00\x02"
                   "\x00\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const unsigned int DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint32(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint32(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint32(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint32(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                 "\xfc"
                   "\x00\x00\x00\x04" "\xfd"
                   "\x00\x00\x00\x04"
                   "\x00\x00\x00\x05" "\xfe"
                   "\x00\x00\x00\x04"
                   "\x00\x00\x00\x05"
                   "\x00\x00\x00\x06" "\xff",
                           NUM_BITS));
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

        if (verbose) cout << "\nTesting putArrayInt24." << endl;
        {
            const int DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt24(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt24(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt24(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt24(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""             "\xff"
                   "\x00\x00\x01" "\xfe"
                   "\x00\x00\x01"
                   "\x00\x00\x02" "\xfd"
                   "\x00\x00\x01"
                   "\x00\x00\x02"
                   "\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const int DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt24(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt24(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt24(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt24(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""             "\xfc"
                   "\x00\x00\x04" "\xfd"
                   "\x00\x00\x04"
                   "\x00\x00\x05" "\xfe"
                   "\x00\x00\x04"
                   "\x00\x00\x05"
                   "\x00\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint24." << endl;
        {
            const unsigned int DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint24(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint24(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint24(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint24(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""             "\xff"
                   "\x00\x00\x01" "\xfe"
                   "\x00\x00\x01"
                   "\x00\x00\x02" "\xfd"
                   "\x00\x00\x01"
                   "\x00\x00\x02"
                   "\x00\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const unsigned int DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint24(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint24(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint24(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint24(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""             "\xfc"
                   "\x00\x00\x04" "\xfd"
                   "\x00\x00\x04"
                   "\x00\x00\x05" "\xfe"
                   "\x00\x00\x04"
                   "\x00\x00\x05"
                   "\x00\x00\x06" "\xff",
                           NUM_BITS));
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

        if (verbose) cout << "\nTesting putArrayInt16." << endl;
        {
            const short DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt16(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt16(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt16(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt16(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                               "\xff"
                   "\x00\x01"                       "\xfe"
                   "\x00\x01" "\x00\x02"            "\xfd"
                   "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const short DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt16(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt16(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt16(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt16(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                               "\xfc"
                   "\x00\x04"                       "\xfd"
                   "\x00\x04" "\x00\x05"            "\xfe"
                   "\x00\x04" "\x00\x05" "\x00\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint16." << endl;
        {
            const unsigned short DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint16(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint16(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint16(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint16(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                               "\xff"
                   "\x00\x01"                       "\xfe"
                   "\x00\x01" "\x00\x02"            "\xfd"
                   "\x00\x01" "\x00\x02" "\x00\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const unsigned short DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint16(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint16(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint16(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint16(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                               "\xfc"
                   "\x00\x04"                       "\xfd"
                   "\x00\x04" "\x00\x05"            "\xfe"
                   "\x00\x04" "\x00\x05" "\x00\x06" "\xff",
                           NUM_BITS));
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

        if (verbose) cout << "\nTesting putArrayInt8(char *)." << endl;
        {
            const char DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt8(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt8(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt8(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt8(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xff"
                   "\x01"               "\xfe"
                   "\x01" "\x02"        "\xfd"
                   "\x01" "\x02" "\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const char DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt8(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt8(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt8(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt8(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                  "\xfc"
                   "\x04"              "\xfd"
                   "\x04" "\x05"       "\xfe"
                   "\x04" "\x05""\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayInt8(signed char *)." << endl;
        {
            const signed char DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt8(DATA, 0);     x.putInt8(0xff);
            x.putArrayInt8(DATA, 1);     x.putInt8(0xfe);
            x.putArrayInt8(DATA, 2);     x.putInt8(0xfd);
            x.putArrayInt8(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xff"
                   "\x01"               "\xfe"
                   "\x01" "\x02"        "\xfd"
                   "\x01" "\x02" "\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const signed char DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayInt8(DATA, 0);     x.putInt8(0xfc);
            x.putArrayInt8(DATA, 1);     x.putInt8(0xfd);
            x.putArrayInt8(DATA, 2);     x.putInt8(0xfe);
            x.putArrayInt8(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xfc"
                   "\x04"               "\xfd"
                   "\x04" "\x05"        "\xfe"
                   "\x04" "\x05" "\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint8(char *)." << endl;
        {
            const char DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint8(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint8(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint8(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint8(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xff"
                   "\x01"               "\xfe"
                   "\x01" "\x02"        "\xfd"
                   "\x01" "\x02" "\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const char DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint8(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint8(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint8(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint8(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xfc"
                   "\x04"               "\xfd"
                   "\x04" "\x05"        "\xfe"
                   "\x04" "\x05" "\x06" "\xff",
                           NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putArrayUint8(unsigned char *)."
                          << endl;
        {
            const unsigned char DATA[] = {1, 2, 3};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint8(DATA, 0);     x.putInt8(0xff);
            x.putArrayUint8(DATA, 1);     x.putInt8(0xfe);
            x.putArrayUint8(DATA, 2);     x.putInt8(0xfd);
            x.putArrayUint8(DATA, 3);     x.putInt8(0xfc);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xff"
                   "\x01"               "\xfe"
                   "\x01" "\x02"        "\xfd"
                   "\x01" "\x02" "\x03" "\xfc",
                           NUM_BITS));
        }
        {
            const unsigned char DATA[] = {4, 5, 6};
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putArrayUint8(DATA, 0);     x.putInt8(0xfc);
            x.putArrayUint8(DATA, 1);     x.putInt8(0xfd);
            x.putArrayUint8(DATA, 2);     x.putInt8(0xfe);
            x.putArrayUint8(DATA, 3);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZEOF_INT8 + 6 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                   ""                   "\xfc"
                   "\x04"               "\xfd"
                   "\x04" "\x05"        "\xfe"
                   "\x04" "\x05" "\x06" "\xff",
                           NUM_BITS));
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

        if (verbose) cout << "\nTesting putFloat64." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                 x.putInt8(0xff);
            x.putFloat64(1);     x.putInt8(0xfe);
            x.putFloat64(2);     x.putInt8(0xfd);
            x.putFloat64(3);     x.putInt8(0xfc);
            x.putFloat64(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x3f\xf0\x00\x00\x00\x00\x00\x00" "\xfe"
                       "\x40\x00\x00\x00\x00\x00\x00\x00" "\xfd"
                       "\x40\x08\x00\x00\x00\x00\x00\x00" "\xfc"
                       "\x40\x10\x00\x00\x00\x00\x00\x00" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                   x.putInt8(0xfb);
            x.putFloat64(1.5);     x.putInt8(0xfc);
            x.putFloat64(2.5);     x.putInt8(0xfd);
            x.putFloat64(3.5);     x.putInt8(0xfe);
            x.putFloat64(5.0);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x3f\xf8\x00\x00\x00\x00\x00\x00" "\xfc"
                       "\x40\x04\x00\x00\x00\x00\x00\x00" "\xfd"
                       "\x40\x0c\x00\x00\x00\x00\x00\x00" "\xfe"
                       "\x40\x14\x00\x00\x00\x00\x00\x00" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putFloat32." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                 x.putInt8(0xff);
            x.putFloat32(1);     x.putInt8(0xfe);
            x.putFloat32(2);     x.putInt8(0xfd);
            x.putFloat32(3);     x.putInt8(0xfc);
            x.putFloat32(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x3f\x80\x00\x00" "\xfe"
                       "\x40\x00\x00\x00" "\xfd"
                       "\x40\x40\x00\x00" "\xfc"
                       "\x40\x80\x00\x00" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                   x.putInt8(0xfb);
            x.putFloat32(1.5);     x.putInt8(0xfc);
            x.putFloat32(2.5);     x.putInt8(0xfd);
            x.putFloat32(3.5);     x.putInt8(0xfe);
            x.putFloat32(5.0);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x3f\xc0\x00\x00" "\xfc"
                       "\x40\x20\x00\x00" "\xfd"
                       "\x40\x60\x00\x00" "\xfe"
                       "\x40\xa0\x00\x00" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt64." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt64(1);     x.putInt8(0xfe);
            x.putInt64(2);     x.putInt8(0xfd);
            x.putInt64(3);     x.putInt8(0xfc);
            x.putInt64(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt64(5);     x.putInt8(0xfc);
            x.putInt64(6);     x.putInt8(0xfd);
            x.putInt64(7);     x.putInt8(0xfe);
            x.putInt64(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint64." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint64(1);     x.putInt8(0xfe);
            x.putUint64(2);     x.putInt8(0xfd);
            x.putUint64(3);     x.putInt8(0xfc);
            x.putUint64(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint64(5);     x.putInt8(0xfc);
            x.putUint64(6);     x.putInt8(0xfd);
            x.putUint64(7);     x.putInt8(0xfe);
            x.putUint64(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt56." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt56(1);     x.putInt8(0xfe);
            x.putInt56(2);     x.putInt8(0xfd);
            x.putInt56(3);     x.putInt8(0xfc);
            x.putInt56(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt56(5);     x.putInt8(0xfc);
            x.putInt56(6);     x.putInt8(0xfd);
            x.putInt56(7);     x.putInt8(0xfe);
            x.putInt56(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint56." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint56(1);     x.putInt8(0xfe);
            x.putUint56(2);     x.putInt8(0xfd);
            x.putUint56(3);     x.putInt8(0xfc);
            x.putUint56(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint56(5);     x.putInt8(0xfc);
            x.putUint56(6);     x.putInt8(0xfd);
            x.putUint56(7);     x.putInt8(0xfe);
            x.putUint56(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt48." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt48(1);     x.putInt8(0xfe);
            x.putInt48(2);     x.putInt8(0xfd);
            x.putInt48(3);     x.putInt8(0xfc);
            x.putInt48(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt48(5);     x.putInt8(0xfc);
            x.putInt48(6);     x.putInt8(0xfd);
            x.putInt48(7);     x.putInt8(0xfe);
            x.putInt48(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint48." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint48(1);     x.putInt8(0xfe);
            x.putUint48(2);     x.putInt8(0xfd);
            x.putUint48(3);     x.putInt8(0xfc);
            x.putUint48(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint48(5);     x.putInt8(0xfc);
            x.putUint48(6);     x.putInt8(0xfd);
            x.putUint48(7);     x.putInt8(0xfe);
            x.putUint48(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt40." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt40(1);     x.putInt8(0xfe);
            x.putInt40(2);     x.putInt8(0xfd);
            x.putInt40(3);     x.putInt8(0xfc);
            x.putInt40(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt40(5);     x.putInt8(0xfc);
            x.putInt40(6);     x.putInt8(0xfd);
            x.putInt40(7);     x.putInt8(0xfe);
            x.putInt40(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint40." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint40(1);     x.putInt8(0xfe);
            x.putUint40(2);     x.putInt8(0xfd);
            x.putUint40(3);     x.putInt8(0xfc);
            x.putUint40(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint40(5);     x.putInt8(0xfc);
            x.putUint40(6);     x.putInt8(0xfd);
            x.putUint40(7);     x.putInt8(0xfe);
            x.putUint40(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x00\x08" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt32." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt32(1);     x.putInt8(0xfe);
            x.putInt32(2);     x.putInt8(0xfd);
            x.putInt32(3);     x.putInt8(0xfc);
            x.putInt32(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt32(5);     x.putInt8(0xfc);
            x.putInt32(6);     x.putInt8(0xfd);
            x.putInt32(7);     x.putInt8(0xfe);
            x.putInt32(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint32." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint32(1);     x.putInt8(0xfe);
            x.putUint32(2);     x.putInt8(0xfd);
            x.putUint32(3);     x.putInt8(0xfc);
            x.putUint32(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x00\x01" "\xfe"
                       "\x00\x00\x00\x02" "\xfd"
                       "\x00\x00\x00\x03" "\xfc"
                       "\x00\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint32(5);     x.putInt8(0xfc);
            x.putUint32(6);     x.putInt8(0xfd);
            x.putUint32(7);     x.putInt8(0xfe);
            x.putUint32(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x00\x05" "\xfc"
                       "\x00\x00\x00\x06" "\xfd"
                       "\x00\x00\x00\x07" "\xfe"
                       "\x00\x00\x00\x08" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt24." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt24(1);     x.putInt8(0xfe);
            x.putInt24(2);     x.putInt8(0xfd);
            x.putInt24(3);     x.putInt8(0xfc);
            x.putInt24(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x01" "\xfe"
                       "\x00\x00\x02" "\xfd"
                       "\x00\x00\x03" "\xfc"
                       "\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt24(5);     x.putInt8(0xfc);
            x.putInt24(6);     x.putInt8(0xfd);
            x.putInt24(7);     x.putInt8(0xfe);
            x.putInt24(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x05" "\xfc"
                       "\x00\x00\x06" "\xfd"
                       "\x00\x00\x07" "\xfe"
                       "\x00\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint24." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint24(1);     x.putInt8(0xfe);
            x.putUint24(2);     x.putInt8(0xfd);
            x.putUint24(3);     x.putInt8(0xfc);
            x.putUint24(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x00\x01" "\xfe"
                       "\x00\x00\x02" "\xfd"
                       "\x00\x00\x03" "\xfc"
                       "\x00\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint24(5);     x.putInt8(0xfc);
            x.putUint24(6);     x.putInt8(0xfd);
            x.putUint24(7);     x.putInt8(0xfe);
            x.putUint24(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x00\x05" "\xfc"
                       "\x00\x00\x06" "\xfd"
                       "\x00\x00\x07" "\xfe"
                       "\x00\x00\x08" "\xff", NUM_BITS));
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

        if (verbose) cout << "\nTesting putInt16." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xff);
            x.putInt16(1);     x.putInt8(0xfe);
            x.putInt16(2);     x.putInt8(0xfd);
            x.putInt16(3);     x.putInt8(0xfc);
            x.putInt16(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x01" "\xfe"
                       "\x00\x02" "\xfd"
                       "\x00\x03" "\xfc"
                       "\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                               x.putInt8(0xfb);
            x.putInt16(5);     x.putInt8(0xfc);
            x.putInt16(6);     x.putInt8(0xfd);
            x.putInt16(7);     x.putInt8(0xfe);
            x.putInt16(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x05" "\xfc"
                       "\x00\x06" "\xfd"
                       "\x00\x07" "\xfe"
                       "\x00\x08" "\xff", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint16." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xff);
            x.putUint16(1);     x.putInt8(0xfe);
            x.putUint16(2);     x.putInt8(0xfd);
            x.putUint16(3);     x.putInt8(0xfc);
            x.putUint16(4);     x.putInt8(0xfb);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xff" "\x00\x01" "\xfe"
                       "\x00\x02" "\xfd"
                       "\x00\x03" "\xfc"
                       "\x00\x04" "\xfb", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
                                x.putInt8(0xfb);
            x.putUint16(5);     x.putInt8(0xfc);
            x.putUint16(6);     x.putInt8(0xfd);
            x.putUint16(7);     x.putInt8(0xfe);
            x.putUint16(8);     x.putInt8(0xff);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 5 * SIZEOF_INT8 + 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(),
                "\xfb" "\x00\x05" "\xfc"
                       "\x00\x06" "\xfd"
                       "\x00\x07" "\xfe"
                       "\x00\x08" "\xff", NUM_BITS));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT OPERATOR TEST:
        //   For each of a small representative set of objects, use
        //   'ostrstream' to write that object's value to a string buffer
        //   and then compare this buffer with the expected output format.
        //
        // Testing:
        //   ostream& operator<<(ostream&, const bdex_ByteOutStreamRaw&);
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
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
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
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putInt8(0);  x.putInt8(1);  x.putInt8(2);  x.putInt8(3);
            const char *EXPECTED =
                "\n0000\t00000000 00000001 00000010 00000011";
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
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.putInt8(0);  x.putInt8(1);  x.putInt8(2);  x.putInt8(3);
            x.putInt8(4);  x.putInt8(5);  x.putInt8(6);  x.putInt8(7);
            x.putInt8(8);  x.putInt8(9);  x.putInt8(10); x.putInt8(11);
            const char *EXPECTED =
                "\n0000\t00000000 00000001 00000010 00000011 "
                        "00000100 00000101 00000110 00000111"
                "\n0008\t00001000 00001001 00001010 00001011";
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
        //   For each independent test, use the default ctor to create an
        //   empty object.  Use function 'putInt8' to change the object's
        //   state, then verify that each of the basic accessors returns
        //   the correct value.
        //
        // Testing:
        //   length();
        //   data();
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
        const int NUM_TEST = sizeof DATA / sizeof *DATA;
        for (int iLen = 0; iLen < NUM_TEST; iLen++) {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            for (int j = 0; j < iLen; j++) x.putInt8(j);

            if (veryVerbose) { P_(iLen); P(x); }
            const int bytes = iLen * SIZEOF_INT8;
            // verify length()
            LOOP_ASSERT(iLen, x.length() == bytes);
            // verify data()
            LOOP_ASSERT(iLen, 1 == eq(x.data(), DATA[iLen], bytes * 8));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST:
        //    To test the 'putInt8' we use first a constructor that sets the
        //    initial size of the underlying buffer, and then we use a default
        //    constructor and a 'reserveCapacity' method to set the size of
        //    the underlying buffer.  We use a primary manipulator function
        //    'putInt8' to set its state.  Verify the correctness of this
        //    function using the basic accessors 'length' and 'data'.  Note
        //    that the destructor is exercised on each configuration as the
        //    object being tested leaves scope.  We also make sure that initial
        //    size was set and no more allocations were necessary during the
        //    test.
        //
        //    We have chosen to test function 'putUint8' here for compactness
        //    and relevancy, though it is not needed until later.
        //
        // Testing:
        //   bdex_ByteOutStreamRaw(char *buffer, int length);
        //   ~bdex_ByteOutStreamRaw();  // via purify
        //   putInt8(int value);
        //   putUint8(int value);
        //   removeAll();
        //   reserveCapacity(int newCapacity);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        const int SIZE = SIZEOF_INT8;

        if (verbose) cout << "\nTesting putInt8, reserveCapacity, and "
                          << "constructor." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            ASSERT(0 == x.length());
            x.putInt8(1);
            x.putInt8(2);
            x.putInt8(3);
            x.putInt8(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(), "\x01\x02\x03\x04", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            x.reserveCapacity(100);
            ASSERT(0 == x.length());
            x.putInt8(5);
            x.putInt8(6);
            x.putInt8(7);
            x.putInt8(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(), "\x05\x06\x07\x08", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting putUint8." << endl;
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            ASSERT(0 == x.length());
            x.putUint8(1);
            x.putUint8(2);
            x.putUint8(3);
            x.putUint8(4);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(), "\x01\x02\x03\x04", NUM_BITS));
        }
        {
            char buffer[4096];
            Obj x(buffer, sizeof buffer);
            ASSERT(0 == x.length());
            x.putUint8(5);
            x.putUint8(6);
            x.putUint8(7);
            x.putUint8(8);
            if (veryVerbose) { P(x); }
            const int NUM_BYTES = 4 * SIZE;
            const int NUM_BITS = 8 * NUM_BYTES;
            ASSERT(NUM_BYTES == x.length());
            ASSERT(1 == eq(x.data(), "\x05\x06\x07\x08", NUM_BITS));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting removeAll()." << endl;
        {
            const int NUM_TEST = 5;
            for (int iLen = 1; iLen < NUM_TEST; iLen++) {
                char buffer[4096];
                Obj x(buffer, sizeof buffer);
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
        //   Create 'bdex_ByteOutStreamRaw' objects using default and copy
        //   constructors.  Exercise these objects using some "put" methods,
        //   basic accessors, equality operators, and the assignment operator.
        //   Display object values frequently in verbose mode.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nCreate object x1 using default ctor." << endl;
        char buffer[4096];
        Obj x1(buffer, sizeof buffer);
        ASSERT(0 == x1.length());

        if (verbose) cout << "\nTry putInt32 with x1." << endl;
        x1.putInt32(1);
        if (veryVerbose) { P(x1); }
        ASSERT(SIZEOF_INT32 == x1.length());
        ASSERT(1 == eq("\x00\x00\x00\x01",
                       x1.data(), SIZEOF_INT32 * 8));

        if (verbose) cout << "\nClear x1 using removeAll method." << endl;
        x1.removeAll();
        ASSERT(0 == x1.length());

        if (verbose) cout << "\nTry putArrayInt8 with x1." << endl;
        const char data[] = {0x01, 0x02, 0x03, 0x04};
        const int size = sizeof data / sizeof *data;
        x1.putArrayInt8(data, size);
        if (veryVerbose) { P(x1); }
        ASSERT(SIZEOF_INT8 * size == x1.length());
        ASSERT(1 == eq("\x01\x02\x03\x04", x1.data(), size * 8));
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
