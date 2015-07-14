// bdlb_bigendian.t.cpp                                              -*-C++-*-
#include <bdlb_bigendian.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <arpa/inet.h> // not a component
#else
#include <winsock2.h> // not a component
#endif

#include <bdlxxxx_instreamfunctions.h>             // for testing only
#include <bdlxxxx_outstreamfunctions.h>            // for testing only
#include <bdlxxxx_testinstream.h>                  // for testing only
#include <bdlxxxx_testinstreamexception.h>         // for testing only
#include <bdlxxxx_testoutstream.h>                 // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>      // isdigit() isupper() islower()
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// All classes in this component are a value-semantic types that represent
// big-endian integer types.  They have the same value if they have the
// same in-core big endian representation.
//--------------------------------------------------------------------------
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
// [ 2] static bdlb::BigEndianInt16::make(short);
// [ 2] static bdlb::BigEndianUint16::make(unsigned short);
// [ 2] static bdlb::BigEndianInt32::make(int);
// [ 2] static bdlb::BigEndianUint32::make(unsigned int);
// [ 2] static bdlb::BigEndianInt64::make(bsls::Types::Int64);
// [ 2] static bdlb::BigEndianUint64::make(bsls::Types::Uint64);
//
// CREATORS
//
// MANIPULATORS
// [10] STREAM& bdexStreamIn(STREAM&, Obj&);
// [10] STREAM& bdexStreamOut(STREAM&, const Obj&) const;
//
// ACCESSORS
// [ 4] bdlb::BigEndianInt16::operator  short() const;
// [ 4] bdlb::BigEndianUint16::operator unsigned short() const;
// [ 4] bdlb::BigEndianInt32::operator  int() const;
// [ 4] bdlb::BigEndianUint32::operator unsigned int() const;
// [ 4] bdlb::BigEndianInt64::operator  Int64() const;
// [ 4] bdlb::BigEndianUint64::operator Uint64() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const bdlb::BigEndianInt16& lhs, rhs);
// [ 6] bool operator!=(const bdlb::BigEndianInt16& lhs, rhs);
// [ 6] bool operator==(const bdlb::BigEndianUint16& lhs, rhs);
// [ 6] bool operator!=(const bdlb::BigEndianUint16& lhs, rhs);
// [ 6] bool operator==(const bdlb::BigEndianInt32& lhs, rhs);
// [ 6] bool operator!=(const bdlb::BigEndianInt32& lhs, rhs);
// [ 6] bool operator==(const bdlb::BigEndianUint32& lhs, rhs);
// [ 6] bool operator!=(const bdlb::BigEndianUint32& lhs, rhs);
// [ 6] bool operator==(const bdlb::BigEndianInt64& lhs, rhs);
// [ 6] bool operator!=(const bdlb::BigEndianInt64& lhs, rhs);
// [ 6] bool operator==(const bdlb::BigEndianUint64& lhs, rhs);
// [ 6] bool operator!=(const bdlb::BigEndianUint64& lhs, rhs);
// [10] operator>>(bdlxxxx::InStream&, Obj&);
// [10] operator<<(bdlxxxx::OutStream&, const Obj&);
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST

//==========================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;

bool isInCoreValueCorrect(short value, const bdlb::BigEndianInt16& obj)
{
   short temp = htons(value);
   return obj == *reinterpret_cast<bdlb::BigEndianInt16*>(&temp);
}

bool isInCoreValueCorrect(unsigned short               value,
                          const bdlb::BigEndianUint16& obj)
{
   unsigned short temp = htons(value);
   return obj == *reinterpret_cast<bdlb::BigEndianUint16*>(&temp);
}

bool isInCoreValueCorrect(int value, const bdlb::BigEndianInt32& obj)
{
   int temp = htonl(value);
   return obj == *reinterpret_cast<bdlb::BigEndianInt32*>(&temp);
}

bool isInCoreValueCorrect(unsigned int value, const bdlb::BigEndianUint32& obj)
{
   unsigned int temp = htonl(value);
   return obj == *reinterpret_cast<bdlb::BigEndianUint32*>(&temp);
}

// __bswap_64 exists on Linux, we will use to ensure consistency.  Note
// that reusing the same function ensures the correctness of swap64 o swap64
// only.

#ifndef __bswap_64
bsls::Types::Uint64 swap64(bsls::Types::Uint64 value) {
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    return ((value & 0xff00000000000000ull) >> 56)
        |  ((value & 0x00ff000000000000ull) >> 40)
        |  ((value & 0x0000ff0000000000ull) >> 24)
        |  ((value & 0x000000ff00000000ull) >> 8)
        |  ((value & 0x00000000ff000000ull) << 8)
        |  ((value & 0x0000000000ff0000ull) << 24)
        |  ((value & 0x000000000000ff00ull) << 40)
        |  ((value & 0x00000000000000ffull) << 56);
#else
    return value;
#endif
}
#else
bsls::Types::Uint64 swap64(bsls::Types::Uint64 value) {
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    return __bswap_64(value);
#else
    return value;
#endif
}
#endif

