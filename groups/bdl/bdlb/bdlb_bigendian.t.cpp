// bdlb_bigendian.t.cpp                                               -*-C++-*-
#include <bdlb_bigendian.h>

#include <bslim_testutil.h>
#include <bsls_asserttest.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <arpa/inet.h> // not a component
#else
#include <winsock2.h>  // not a component
#endif

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cctype.h>      // isdigit() isupper() islower()
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// All classes in this component are a value-semantic types that represent
// big-endian integer types.  They have the same value if they have the same
// in-core big endian representation.
// ----------------------------------------------------------------------------
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion(int);
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
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)


// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 20140601

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

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

// __bswap_64 exists on Linux, we will use to ensure consistency.  Note that
// reusing the same function ensures the correctness of swap64 o swap64 only.

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

bool isInCoreValueCorrect(bsls::Types::Int64          value,
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

}  // close unnamed namespace


// Global Data for testing
//

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bsl::is_trivially_copyable<bdlb::BigEndianInt16>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<bdlb::BigEndianInt16>::value);

BSLMF_ASSERT(true == bsl::is_trivially_copyable<bdlb::BigEndianUint16>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<bdlb::BigEndianUint16>::value);

BSLMF_ASSERT(true == bsl::is_trivially_copyable<bdlb::BigEndianInt32>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<bdlb::BigEndianInt32>::value);

BSLMF_ASSERT(true == bsl::is_trivially_copyable<bdlb::BigEndianUint32>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<bdlb::BigEndianUint32>::value);

BSLMF_ASSERT(true == bsl::is_trivially_copyable<bdlb::BigEndianInt64>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<bdlb::BigEndianInt64>::value);

BSLMF_ASSERT(true == bsl::is_trivially_copyable<bdlb::BigEndianUint64>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<bdlb::BigEndianUint64>::value);

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------
const short VALUES_SHORT[] = {0,
                              1,
                              -1,
                              123,
                              -123,
                              SHRT_MAX,
                              SHRT_MIN};

const unsigned short VALUES_USHORT[] = {0,
                                        1,
                                        123,
                                        0xC3,
                                        0xC300,
                                        USHRT_MAX};

const int VALUES_INT[] = {0,
                          1,
                          -1,
                          123,
                          -123,
                          SHRT_MAX,
                          SHRT_MIN,
                          INT_MAX,
                          INT_MIN};

const unsigned int VALUES_UINT[] = {0,
                                    1,
                                    0xC33C,
                                    0xC3C3,
                                    0xC33C0000,
                                    USHRT_MAX,
                                    UINT_MAX};


const bsls::Types::Int64 VALUES_INT64[] = {0,
                                           1,
                                           -1,
                                           123,
                                           -123,
                                           0xC33C,
                                           0xC3C3,
                                           0xC33C0000,
                                           SHRT_MAX,
                                           SHRT_MIN,
                                           INT_MAX,
                                           INT_MIN,
                                           LLONG_MAX,
                                           LLONG_MIN};

