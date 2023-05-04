// bdlb_bigendian.t.cpp                                               -*-C++-*-
#include <bdlb_bigendian.h>

#include <bsls_asserttest.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <arpa/inet.h> // not a component
#else
#include <winsock2.h>  // not a component
#endif

#include <bslh_siphashalgorithm.h>

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_set.h>

#include <bsl_cstdlib.h>     // 'atoi'

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// All classes in this component are a value-semantic types that represent
// big-endian integer types.  They have the same value if they have the same
// in-core big endian representation.
//-----------------------------------------------------------------------------
//
// CLASS METHODS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
// [ 9] static int maxSupportedBdexVersion();
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22
// [ 9] static int maxSupportedBdexVersion(int);
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
// [ 9] STREAM& bdexStreamIn(STREAM&, Obj&);
// [ 9] STREAM& bdexStreamOut(STREAM&, const Obj&) const;
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
// [10] hashAppend(hashAlg, BigEndianInt16)
// [10] hashAppend(hashAlg, BigEndianUint16)
// [10] hashAppend(hashAlg, BigEndianInt32)
// [10] hashAppend(hashAlg, BigEndianUint32)
// [10] hashAppend(hashAlg, BigEndianInt64)
// [10] hashAppend(hashAlg, BigEndianUint64)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] TESTING HASH ENDIAN-NESS
// [12] USAGE EXAMPLE

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

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

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

//=============================================================================
//                                 TYPE TRAITS
//-----------------------------------------------------------------------------

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

//=============================================================================
//                             GLOBAL TEST DATA
//-----------------------------------------------------------------------------
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
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
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
/// Example 1: Basic Use of 'bdlb::BigEndian'
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using 'bdlb::BigEndian' types to represent a
// structure meant to be exchanged over the network ( which historically uses
// big-endian byte order ) or stored in-core as big-endian integers.  First, we
// define the structure:
//..
    struct ProtocolHeader {
        // This structure represents the header of the protocol.  All integer
        // values are stored in the network byte-order (i.e., big-endian).

        bdlb::BigEndianUint16 d_protocolVersion;
        bdlb::BigEndianUint16 d_messageType;
        bdlb::BigEndianUint32 d_messageLength;
    };
//..
// Next, we prepare in-memory representation of the protocol header with
// protocol version set to '0x1', message type set to '0x02' and message length
// set to '0x1234' in the big-endian byte order ( most significant bytes first
// ):
//..
    const char buffer[8] = { 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x12, 0x34 };
//..
// Now, we create an instance of the 'ProtocolHeader' structure and emulate
// packet reception over the network:
//..
    struct ProtocolHeader header;
    ASSERT(8 == sizeof(header));
    memcpy(static_cast<void*>(&header), buffer, 8);
//..
// Next, we verify that actual in-core values depend on the endianess of the
// underlying platform:
//..
    #ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    ASSERT(0x0100 ==
           static_cast<short>(*(reinterpret_cast<unsigned short*>(
                                                 &header.d_protocolVersion))));

    ASSERT(0x0200 ==
           static_cast<short>(*(reinterpret_cast<unsigned short*>(
                                                 &header.d_messageType))));

    ASSERT(0x34120000 == *(reinterpret_cast<unsigned int*>(
                                                 &header.d_messageLength)));
    #endif // BSLS_PLATFORM_IS_LITTLE_ENDIAN

    #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    ASSERT(0x01 ==
           static_cast<short>(*(reinterpret_cast<unsigned short*>(
                                                 &header.d_protocolVersion))));

    ASSERT(0x02 ==
           static_cast<short>(*(reinterpret_cast<unsigned short*>(
                                                 &header.d_messageType))));

    ASSERT(0x1234 == *(reinterpret_cast<unsigned int*>(
                                                 &header.d_messageLength)));
    #endif // BSLS_PLATFORM_IS_BIG_ENDIAN
//..
// Finally, we verify that the received protocol header can be validated on
// platforms of any endianess:
//..
    ASSERT(0x01   == header.d_protocolVersion);
    ASSERT(0x02   == header.d_messageType);
    ASSERT(0x1234 == header.d_messageLength);