bool isInCoreValueCorrect(bsls::Types::Int64 value,
                          const bdlb::BigEndianInt64& obj)
{
    bsls::Types::Int64 temp = swap64(value);
    return obj == *reinterpret_cast<bdlb::BigEndianInt64*>(&temp);
}

bool isInCoreValueCorrect(bsls::Types::Uint64          value,
                          const bdlb::BigEndianUint64& obj)
{
    bsls::Types::Uint64 temp = swap64(value);
    return obj == *reinterpret_cast<bdlb::BigEndianUint64*>(&temp);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //   That all of the classes defined in the component under test have
        //   the expected traits declared.
        //
        // Plan:
        //   Using the 'bslalg::HasTrait' meta-function, verify that the 6
        //   classes defined in the component under test define the expected
        //   traits, namely 'bslalg::TypeTraitBitwiseCopyable' and
        //   'bdlb::TypeTraitHasPrintMethod'.
        //
        // Testing:
        //   bslalg::TypeTraitBitwiseCopyable
        //   bdlb::TypeTraitHasPrintMethod
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Traits"
                          << "\n==============" << endl;

        if (verbose) cout << "\nTesting bdeut_BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            ASSERT((1 ==
              bslalg::HasTrait<Obj, bslalg::TypeTraitBitwiseCopyable>::VALUE));
            ASSERT((1 ==
                bslalg::HasTrait<Obj, bdlb::TypeTraitHasPrintMethod>::VALUE));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            ASSERT((1 ==
              bslalg::HasTrait<Obj, bslalg::TypeTraitBitwiseCopyable>::VALUE));
            ASSERT((1 ==
                bslalg::HasTrait<Obj, bdlb::TypeTraitHasPrintMethod>::VALUE));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            ASSERT((1 ==
              bslalg::HasTrait<Obj, bslalg::TypeTraitBitwiseCopyable>::VALUE));
            ASSERT((1 ==
                bslalg::HasTrait<Obj, bdlb::TypeTraitHasPrintMethod>::VALUE));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            ASSERT((1 ==
              bslalg::HasTrait<Obj, bslalg::TypeTraitBitwiseCopyable>::VALUE));
            ASSERT((1 ==
                bslalg::HasTrait<Obj, bdlb::TypeTraitHasPrintMethod>::VALUE));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            ASSERT((1 ==
              bslalg::HasTrait<Obj, bslalg::TypeTraitBitwiseCopyable>::VALUE));
            ASSERT((1 ==
                bslalg::HasTrait<Obj, bdlb::TypeTraitHasPrintMethod>::VALUE));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            ASSERT((1 ==
              bslalg::HasTrait<Obj, bslalg::TypeTraitBitwiseCopyable>::VALUE));
            ASSERT((1 ==
                bslalg::HasTrait<Obj, bdlb::TypeTraitHasPrintMethod>::VALUE));
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective member functions 'bdexStreamOut' and
        //      'bdexStreamIn'.
        //   2. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //
        // Plan:
        //   To address concern 1, perform a trivial direct (breathing) test of
        //   the 'bdexStreamOut' and 'bdexStreamIn' methods.  Note that the
        //   rest of the testing will use the stream operators.
        //
        //   To address concern 2, specify a set S of unique object values with
        //   substantial and varied differences, ordered by increasing
        //   complexity.
        //
        //   VALID STREAMS (and exceptions)
        //     Using all combinations of (u, v) in S X S, stream-out the value
        //     of u into a buffer and stream it back into (an independent
        //     object of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in S, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream became invalid immediately
        //     after the first incomplete read.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fails every field,
        //     including a char (multi-byte representation).  Hence we need to
        //     produce values that are inconsistent with a valid value and
        //     verify that they are detected.  Use the underlying stream
        //     package to simulate a typical valid (control) stream and verify
        //     that it can be streamed in successfully.  Then for each data
        //     field in the stream (beginning with the version number), provide
        //     one or more similar tests with that data field corrupted.  After
        //     each test, verify that the object is unchanged after streaming.
        //
        // Testing:
        //   static int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM&, Obj&);
        //   STREAM& bdexStreamOut(STREAM&, const Obj&) const;
        //   operator>>(bdlxxxx::InStream&, Obj&);
        //   operator<<(bdlxxxx::OutStream&, const Obj&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nTesting bdeut_BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                if (verbose) cout << "\tusing object syntax:" << endl;
                const Obj X = Obj::make(1);
                ASSERT(1 == X.maxSupportedBdexVersion());
                if (verbose) cout << "\tusing class method syntax:" << endl;
                ASSERT(1 == Obj::maxSupportedBdexVersion());
            }

            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                const int version = 1;
                Obj mX = Obj::make(123);
                const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                bdlxxxx::TestOutStream out; X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                if (verbose) cout << "\nDirect test that the value is streamed"
                                  << " in big endian order, byte by byte"
                                  << endl;
                if (veryVerbose) cout << "\nTesting that the size of the data"
                                      << " streamed is the same as the size of"
                                      << " the original data."
                                      << endl;
                // big endian representation of the value being tested
                const char TD[]  = {0,123};

                // bdlxxxx::TestOutStream fills one byte more with type info
                // plus four bytes for the size.
                if (veryVeryVerbose) cout << "\n\tsizeof(TD) " << sizeof(TD)
                                          << " sizeof(OD) "    << LOD
                                          << endl;

                ASSERT(sizeof(TD) == LOD - 5);

                if (veryVerbose) cout << "\nTesting that the streamed data"
                                      << " and the original data coincide"
                                      << endl;

                if (veryVeryVerbose) {
                    cout << "\n\t  OD:\t  TD:" << endl;
                    for(int i = 5; i < LOD; ++i) {
                        cout << "\t  [" << (int)OD[i]   << "]"
                             << "\t  [" << (int)TD[i-5] << "]"
                             << endl;
                    }
                }

                for(int i = 5; i < LOD; ++i) {
                    LOOP_ASSERT(i, OD[i] == TD[i-5]);
                }

                bdlxxxx::TestInStream in(OD, LOD); ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);

            }

            const short VALUES[] = {1, SHRT_MAX, SHRT_MIN, -1, 0, 123, -123};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]);
                        const Obj& U = mU;
                        bdlxxxx::TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, 1);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdlxxxx::TestInStream testInStream(OD, LOD);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, !testInStream.isEmpty());
                        testInStream.setSuppressVersionCheck(1);

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;
                        testInStream.reset();
                        LOOP2_ASSERT(ui, vi, testInStream);
                        LOOP2_ASSERT(ui, vi, !testInStream.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, VV == V);
                        LOOP2_ASSERT(ui, vi, (ui == vi) == (U == V));

                        // Testing stream-in operator here.
                        bdex_InStreamFunctions::streamIn(testInStream, mV, 1);

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, UU == V);
                        LOOP2_ASSERT(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                if (verbose) cout << "\tusing object syntax:" << endl;
                const Obj X = Obj::make(1);
                ASSERT(1 == X.maxSupportedBdexVersion());
                if (verbose) cout << "\tusing class method syntax:" << endl;
                ASSERT(1 == Obj::maxSupportedBdexVersion());
            }

            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                const int version = 1;
                Obj mX = Obj::make(123);
                const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                bdlxxxx::TestOutStream out;  X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                bdlxxxx::TestInStream in(OD, LOD); ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);     ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);
            }

            const unsigned short VALUES[] = {1, USHRT_MAX, 0, 0x48, 0x4800};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]);
                        const Obj& U = mU;
                        bdlxxxx::TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, 1);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdlxxxx::TestInStream testInStream(OD, LOD);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, !testInStream.isEmpty());
                        testInStream.setSuppressVersionCheck(1);

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;
                        testInStream.reset();
                        LOOP2_ASSERT(ui, vi, testInStream);
                        LOOP2_ASSERT(ui, vi, !testInStream.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, VV == V);
                        LOOP2_ASSERT(ui, vi, (ui == vi) == (U == V));

                        bdex_InStreamFunctions::streamIn(testInStream, mV, 1);

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, UU == V);
                        LOOP2_ASSERT(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                if (verbose) cout << "\tusing object syntax:" << endl;
                const Obj X = Obj::make(1);
                ASSERT(1 == X.maxSupportedBdexVersion());
                if (verbose) cout << "\tusing class method syntax:" << endl;
                ASSERT(1 == Obj::maxSupportedBdexVersion());
            }

            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                const int version = 1;
                Obj mX = Obj::make(123);
                const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                bdlxxxx::TestOutStream out;  X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                if (verbose) cout << "\nDirect test that the value is streamed"
                                  << " in big endian order, byte by byte"
                                  << endl;
                if (veryVerbose) cout << "\nTesting that the size of the data"
                                      << " streamed is the same as the size of"
                                      << " the original data."
                                      << endl;
                // big endian representation of the value being tested
                const char TD[]  = {0,0,0,123};

                // bdlxxxx::TestOutStream fills one byte more with type info
                // plus four bytes for the size.
                if (veryVeryVerbose) cout << "\n\tsizeof(TD) " << sizeof(TD)
                                          << " sizeof(OD) "    << LOD
                                          << endl;

                ASSERT( sizeof(TD) == LOD - 5);

                if (veryVerbose) cout << "\nTesting that the streamed data"
                                      << " and the original data coincide"
                                      << endl;

                if (veryVeryVerbose) {
                    cout << "\n\t  OD:\t  TD:" << endl;
                    for(int i = 5; i < LOD; ++i) {
                        cout << "\t  [" << (int)OD[i]   << "]"
                             << "\t  [" << (int)TD[i-5] << "]"
                             << endl;
                    }
                }

                for(int i = 5; i < LOD; ++i) {
                    LOOP_ASSERT(i, OD[i] == TD[i-5]);
                }

                bdlxxxx::TestInStream in(OD, LOD); ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);

           }

            const int VALUES[] = {1, INT_MAX, INT_MIN, -1, 0, 123, -123,
                                    SHRT_MAX, SHRT_MIN};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]);
                        const Obj& U = mU;
                        bdlxxxx::TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, 1);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdlxxxx::TestInStream testInStream(OD, LOD);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, !testInStream.isEmpty());
                        testInStream.setSuppressVersionCheck(1);

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;
                        testInStream.reset();
                        LOOP2_ASSERT(ui, vi, testInStream);
                        LOOP2_ASSERT(ui, vi, !testInStream.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, VV == V);
                        LOOP2_ASSERT(ui, vi, (ui == vi) == (U == V));

                        bdex_InStreamFunctions::streamIn(testInStream, mV, 1);

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, UU == V);
                        LOOP2_ASSERT(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                if (verbose) cout << "\tusing object syntax:" << endl;
                const Obj X = Obj::make(1);
                ASSERT(1 == X.maxSupportedBdexVersion());
                if (verbose) cout << "\tusing class method syntax:" << endl;
                ASSERT(1 == Obj::maxSupportedBdexVersion());
            }

            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                const int version = 1;
                Obj mX = Obj::make(123);
                const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                bdlxxxx::TestOutStream out;  X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                bdlxxxx::TestInStream in(OD, LOD); ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);     ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);
            }

            const unsigned int VALUES[] = {1, USHRT_MAX, 0, UINT_MAX,
                                             0xC33C, 0xC33C0000};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]);
                        const Obj& U = mU;
                        bdlxxxx::TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, 1);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdlxxxx::TestInStream testInStream(OD, LOD);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, !testInStream.isEmpty());
                        testInStream.setSuppressVersionCheck(1);

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;
                        testInStream.reset();
                        LOOP2_ASSERT(ui, vi, testInStream);
                        LOOP2_ASSERT(ui, vi, !testInStream.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, VV == V);
                        LOOP2_ASSERT(ui, vi, (ui == vi) == (U == V));

                        bdex_InStreamFunctions::streamIn(testInStream, mV, 1);

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, UU == V);
                        LOOP2_ASSERT(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                if (verbose) cout << "\tusing object syntax:" << endl;
                const Obj X = Obj::make(1);
                ASSERT(1 == X.maxSupportedBdexVersion());
                if (verbose) cout << "\tusing class method syntax:" << endl;
                ASSERT(1 == Obj::maxSupportedBdexVersion());
            }

            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                const int version = 1;
                Obj mX = Obj::make(123);
                const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                bdlxxxx::TestOutStream out;  X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();
                if (verbose) cout << "\nDirect test that the value is streamed"
                                  << " in big endian order, byte by byte"
                                  << endl;
                if (veryVerbose) cout << "\nTesting that the size of the data"
                                      << " streamed is the same as the size of"
                                      << " the original data."
                                      << endl;
                // big endian representation of the value being tested
                const char TD[]  = {0,0,0,0,0,0,0,123};

                // bdlxxxx::TestOutStream fills one byte more with type info
                // plus four bytes for the size.
                if (veryVeryVerbose) cout << "\n\tsizeof(TD) " << sizeof(TD)
                                          << " sizeof(OD) "    << LOD
                                          << endl;

                ASSERT(sizeof(TD) == LOD - 5);

                if (veryVerbose) cout << "\nTesting that the streamed data"
                                      << " and the original data coincide"
                                      << endl;

                if (veryVeryVerbose) {
                    cout << "\n\t  OD:\t  TD:" << endl;
                    for(int i = 5; i < LOD; ++i) {
                        cout << "\t  [" << (int)OD[i]   << "]"
                             << "\t  [" << (int)TD[i-5] << "]"
                             << endl;
                    }
                }

                for(int i = 5; i < LOD; ++i) {
                    LOOP_ASSERT(i, OD[i] == TD[i-5]);
                }

                bdlxxxx::TestInStream in(OD, LOD); ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);

            }

            const bsls::Types::Int64 VALUES[] = {1,
                                                0x7fffffffffffffffull,
                                                0x8000000000000000ull, -1, 0,
                                                123, -123, SHRT_MAX, SHRT_MIN,
                                                INT_MAX, INT_MIN};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]);
                        const Obj& U = mU;
                        bdlxxxx::TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, 1);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdlxxxx::TestInStream testInStream(OD, LOD);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, !testInStream.isEmpty());
                        testInStream.setSuppressVersionCheck(1);

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;
                        testInStream.reset();
                        LOOP2_ASSERT(ui, vi, testInStream);
                        LOOP2_ASSERT(ui, vi, !testInStream.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, VV == V);
                        LOOP2_ASSERT(ui, vi, (ui == vi) == (U == V));

                        bdex_InStreamFunctions::streamIn(testInStream, mV, 1);

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, UU == V);
                        LOOP2_ASSERT(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                if (verbose) cout << "\tusing object syntax:" << endl;
                const Obj X = Obj::make(1);
                ASSERT(1 == X.maxSupportedBdexVersion());
                if (verbose) cout << "\tusing class method syntax:" << endl;
                ASSERT(1 == Obj::maxSupportedBdexVersion());
            }

            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                const int version = 1;
                Obj mX = Obj::make(123);
                const Obj& X = mX;
                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                bdlxxxx::TestOutStream out;  X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                bdlxxxx::TestInStream in(OD, LOD); ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);     ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);
            }

            const bsls::Types::Uint64 VALUES[] = {1,
                                                0x7fffffffffffffffull,
                                                0xffffffffffffffffull,
                                                0x8000000000000000ull, 0, 123,
                                                SHRT_MAX, INT_MAX, UINT_MAX};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]);
                        const Obj& U = mU;
                        bdlxxxx::TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, 1);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdlxxxx::TestInStream testInStream(OD, LOD);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, !testInStream.isEmpty());
                        testInStream.setSuppressVersionCheck(1);

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;
                        testInStream.reset();
                        LOOP2_ASSERT(ui, vi, testInStream);
                        LOOP2_ASSERT(ui, vi, !testInStream.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, VV == V);
                        LOOP2_ASSERT(ui, vi, (ui == vi) == (U == V));

                        bdex_InStreamFunctions::streamIn(testInStream, mV, 1);

                        LOOP2_ASSERT(ui, vi, UU == U);
                        LOOP2_ASSERT(ui, vi, UU == V);
                        LOOP2_ASSERT(ui, vi,  U == V);
                    }
                }
            }
        }