const bsls::Types::Uint64 VALUES_UINT64[] = {0,
                                             1,
                                             123,
                                             0xC33C,
                                             0xC3C3,
                                             0xC33C0000,
                                             USHRT_MAX,
                                             UINT_MAX,
                                             ULLONG_MAX};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
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
        //   bsl::is_trivially_copyable
        //   bdlb::HasPrintMethod
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Traits"
                          << "\n==============" << endl;

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            ASSERT((bsl::is_trivially_copyable<Obj>::value));
            ASSERT((bdlb::HasPrintMethod<Obj>::value));
        }

        if (verbose) cout << "\nTesting BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            ASSERT((bsl::is_trivially_copyable<Obj>::value));
            ASSERT((bdlb::HasPrintMethod<Obj>::value));
        }

        if (verbose) cout << "\nTesting BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            ASSERT((bsl::is_trivially_copyable<Obj>::value));
            ASSERT((bdlb::HasPrintMethod<Obj>::value));
        }

        if (verbose) cout << "\nTesting BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            ASSERT((bsl::is_trivially_copyable<Obj>::value));
            ASSERT((bdlb::HasPrintMethod<Obj>::value));
        }

        if (verbose) cout << "\nTesting BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            ASSERT((bsl::is_trivially_copyable<Obj>::value));
            ASSERT((bdlb::HasPrintMethod<Obj>::value));
        }

        if (verbose) cout << "\nTesting BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            ASSERT((bsl::is_trivially_copyable<Obj>::value));
            ASSERT((bdlb::HasPrintMethod<Obj>::value));
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
        //   static int maxSupportedBdexVersion(int) const;
        //   STREAM& bdexStreamIn(STREAM&, Obj&);
        //   STREAM& bdexStreamOut(STREAM&, const Obj&) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        using bslx::OutStreamFunctions::bdexStreamOut;
        using bslx::InStreamFunctions::bdexStreamIn;

        const int VERSION = 1;

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion(int)'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj& X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion(VERSION_SELECTOR));
                ASSERT(VERSION ==
                               Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
            }


            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                Obj mX = Obj::make(123); const Obj& X  = mX;

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                Out out(VERSION_SELECTOR);

                X.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                if (verbose) cout << "\nDirect test that the value is streamed"
                                  << " in big endian order, byte by byte"
                                  << endl;
                if (veryVerbose) cout << "\nTesting that the size of the data"
                                      << " streamed is the same as the size of"
                                      << " the original data."
                                      << endl;
                // big endian representation of the value being tested
                const char TD[]  = {0,123};

                // bslx::TestOutStream fills one byte more with type info
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
                    for(bsl::size_t i = 5; i < LOD; ++i) {
                        cout << "\t  [" << static_cast<int>(OD[i])   << "]"
                             << "\t  [" << static_cast<int>(TD[i-5]) << "]"
                             << endl;
                    }
                }

                for(bsl::size_t i = 5; i < LOD; ++i) {
                    ASSERTV(i, OD[i] == TD[i-5]);
                }

                In in(OD, LOD);

                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, VERSION);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);

            }

            const short *VALUES = VALUES_SHORT;
            enum { NUM_VALUES = sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT) };

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]); const Obj& U = mU;

                        Out out(VERSION_SELECTOR);

                        bdexStreamOut(out, U, VERSION);

                        In in(out.data(), out.length());

                        ASSERTV(ui, in);
                        ASSERTV(ui, !in.isEmpty());

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;

                        in.reset();
                        ASSERTV(ui, vi, in);
                        ASSERTV(ui, vi, !in.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, VV == V);
                        ASSERTV(ui, vi, (ui == vi) == (U == V));

                        bdexStreamIn(in, mV, VERSION);

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, UU == V);
                        ASSERTV(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion(int)'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj& X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion(VERSION_SELECTOR));
                ASSERT(VERSION ==
                               Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
            }


            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                Obj mX = Obj::make(123); const Obj& X = mX;

                Out out(VERSION_SELECTOR);

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                X.bdexStreamOut(out, VERSION);

                In in(out.data(), out.length());

                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, VERSION);

                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);
            }

            const unsigned short *VALUES = VALUES_USHORT;
            enum { NUM_VALUES = sizeof(VALUES_USHORT) /
                                sizeof(*VALUES_USHORT) };

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]); const Obj& U = mU;

                        Out out(VERSION_SELECTOR);

                        bdexStreamOut(out, U, VERSION);

                        In in(out.data(), out.length());

                        ASSERTV(ui, in);
                        ASSERTV(ui, !in.isEmpty());

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;

                        in.reset();

                        ASSERTV(ui, vi, in);
                        ASSERTV(ui, vi, !in.isEmpty());

                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, VV == V);
                        ASSERTV(ui, vi, (ui == vi) == (U == V));

                        bdexStreamIn(in, mV, VERSION);

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, UU == V);
                        ASSERTV(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion(int)'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion(VERSION_SELECTOR));
                ASSERT(VERSION ==
                               Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
            }


            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                Obj mX = Obj::make(123); const Obj& X = mX;

                Out out(VERSION_SELECTOR);

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                X.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                if (verbose) cout << "\nDirect test that the value is streamed"
                                  << " in big endian order, byte by byte"
                                  << endl;
                if (veryVerbose) cout << "\nTesting that the size of the data"
                                      << " streamed is the same as the size of"
                                      << " the original data."
                                      << endl;
                // big endian representation of the value being tested
                const char TD[]  = {0,0,0,123};

                // bslx::TestOutStream fills one byte more with type info
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
                    for(bsl::size_t i = 5; i < LOD; ++i) {
                        cout << "\t  [" << static_cast<int>(OD[i])   << "]"
                             << "\t  [" << static_cast<int>(TD[i-5]) << "]"
                             << endl;
                    }
                }

                for(bsl::size_t i = 5; i < LOD; ++i) {
                    ASSERTV(i, OD[i] == TD[i-5]);
                }

                In in(OD, LOD);
                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, VERSION);

                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);

           }

            const int *VALUES = VALUES_INT;
            enum { NUM_VALUES = sizeof(VALUES_INT) / sizeof(*VALUES_INT) };

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]); const Obj& U = mU;

                        Out out(VERSION_SELECTOR);

                        bdexStreamOut(out, U, VERSION);

                        In in(out.data(), out.length());

                        ASSERTV(ui, in);
                        ASSERTV(ui, !in.isEmpty());

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;

                        in.reset();
                        ASSERTV(ui, vi, in);
                        ASSERTV(ui, vi, !in.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, VV == V);
                        ASSERTV(ui, vi, (ui == vi) == (U == V));

                        bdexStreamIn(in, mV, VERSION);

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, UU == V);
                        ASSERTV(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion(int)'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion(VERSION_SELECTOR));
                ASSERT(VERSION ==
                               Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
            }


            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                Obj mX = Obj::make(123); const Obj& X = mX;

                Out out(VERSION_SELECTOR);

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                X.bdexStreamOut(out, VERSION);

                In in(out.data(), out.length());
                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, VERSION);

                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);
            }

            const unsigned int *VALUES = VALUES_UINT;
            enum { NUM_VALUES = sizeof(VALUES_UINT) / sizeof(*VALUES_UINT) };

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]); const Obj& U = mU;

                        Out out(VERSION_SELECTOR);

                        bdexStreamOut(out, U, VERSION);

                        In in(out.data(), out.length());
                        ASSERTV(ui, in);
                        ASSERTV(ui, !in.isEmpty());

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;

                        in.reset();
                        ASSERTV(ui, vi, in);
                        ASSERTV(ui, vi, !in.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, VV == V);
                        ASSERTV(ui, vi, (ui == vi) == (U == V));

                        bdexStreamIn(in, mV, VERSION);

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, UU == V);
                        ASSERTV(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion(int)'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion(VERSION_SELECTOR));
                ASSERT(VERSION ==
                               Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
            }


            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                Obj mX = Obj::make(123); const Obj& X = mX;

                Out out(VERSION_SELECTOR);

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                X.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();
                if (verbose) cout << "\nDirect test that the value is streamed"
                                  << " in big endian order, byte by byte"
                                  << endl;
                if (veryVerbose) cout << "\nTesting that the size of the data"
                                      << " streamed is the same as the size of"
                                      << " the original data."
                                      << endl;
                // big endian representation of the value being tested
                const char TD[]  = {0,0,0,0,0,0,0,123};

                // bslx::TestOutStream fills one byte more with type info plus
                // four bytes for the size.
                if (veryVeryVerbose) cout << "\n\tsizeof(TD) " << sizeof(TD)
                                          << " sizeof(OD) "    << LOD
                                          << endl;

                ASSERT(sizeof(TD) == LOD - 5);

                if (veryVerbose) cout << "\nTesting that the streamed data"
                                      << " and the original data coincide"
                                      << endl;

                if (veryVeryVerbose) {
                    cout << "\n\t  OD:\t  TD:" << endl;
                    for(bsl::size_t i = 5; i < LOD; ++i) {
                        cout << "\t  [" << static_cast<int>(OD[i])   << "]"
                             << "\t  [" << static_cast<int>(TD[i-5]) << "]"
                             << endl;
                    }
                }

                for(bsl::size_t i = 5; i < LOD; ++i) {
                    ASSERTV(i, OD[i] == TD[i-5]);
                }

                In in(OD, LOD);
                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, VERSION);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);

            }

            const bsls::Types::Int64 *VALUES = VALUES_INT64;
            enum { NUM_VALUES = sizeof(VALUES_INT64) / sizeof(*VALUES_INT64) };

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]); const Obj& U = mU;

                        Out out(VERSION_SELECTOR);

                        bdexStreamOut(out, U, VERSION);

                        In in(out.data(), out.length());
                        ASSERTV(ui, in);
                        ASSERTV(ui, !in.isEmpty());

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;

                        in.reset();
                        ASSERTV(ui, vi, in);
                        ASSERTV(ui, vi, !in.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, VV == V);
                        ASSERTV(ui, vi, (ui == vi) == (U == V));

                        bdexStreamIn(in, mV, VERSION);

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, UU == V);
                        ASSERTV(ui, vi,  U == V);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion(int)'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion(VERSION_SELECTOR));
                ASSERT(VERSION ==
                               Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
            }


            if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut'"
                              << "and (valid) 'bdexStreamIn' functionality."
                              << endl;

            {
                Obj mX = Obj::make(123); const Obj& X = mX;

                Out out(VERSION_SELECTOR);

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X);
                }

                X.bdexStreamOut(out, VERSION);

                In in(out.data(), out.length());
                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj t = Obj::make(-123);

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t);
                }
                ASSERT(X != t);

                t.bdexStreamIn(in, VERSION);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t);
                }
                ASSERT(X == t);
            }

            const bsls::Types::Uint64 *VALUES = VALUES_UINT64;
            enum { NUM_VALUES = sizeof(VALUES_UINT64) /
                                sizeof(*VALUES_UINT64) };

            if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
            {
                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    const Obj UU = Obj::make(VALUES[ui]);

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        Obj mU = Obj::make(VALUES[ui]); const Obj& U = mU;

                        Out out(VERSION_SELECTOR);

                        bdexStreamOut(out, U, VERSION);

                        In in(out.data(), out.length());

                        ASSERTV(ui, in);
                        ASSERTV(ui, !in.isEmpty());

                        const Obj VV = Obj::make(VALUES[vi]);

                        Obj mV; const Obj& V = mV;

                        in.reset();
                        ASSERTV(ui, vi, in);
                        ASSERTV(ui, vi, !in.isEmpty());
                        mV = VALUES[vi];

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, VV == V);
                        ASSERTV(ui, vi, (ui == vi) == (U == V));

                        bdexStreamIn(in, mV, VERSION);

                        ASSERTV(ui, vi, UU == U);
                        ASSERTV(ui, vi, UU == V);
                        ASSERTV(ui, vi,  U == V);
                    }
                }
            }
        }
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

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            const short *VALUES = VALUES_SHORT;
            enum { NUM_VALUES = sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT) };

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(j, Y == VALUES[j]);

                    ASSERTV(i, j, (i == j) == (X == Y));
                    ASSERTV(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            const unsigned short *VALUES = VALUES_USHORT;
            enum { NUM_VALUES = sizeof(VALUES_USHORT) /
                                sizeof(*VALUES_USHORT) };

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(j, Y == VALUES[j]);

                    ASSERTV(i, j, (i == j) == (X == Y));
                    ASSERTV(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            const int *VALUES = VALUES_INT;
            enum { NUM_VALUES = sizeof(VALUES_INT) / sizeof(*VALUES_INT) };

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(j, Y == VALUES[j]);

                    ASSERTV(i, j, (i == j) == (X == Y));
                    ASSERTV(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            const unsigned int *VALUES = VALUES_UINT;
            enum { NUM_VALUES = sizeof(VALUES_UINT) / sizeof(*VALUES_UINT) };

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(j, Y == VALUES[j]);

                    ASSERTV(i, j, (i == j) == (X == Y));
                    ASSERTV(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            const bsls::Types::Int64 *VALUES = VALUES_INT64;
            enum { NUM_VALUES = sizeof(VALUES_INT64) / sizeof(*VALUES_INT64) };

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(j, Y == VALUES[j]);

                    ASSERTV(i, j, (i == j) == (X == Y));
                    ASSERTV(i, j, (i != j) == (X != Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            const bsls::Types::Uint64 *VALUES = VALUES_UINT64;
            enum { NUM_VALUES = sizeof(VALUES_UINT64) /
                                sizeof(*VALUES_UINT64) };

            for (int i = 0; i < NUM_VALUES; ++i) {

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(j, Y == VALUES[j]);

                    ASSERTV(i, j, (i == j) == (X == Y));
                    ASSERTV(i, j, (i != j) == (X != Y));
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
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        if (verbose) cout << endl
                          << "Testing 'print' and 'operator<<'." << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            const short *VALUES = VALUES_SHORT;
            enum { NUM_VALUES = sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT) };

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
                        ASSERTV(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                ASSERTV(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            const unsigned short *VALUES = VALUES_USHORT;
            enum { NUM_VALUES = sizeof(VALUES_USHORT) /
                                sizeof(*VALUES_USHORT) };

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
                        ASSERTV(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                ASSERTV(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            const int *VALUES = VALUES_INT;
            enum { NUM_VALUES = sizeof(VALUES_INT) / sizeof(*VALUES_INT) };

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
                        ASSERTV(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                ASSERTV(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            const unsigned int *VALUES = VALUES_UINT;
            enum { NUM_VALUES = sizeof(VALUES_UINT) / sizeof(*VALUES_UINT) };

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
                        ASSERTV(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                ASSERTV(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            const bsls::Types::Int64 *VALUES = VALUES_INT64;
            enum { NUM_VALUES = sizeof(VALUES_INT64) / sizeof(*VALUES_INT64) };

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
                        ASSERTV(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                ASSERTV(i, ss.str() == streamValue.str());
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            const bsls::Types::Uint64 *VALUES = VALUES_UINT64;
            enum { NUM_VALUES = sizeof(VALUES_UINT64) /
                                sizeof(*VALUES_UINT64) };

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
                        ASSERTV(i, j, ss2.str() == ss.str());
                    }

                }
                bsl::stringstream ss;
                ss << X;
                ASSERTV(i, ss.str() == streamValue.str());
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
                ASSERT(0 == static_cast<short>(X));
            }
            for (bsl::size_t i = 0; i < sizeof(short) * 8; ++i) {
                const Obj X = Obj::make(static_cast<short>(1 << i));
                ASSERTV(i,
                          static_cast<short>(1 << i) == static_cast<short>(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianUint16" << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == static_cast<unsigned short>(X));
            }
            for (bsl::size_t i = 0; i < sizeof(short) * 8; ++i) {
                const Obj X = Obj::make(static_cast<unsigned short>(1 << i));
                ASSERTV(i, (1 << i) == static_cast<unsigned short>(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianInt32" << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == static_cast<int>(X));
            }
            for (bsl::size_t i = 0; i <= sizeof(int) * 8; ++i) {
                const Obj X = Obj::make(1 << i);
                ASSERTV(i, (1 << i) == static_cast<int>(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianUint32" << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == static_cast<unsigned int>(X));
            }
            for (bsl::size_t i = 0; i <= sizeof(int) * 8; ++i) {
                const Obj X = Obj::make(1U << i);
                ASSERTV(i, (1U << i) == static_cast<unsigned int>(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianInt64" << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == static_cast<bsls::Types::Int64>(X));
            }
            for (bsl::size_t i = 0; i <= sizeof(bsls::Types::Int64) * 8; ++i) {
                const Obj X = Obj::make(1LL << i);
                ASSERTV(i, (1LL << i) == static_cast<bsls::Types::Int64>(X));
            }
        }

        if (verbose) cout << "\nTesting bdlb::BigEndianUint64" << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == static_cast<bsls::Types::Uint64>(X));
            }
            for (bsl::size_t i = 0; i <= sizeof(bsls::Types::Uint64) * 8; ++i)
            {
                const Obj X = Obj::make(1ULL << i);
                ASSERTV(i, (1ULL << i) == static_cast<bsls::Types::Uint64>(X));
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

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            const short *VALUES = VALUES_SHORT;
            enum { NUM_VALUES = sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT) };

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 previousTotal =
                                                testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);
                ASSERTV(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint16." << endl;
        {
            typedef bdlb::BigEndianUint16 Obj;

            const unsigned short *VALUES = VALUES_USHORT;
            enum { NUM_VALUES = sizeof(VALUES_USHORT) /
                                sizeof(*VALUES_USHORT) };

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 previousTotal =
                                                testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);
                ASSERTV(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt32." << endl;
        {
            typedef bdlb::BigEndianInt32 Obj;

            const int *VALUES = VALUES_INT;
            enum { NUM_VALUES = sizeof(VALUES_INT) / sizeof(*VALUES_INT) };

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 previousTotal =
                                                testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);
                ASSERTV(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint32." << endl;
        {
            typedef bdlb::BigEndianUint32 Obj;

            const unsigned int *VALUES = VALUES_UINT;
            enum { NUM_VALUES = sizeof(VALUES_UINT) / sizeof(*VALUES_UINT) };

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 previousTotal =
                                                testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);
                ASSERTV(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianInt64." << endl;
        {
            typedef bdlb::BigEndianInt64 Obj;

            const bsls::Types::Int64 *VALUES = VALUES_INT64;
            enum { NUM_VALUES = sizeof(VALUES_INT64) / sizeof(*VALUES_INT64) };

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 previousTotal =
                                                testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);
                ASSERTV(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(i, j, (i == j) == (X == Y));
                }
            }
        }

        if (verbose) cout << "\nTesting BigEndianUint64." << endl;
        {
            typedef bdlb::BigEndianUint64 Obj;

            const bsls::Types::Uint64 *VALUES = VALUES_UINT64;
            enum { NUM_VALUES = sizeof(VALUES_UINT64) /
                                sizeof(*VALUES_UINT64) };

            const bslma::DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 previousTotal =
                                                testAllocator.numBlocksTotal();

                const Obj X = Obj::make(VALUES[i]);
                ASSERTV(i, X == VALUES[i]);
                ASSERTV(i, isInCoreValueCorrect(VALUES[i], X));

                ASSERT(testAllocator.numBlocksTotal() == previousTotal);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const Obj Y = Obj::make(VALUES[j]);
                    ASSERTV(i, j, (i == j) == (X == Y));
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

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
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
                T_ P(X1);
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
                T_ P(X2);
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
                T_ P(X1);
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
                T_ P(X3);
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
                T_ P(X4);
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
                T_ P(X3);
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
                T_ P(X2);
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
                T_ P(X2);
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
                T_ P(X1);
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

        if (verbose) cout << "\nTesting BigEndianUint16." << endl;
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
                T_ P(X1);
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
                T_ P(X2);
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
                T_ P(X1);
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
                T_ P(X3);
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
                T_ P(X4);
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
                T_ P(X3);
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
                T_ P(X2);
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
                T_ P(X2);
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
                T_ P(X1);
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

        if (verbose) cout << "\nTesting BigEndianInt32." << endl;
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
                T_ P(X1);
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
                T_ P(X2);
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
                T_ P(X1);
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
                T_ P(X3);
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
                T_ P(X4);
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
                T_ P(X3);
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
                T_ P(X2);
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
                T_ P(X2);
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
                T_ P(X1);
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

        if (verbose) cout << "\nTesting BigEndianUint32." << endl;
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
                T_ P(X1);
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
                T_ P(X2);
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
                T_ P(X1);
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
                T_ P(X3);
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
                T_ P(X4);
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
                T_ P(X3);
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
                T_ P(X2);
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
                T_ P(X2);
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
                T_ P(X1);
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

        if (verbose) cout << "\nTesting BigEndianInt64." << endl;
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
                T_ P(X1);
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
                T_ P(X2);
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
                T_ P(X1);
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
                T_ P(X3);
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
                T_ P(X4);
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
                T_ P(X3);
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
                T_ P(X2);
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
                T_ P(X2);
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
                T_ P(X1);
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

        if (verbose) cout << "\nTesting BigEndianUint64." << endl;
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
                T_ P(X1);
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
                T_ P(X2);
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
                T_ P(X1);
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
                T_ P(X3);
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
                T_ P(X4);
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
                T_ P(X3);
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
                T_ P(X2);
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
                T_ P(X2);
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
                T_ P(X1);
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