//..
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING HASH ENDIAN-NESS
        //
        // Concerns:
        //: 1 A 'BigEndianInt16' object will have the same SipHash hash value
        //:   for both small and big endian architecture.
        //:
        //: 2 A 'BigEndianUint16' object will have the same SipHash hash value
        //:   for both small and big endian architecture.
        //:
        //: 3 A 'BigEndianInt32' object will have the same SipHash hash value
        //:   for both small and big endian architecture.
        //:
        //: 4 A 'BigEndianUint32' object will have the same SipHash hash value
        //:   for both small and big endian architecture.
        //:
        //: 5 A 'BigEndianInt64' object will have the same SipHash hash value
        //:   for both small and big endian architecture.
        //:
        //: 6 A 'BigEndianUint64' object will have the same SipHash hash value
        //:   for both small and big endian architecture.
        //
        // Plan:
        //: 1 Hash some different 'BigEndianInt16' objects and verify that the
        //:   result is the same for the various operating systems.  (C-1)
        //:
        //: 2 Hash some different 'BigEndianUint16' objects and verify that the
        //:   result is the same for the various operating systems.  (C-2)
        //:
        //: 3 Hash some different 'BigEndianInt32' objects and verify that the
        //:   result is the same for the various operating systems.  (C-3)
        //:
        //: 4 Hash some different 'BigEndianUint32' objects and verify that the
        //:   result is the same for the various operating systems.  (C-4)
        //:
        //: 5 Hash some different 'BigEndianInt64' objects and verify that the
        //:   result is the same for the various operating systems.  (C-5)
        //:
        //: 6 Hash some different 'BigEndianUint64' objects and verify that the
        //:   result is the same for the various operating systems.  (C-6)
        //
        // Testing:
        //   TESTING HASH ENDIAN-NESS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING HASH ENDIAN-NESS"
                             "\n========================\n";

        static struct TestData {
            long long           d_line;
            short               d_i16;
            bsls::Types::Uint64 d_i16ExpectedHash;
            unsigned short      d_u16;
            bsls::Types::Uint64 d_u16ExpectedHash;
            int                 d_i32;
            bsls::Types::Uint64 d_i32ExpectedHash;
            unsigned int        d_u32;
            bsls::Types::Uint64 d_u32ExpectedHash;
            bsls::Types::Int64  d_i64;
            bsls::Types::Uint64 d_i64ExpectedHash;
            bsls::Types::Uint64 d_u64;
            bsls::Types::Uint64 d_u64ExpectedHash;
        } k_DATA[] =
        {
            { L_,  0, 0x3a6d9523170345d0ULL,  0, 0x3a6d9523170345d0ULL,
                   0, 0x7bf55e51b22b9698ULL,  0, 0x7bf55e51b22b9698ULL,
                   0, 0xe849e8bb6ffe2567ULL,  0, 0xe849e8bb6ffe2567ULL },
            { L_, -2, 0xc76eec30e90d1347ULL,  1, 0x62c3506f27376c25ULL,
                  -2, 0x7ae779acfdd82855ULL,  1, 0x6b7666f5639fb7f7ULL,
                  -2, 0xed30d6f3984ef683ULL,  1, 0x0a77dc4441ee47baULL },
            { L_,  1, 0x62c3506f27376c25ULL, 10, 0xf5023c733d287ae4ULL,
                   1, 0x6b7666f5639fb7f7ULL, 10, 0xc7deb7a34bafb984ULL,
                   1, 0x0a77dc4441ee47baULL, 10, 0x6f560119f69841adULL },
            { L_, -3, 0xf5b7cc9bbe5df647ULL, 31, 0xf796f73fb1c76672ULL,
                  -3, 0xf3b08ee698c2f6f2ULL, 31, 0xa2e8af3fec05b185ULL,
                  -3, 0x8f69ecdc2bf99cd7ULL, 31, 0xccd53bf79f6f26e2ULL },
            { L_, 32, 0xf2f9ad244df43f35ULL, 32, 0xf2f9ad244df43f35ULL,
                  32, 0x5182999810dd092cULL, 32, 0x5182999810dd092cULL,
                  32, 0xda7863cff94b954cULL, 32, 0xda7863cff94b954cULL },
            { L_,-33, 0x8252a4497a1ede71ULL, 42, 0x38db1c88928275e8ULL,
                 -33, 0x4b1ac3e520f5a46cULL, 42, 0xda3e1d5149313583ULL,
                 -33, 0x670eb2d6a4081f8cULL, 42, 0x260d52c3dc8ae7abULL },
            { L_, 42, 0x38db1c88928275e8ULL,132, 0x5d508a9cb51a3ce6ULL,
                  42, 0xda3e1d5149313583ULL,132, 0x1ffcf1e8bc8391b1ULL,
                  42, 0x260d52c3dc8ae7abULL,132, 0xc4a550a50c4986ebULL },
            { L_,-43, 0x14204a2a24431034ULL,742, 0x495317f46d6e01f9ULL,
                 -43, 0xd6acbfeb3f547371ULL,742, 0x0a64788942e106a0ULL,
                 -43, 0xc4d2829c578c1c53ULL,742, 0x5e0299d015c2da13ULL },
        };

        using namespace bdlb;
        const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

        const char genericSeed[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
            BigEndianInt16  bei16 = BigEndianInt16::make (k_DATA[i].d_i16);
            BigEndianUint16 beu16 = BigEndianUint16::make(k_DATA[i].d_u16);
            BigEndianInt32  bei32 = BigEndianInt32::make (k_DATA[i].d_i32);
            BigEndianUint32 beu32 = BigEndianUint32::make(k_DATA[i].d_u32);
            BigEndianInt64  bei64 = BigEndianInt64::make (k_DATA[i].d_i64);
            BigEndianUint64 beu64 = BigEndianUint64::make(k_DATA[i].d_u64);

            bslh::SipHashAlgorithm hashAlgI16(genericSeed);
            bslh::SipHashAlgorithm hashAlgU16(genericSeed);
            bslh::SipHashAlgorithm hashAlgI32(genericSeed);
            bslh::SipHashAlgorithm hashAlgU32(genericSeed);
            bslh::SipHashAlgorithm hashAlgI64(genericSeed);
            bslh::SipHashAlgorithm hashAlgU64(genericSeed);

            hashAppend(hashAlgI16, bei16);
            hashAppend(hashAlgU16, beu16);
            hashAppend(hashAlgI32, bei32);
            hashAppend(hashAlgU32, beu32);
            hashAppend(hashAlgI64, bei64);
            hashAppend(hashAlgU64, beu64);

            bsls::Types::Uint64 resI16 = hashAlgI16.computeHash();
            bsls::Types::Uint64 resU16 = hashAlgU16.computeHash();
            bsls::Types::Uint64 resI32 = hashAlgI32.computeHash();
            bsls::Types::Uint64 resU32 = hashAlgU32.computeHash();
            bsls::Types::Uint64 resI64 = hashAlgI64.computeHash();
            bsls::Types::Uint64 resU64 = hashAlgU64.computeHash();

            LOOP4_ASSERT(i,
                         k_DATA[i].d_i16,
                         k_DATA[i].d_i16ExpectedHash,
                         resI16,
                         k_DATA[i].d_i16ExpectedHash == resI16);
            LOOP4_ASSERT(i,
                         k_DATA[i].d_u16,
                         k_DATA[i].d_u16ExpectedHash,
                         resU16,
                         k_DATA[i].d_u16ExpectedHash == resU16);
            LOOP4_ASSERT(i,
                         k_DATA[i].d_i32,
                         k_DATA[i].d_i32ExpectedHash,
                         resI32,
                         k_DATA[i].d_i32ExpectedHash == resI32);
            LOOP4_ASSERT(i,
                         k_DATA[i].d_u32,
                         k_DATA[i].d_u32ExpectedHash,
                         resU32,
                         k_DATA[i].d_u32ExpectedHash == resU32);
            LOOP4_ASSERT(i,
                         k_DATA[i].d_i64,
                         k_DATA[i].d_i64ExpectedHash,
                         resI64,
                         k_DATA[i].d_i64ExpectedHash == resI64);
            LOOP4_ASSERT(i,
                         k_DATA[i].d_u64,
                         k_DATA[i].d_u64ExpectedHash,
                         resU64,
                         k_DATA[i].d_u64ExpectedHash == resU64);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION
        //
        // Concerns:
        //: 1 A 'BigEndianInt16' object can be hashed by instances of
        //:   'bsl::hash<bdlb::BigEndianInt16>'.
        //:
        //: 2 A small sample of different 'BigEndianInt16' objects produce
        //:   different hashes.
        //:
        //: 3 Invoking 'bsl::hash<bdlb::BigEndianInt16>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the
        //:   'BigEndianInt16' object.
        //:
        //: 4 A 'BigEndianUint16' object can be hashed by instances of
        //:   'bsl::hash<bdlb::BigEndianUint16>'.
        //:
        //: 5 A small sample of different 'BigEndianUint16' objects produce
        //:   different hashes.
        //:
        //: 6 Invoking 'bsl::hash<bdlb::BigEndianUint16>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the
        //:   'BigEndianUint16' object.
        //:
        //: 7 A 'BigEndianInt32' object can be hashed by instances of
        //:   'bsl::hash<bdlb::BigEndianInt32>'.
        //:
        //: 8 A small sample of different 'BigEndianInt32' objects produce
        //:   different hashes.
        //:
        //: 9 Invoking 'bsl::hash<bdlb::BigEndianInt32>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the
        //:   'BigEndianInt32' object.
        //:
        //:10 A 'BigEndianUint32' object can be hashed by instances of
        //:   'bsl::hash<bdlb::BigEndianUint32>'.
        //:
        //:11 A small sample of different 'BigEndianUint32' objects produce
        //:   different hashes.
        //:
        //:12 Invoking 'bsl::hash<bdlb::BigEndianUint32>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the
        //:   'BigEndianUint32' object.
        //:
        //:13 A 'BigEndianInt64' object can be hashed by instances of
        //:   'bsl::hash<bdlb::BigEndianInt6432>'.
        //:
        //:14 A small sample of different 'BigEndianInt64' objects produce
        //:   different hashes.
        //:
        //:15 Invoking 'bsl::hash<bdlb::BigEndianInt64>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the
        //:   'BigEndianInt64' object.
        //:
        //:16 A 'BigEndianUint64' object can be hashed by instances of
        //:   'bsl::hash<bdlb::BigEndianUint64>'.
        //:
        //:17 A small sample of different 'BigEndianUint64' objects produce
        //:   different hashes.
        //:
        //:18 Invoking 'bsl::hash<bdlb::BigEndianUint64>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the
        //:   'BigEndianUint64' object.
        //
        // Plan:
        //: 1 Hash some different 'BigEndianInt16' objects and verify that the
        //:   result of using 'bsl::hash<bdlb::BigEndianInt16>' is identical to
        //:   invoking 'bslh::DefaultHashAlgorithm' on the underlying
        //:   attributes of the 'BigEndianInt16' object.  (C-1,3)
        //:
        //: 2 Hash a number of different 'BigEndianInt16' objects and verify that
        //:   they produce distinct hashes.  (C-2)
        //:
        //: 3 Hash some different 'BigEndianUint16' objects and verify that the
        //:   result of using 'bsl::hash<bdlb::BigEndianUint16>' is identical to
        //:   invoking 'bslh::DefaultHashAlgorithm' on the underlying
        //:   attributes of the 'BigEndianUint16' object.  (C-4,7)
        //:
        //: 4 Hash a number of different 'BigEndianUint16' objects and verify that
        //:   they produce distinct hashes.  (C-5)
        //:
        //: 5 Hash some different 'BigEndianInt32' objects and verify that the
        //:   result of using 'bsl::hash<bdlb::BigEndianInt32>' is identical to
        //:   invoking 'bslh::DefaultHashAlgorithm' on the underlying
        //:   attributes of the 'BigEndianInt32' object.  (C-7,9)
        //:
        //: 6 Hash a number of different 'BigEndianInt32' objects and verify that
        //:   they produce distinct hashes.  (C-8)
        //:
        //: 7 Hash some different 'BigEndianUint32' objects and verify that the
        //:   result of using 'bsl::hash<bdlb::BigEndianUint32>' is identical to
        //:   invoking 'bslh::DefaultHashAlgorithm' on the underlying
        //:   attributes of the 'BigEndianUint32' object.  (C-10,12)
        //:
        //: 8 Hash a number of different 'BigEndianUint32' objects and verify that
        //:   they produce distinct hashes.  (C-11)
        //:
        //: 9 Hash some different 'BigEndianInt64' objects and verify that the
        //:   result of using 'bsl::hash<bdlb::BigEndianInt64>' is identical to
        //:   invoking 'bslh::DefaultHashAlgorithm' on the underlying
        //:   attributes of the 'BigEndianInt64' object.  (C-13,15)
        //:
        //:10 Hash a number of different 'BigEndianInt64' objects and verify that
        //:   they produce distinct hashes.  (C-14)
        //:
        //:11 Hash some different 'BigEndianUint64' objects and verify that the
        //:   result of using 'bsl::hash<bdlb::BigEndianUint64>' is identical to
        //:   invoking 'bslh::DefaultHashAlgorithm' on the underlying
        //:   attributes of the 'BigEndianUint64' object.  (C-16,18)
        //:
        //:12 Hash a number of different 'BigEndianUint64' objects and verify that
        //:   they produce distinct hashes.  (C-17)
        //
        // Testing:
        //   hashAppend(hashAlg, BigEndianInt16)
        //   hashAppend(hashAlg, BigEndianUint16)
        //   hashAppend(hashAlg, BigEndianInt32)
        //   hashAppend(hashAlg, BigEndianUint32)
        //   hashAppend(hashAlg, BigEndianInt64)
        //   hashAppend(hashAlg, BigEndianUint64)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING HASH FUNCTION"
                             "\n=====================\n";

        static struct TestData {
            long long           d_line;
            short               d_i16;
            unsigned short      d_u16;
            int                 d_i32;
            unsigned int        d_u32;
            bsls::Types::Int64  d_i64;
            bsls::Types::Uint64 d_u64;
        } k_DATA[] =
        {
            { L_,  0,  0,  0,  0,  0,  0 },
            { L_, -2,  1, -2,  1, -2,  1 },
            { L_,  1, 10,  1, 10,  1, 10 },
            { L_, -3, 31, -3, 31, -3, 31 },
            { L_, 32, 32, 32, 32, 32, 32 },
            { L_,-33, 42,-33, 42,-33, 42 },
            { L_, 42,132, 42,132, 42,132 },
            { L_,-43,742,-43,742,-43,742 },
        };

        using namespace bdlb;
        const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

        bsl::hash<BigEndianInt16>  bslHashFuncI16;
        bsl::hash<BigEndianUint16> bslHashFuncU16;
        bsl::hash<BigEndianInt32>  bslHashFuncI32;
        bsl::hash<BigEndianUint32> bslHashFuncU32;
        bsl::hash<BigEndianInt64>  bslHashFuncI64;
        bsl::hash<BigEndianUint64> bslHashFuncU64;
        bsl::set<bsl::size_t>      hashResI16;
        bsl::set<bsl::size_t>      hashResU16;
        bsl::set<bsl::size_t>      hashResI32;
        bsl::set<bsl::size_t>      hashResU32;
        bsl::set<bsl::size_t>      hashResI64;
        bsl::set<bsl::size_t>      hashResU64;

        for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
            BigEndianInt16  bei16 = BigEndianInt16::make (k_DATA[i].d_i16);
            BigEndianUint16 beu16 = BigEndianUint16::make(k_DATA[i].d_u16);
            BigEndianInt32  bei32 = BigEndianInt32::make (k_DATA[i].d_i32);
            BigEndianUint32 beu32 = BigEndianUint32::make(k_DATA[i].d_u32);
            BigEndianInt64  bei64 = BigEndianInt64::make (k_DATA[i].d_i64);
            BigEndianUint64 beu64 = BigEndianUint64::make(k_DATA[i].d_u64);

            bslh::Hash<>          defaultHashAlgorithm;

            ASSERT(bslHashFuncI16(bei16) == defaultHashAlgorithm(bei16));
            ASSERT(bslHashFuncU16(beu16) == defaultHashAlgorithm(beu16));
            ASSERT(bslHashFuncI32(bei32) == defaultHashAlgorithm(bei32));
            ASSERT(bslHashFuncU32(beu32) == defaultHashAlgorithm(beu32));
            ASSERT(bslHashFuncI64(bei64) == defaultHashAlgorithm(bei64));
            ASSERT(bslHashFuncU64(beu64) == defaultHashAlgorithm(beu64));
            ASSERT(hashResI16.insert(bslHashFuncI16(bei16)).second);
            ASSERT(hashResU16.insert(bslHashFuncU16(beu16)).second);
            ASSERT(hashResI32.insert(bslHashFuncI32(bei32)).second);
            ASSERT(hashResU32.insert(bslHashFuncU32(beu32)).second);
            ASSERT(hashResI64.insert(bslHashFuncI64(bei64)).second);
            ASSERT(hashResU64.insert(bslHashFuncU64(beu64)).second);
        }
        ASSERT(hashResI16.size() == k_NUM_TESTS);
        ASSERT(hashResU16.size() == k_NUM_TESTS);
        ASSERT(hashResI32.size() == k_NUM_TESTS);
        ASSERT(hashResU32.size() == k_NUM_TESTS);
        ASSERT(hashResI64.size() == k_NUM_TESTS);
        ASSERT(hashResU64.size() == k_NUM_TESTS);
      } break;
      case 9: {
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
      case 8: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
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
                          << "TESTING STREAMING FUNCTIONALITY" << endl
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj& X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion());
                ASSERT(VERSION == Obj::maxSupportedBdexVersion());
            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22

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
                    for (bsl::size_t i = 5; i < LOD; ++i) {
                        cout << "\t  [" << static_cast<int>(OD[i])   << "]"
                             << "\t  [" << static_cast<int>(TD[i-5]) << "]"
                             << endl;
                    }
                }

                for (bsl::size_t i = 5; i < LOD; ++i) {
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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT));

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj& X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion());
                ASSERT(VERSION == Obj::maxSupportedBdexVersion());
            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_USHORT) / sizeof(*VALUES_USHORT));

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion());
                ASSERT(VERSION == Obj::maxSupportedBdexVersion());
            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22

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
                    for (bsl::size_t i = 5; i < LOD; ++i) {
                        cout << "\t  [" << static_cast<int>(OD[i])   << "]"
                             << "\t  [" << static_cast<int>(TD[i-5]) << "]"
                             << endl;
                    }
                }

                for (bsl::size_t i = 5; i < LOD; ++i) {
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
            const int NUM_VALUES = static_cast<int>(
                                     sizeof(VALUES_INT) / sizeof(*VALUES_INT));

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion());
                ASSERT(VERSION == Obj::maxSupportedBdexVersion());
            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22

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
            const int NUM_VALUES = static_cast<int>(
                                   sizeof(VALUES_UINT) / sizeof(*VALUES_UINT));

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion());
                ASSERT(VERSION == Obj::maxSupportedBdexVersion());
            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22

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
                    for (bsl::size_t i = 5; i < LOD; ++i) {
                        cout << "\t  [" << static_cast<int>(OD[i])   << "]"
                             << "\t  [" << static_cast<int>(TD[i-5]) << "]"
                             << endl;
                    }
                }

                for (bsl::size_t i = 5; i < LOD; ++i) {
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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_INT64) / sizeof(*VALUES_INT64));

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
            {
                Obj mX = Obj::make(1); const Obj X = mX;

                ASSERT(VERSION == X.maxSupportedBdexVersion());
                ASSERT(VERSION == Obj::maxSupportedBdexVersion());
            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED  -- BDE2.22

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_UINT64) / sizeof(*VALUES_UINT64));

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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT));

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_USHORT) / sizeof(*VALUES_USHORT));

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
            const int NUM_VALUES = static_cast<int>(
                                     sizeof(VALUES_INT) / sizeof(*VALUES_INT));

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
            const int NUM_VALUES = static_cast<int>(
                                   sizeof(VALUES_UINT) / sizeof(*VALUES_UINT));

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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_INT64) / sizeof(*VALUES_INT64));

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_UINT64) / sizeof(*VALUES_UINT64));

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
        const int NUM_DATA = static_cast<int>(sizeof(DATA) / sizeof(*DATA));

        if (verbose) cout << endl
                          << "Testing 'print' and 'operator<<'." << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef bdlb::BigEndianInt16 Obj;

            const short *VALUES = VALUES_SHORT;
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT));

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_USHORT) / sizeof(*VALUES_USHORT));

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
            const int NUM_VALUES = static_cast<int>(
                                     sizeof(VALUES_INT) / sizeof(*VALUES_INT));

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
            const int NUM_VALUES = static_cast<int>(
                                   sizeof(VALUES_UINT) / sizeof(*VALUES_UINT));

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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_INT64) / sizeof(*VALUES_INT64));

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_UINT64) / sizeof(*VALUES_UINT64));

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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_SHORT) / sizeof(*VALUES_SHORT));

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_USHORT) / sizeof(*VALUES_USHORT));

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
            const int NUM_VALUES = static_cast<int>(
                                     sizeof(VALUES_INT) / sizeof(*VALUES_INT));

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
            const int NUM_VALUES = static_cast<int>(
                                   sizeof(VALUES_UINT) / sizeof(*VALUES_UINT));

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
            const int NUM_VALUES = static_cast<int>(
                                 sizeof(VALUES_INT64) / sizeof(*VALUES_INT64));

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
            const int NUM_VALUES = static_cast<int>(
                               sizeof(VALUES_UINT64) / sizeof(*VALUES_UINT64));

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
        // BREATHING TEST
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