// TO BE FINISHED XXX TBD
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // TBD
        // --------------------------------------------------------------------

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //
        // This component test driver does not have a generator function.
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // It does not need to be tested here.  This method is provided by the
        // compiler in this component.  Note that it is still indirectly tested
        // by case 2 since 'make' relies on the copy constructor.
        // --------------------------------------------------------------------

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS: '==' and '!='
        //
        // Concerns:
        //   That 'operator==' and 'operator!=' produce the correct result
        //   for all values.
        //
        // Plan:
        //   Specify a set S of varied object values, including the usual
        //   "edge" cases.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product S X S.
        //
        // Testing:
        //   bool operator==(const bdlb::BigEndianInt16& lhs, rhs);
        //   bool operator!=(const bdlb::BigEndianInt16& lhs, rhs);
        //   bool operator==(const bdlb::BigEndianUint16& lhs, rhs);
        //   bool operator!=(const bdlb::BigEndianUint16& lhs, rhs);
        //   bool operator==(const bdlb::BigEndianInt32& lhs, rhs);
        //   bool operator!=(const bdlb::BigEndianInt32& lhs, rhs);
        //   bool operator==(const bdlb::BigEndianUint32& lhs, rhs);
        //   bool operator!=(const bdlb::BigEndianUint32& lhs, rhs);
        //   bool operator==(const bdlb::BigEndianInt64& lhs, rhs);
        //   bool operator!=(const bdlb::BigEndianInt64& lhs, rhs);
        //   bool operator==(const bdlb::BigEndianUint64& lhs, rhs);
        //   bool operator!=(const bdlb::BigEndianUint64& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'operator==' and 'operator!='." << endl
                          << "======================================" << endl;

        if (verbose) cout << "\nTesting bdeut_BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;
            const short VALUES[] = {
                1, SHRT_MAX, SHRT_MIN, -1, 0, 123, -123
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP_ASSERT(j, Y == VALUES[j]);

                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                    LOOP2_ASSERT(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;
            const unsigned short VALUES[] = {
                1, USHRT_MAX, 0, 0x48, 0x4800
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP_ASSERT(j, Y == VALUES[j]);

                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                    LOOP2_ASSERT(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;
            const int VALUES[] = {
                1, INT_MAX, INT_MIN, -1, 0, 123, -123, SHRT_MAX, SHRT_MIN
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP_ASSERT(j, Y == VALUES[j]);

                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                    LOOP2_ASSERT(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;
            const unsigned int VALUES[] = {
                1, USHRT_MAX, 0, UINT_MAX, 0xC33C, 0xC33C0000
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP_ASSERT(j, Y == VALUES[j]);

                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                    LOOP2_ASSERT(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;
            const bsls::Types::Int64 VALUES[] = {
                1, 0x7fffffffffffffffull, 0x8000000000000000ull, -1, 0,
                123, -123, SHRT_MAX, SHRT_MIN, INT_MAX, INT_MIN
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP_ASSERT(j, Y == VALUES[j]);

                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                    LOOP2_ASSERT(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;
            const bsls::Types::Uint64 VALUES[] = {
                1, 0x7fffffffffffffffull, 0xffffffffffffffffull,
                0x8000000000000000ull, 0, 123, SHRT_MAX, INT_MAX, UINT_MAX
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP_ASSERT(j, Y == VALUES[j]);

                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                    LOOP2_ASSERT(i, j, (i != j) == (X != Y));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR and 'print' method:
        //
        // Concerns:
        //  We want to ensure that the 'print' method correctly formats
        //  our objects output with any valid 'level' and 'spacesPerLevel'
        //  values and returns the specified stream and does not use
        //  any memory.
        //
        // Plan:
        //  Exercise the print method with different levels and spaces and
        //  compare the result against a generated string.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Table-Driven Implementation Technique
        //
        // Testing:
        //   bsl::ostream& Obj::print(bsl::ostream& stream,
        //                            int           level,
        //                            int           spacesPerLevel) const
        //   bsl::ostream& operator<<(bsl::ostream& stream,
        //                            const Obj&    calendar)
        // --------------------------------------------------------------------

        static const struct {
            int   d_level;
            int   d_spacesPerLevel;
        } DATA[] = {
           //LEVEL SPACE PER LEVEL
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {2,    -1,             },
            {3,    -2,             },
            {1,     2,             },
            {1,     2,             },
            {1,    -2,             },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << endl
                          << "Testing 'print' and 'operator<<'." << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting bdeut_BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;
            const short VALUES[] = {1, SHRT_MAX, SHRT_MIN, -1, 0, 123, -123};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                bsl::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        bsl::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        bsl::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << bsl::string(bsl::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;
            const unsigned short VALUES[] = {1, USHRT_MAX, 0, 0x48, 0x4800};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                bsl::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        bsl::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        bsl::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << bsl::string(bsl::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;
            const int VALUES[] = {1, INT_MAX, INT_MIN, -1, 0, 123, -123,
                                  SHRT_MAX, SHRT_MIN};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                bsl::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        bsl::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        bsl::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << bsl::string(bsl::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;
            const unsigned int VALUES[] = {1, USHRT_MAX, 0, UINT_MAX,
                                             0xC33C, 0xC33C0000};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                bsl::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        bsl::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        bsl::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << bsl::string(bsl::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;
            const bsls::Types::Int64 VALUES[] = {1,
                                                0x7fffffffffffffffull,
                                                0x8000000000000000ull, -1, 0,
                                                123, -123, SHRT_MAX, SHRT_MIN,
                                                INT_MAX, INT_MIN};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                bsl::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        bsl::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        bsl::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << bsl::string(bsl::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;
            const bsls::Types::Uint64 VALUES[] = {1,
                                                0x7fffffffffffffffull,
                                                0xffffffffffffffffull,
                                                0x8000000000000000ull, 0, 123,
                                                SHRT_MAX, INT_MAX, UINT_MAX};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                bsl::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        bsl::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        bsl::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << bsl::string(bsl::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //   1. operator T() returns the expected value
        //
        // Plan:
        //   To address concern 1, we will construct different objects
        //   and verify that T() returns the expected value.
        //
        // Tactics:
        //   - Ad Hoc test data selection method
        //   - Table-Driven test case implementation technique
        //
        // Testing:
        //   bdlb::BigEndianInt16::operator  short() const;
        //   bdlb::BigEndianUint16::operator unsigned short() const;
        //   bdlb::BigEndianInt32::operator  int() const;
        //   bdlb::BigEndianUint32::operator unsigned int() const;
        //   bdlb::BigEndianInt64::operator  Int64() const;
        //   bdlb::BigEndianUint64::operator Uint64() const;
        //  -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting bdlb::BigEndianInt16" << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == short(X));
            }
            for (int i = 1; i < sizeof(short) * 8; ++i) {
                const Obj X = Obj::make(1 << (i - 1));
                LOOP_ASSERT(i, (1 << (i - 1)) == short(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianUint16" << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == (unsigned short)(X));
            }
            for (int i = 1; i < sizeof(short) * 8; ++i) {
                const Obj X = Obj::make(1 << (i - 1));
                LOOP_ASSERT(i, (1 << (i - 1)) == (unsigned short)(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianInt32" << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == int(X));
            }
            for (int i = 1; i < sizeof(int) * 8; ++i) {
                const Obj X = Obj::make(1 << (i - 1));
                LOOP_ASSERT(i, (1 << (i - 1)) == int(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianUint32" << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == (unsigned int)(X));
            }
            for (int i = 1; i < sizeof(int) * 8; ++i) {
                const Obj X = Obj::make(1 << (i - 1));
                LOOP_ASSERT(i, (1 << (i - 1)) == (unsigned int)(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianInt64" << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == bsls::Types::Int64(X));
            }
            for (int i = 1; i < sizeof(bsls::Types::Int64) * 8; ++i) {
                const Obj X = Obj::make(1LL << (i - 1));
                LOOP_ASSERT(i, (1LL << (i - 1)) == bsls::Types::Int64(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianUint64" << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == bsls::Types::Uint64(X));
            }
            for (int i = 1; i < sizeof(bsls::Types::Uint64) * 8; ++i) {
                const Obj X = Obj::make(1LL << (i - 1));
                LOOP_ASSERT(i, (1LL << (i - 1)) == bsls::Types::Uint64(X));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //
        // This component test driver does not have a generator function.
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // We want to exercise the set of primary manipulators, which can put
        // the object in any state.
        //
        // Concerns:
        //  1. Obj::make
        //      a. creates an object with the expected value
        //      b. does not allocate any memory (which implies here exception
        //         safety)
        //
        //  Note that there is no "stretching" in this object.  We are adopting
        //  a black-box attitude while testing this function with regard to the
        //  containers used by the object.
        //
        // Plan:
        //  To address concerns for 1, create an object using the default
        //  constructor.  Then use 'operator T()' and 'isInCoreValueCorrect' to
        //  check the value and a default allocator guard to verify that no
        //  memory was allocated.  Then we do a cross-test of all values and
        //  verify consistency.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Array Implementation technique
        //
        // Testing:
        //   static bdlb::BigEndianInt16::make(short);
        //   static bdlb::BigEndianUint16::make(unsigned short);
        //   static bdlb::BigEndianInt32::make(int);
        //   static bdlb::BigEndianUint32::make(unsigned int);
        //   static bdlb::BigEndianInt64::make(bsls::Types::Int64);
        //   static bdlb::BigEndianUint64::make(bsls::Types::Uint64);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting bdeut_BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;
            const short VALUES[] = {1, SHRT_MAX, SHRT_MIN, -1, 0, 123, -123};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int previousTotal = testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);
                LOOP_ASSERT(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;
            const unsigned short VALUES[] = {1, USHRT_MAX, 0, 0x48, 0x4800};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int previousTotal = testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);
                LOOP_ASSERT(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;
            const int VALUES[] = {1, INT_MAX, INT_MIN, -1, 0, 123, -123,
                                  SHRT_MAX, SHRT_MIN};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int previousTotal = testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);
                LOOP_ASSERT(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;
            const unsigned int VALUES[] = {1, USHRT_MAX, 0, UINT_MAX,
                                             0xC33C, 0xC33C0000};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int previousTotal = testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);
                LOOP_ASSERT(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;
            const bsls::Types::Int64 VALUES[] = {1,
                                                0x7fffffffffffffffull,
                                                0x8000000000000000ull, -1, 0,
                                                123, -123, SHRT_MAX, SHRT_MIN,
                                                INT_MAX, INT_MIN};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int previousTotal = testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);
                LOOP_ASSERT(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;
            const bsls::Types::Uint64 VALUES[] = {1,
                                                0x7fffffffffffffffull,
                                                0xffffffffffffffffull,
                                                0x8000000000000000ull, 0, 123,
                                                SHRT_MAX, INT_MAX, UINT_MAX};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int previousTotal = testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                LOOP_ASSERT(i, X == VALUES[i]);
                LOOP_ASSERT(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   object [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1 (dctor + =).           x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1).          x1:A x2:
        //    4. Append the same element value A to x2).    x1:A x2:A
        //    5. Append another element value B to x2).     x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3 (dctor + =)       x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTesting bdeut_BigEndianInt16." << endl;
        {
            const short VA = 123;
            const short VB = SHRT_MAX;
            const short VC = SHRT_MIN;

            typedef bdlb::BigEndianInt16 Obj;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = Obj::make(VA);
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);
            ASSERT(isInCoreValueCorrect(VA, X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);
            ASSERT(isInCoreValueCorrect(VA, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:0 }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            mX3 = 0;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x3." << endl;
            }
            ASSERT(0 == X3);
            ASSERT(isInCoreValueCorrect(0, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:0  x4:0 }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x4." << endl;
            }
            ASSERT(0 == X4);
            ASSERT(isInCoreValueCorrect(0, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:0 }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);
            ASSERT(isInCoreValueCorrect(VC, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);
            ASSERT(isInCoreValueCorrect(VB, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);
            ASSERT(isInCoreValueCorrect(VC, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint16." << endl;
        {
            const unsigned short VA = 0x48;
            const unsigned short VB = USHRT_MAX;
            const unsigned short VC = 0;

            typedef bdlb::BigEndianUint16 Obj;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = Obj::make(VA);
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);
            ASSERT(isInCoreValueCorrect(VA, X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);
            ASSERT(isInCoreValueCorrect(VA, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:1 }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            mX3 = 1;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x3." << endl;
            }
            ASSERT(1 == X3);
            ASSERT(isInCoreValueCorrect(1, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:1  x4:1 }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }
            mX4 = 1;

            if (verbose) {
                cout << "\ta. Check initial state of x4." << endl;
            }
            ASSERT(1 == X4);
            ASSERT(isInCoreValueCorrect(1, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:1 }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);
            ASSERT(isInCoreValueCorrect(VC, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:1 }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);
            ASSERT(isInCoreValueCorrect(VB, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);
            ASSERT(isInCoreValueCorrect(VC, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt32." << endl;
        {
            const int VA = 123;
            const int VB = INT_MAX;
            const int VC = INT_MIN;

            typedef bdlb::BigEndianInt32 Obj;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = Obj::make(VA);
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);
            ASSERT(isInCoreValueCorrect(VA, X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);
            ASSERT(isInCoreValueCorrect(VA, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:0 }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            mX3 = 0;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x3." << endl;
            }
            ASSERT(0 == X3);
            ASSERT(isInCoreValueCorrect(0, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:0  x4:0 }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x4." << endl;
            }
            ASSERT(0 == X4);
            ASSERT(isInCoreValueCorrect(0, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:0 }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);
            ASSERT(isInCoreValueCorrect(VC, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:0 }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);
            ASSERT(isInCoreValueCorrect(VB, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);
            ASSERT(isInCoreValueCorrect(VC, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint32." << endl;
        {
            const unsigned int VA = 0xC33C;
            const unsigned int VB = UINT_MAX;
            const unsigned int VC = 0;

            typedef bdlb::BigEndianUint32 Obj;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = Obj::make(VA);
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);
            ASSERT(isInCoreValueCorrect(VA, X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);
            ASSERT(isInCoreValueCorrect(VA, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:1 }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            mX3 = 1;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x3." << endl;
            }
            ASSERT(1 == X3);
            ASSERT(isInCoreValueCorrect(1, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:1  x4:1 }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }
            mX4 = 1;

            if (verbose) {
                cout << "\ta. Check initial state of x4." << endl;
            }
            ASSERT(1 == X4);
            ASSERT(isInCoreValueCorrect(1, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:1 }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);
            ASSERT(isInCoreValueCorrect(VC, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:1 }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);
            ASSERT(isInCoreValueCorrect(VB, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);
            ASSERT(isInCoreValueCorrect(VC, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianInt64." << endl;
        {
            const bsls::Types::Int64 VA = 123;
            const bsls::Types::Int64 VB = 0x7fffffffffffffffull;
            const bsls::Types::Int64 VC = 0x8000000000000000ull;

            typedef bdlb::BigEndianInt64 Obj;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = Obj::make(VA);
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);
            ASSERT(isInCoreValueCorrect(VA, X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);
            ASSERT(isInCoreValueCorrect(VA, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:0 }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            mX3 = 0;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x3." << endl;
            }
            ASSERT(0 == X3);
            ASSERT(isInCoreValueCorrect(0, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:0  x4:0 }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x4." << endl;
            }
            ASSERT(0 == X4);
            ASSERT(isInCoreValueCorrect(0, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:0 }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);
            ASSERT(isInCoreValueCorrect(VC, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:0 }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);
            ASSERT(isInCoreValueCorrect(VB, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);
            ASSERT(isInCoreValueCorrect(VC, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
        }

        if (verbose) cout << "\nTesting bdeut_BigEndianUint64." << endl;
        {
            const bsls::Types::Uint64 VA = 0;
            const bsls::Types::Uint64 VB = 0x7fffffffffffffffull;
            const bsls::Types::Uint64 VC = 0x8000000000000000ull;

            typedef bdlb::BigEndianUint64 Obj;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = Obj::make(VA);
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);
            ASSERT(isInCoreValueCorrect(VA, X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);
            ASSERT(isInCoreValueCorrect(VA, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:1 }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            mX3 = 1;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x3." << endl;
            }
            ASSERT(1 == X3);
            ASSERT(isInCoreValueCorrect(1, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:1  x4:1 }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x4." << endl;
            }
            ASSERT(1 == X4);
            ASSERT(isInCoreValueCorrect(1, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:1 }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);
            ASSERT(isInCoreValueCorrect(VC, X3));

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:1 }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);
            ASSERT(isInCoreValueCorrect(VB, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:1 }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);
            ASSERT(isInCoreValueCorrect(VC, X2));

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:1 }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);
            ASSERT(isInCoreValueCorrect(VB, X1));

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
