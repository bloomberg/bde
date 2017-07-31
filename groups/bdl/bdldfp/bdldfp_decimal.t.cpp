// bdldfp_decimal.t.cpp                                               -*-C++-*-
#include <bdldfp_decimal.h>

#include <bdlb_randomdevice.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_climits.h> // CHAR_BIT
#include <bsl_limits.h>

#include <typeinfo>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides a set of value-semantic classes which
// represent a floating point numbers in the decimal base, where each class has
// a different precision.
// ----------------------------------------------------------------------------
// CREATORS
//: o 'Decimal_Type32()'
//: o 'Decimal_Type32(DecimalImplUtil::ValueType32)'
//: o 'Decimal_Type32(Decimal64)'
//: o 'Decimal_Type32(float)'
//: o 'Decimal_Type32(double)'
//: o 'Decimal_Type32(long double)'
//: o 'Decimal_Type32(int)'
//: o 'Decimal_Type32(unsigned int)'
//: o 'Decimal_Type32(long int)'
//: o 'Decimal_Type32(unsigned long int)'
//: o 'Decimal_Type32(long long)'
//: o 'Decimal_Type32(unsigned long long)'
//: o 'Decimal_Type64()'
//: o 'Decimal_Type64(DecimalImplUtil::ValueType64)'
//: o 'Decimal_Type64(Decimal32)'
//: o 'Decimal_Type64(Decimal128)'
//: o 'Decimal_Type64(float)'
//: o 'Decimal_Type64(double)'
//: o 'Decimal_Type64(long double)'
//: o 'Decimal_Type64(int)'
//: o 'Decimal_Type64(unsigned int)'
//: o 'Decimal_Type64(long)'
//: o 'Decimal_Type64(unsigned long)'
//: o 'Decimal_Type64(long long)'
//: o 'Decimal_Type64(unsigned long long)'
//: o 'Decimal_Type128()'
//: o 'Decimal_Type128(DecimalImplUtil::ValueType128)'
//: o 'Decimal_Type128(Decimal32)'
//: o 'Decimal_Type128(Decimal64)'
//: o 'Decimal_Type128(float)'
//: o 'Decimal_Type128(double)'
//: o 'Decimal_Type128(long double)'
//: o 'Decimal_Type128(int)'
//: o 'Decimal_Type128(unsigned int)'
//: o 'Decimal_Type128(long)'
//: o 'Decimal_Type128(unsigned long)'
//: o 'Decimal_Type128(long long)'
//: o 'Decimal_Type128(unsigned long long)'
//
// MANIPULATORS
//: o 'operator='
//: o 'operator++'
//: o 'operator--'
//: o 'operator+='
//: o 'operator-='
//: o 'operator*='
//: o 'operator/='
//: o 'data'
//
// ACCESSORS
//: o 'data'
//: o 'value'
//: o 'print'
//
// FREE OPERATORS
//: o 'operator+' -- Unary
//: o 'operator++'
//: o 'operator--'
//: o 'operator+'
//: o 'operator-'
//: o 'operator*'
//: o 'operator/'
//: o 'operator=='
//: o 'operator!='
//: o 'operator<'
//: o 'operator>'
//: o 'operator<='
//: o 'operator>='
//: o 'operator>> (bsl::basic_istream<CHARTYPE, TRAITS>& stream...'
//: o 'operator>> (bsl::basic_ostream<CHARTYPE, TRAITS>& stream...'
//
// FREE FUNCTIONS
//: o void hashAppend(HASHALG& hashAlg, const Decimal32& object);
//: o void hashAppend(HASHALG& hashAlg, const Decimal64& object);
//: o void hashAppend(HASHALG& hashAlg, const Decimal128& object);
//
// ----------------------------------------------------------------------------
// [ 1] Implementation Assumptions
// [ 2] Decimal32 Type
// [ 3] Decimal64 Type
// [ 4] Decimal128 Type
// [ 8] USAGE EXAMPLE
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

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;
static bslma::TestAllocator *pa;

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
  // TBD
}  // close namespace UsageExample

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                 // stringstream helpers - not thread safe!

void getStringFromStream(bsl::ostringstream &o, bsl::string  *out)
{
    bslma::TestAllocator osa("osstream");
    bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

void getStringFromStream(bsl::wostringstream &o, bsl::wstring *out)
{
    bslma::TestAllocator osa("osstream");
    bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

 // String compare for decimal floating point numbers needs 'e'/'E' conversion

bsl::string& decLower(bsl::string& s)
{
    for (size_t i = 0; i < s.length(); ++i) if ('E' == s[i]) s[i] = 'e';
    return s;
}

bsl::wstring& decLower(bsl::wstring& s)
{
    for (size_t i = 0; i < s.length(); ++i) if (L'E' == s[i]) s[i] = L'e';
    return s;
}

//-----------------------------------------------------------------------------


template <class EXPECT, class RECEIVED>
void checkType(const RECEIVED&)
{
    ASSERT(typeid(EXPECT) == typeid(RECEIVED));
}

                          // Stream buffer helpers

template <int SIZE>
struct BufferBuf : bsl::streambuf {
    BufferBuf() { reset(); }
    const char *str() { *this->pptr() =0; return this->pbase(); }
    void reset() { this->setp(this->d_buf, this->d_buf + SIZE); }
    char d_buf[SIZE + 1];
};

struct PtrInputBuf : bsl::streambuf {
    PtrInputBuf(const char *s) {
        char *x = const_cast<char *>(s);
        this->setg(x, x, x + strlen(x));
    }
};

struct NulBuf : bsl::streambuf {
    char d_dummy[64];
    virtual int overflow(int c) {
        setp( d_dummy, d_dummy + sizeof(d_dummy));
        return traits_type::not_eof(c);
    }
};

long long int clipValue(long long int value,
                        long long int min,
                        long long int max)
    // Clip the specified 'value' to fit segment between the specified 'min'
    // and the specified 'max'.
{
    return static_cast<long long int>(
           (static_cast<unsigned long long int>(value)%(max - min + 1)) + min);
}

bdldfp::Decimal32 randomDecimal32()
    // Return randomly generated Decimal32 object having finite value on
    // success and infinity value otherwise.
{
    int significand = 0;
    int exponent = 0;

    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                reinterpret_cast<unsigned char*>(&significand),
                                sizeof(int));
    ASSERTV(rc, 0 == rc);

    significand = static_cast<int>(clipValue( significand,
                                             -9999999,
                                              9999999));

    rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                   reinterpret_cast<unsigned char*>(&exponent),
                                   sizeof(int));
    ASSERTV(rc, 0 == rc);

    exponent = static_cast<int>(clipValue(exponent, -101, 90));

    return BDEC::DecimalImpUtil::makeDecimalRaw32(significand, exponent);
}

bdldfp::Decimal64 randomDecimal64()
    // Return randomly generated Decimal64 object having finite value on
    // success and infinity value otherwise.
{
    long long int significand = 0;
    int           exponent = 0;

    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                reinterpret_cast<unsigned char*>(&significand),
                                sizeof(long long int));
    ASSERTV(rc, 0 == rc);

    significand = clipValue( significand,
                            -9999999999999999LL,
                             9999999999999999LL);

    rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                   reinterpret_cast<unsigned char*>(&exponent),
                                   sizeof(int));
    ASSERTV(rc, 0 == rc);

    exponent = static_cast<int>(clipValue(exponent, -398, 369));

    return BDEC::DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}

bdldfp::Decimal128 randomDecimal128()
    // Return randomly generated Decimal128 object having finite value on
    // success and infinity value otherwise.
{
    long long int significand = 0;
    int           exponent = 0;

    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                reinterpret_cast<unsigned char*>(&significand),
                                sizeof(long long int));
    ASSERTV(rc, 0 == rc);

    rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
                                   reinterpret_cast<unsigned char*>(&exponent),
                                   sizeof(int));
    ASSERTV(rc, 0 == rc);

    exponent = static_cast<int>(clipValue(exponent, -6176, 6111));

    return BDEC::DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}


//=============================================================================
//                      TEST DRIVER NAMESPACE CLASS
//-----------------------------------------------------------------------------

struct TestDriver {
    // This class provides a namespace for the various test-case functions in
    // the test driver main program.  This class is necessitated by
    // compile-time performance issues on some platforms.

    static void testCase8();
    static void testCase7();
    static void testCase6();
    static void testCase5();
    static void testCase4();
    static void testCase3();
    static void testCase2();
    static void testCase1();

};

void TestDriver::testCase8()
{
    // ------------------------------------------------------------------------
    // USAGE EXAMPLE
    //   Extracted from component header file.
    //
    // Concerns:
    //: 1 The usage example provided in the component header file compiles,
    //:   links, and runs as shown.
    //
    // Plan:
    //: 1 Incorporate usage example from header into test driver, remove
    //:   leading comment characters, and replace 'assert' with 'ASSERT'.
    //:   (C-1)
    //
    // Testing:
    //   USAGE EXAMPLE
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << bsl::endl
                           << "Testing Usage Example" << bsl::endl
                           << "=====================" << bsl::endl;

    if (veryVerbose) bsl::cout << bsl::endl
                               << "Portable initialization of "
                               << "non-integer, constant values"
                               << bsl::endl;
    {
        // If your compiler does not support the C Decimal TR, it does not
        // support decimal floating point literals, only binary floating
        // point literals.  The problem with binary floating point literals
        // is the same as with binary floating point numbers in general:
        // they cannot represent the decimal numbers we care about.  To
        // solve this problem there are 3 macros provided by this component
        // that can be used to initialize decimal floating point types with
        // non-integer values, precisely.  These macros will evaluate to
        // real, C language literals where those are supported and to a
        // runtime-parsed solution otherwise.  The following code
        // demonstrates the use of these macros as well as mixed-type
        // arithmetics and comparisons:
        //..
        bdldfp::Decimal32  d32( BDLDFP_DECIMAL_DF(0.1));
        bdldfp::Decimal64  d64( BDLDFP_DECIMAL_DD(0.2));
        bdldfp::Decimal128 d128(BDLDFP_DECIMAL_DL(0.3));

        ASSERT(d32 + d64 == d128);
        ASSERT(bdldfp::Decimal64(d32)  * 10 == bdldfp::Decimal64(1));
        ASSERT(d64  * 10 == bdldfp::Decimal64(2));
        ASSERT(d128 * 10 == bdldfp::Decimal128(3));
    }

    if (veryVerbose) bsl::cout << bsl::endl
                               << "Precise calculations with decimal "
                               << "values" << bsl::endl;
    {
        //..
        // Suppose we need to add two (decimal) numbers and then tell if
        // the result is a particular decimal number or not.  That can get
        // difficult with binary floating-point, but easy with decimal:
        //..
        if (std::numeric_limits<double>::radix == 2) {
           ASSERT(.1 + .2 != .3);
        }
        ASSERT(BDLDFP_DECIMAL_DD(0.1) +  BDLDFP_DECIMAL_DD(0.2) ==
               BDLDFP_DECIMAL_DD(0.3));
        //..
    }
}

void TestDriver::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //: 1 The 'hashAppend' function hashes objects with different values
    //:   differently.
    //:
    //: 2 The 'hashAppend' function hashes objects with the same values
    //:   identically irregardless their representations.
    //:
    //: 3 The 'hashAppend' function hashes const and non-const objects.
    //
    // Plan:
    //: 1 Brute force test of a several hand picked and randomly generated
    //:   values, ensuring that hashes of equivalent objects match and hashes
    //:   of unequal objects do not.  (C-1..3)
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const Decimal32& object);
    //   void hashAppend(HASHALG& hashAlg, const Decimal64& object);
    //   void hashAppend(HASHALG& hashAlg, const Decimal128& object);
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << bsl::endl
                           << "Testing 'hashAppend'" << bsl::endl
                           << "====================" << bsl::endl;

    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef Hasher::result_type         HashType;

    Hasher hasher;

    bslma::TestAllocator va("vector", veryVeryVeryVerbose);

    if (verbose) bsl::cout << "\tTesting Decimal32" << bsl::endl;
    {
        typedef BDEC::Decimal32                      Obj;
        typedef bsl::numeric_limits<BDEC::Decimal32> d32_limits;
        typedef bsl::pair<Obj, HashType>             TestDataPair;
        typedef bsl::vector<TestDataPair>            TestDataVector;

        TestDataVector testData(&va);

        // Adding boundary and special values.

        const Obj ZERO_P        = BDLDFP_DECIMAL_DF(0.00);
        const Obj MIN_P         = d32_limits::min();
        const Obj MAX_P         = d32_limits::max();
        const Obj EPSILON_P     = d32_limits::epsilon();
        const Obj ROUND_ERROR_P = d32_limits::round_error();
        const Obj INFINITE_P    = d32_limits::infinity();
        const Obj DENORM_MIN_P  = d32_limits::denorm_min();
        const Obj ZERO_N        = -ZERO_P;
        const Obj MIN_N         = -MIN_P;
        const Obj MAX_N         = -MAX_P;
        const Obj EPSILON_N     = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N    = -INFINITE_P;
        const Obj DENORM_MIN_N  = -DENORM_MIN_P;

        const Obj Q_NAN      = d32_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN      = d32_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P       , hasher(ZERO_P       )));
        testData.push_back(TestDataPair(MIN_P        , hasher(MIN_P        )));
        testData.push_back(TestDataPair(MAX_P        , hasher(MAX_P        )));
        testData.push_back(TestDataPair(EPSILON_P    , hasher(EPSILON_P    )));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P   , hasher(INFINITE_P   )));
        testData.push_back(TestDataPair(DENORM_MIN_P , hasher(DENORM_MIN_P )));
        testData.push_back(TestDataPair(ZERO_N       , hasher(ZERO_N       )));
        testData.push_back(TestDataPair(MIN_N        , hasher(MIN_N        )));
        testData.push_back(TestDataPair(MAX_N        , hasher(MAX_N        )));
        testData.push_back(TestDataPair(EPSILON_N    , hasher(EPSILON_N    )));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N   , hasher(INFINITE_N   )));
        testData.push_back(TestDataPair(DENORM_MIN_N , hasher(DENORM_MIN_N )));

        // Adding identical values, having different representations:
        // 1e+3
        // 10e+2
        // ...
        // 100000e-2
        // 1000000e-3

        int       cloneSignificand = 1;
        int       cloneExponent    = 3;
        const Obj ORIGINAL         = BDEC::DecimalImpUtil::makeDecimalRaw32(
                                                              cloneSignificand,
                                                              cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 6; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw32(
                                                              cloneSignificand,
                                                              cloneExponent);
            ASSERTV(ORIGINAL, CLONE, ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DF(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 90);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            Obj randomValue = randomDecimal32();
            testData.push_back(TestDataPair(randomValue,
                                            hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1  = iter1->second;
            TestDataVector::iterator iter2  = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2  = iter2->second;
                if (VALUE1 == VALUE2) {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 == HASH2);
                } else {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 != HASH2);
                }
                ++iter2;
            }
            ASSERT(Q_NAN_HASH != HASH1);
            ASSERT(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }

    if (verbose) bsl::cout << "\tTesting Decimal64" << bsl::endl;
    {
        typedef BDEC::Decimal64                      Obj;
        typedef bsl::numeric_limits<BDEC::Decimal32> d64_limits;
        typedef bsl::pair<Obj, HashType>             TestDataPair;
        typedef bsl::vector<TestDataPair>            TestDataVector;

        TestDataVector       testData(&va);

        // Adding boundary and special values.

        const Obj ZERO_P        = BDLDFP_DECIMAL_DD(0.00);
        const Obj MIN_P         = d64_limits::min();
        const Obj MAX_P         = d64_limits::max();
        const Obj EPSILON_P     = d64_limits::epsilon();
        const Obj ROUND_ERROR_P = d64_limits::round_error();
        const Obj INFINITE_P    = d64_limits::infinity();
        const Obj DENORM_MIN_P  = d64_limits::denorm_min();
        const Obj ZERO_N        = -ZERO_P;
        const Obj MIN_N         = -MIN_P;
        const Obj MAX_N         = -MAX_P;
        const Obj EPSILON_N     = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N    = -INFINITE_P;
        const Obj DENORM_MIN_N  = -DENORM_MIN_P;

        const Obj Q_NAN      = d64_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN      = d64_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P       , hasher(ZERO_P       )));
        testData.push_back(TestDataPair(MIN_P        , hasher(MIN_P        )));
        testData.push_back(TestDataPair(MAX_P        , hasher(MAX_P        )));
        testData.push_back(TestDataPair(EPSILON_P    , hasher(EPSILON_P    )));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P   , hasher(INFINITE_P   )));
        testData.push_back(TestDataPair(DENORM_MIN_P , hasher(DENORM_MIN_P )));
        testData.push_back(TestDataPair(ZERO_N       , hasher(ZERO_N       )));
        testData.push_back(TestDataPair(MIN_N        , hasher(MIN_N        )));
        testData.push_back(TestDataPair(MAX_N        , hasher(MAX_N        )));
        testData.push_back(TestDataPair(EPSILON_N    , hasher(EPSILON_N    )));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N   , hasher(INFINITE_N   )));
        testData.push_back(TestDataPair(DENORM_MIN_N , hasher(DENORM_MIN_N )));

        // Adding identical values, having different representations:
        // 1e+7
        // 10e+6
        // ...
        // 100000000000000e-7
        // 1000000000000000e-8

        long long cloneSignificand = 1;
        int       cloneExponent    = 7;
        const Obj ORIGINAL         = BDEC::DecimalImpUtil::makeDecimalRaw64(
                                                              cloneSignificand,
                                                              cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 15; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw64(
                                                              cloneSignificand,
                                                              cloneExponent);
            ASSERTV(ORIGINAL, CLONE, ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DD(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 100);
        const Obj ZERO5 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 369);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));
        testData.push_back(TestDataPair(ZERO5, hasher(ZERO5)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            const Obj randomValue = randomDecimal64();
            testData.push_back(TestDataPair(randomValue,
                                            hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1  = iter1->second;
            TestDataVector::iterator iter2  = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2  = iter2->second;
                if (VALUE1 == VALUE2) {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 == HASH2);
                } else {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 != HASH2);
                }
                ++iter2;
            }
            ASSERT(Q_NAN_HASH != HASH1);
            ASSERT(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }

    if (verbose) bsl::cout << "\tTesting Decimal128" << bsl::endl;
    {
        typedef BDEC::Decimal128                     Obj;
        typedef bsl::numeric_limits<BDEC::Decimal32> d128_limits;
        typedef bsl::pair<Obj, HashType>             TestDataPair;
        typedef bsl::vector<TestDataPair>            TestDataVector;

        TestDataVector       testData(&va);

        // Adding boundary and special values.

        const Obj ZERO_P        = BDLDFP_DECIMAL_DL(0.00);
        const Obj MIN_P         = d128_limits::min();
        const Obj MAX_P         = d128_limits::max();
        const Obj EPSILON_P     = d128_limits::epsilon();
        const Obj ROUND_ERROR_P = d128_limits::round_error();
        const Obj INFINITE_P    = d128_limits::infinity();
        const Obj DENORM_MIN_P  = d128_limits::denorm_min();
        const Obj ZERO_N        = -ZERO_P;
        const Obj MIN_N         = -MIN_P;
        const Obj MAX_N         = -MAX_P;
        const Obj EPSILON_N     = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N    = -INFINITE_P;
        const Obj DENORM_MIN_N  = -DENORM_MIN_P;

        const Obj Q_NAN      = d128_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN      = d128_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P       , hasher(ZERO_P       )));
        testData.push_back(TestDataPair(MIN_P        , hasher(MIN_P        )));
        testData.push_back(TestDataPair(MAX_P        , hasher(MAX_P        )));
        testData.push_back(TestDataPair(EPSILON_P    , hasher(EPSILON_P    )));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P   , hasher(INFINITE_P   )));
        testData.push_back(TestDataPair(DENORM_MIN_P , hasher(DENORM_MIN_P )));
        testData.push_back(TestDataPair(ZERO_N       , hasher(ZERO_N       )));
        testData.push_back(TestDataPair(MIN_N        , hasher(MIN_N        )));
        testData.push_back(TestDataPair(MAX_N        , hasher(MAX_N        )));
        testData.push_back(TestDataPair(EPSILON_N    , hasher(EPSILON_N    )));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N   , hasher(INFINITE_N   )));
        testData.push_back(TestDataPair(DENORM_MIN_N , hasher(DENORM_MIN_N )));

        // Adding identical values, having different representations:
        // 1e+9
        // 10e+8
        // ...
        // 100000000000000000e-8
        // 1000000000000000000e-9

        long long cloneSignificand = 1;
        int       cloneExponent    = 9;
        const Obj ORIGINAL         = BDEC::DecimalImpUtil::makeDecimalRaw128(
                                                              cloneSignificand,
                                                              cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 18; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw128(
                                                              cloneSignificand,
                                                              cloneExponent);
            ASSERTV(ORIGINAL, CLONE, ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DL(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 100);
        const Obj ZERO5 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 1000);
        const Obj ZERO6 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 6111);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));
        testData.push_back(TestDataPair(ZERO5, hasher(ZERO5)));
        testData.push_back(TestDataPair(ZERO6, hasher(ZERO6)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            const Obj randomValue = randomDecimal128();
            testData.push_back(TestDataPair(randomValue,
                                            hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1  = iter1->second;
            TestDataVector::iterator iter2  = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2  = iter2->second;
                if (VALUE1 == VALUE2) {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 == HASH2);
                } else {
                    ASSERTV(VALUE1, VALUE2, HASH1, HASH2, HASH1 != HASH2);
                }
                ++iter2;
            }
            ASSERT(Q_NAN_HASH != HASH1);
            ASSERT(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }
}

void TestDriver::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING BDEX STREAMING
    //   Verify the BDEX streaming implementation works correctly.  Specific
    //   concerns include wire format, handling of stream states (valid, empty,
    //   invalid, incomplete, and corrupted), and exception neutrality.
    //
    // Concerns:
    //: 1 The class method 'maxSupportedBdexVersion' returns the correct
    //:   version to be used for the specified 'versionSelector'.
    //:
    //: 2 The 'bdexStreamOut' method is callable on a reference providing only
    //:   non-modifiable access.
    //:
    //: 3 For valid streams, externalization and unexternalization are inverse
    //:   operations.
    //:
    //: 4 For invalid streams, externalization leaves the stream invalid and
    //:   unexternalization does not alter the value of the object and leaves
    //:   the stream invalid.
    //:
    //: 5 Unexternalizing of incomplete, invalid, or corrupted data results in
    //:   a valid object of unspecified value and an invalidated stream.
    //:
    //: 6 The wire format of the object is as expected.
    //:
    //: 7 All methods are exception neutral.
    //:
    //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference to
    //:   the provided stream in all situations.
    //:
    //: 9 The initial value of the object has no affect on unexternalization.
    //:
    //:10 Streaming version 1 provides the expected compatibility between
    //:   the two calendar modes.
    //
    // Testing:
    //   static int maxSupportedBdexVersion(int versionSelector);
    //   static int maxSupportedBdexVersion();
    //   STREAM& bdexStreamIn(STREAM& stream, int version);
    //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING BDEX STREAMING" << endl
                      << "======================" << endl;

    // Allocator to use instead of the default allocator.
    bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

    // Scalar obj
    // ect values used in various stream tests.

    typedef BDEC::Decimal64 Obj;
    typedef bslx::TestInStream In;
    typedef bslx::TestOutStream Out;

    const Obj VA(BDLDFP_DECIMAL_DD(1.12));
    const Obj VB(BDLDFP_DECIMAL_DD(2.55));
    const Obj VC(BDLDFP_DECIMAL_DD(3.67));
    const Obj VD(BDLDFP_DECIMAL_DD(5.17));
    const Obj VE(BDEC::DecimalImpUtil::parse64("Inf"));
    const Obj VF(BDEC::DecimalImpUtil::parse64("-Inf"));
    const Obj VG(BDLDFP_DECIMAL_DD(-7.64));

    // Array object used in various stream tests.
    const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
    const int NUM_VALUES =
        static_cast<int>(sizeof VALUES / sizeof *VALUES);
    const int VERSION_SELECTOR = 20151026;

    // Testing maxSupportedBDexVersion
    {
        ASSERT(1 == Obj::maxSupportedBdexVersion());
        ASSERT(1 == Obj::maxSupportedBdexVersion(0));
        ASSERT(1 == Obj::maxSupportedBdexVersion(
                   VERSION_SELECTOR));

        using bslx::VersionFunctions::maxSupportedBdexVersion;
        ASSERT(1 == maxSupportedBdexVersion(
                   reinterpret_cast<Obj *>(0), 0));
        ASSERT(1 == maxSupportedBdexVersion(
                   reinterpret_cast<Obj *>(0), VERSION_SELECTOR));
    }

    // Direct initial trial of 'bdexStreamOut' and (valid) 'bdexStreamIn'
    // functionality.
    const int VERSION = Obj::maxSupportedBdexVersion(0);
    {
        const Obj X(VC);
        Out out(VERSION_SELECTOR, &allocator);

        Out& rvOut = X.bdexStreamOut(out, VERSION);
        ASSERT(&out == &rvOut);

        const char *const OD  = out.data();
        const int         LOD = static_cast<int>(out.length());

        In in(OD, LOD);
        ASSERT(in);
        ASSERT(!in.isEmpty());

        Obj mT(VA);  const Obj& T = mT;
        ASSERT(X != T);

        In& rvIn = mT.bdexStreamIn(in, VERSION);
        ASSERT(&in == &rvIn);
        ASSERT(X == T);
        ASSERT(in);
        ASSERT(in.isEmpty());
    }

    // Testing valid streams
    {
        for (int i = 0; i < NUM_VALUES; ++i) {
            const Obj X(VALUES[i]);

            Out out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            Out& rvOut = bdexStreamOut(out, X, VERSION);
            LOOP_ASSERT(i, &out == &rvOut);
            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            // Verify that each new value overwrites every old value and
            // that the input stream is emptied, but remains valid.

            for (int j = 0; j < NUM_VALUES; ++j) {
                In in(OD, LOD);
                LOOP2_ASSERT(i, j, in);
                LOOP2_ASSERT(i, j, !in.isEmpty());

                Obj mT(VALUES[j]);  const Obj& T = mT;
                LOOP2_ASSERT(i, j, (X == T) == (i == j));

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP2_ASSERT(i, j, &in == &rvIn);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;

                LOOP2_ASSERT(i, j, X == T);
                LOOP2_ASSERT(i, j, in);
                LOOP2_ASSERT(i, j, in.isEmpty());
            }
        }
    }

    // Testing empty streams (valid and invalid).
    {
        Out               out(VERSION_SELECTOR, &allocator);
        const char *const OD  = out.data();
        const int         LOD = static_cast<int>(out.length());
        ASSERT(0 == LOD);

        for (int i = 0; i < NUM_VALUES; ++i) {
            In in(OD, LOD);
            LOOP_ASSERT(i, in);
            LOOP_ASSERT(i, in.isEmpty());

            // Ensure that reading from an empty or invalid input stream
            // leaves the stream invalid and the target object unchanged.

            using bslx::InStreamFunctions::bdexStreamIn;

            const Obj  X(VALUES[i]);
            Obj        mT(X);
            const Obj& T = mT;
            LOOP_ASSERT(i, X == T);

            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                in.reset();

                // Stream is valid.
                In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(i, &in == &rvIn1);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == T);

                // Stream is invalid.
                In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(i, &in == &rvIn2);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == T);
            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
        }
    }

    // Testing non-empty, invalid streams.

    {
        Out out(VERSION_SELECTOR, &allocator);

        using bslx::OutStreamFunctions::bdexStreamOut;
        Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
        ASSERT(&out == &rvOut);

        const char *const OD  = out.data();
        const int         LOD = static_cast<int>(out.length());
        ASSERT(0 < LOD);

        for (int i = 0; i < NUM_VALUES; ++i) {
            In in(OD, LOD);
            in.invalidate();
            LOOP_ASSERT(i, !in);
            LOOP_ASSERT(i, !in.isEmpty());

            // Ensure that reading from a non-empty, invalid input stream
            // leaves the stream invalid and the target object unchanged.

            using bslx::InStreamFunctions::bdexStreamIn;

            const Obj  X(VALUES[i]);
            Obj        mT(X);
            const Obj& T = mT;
            LOOP_ASSERT(i, X == T);

            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                in.reset();
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                In& rvIn = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(i, &in == &rvIn);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == T);
            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
        }
    }

    // Test wire-format
    {
        for (int i = 0; i < NUM_VALUES; ++i) {
            const Obj X(VALUES[i]);
            bslx::ByteOutStream out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamOut(out, X, VERSION);
            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            bdldfp::BinaryIntegralDecimalImpUtil::StorageType64 bidVal =
                bdldfp::DecimalImpUtil::convertToBID(VALUES[i].value());
            bsls::Types::Uint64 expectedValue = BSLS_BYTEORDER_HTONLL(
                bidVal.d_raw);

            bsl::cout << LOD << bsl::endl;
            ASSERTV(i, X, LOD == 8);
            ASSERT(memcmp(OD, &expectedValue, 8) == 0);
        }
    }
}

void TestDriver::testCase5()
{
    // ------------------------------------------------------------------------
    // TESTING PRINT METHOD
    //
    // Concerns:
    //: 1 The 'print' method writes the value to the specified 'ostream'.
    //:
    //: 2 The 'print' method write the value in the intended format.
    //:
    //: 3 The 'print' method uses the same underlying formatting function as
    //:   'operator<<'.
    //:
    //: 4 The 'print' method's signature and return type are standard.
    //:
    //: 5 The 'print' method returns the supplied 'ostream'.
    //:
    //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
    //:   correct default values (0 and 4, respectively).
    //
    // Testing:
    //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTESTING PRINT METHOD"
                           << "\n===================="
                           << bsl::endl;

    // Verify that the signatures and return types are standard.
    {
        typedef bsl::ostream& (BDEC::Decimal64::*funcPtr)(bsl::ostream&,
                                                          int, int) const;

        funcPtr     printMember = &BDEC::Decimal64::print;

        (void)printMember;
    }

#define DFP(X) BDLDFP_DECIMAL_DD(X)
    static const struct {
        int              d_line;
        BDEC::Decimal64  d_decimalValue;
        int              d_level;
        int              d_spacesPerLevel;
        const char      *d_expected_p;
    } DATA[] = {

        // -8 implies using default values

        //LINE  NUMBER     LEVEL  SPL  EXPECTED
        //----  ---------  -----  ---  --------
        { L_,  DFP(4.25),     0,   0,  "4.25\n" },
        { L_,  DFP(4.25),     0,   1,  "4.25\n" },
        { L_,  DFP(4.25),     0,  -1,  "4.25"   },
        { L_,  DFP(4.25),     0,  -8,  "4.25\n" },

        { L_,  DFP(4.25),     3,   0,  "4.25\n" },
        { L_,  DFP(4.25),     3,   2,  "      4.25\n" },
        { L_,  DFP(4.25),     3,  -2,  "      4.25" },
        { L_,  DFP(4.25),     3,  -8,  "            4.25\n" },
        { L_,  DFP(4.25),    -3,   0,  "4.25\n" },
        { L_,  DFP(4.25),    -3,   2,  "4.25\n" },
        { L_,  DFP(4.25),    -3,  -2,  "4.25" },
        { L_,  DFP(4.25),    -3,  -8,  "4.25\n" },

        { L_,  DFP(4.25),    -8,  -8,  "4.25\n" },
    };

#undef DFP

    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int             LINE     = DATA[ti].d_line;
        const BDEC::Decimal64 VALUE    = DATA[ti].d_decimalValue;
        const int             LEVEL    = DATA[ti].d_level;
        const int             SPL      = DATA[ti].d_spacesPerLevel;
        const char*           EXPECTED = DATA[ti].d_expected_p;

        bsl::stringstream outdec;
        if (-8 != SPL) {
            VALUE.print(outdec, LEVEL, SPL);
        }
        else if (-8 != LEVEL) {
            VALUE.print(outdec, LEVEL);
        }
        else {
            VALUE.print(outdec);
        }
        bsl::string ACTUAL = outdec.str();

        ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
    }
}

void TestDriver::testCase4()
{
    // ------------------------------------------------------------------------
    // TESTING IOSTREAM OPERATORS
    //
    // Concerns:
    //: 1 Calling 'operator<<' on a 'Decimal32', 'Decimal64', 'Decimal128' type
    //:   renders the decimals value to the appropriate stream.
    //:
    //: 2 That 'operator<<' renders a simple decimal value in a fixed point
    //:   format using the correct digits of precisions.
    //:
    //: 3 That 'operator<<' correctly renders infinity, and negative infinity.
    //:
    //: 4 That 'operator<<' correctly renders NaN.
    //:
    //: 5 That 'operator<<' correctly handles a set width.
    //:
    //: 6 That 'operator<<' correctly handles a set width with either a left,
    //:   internal, or right justification.
    //:
    //: 7 That 'operator<<' correctly handles 'bsl::uppercase'.
    //:
    //: 8 That the 'print' method writes the value to the specified 'ostream'.
    //:
    //: 9 That the 'print' method write the vlaue in the intended format.
    //:
    //:10 That the output using 's << obj' is the same as
    //:   'obj.print(s, 0, -1)'.
    //:
    //:11 That 'operator<<' sets the fail and errors bit if the memory buffer
    //:   in the supplied output stream is not large enough.
    //
    // Note that this is not (yet) a complete set of concerns (or test) for
    // this function.
    //
    // Plan:
    //  1 Create a test table, where each element contains a decimal value, a
    //    set of formatting flags, and an expected output.  Iterate over the
    //    test table for 'Decimal32', 'Decimal64', and 'Decimal128' types, and
    //    ensure the streamed output matches the expected value.
    //
    //  2 Stream out a value to an output stream with a fixed memory buffer
    //    that is not large enough.  Make sure that the bad and fail bits are
    //    set in the output stream.
    //
    // Testing:
    //   bsl::basic_ostream& operator<<(bsl::basic_ostream& , Decimal32 );
    //   bsl::basic_ostream& operator<<(bsl::basic_ostream& , Decimal64 );
    //   bsl::basic_ostream& operator<<(bsl::basic_ostream& , Decimal64 );
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTESTING IOSTREAM OPERATORS"
                           << "\n=========================="
                           << bsl::endl;

    // Note that this test is not yet complete.  Possible improvements:
    //
    //: o Providing expected values more precisely (significant, exponent)
    //:   rather than using 'BDLDFP_DECIMAL_DF'
    //:
    //: o Testing a wider array of numbers, including values rendered in
    //:   scientific notation.

#define DFP(X) BDLDFP_DECIMAL_DF(X)

    BDEC::Decimal32 INF_P = BDEC::Decimal32(
                                  bsl::numeric_limits<double>::infinity());
    BDEC::Decimal32 INF_N = BDEC::Decimal32(
                                 -bsl::numeric_limits<double>::infinity());
    BDEC::Decimal32 NAN_Q = BDEC::Decimal32(
                                 bsl::numeric_limits<double>::quiet_NaN());
    static const struct {
        int              d_line;
        BDEC::Decimal32  d_decimalValue;
        int              d_width;
        char             d_justification;
        bool             d_capital;
        const char      *d_expected;
    } DATA[] = {
        // L   NUMBER    WIDTH   JUST  CAPITAL      EXPECTED
        // --- ------    -----   ----  -------      --------
        {  L_, DFP(4.25),  0,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  1,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  2,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  3,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  4,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  5,     'l', false,        "4.25 " },
        {  L_, DFP(4.25),  6,     'l', false,       "4.25  " },
        {  L_, DFP(4.25),  7,     'l', false,      "4.25   " },
        {  L_, DFP(4.25),  8,     'l', false,     "4.25    " },
        {  L_, DFP(4.25),  9,     'l', false,    "4.25     " },

        {  L_, DFP(4.25),  0,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  1,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  2,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  3,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  4,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  5,     'i', false,        " 4.25" },
        {  L_, DFP(4.25),  6,     'i', false,       "  4.25" },
        {  L_, DFP(4.25),  7,     'i', false,      "   4.25" },
        {  L_, DFP(4.25),  8,     'i', false,     "    4.25" },
        {  L_, DFP(4.25),  9,     'i', false,    "     4.25" },

        {  L_, DFP(4.25),  0,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  1,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  2,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  3,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  4,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  5,     'r', false,        " 4.25" },
        {  L_, DFP(4.25),  6,     'r', false,       "  4.25" },
        {  L_, DFP(4.25),  7,     'r', false,      "   4.25" },
        {  L_, DFP(4.25),  8,     'r', false,     "    4.25" },
        {  L_, DFP(4.25),  9,     'r', false,    "     4.25" },

        {  L_, DFP(-4.25), 0,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 1,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 2,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 3,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 4,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 5,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 6,     'l', false,       "-4.25 " },
        {  L_, DFP(-4.25), 7,     'l', false,      "-4.25  " },
        {  L_, DFP(-4.25), 8,     'l', false,     "-4.25   " },
        {  L_, DFP(-4.25), 9,     'l', false,    "-4.25    " },

        {  L_, DFP(-4.25), 0,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 1,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 2,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 3,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 4,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 5,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 6,     'i', false,       "- 4.25" },
        {  L_, DFP(-4.25), 7,     'i', false,      "-  4.25" },
        {  L_, DFP(-4.25), 8,     'i', false,     "-   4.25" },
        {  L_, DFP(-4.25), 9,     'i', false,    "-    4.25" },

        {  L_, DFP(-4.25), 0,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 1,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 2,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 3,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 4,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 5,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 6,     'r', false,       " -4.25" },
        {  L_, DFP(-4.25), 7,     'r', false,      "  -4.25" },
        {  L_, DFP(-4.25), 8,     'r', false,     "   -4.25" },
        {  L_, DFP(-4.25), 9,     'r', false,    "    -4.25" },

        {  L_, INF_P,      0,     'l', false,     "infinity" },
        {  L_, INF_P,      1,     'l', false,     "infinity" },
        {  L_, INF_P,      2,     'l', false,     "infinity" },
        {  L_, INF_P,      3,     'l', false,     "infinity" },
        {  L_, INF_P,      4,     'l', false,     "infinity" },
        {  L_, INF_P,      5,     'l', false,     "infinity" },
        {  L_, INF_P,      6,     'l', false,     "infinity" },
        {  L_, INF_P,      7,     'l', false,     "infinity" },
        {  L_, INF_P,      8,     'l', false,     "infinity" },
        {  L_, INF_P,      9,     'l', false,    "infinity " },
        {  L_, INF_P,     10,     'l', false,   "infinity  " },
        {  L_, INF_P,      0,     'l', true,      "INFINITY" },

        {  L_, INF_N,      0,     'l', false,    "-infinity" },
        {  L_, INF_N,      1,     'l', false,    "-infinity" },
        {  L_, INF_N,      2,     'l', false,    "-infinity" },
        {  L_, INF_N,      3,     'l', false,    "-infinity" },
        {  L_, INF_N,      4,     'l', false,    "-infinity" },
        {  L_, INF_N,      5,     'l', false,    "-infinity" },
        {  L_, INF_N,      6,     'l', false,    "-infinity" },
        {  L_, INF_N,      7,     'l', false,    "-infinity" },
        {  L_, INF_N,      8,     'l', false,    "-infinity" },
        {  L_, INF_N,      9,     'l', false,    "-infinity" },
        {  L_, INF_N,     10,     'l', false,   "-infinity " },
        {  L_, INF_N,      0,     'l', true,     "-INFINITY" },


        {  L_, INF_N,      0,     'i', false,    "-infinity" },
        {  L_, INF_N,      1,     'i', false,    "-infinity" },
        {  L_, INF_N,      2,     'i', false,    "-infinity" },
        {  L_, INF_N,      3,     'i', false,    "-infinity" },
        {  L_, INF_N,      4,     'i', false,    "-infinity" },
        {  L_, INF_N,      5,     'i', false,    "-infinity" },
        {  L_, INF_N,      6,     'i', false,    "-infinity" },
        {  L_, INF_N,      7,     'i', false,    "-infinity" },
        {  L_, INF_N,      8,     'i', false,    "-infinity" },
        {  L_, INF_N,      9,     'i', false,    "-infinity" },
        {  L_, INF_N,     10,     'i', false,   "- infinity" },

        {  L_, INF_N,      0,     'r', false,    "-infinity" },
        {  L_, INF_N,      1,     'r', false,    "-infinity" },
        {  L_, INF_N,      2,     'r', false,    "-infinity" },
        {  L_, INF_N,      3,     'r', false,    "-infinity" },
        {  L_, INF_N,      4,     'r', false,    "-infinity" },
        {  L_, INF_N,      5,     'r', false,    "-infinity" },
        {  L_, INF_N,      6,     'r', false,    "-infinity" },
        {  L_, INF_N,      7,     'r', false,    "-infinity" },
        {  L_, INF_N,      8,     'r', false,    "-infinity" },
        {  L_, INF_N,      9,     'r', false,    "-infinity" },
        {  L_, INF_N,     10,     'r', false,   " -infinity" },

        {  L_, NAN_Q,      0,     'l', false,         "nan" },
        {  L_, NAN_Q,      1,     'l', false,         "nan" },
        {  L_, NAN_Q,      2,     'l', false,         "nan" },
        {  L_, NAN_Q,      3,     'l', false,         "nan" },
        {  L_, NAN_Q,      4,     'l', false,         "nan " },

        {  L_, NAN_Q,      0,     'i', false,         "nan" },
        {  L_, NAN_Q,      1,     'i', false,         "nan" },
        {  L_, NAN_Q,      2,     'i', false,         "nan" },
        {  L_, NAN_Q,      3,     'i', false,         "nan" },
        {  L_, NAN_Q,      4,     'i', false,         " nan" },

        {  L_, NAN_Q,      0,     'r', false,         "nan" },
        {  L_, NAN_Q,      1,     'r', false,         "nan" },
        {  L_, NAN_Q,      2,     'r', false,         "nan" },
        {  L_, NAN_Q,      3,     'r', false,         "nan" },
        {  L_, NAN_Q,      4,     'r', false,         " nan" },

        {  L_, NAN_Q,      0,     'i', true,          "NAN" },
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int             LINE       = DATA[ti].d_line;
        const BDEC::Decimal32 DECIMAL32  = DATA[ti].d_decimalValue;
        const int             WIDTH      = DATA[ti].d_width;
        const bool            LEFT       = (DATA[ti].d_justification == 'l');
        const bool            INTERNAL   = (DATA[ti].d_justification == 'i');
        const bool            RIGHT      = (DATA[ti].d_justification == 'r');
        const bool            CAPITAL    = DATA[ti].d_capital;
        const char           *EXPECTED   = DATA[ti].d_expected;

        if (veryVerbose) {
            P_(LINE); P_(EXPECTED); P(DECIMAL32);
        }

        // Test with Decimal32.
        {
            const BDEC::Decimal32 VALUE(DECIMAL32);

            bsl::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT)     { outdec << bsl::left;        }
            if (INTERNAL) { outdec << bsl::internal;    }
            if (RIGHT)    { outdec << bsl::right;       }
            if (CAPITAL)  { outdec << bsl::uppercase;   }
            else          { outdec << bsl::nouppercase; }
            outdec << VALUE;

            bsl::string ACTUAL = outdec.str();

            ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            ASSERTV(outdec.good());
        }

        // Test with Decimal64.
        {
            const BDEC::Decimal64 VALUE(DECIMAL32);

            bsl::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT)     { outdec << bsl::left;        }
            if (INTERNAL) { outdec << bsl::internal;    }
            if (RIGHT)    { outdec << bsl::right;       }
            if (CAPITAL)  { outdec << bsl::uppercase;   }
            else          { outdec << bsl::nouppercase; }
            outdec << VALUE;

            bsl::string ACTUAL = outdec.str();

            ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            ASSERTV(outdec.good());
        }

        // Test with print
        {
            const BDEC::Decimal64 VALUE(DECIMAL32);

            bsl::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT)     { outdec << bsl::left;        }
            if (INTERNAL) { outdec << bsl::internal;    }
            if (RIGHT)    { outdec << bsl::right;       }
            if (CAPITAL)  { outdec << bsl::uppercase;   }
            else          { outdec << bsl::nouppercase; }
            VALUE.print(outdec, 0, -1);

            bsl::string ACTUAL = outdec.str();

            ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
        }

        // Test with Decimal128.
        {
            const BDEC::Decimal128 VALUE(DECIMAL32);

            bsl::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT)     { outdec << bsl::left;        }
            if (INTERNAL) { outdec << bsl::internal;    }
            if (RIGHT)    { outdec << bsl::right;       }
            if (CAPITAL)  { outdec << bsl::uppercase;   }
            else          { outdec << bsl::nouppercase; }
            outdec << VALUE;

            bsl::string ACTUAL = outdec.str();

            ASSERTV(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            ASSERTV(outdec.good());
        }
    }

    // Bug in Studio Studio's C++ standard library: 'ostreambuf_iterator'
    // doesn't set the 'failed' attribute when the iterator reaches the end of
    // EOF of the 'streambuf'.  Therefore, 'operator<<' for the decimal types
    // does not set the 'fail' and 'bad' bits when streaming to an 'ostream'
    // with a 'streambuf' that is not large enough.  This is consistent with
    // the behavior for 'int' and 'double'.  Note that the bug does not exist
    // when using stlport.

#if (!defined(BSLS_PLATFORM_OS_SUNOS) &&                                      \
     !defined(BSLS_PLATFORM_OS_SOLARIS)) ||                                   \
    defined(BDE_BUILD_TARGET_STLPORT)

    {
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 4);
            bsl::ostream out(&obuf);
            BDEC::Decimal64 value = DFP(-1.0);
            out << value;
            ASSERTV(out.fail(), !out.fail());
            ASSERTV(out.bad(), !out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            bsl::ostream out(&obuf);
            BDEC::Decimal32 value = DFP(-1.0);
            out << value;
            ASSERTV(out.fail(), out.fail());
            ASSERTV(out.bad(), out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            bsl::ostream out(&obuf);
            BDEC::Decimal64 value = DFP(-1.0);
            out << value;
            ASSERTV(out.fail(), out.fail());
            ASSERTV(out.bad(), out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            bsl::ostream out(&obuf);
            BDEC::Decimal128 value = DFP(-1.0);
            out << value;
            ASSERTV(out.fail(), out.fail());
            ASSERTV(out.bad(), out.bad());
        }
    }
#endif

#undef DFP
}

void TestDriver::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal128'
    //   Ensure that 'Decimal128' functions correctly
    //
    // Concerns:
    //: 1 'Decimal128' operations are properly forwarded to implementation
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will
    //:   be tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal128'
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTesting class Decimal128"
                            << "\n========================" << bsl::endl;

    if (veryVerbose) bsl::cout << "Constructors" << bsl::endl;

    if (veryVeryVerbose) bsl::cout << "Copy/convert" << bsl::endl;
    {
        const BDEC::Decimal32  c32  = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64  = BDEC::Decimal64(64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        ASSERT(BDLDFP_DECIMAL_DL( 32.0) == BDEC::Decimal128(c32));
        ASSERT(BDLDFP_DECIMAL_DL( 64.0) == BDEC::Decimal128(c64));
        ASSERT(BDLDFP_DECIMAL_DL(128.0) == BDEC::Decimal128(c128));
    }

    if (veryVeryVerbose) bsl::cout << "Integral" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(  0.0) == BDEC::Decimal128());  // default
    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42)); // int
    ASSERT(BDLDFP_DECIMAL_DL( 42.0) == BDEC::Decimal128(42u)); // unsigned
    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42l)); // long
    ASSERT(BDLDFP_DECIMAL_DL( 42.0) == BDEC::Decimal128(42ul)); // ulong
    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42ll)); //longlong
    ASSERT(BDLDFP_DECIMAL_DL( 42.0) == BDEC::Decimal128(42ull)); // ulongl

    BDEC::Decimal128 cDefault;
    BDEC::Decimal128 cExpectedDefault = BDLDFP_DECIMAL_DL(0e-6176);
    ASSERTV(cDefault, cExpectedDefault,
            0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal128 cZero(0);
    BDEC::Decimal128 cExpectedZero = BDLDFP_DECIMAL_DL(0e0);
    ASSERTV(cZero, cExpectedZero,
            0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    ASSERTV(cDefault, cZero, 0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) bsl::cout << "Binary FP" << bsl::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    ASSERT(BDLDFP_DECIMAL_DL(4.25) == BDEC::Decimal128(4.25f)); // float
    ASSERT(BDLDFP_DECIMAL_DL(4.25) == BDEC::Decimal128(4.25)); // double

    if (veryVeryVerbose) bsl::cout << "Decimal FP" << bsl::endl;

    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(-42.0), BDLDFP_DECIMAL_DF(-42.0),
                 BDLDFP_DECIMAL_DL(-42.0) == BDLDFP_DECIMAL_DF(-42.0));
                                                              // Decimal32
    ASSERT(BDLDFP_DECIMAL_DL(42.0) == BDLDFP_DECIMAL_DF(42.0));
    ASSERT(BDLDFP_DECIMAL_DL(4.2) == BDLDFP_DECIMAL_DF(4.2));
    ASSERT(BDLDFP_DECIMAL_DL(4.2e9) == BDLDFP_DECIMAL_DF(4.2e9));

    ASSERT(BDLDFP_DECIMAL_DL(-42.0) == BDLDFP_DECIMAL_DD(-42.0));// Dec64
    ASSERT(BDLDFP_DECIMAL_DL(42.0) == BDLDFP_DECIMAL_DD(42.0));
    ASSERT(BDLDFP_DECIMAL_DL(4.2) == BDLDFP_DECIMAL_DD(4.2));
    ASSERT(BDLDFP_DECIMAL_DL(4.2e9) == BDLDFP_DECIMAL_DD(4.2e9));

    if (veryVerbose) bsl::cout << "Propriatery accessors" << bsl::endl;
    {

        BDEC::Decimal128 d128(42);
        ASSERT((void*)d128.data() == (void*)&d128);

    // XLC versions prior to 12.0 incorrectly pass decimal128 values in
    // some contexts (0x0c00 -> 12.00)
#if defined(BSLS_PLATFORM_CMP_IBM) && (BSLS_PLATFORM_CMP_VERSION >= 0x0c00)
        ASSERTV(BDEC::Decimal128(d128.value()) == BDEC::Decimal128(42));
#endif
    }

    if (veryVerbose) bsl::cout << "Operator==" << bsl::endl;

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator!=" << bsl::endl;

    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(4.0), BDLDFP_DECIMAL_DL(5.0),
                 BDLDFP_DECIMAL_DL(4.0) != BDLDFP_DECIMAL_DL(5.0));
    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(7.0), BDLDFP_DECIMAL_DL(5.0),
                 BDLDFP_DECIMAL_DL(7.0) != BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) !=
              BDLDFP_DECIMAL_DL(-9.345e27)));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator<" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) <
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) <
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DD(4.0)));
    ASSERT(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) <
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) <
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) < BDLDFP_DECIMAL_DL(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) <
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    if (veryVerbose) bsl::cout << "Operator>" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DL(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) >
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DD(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) >
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DL(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) >
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DF(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DL(-9.345e27) >
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DL(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) >
              BDLDFP_DECIMAL_DL(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    if (veryVerbose) bsl::cout << "Operator<=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DD(4.0)));
    ASSERT(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    ASSERT(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    if (veryVerbose) bsl::cout << "Operator>=" << bsl::endl;

    LOOP2_ASSERT(BDLDFP_DECIMAL_DL(5.0),   BDLDFP_DECIMAL_DL(4.0),
                 BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    ASSERT(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    ASSERT(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.
    //
    // Expecting to see all digits is wrong because that is not how the
    // stream output should behave: it should print with the default
    // precision as it would for binary floating point *or* with the
    // implied precision of the cohort if that is larger.  AFAIU

    if (veryVerbose) bsl::cout << "Test stream out" << bsl::endl;
    {
        bsl::ostringstream  out(pa);

        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(
                                -1.234567890123456789012345678901234e-24));
        out << d1;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP2_ASSERT(
                decLower(s),
                "-1.234567890123456789012345678901234e-24",
                decLower(s) == "-1.234567890123456789012345678901234e-24");
    }

    if (veryVerbose) bsl::cout << "Test stream in" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-1.234567890123456789012345678901234e-24", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 ==
              BDLDFP_DECIMAL_DL(-1.234567890123456789012345678901234e-24));
    }

    // bdldfp does not know how to parse128("NaN") etc.
    if (veryVerbose) bsl::cout << "Test stream in NaN" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaN", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("nan", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }

    if (veryVerbose) bsl::cout << "Test stream in Inf" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal128>::max());
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Inf" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal128>::max());
    }
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-inf", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in NaNa (bad)" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaNa", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(in.fail() == true);
    }

    if (veryVerbose) bsl::cout << "Test stream in Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Infinity", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Infinity", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in Infin (bad)" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Infin", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(in.fail() == true);
    }

    if (veryVerbose) bsl::cout << "Test wide stream out" << bsl::endl;
    {
        bsl::wostringstream  out(pa);
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(
                                -1.234567890123456789012345678901234e-24));
        out << d1;
        bsl::wstring s(pa);
        getStringFromStream(out, &s);
        ASSERT(decLower(s) == L"-1.234567890123456789012345678901234e-24");
    }

    if (veryVerbose) bsl::cout << "Test wide stream in" << bsl::endl;
    {
        bsl::wistringstream  in(pa);
        bsl::wstring ins(L"-1.234567890123456789012345678901234e-24", pa);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        ASSERT(d1 ==
              BDLDFP_DECIMAL_DL(-1.234567890123456789012345678901234e-24));
    }

    if (veryVerbose) bsl::cout << "Operator++" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2 = d1++;
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DL(-4.0) == d1);
        BDEC::Decimal128 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DL(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DL(-4.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Operator--" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2 = d1--;
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DL(-6.0) == d1);
        BDEC::Decimal128 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DL(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DL(-6.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Unary-" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2(BDLDFP_DECIMAL_DL( 5.0));
        ASSERT(BDLDFP_DECIMAL_DL( 5.0) == -d1);
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == -d2);

        // Unary - must make +0 into -0

        bsl::ostringstream out(pa);
        out << -BDLDFP_DECIMAL_DL(0.0);
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        ASSERT(-BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128(0)); // and 0
    }

    if (veryVerbose) bsl::cout << "Unary+" << bsl::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2(BDLDFP_DECIMAL_DL( 5.0));
        ASSERT(BDLDFP_DECIMAL_DL(-5.0) == +d1);
        ASSERT(BDLDFP_DECIMAL_DL( 5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DL(0.0));
        bsl::ostringstream out(pa);
        out << +negzero;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        ASSERT(-BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128(0)); // and 0
    }

    if (veryVerbose) bsl::cout << "+=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "+=(int)" << bsl::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-4.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned int)" << bsl::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long int)" << bsl::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-3.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long int)"
                                       << bsl::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long long int)" << bsl::endl;
        d += -2ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long long int)"
                                << bsl::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(42.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal32)" << bsl::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(20.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(23.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(32.0) == d);
    }

    if (veryVerbose) bsl::cout << "-=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "-=(int)" << bsl::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-6.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned int)" << bsl::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long int)" << bsl::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long int)"
                                       << bsl::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long long int)" << bsl::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long long int)"
                                << bsl::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-42.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal32)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-20.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-23.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-32.0) == d);
    }

    if (veryVerbose) bsl::cout << "*=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "*=(int)" << bsl::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(10.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned int)" << bsl::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(20000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long int)" << bsl::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-200000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long int)"
                                       << bsl::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long long int)" << bsl::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long long int)"
                                << bsl::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(3000000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal32)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(30.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-90.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-2.16e-132) == d);
    }

    if (veryVerbose) bsl::cout << "/=" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "/=(int)" << bsl::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.5) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned int)" << bsl::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.00125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long int)" << bsl::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.000125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long int)"
                                       << bsl::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.000025) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long long int)" << bsl::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.000005) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long long int)"
                                << bsl::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.000001) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal32)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.1) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.02) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal128)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DL(-2.5e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(8.e131) == d);
    }

    if (veryVerbose) bsl::cout << "operator+" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec + int" << bsl::endl;
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec128" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DL(-3.0),
                    d + BDLDFP_DECIMAL_DL(-3.0) ==
                    BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec32" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
                    d + BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DL(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) + d,
                    BDLDFP_DECIMAL_DF(-3.0) + d ==
                    BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec64" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DD(-3.0),
                    d + BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DL(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-3.0) + d,
                    BDLDFP_DECIMAL_DD(-3.0) + d ==
                    BDLDFP_DECIMAL_DL(-8.0));
    }

    if (veryVerbose) bsl::cout << "operator-" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec - int" << bsl::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DL( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DL( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DL( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DL( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DL( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DL( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec128" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DL(-3.0),
                    d - BDLDFP_DECIMAL_DL(-3.0) ==
                    BDLDFP_DECIMAL_DL(-2.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec32" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
                    d - BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DL(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) - d,
                    BDLDFP_DECIMAL_DF(-3.0) - d ==
                    BDLDFP_DECIMAL_DL( 2.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec64" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DD(-3.0),
                    d - BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DL(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-3.0) - d,
                    BDLDFP_DECIMAL_DD(-3.0) - d ==
                    BDLDFP_DECIMAL_DL( 2.0));
    }

    if (veryVerbose) bsl::cout << "operator*" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec * int" << bsl::endl;
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long int" << bsl::endl;
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long long int"
                                << bsl::endl;
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec64" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DD(-3.0),
                    d * BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec32" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
                    d * BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) * d,
                    BDLDFP_DECIMAL_DF(-3.0) * d ==
                    BDLDFP_DECIMAL_DD(15.0));
    }

    if (veryVerbose) bsl::cout << "operator/" << bsl::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec / int" << bsl::endl;
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DL( -0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long int" << bsl::endl;
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DL( -0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long long int"
                                << bsl::endl;
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DL( -0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / dec128" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DL(-50.0),
                    d / BDLDFP_DECIMAL_DL(-50.0) ==
                    BDLDFP_DECIMAL_DL(0.1));

        if (veryVeryVerbose) bsl::cout << "dec / dec32" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
                    d / BDLDFP_DECIMAL_DF(-50.0) ==
                    BDLDFP_DECIMAL_DL(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-50.0) / d,
                    BDLDFP_DECIMAL_DF(-50.0) / d ==
                    BDLDFP_DECIMAL_DL(10.0));

        if (veryVeryVerbose) bsl::cout << "dec / dec64" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DD(-50.0),
                    d / BDLDFP_DECIMAL_DD(-50.0) ==
                    BDLDFP_DECIMAL_DL(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-50.0) / d,
                    BDLDFP_DECIMAL_DD(-50.0) / d ==
                    BDLDFP_DECIMAL_DL(10.0));
    }

    if (veryVerbose) bsl::cout << "Create test objects" << bsl::endl;

    BDEC::Decimal32        d32  = BDEC::Decimal32();
    BDEC::Decimal128       d128 = BDEC::Decimal128();
    const BDEC::Decimal128 c128 = BDEC::Decimal128();

    if (veryVerbose) bsl::cout << "Check return types" << bsl::endl;

    checkType<BDEC::Decimal128&>(++d128);
    checkType<BDEC::Decimal128>(d128++);
    checkType<BDEC::Decimal128&>(--d128);
    checkType<BDEC::Decimal128>(d128--);
    checkType<BDEC::Decimal128&>(d128 += static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 +=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 -=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 *=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 /=
                                 static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal128>(1));

    checkType<BDEC::Decimal128>(+d128);
    checkType<BDEC::Decimal128>(-d128);

    checkType<BDEC::Decimal128>(d128 + d128);
    checkType<BDEC::Decimal128>(d128 - d128);
    checkType<BDEC::Decimal128>(d128 * d128);
    checkType<BDEC::Decimal128>(d128 / d128);
    checkType<bool>(d128 == d128);
    checkType<bool>(d32 == d128);
    checkType<bool>(d128 == d32);
    checkType<bool>(d128 != d128);
    checkType<bool>(d32 != d128);
    checkType<bool>(d128 != d32);
    checkType<bool>(d128 < d128);
    checkType<bool>(d32 < d128);
    checkType<bool>(d128 < d32);
    checkType<bool>(d128 <= d128);
    checkType<bool>(d32 <= d128);
    checkType<bool>(d128 <= d32);
    checkType<bool>(d128 > d128);
    checkType<bool>(d32 > d128);
    checkType<bool>(d128 > d32);
    checkType<bool>(d128 >= d128);
    checkType<bool>(d32 >= d128);
    checkType<bool>(d128 >= d32);

    {
        bsl::istringstream  in(pa);
        bsl::wistringstream win(pa);
        bsl::ostringstream  out(pa);
        bsl::wostringstream wout(pa);

        checkType<bsl::istream&>(in >> d128);
        checkType<bsl::wistream&>(win >> d128);
        checkType<bsl::ostream&>(out << c128);
        checkType<bsl::wostream&>(wout << c128);
    }


    typedef bsl::numeric_limits<BDEC::Decimal128> d128_limits;
    checkType<bool>(d128_limits::is_specialized);
    checkType<BDEC::Decimal128>(d128_limits::min());
    checkType<BDEC::Decimal128>(d128_limits::max());
    checkType<int>(d128_limits::digits);
    checkType<int>(d128_limits::digits10);
    checkType<int>(d128_limits::max_digits10);
    checkType<bool>(d128_limits::is_signed);
    checkType<bool>(d128_limits::is_integer);
    checkType<bool>(d128_limits::is_exact);
    checkType<int>(d128_limits::radix);
    checkType<BDEC::Decimal128>(d128_limits::epsilon());
    checkType<BDEC::Decimal128>(d128_limits::round_error());
    checkType<int>(d128_limits::min_exponent);
    checkType<int>(d128_limits::min_exponent10);
    checkType<int>(d128_limits::max_exponent);
    checkType<int>(d128_limits::max_exponent10);
    checkType<bool>(d128_limits::has_infinity);
    checkType<bool>(d128_limits::has_quiet_NaN);
    checkType<bool>(d128_limits::has_signaling_NaN);
    checkType<bsl::float_denorm_style>(d128_limits::has_denorm);
    checkType<bool>(d128_limits::has_denorm_loss);
    checkType<BDEC::Decimal128>(d128_limits::infinity());
    checkType<BDEC::Decimal128>(d128_limits::quiet_NaN());
    checkType<BDEC::Decimal128>(d128_limits::signaling_NaN());
    checkType<BDEC::Decimal128>(d128_limits::denorm_min());
    checkType<bool>(d128_limits::is_iec559);
    checkType<bool>(d128_limits::is_bounded);
    checkType<bool>(d128_limits::is_modulo);
    checkType<bool>(d128_limits::traps);
    checkType<bool>(d128_limits::tinyness_before);
    checkType<bsl::float_round_style>(d128_limits::round_style);
}

void TestDriver::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal64'
    //   Ensure that 'Decimal64' functions correctly
    //
    // Concerns:
    //: 1 'Decimal64' operations are properly forwarded to implementation
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will be
    //:   tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal64'
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTesting class Decimal64"
                           << "\n=======================" << bsl::endl;

    if (veryVerbose) bsl::cout << "Constructors" << bsl::endl;

    if (veryVeryVerbose) bsl::cout << "Copy/convert" << bsl::endl;
    {
        const BDEC::Decimal32  c32  = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64  = BDEC::Decimal64(64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        LOOP2_ASSERT(BDLDFP_DECIMAL_DD( 32.0),   BDEC::Decimal64(c32),
                     BDLDFP_DECIMAL_DD( 32.0) == BDEC::Decimal64(c32));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DD( 64.0),   BDEC::Decimal64(c64),
                     BDLDFP_DECIMAL_DD( 64.0) == BDEC::Decimal64(c64));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DD(128.0),   BDEC::Decimal64(c128),
                     BDLDFP_DECIMAL_DD(128.0) == BDEC::Decimal64(c128));
    }

    if (veryVeryVerbose) bsl::cout << "Integral" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(  0.0) == BDEC::Decimal64());  // default
    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42)); // int
    ASSERT(BDLDFP_DECIMAL_DD( 42.0) == BDEC::Decimal64(42u)); // unsigned
    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42l)); // long
    ASSERT(BDLDFP_DECIMAL_DD( 42.0) == BDEC::Decimal64(42ul)); // ulong
    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42ll)); // longlong
    ASSERT(BDLDFP_DECIMAL_DD( 42.0) == BDEC::Decimal64(42ull)); // ulongl

    BDEC::Decimal64 cDefault;
    BDEC::Decimal64 cExpectedDefault = BDLDFP_DECIMAL_DD(0e-398);
    ASSERTV(cDefault, cExpectedDefault,
            0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal64 cZero(0);
    BDEC::Decimal64 cExpectedZero = BDLDFP_DECIMAL_DD(0e0);
    ASSERTV(cZero, cExpectedZero,
            0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    ASSERTV(cDefault, cZero, 0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) bsl::cout << "Binary FP" << bsl::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    ASSERT(BDLDFP_DECIMAL_DD(4.25) == BDEC::Decimal64(4.25f)); // float
    ASSERT(BDLDFP_DECIMAL_DD(4.25) == BDEC::Decimal64(4.25)); // double

    if (veryVeryVerbose) bsl::cout << "Decimal FP" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(-42.0) ==
           BDLDFP_DECIMAL_DF(-42.0));// Decimal342
    ASSERT(BDLDFP_DECIMAL_DD(42.0) == BDLDFP_DECIMAL_DF(42.0));
    ASSERT(BDLDFP_DECIMAL_DD(4.2) == BDLDFP_DECIMAL_DF(4.2));
    ASSERT(BDLDFP_DECIMAL_DD(4.2e9) == BDLDFP_DECIMAL_DF(4.2e9));

    ASSERT(BDLDFP_DECIMAL_DD(-42.0) == BDLDFP_DECIMAL_DL(-42.0));// Dec128
    ASSERT(BDLDFP_DECIMAL_DD(42.0) == BDLDFP_DECIMAL_DL(42.0));
    ASSERT(BDLDFP_DECIMAL_DD(4.2) == BDLDFP_DECIMAL_DL(4.2));
    ASSERT(BDLDFP_DECIMAL_DD(4.2e9) == BDLDFP_DECIMAL_DL(4.2e9));

    if (veryVerbose) bsl::cout << "Propriatery accessors" << bsl::endl;
    {
        BDEC::Decimal64 d64(42);
        ASSERT((void*)d64.data() == (void*)&d64);

        const BDEC::Decimal64 cd64(42);
        ASSERT((const void*)d64.data() == (const void*)&d64);

        ASSERT(BDEC::Decimal64(cd64.value()) == BDEC::Decimal64(42));
    }

    if (veryVerbose) bsl::cout << "Operator==" << bsl::endl;

    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));

    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator!=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(4.0) != BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) !=
              BDLDFP_DECIMAL_DD(-9.345e27)));

    ASSERT(BDLDFP_DECIMAL_DD(4.0) != BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) !=
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) != BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) !=
              BDLDFP_DECIMAL_DD(-9.345e27)));

    if (veryVerbose) bsl::cout << "Operator<" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) <
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DD(4.0)));

    ASSERT(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) <
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) < BDLDFP_DECIMAL_DD(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) <
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DD(4.0)));

    if (veryVerbose) bsl::cout << "Operator>" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DD(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) >
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DD(5.0)));

    ASSERT(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DF(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DD(-9.345e27) >
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DD(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) >
              BDLDFP_DECIMAL_DD(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DD(5.0)));

    if (veryVerbose) bsl::cout << "Operator<=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DD(4.0)));

    ASSERT(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DF(4.0)));
    ASSERT(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DD(4.0)));

    if (veryVerbose) bsl::cout << "Operator>=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DD(5.0)));

    ASSERT(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    ASSERT(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DD(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.
    //
    // Expecting to see all digits is wrong because that is not how the
    // stream output should behave: it should print with the default
    // precision as it would for binary floating point *or* with the
    // implied precision of the cohort if that is larger.  AFAIU

    if (veryVerbose) bsl::cout << "Test stream out" << bsl::endl;
    {
        bsl::ostringstream  out(pa);

        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
        out << d1;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, decLower(s) == "-1.234567890123456e-24");
    }

    if (veryVerbose) bsl::cout << "Test stream in" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-1.234567890123456e-24", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
    }

    if (veryVerbose) bsl::cout << "Test stream in NaN" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaN", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }

    if (veryVerbose) bsl::cout << "Test stream in Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Inf", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal64>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Inf", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal64>::max());
    }

    if (veryVerbose) bsl::cout << "Test wide stream out" << bsl::endl;
    {
        bsl::wostringstream  out(pa);
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
        out << d1;
        bsl::wstring s(pa);
        getStringFromStream(out, &s);
        ASSERT(decLower(s) == L"-1.234567890123456e-24");
    }

    if (veryVerbose) bsl::cout << "Test wide stream in" << bsl::endl;
    {
        bsl::wistringstream  in(pa);
        bsl::wstring ins(L"-1.234567890123456e-24", pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
    }

    if (veryVerbose) bsl::cout << "Operator++" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2 = d1++;
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DD(-4.0) == d1);
        BDEC::Decimal64 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DD(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DD(-4.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Operator--" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2 = d1--;
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DD(-6.0) == d1);
        BDEC::Decimal64 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DD(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DD(-6.0) == d3);
    }

    if (veryVerbose) bsl::cout << "Unary-" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2(BDLDFP_DECIMAL_DD( 5.0));
        ASSERT(BDLDFP_DECIMAL_DD( 5.0) == -d1);
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == -d2);

        // Unary - must make +0 into -0

        bsl::ostringstream out(pa);
        out << -BDLDFP_DECIMAL_DD(0.0);
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        ASSERT(-BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64(0)) // and 0

        BDEC::Decimal64  dd =  BDLDFP_DECIMAL_DD(0.0);
        BDEC::Decimal64 ndd = -BDLDFP_DECIMAL_DD(0.0);
        LOOP2_ASSERT( dd, ndd,  bsl::memcmp(&ndd, &dd, sizeof(dd)));
        dd= -dd;
        LOOP2_ASSERT(dd, ndd, !bsl::memcmp(&ndd, &dd, sizeof(dd)));
    }

    if (veryVerbose) bsl::cout << "Unary+" << bsl::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2(BDLDFP_DECIMAL_DD( 5.0));
        ASSERT(BDLDFP_DECIMAL_DD(-5.0) == +d1);
        ASSERT(BDLDFP_DECIMAL_DD( 5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DD(0.0));
        bsl::ostringstream out(pa);
        out << +negzero;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        ASSERT(-BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64(0)) // and 0
    }

    if (veryVerbose) bsl::cout << "+=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "+=(int)" << bsl::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-4.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned int)" << bsl::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long int)" << bsl::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-3.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long int)"
                                       << bsl::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(long long int)" << bsl::endl;
        d += -2ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(unsigned long long int)"
                                       << bsl::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(42.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal32)" << bsl::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(20.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal64)" << bsl::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(23.0) == d);

        if (veryVeryVerbose) bsl::cout << "+=(Decimal128)" << bsl::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(32.0) == d);
    }

    if (veryVerbose) bsl::cout << "-=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "-=(int)" << bsl::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-6.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned int)" << bsl::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long int)" << bsl::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long int)"
                                       << bsl::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-8.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(long long int)" << bsl::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(unsigned long long int)"
                                       << bsl::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-42.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal32)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-20.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal64)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-23.0) == d);

        if (veryVeryVerbose) bsl::cout << "-=(Decimal128)" << bsl::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-32.0) == d);
    }

    if (veryVerbose) bsl::cout << "*=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "*=(int)" << bsl::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(10.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned int)" << bsl::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(20000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long int)" << bsl::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-200000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long int)"
                                       << bsl::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(long long int)" << bsl::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(600000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(unsigned long long int)"
                                       << bsl::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(3000000.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal32)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(30.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal64)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-90.0) == d);

        if (veryVeryVerbose) bsl::cout << "*=(Decimal128)" << bsl::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.16e-132) == d);
    }

    if (veryVerbose) bsl::cout << "/=" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "/=(int)" << bsl::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.5) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned int)" << bsl::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.00125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long int)" << bsl::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.000125) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long int)"
                                       << bsl::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.000025) == d);

        if (veryVeryVerbose) bsl::cout << "/=(long long int)" << bsl::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.000005) == d);

        if (veryVeryVerbose) bsl::cout << "/=(unsigned long long int)"
                                       << bsl::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.000001) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal32)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.1) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal64)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.02) == d);

        if (veryVeryVerbose) bsl::cout << "/=(Decimal128)" << bsl::endl;
        d /= BDLDFP_DECIMAL_DL(-2.5e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(8.e131) == d);
    }

    if (veryVerbose) bsl::cout << "operator+" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec + int" << bsl::endl;
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long int" << bsl::endl;
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) bsl::cout << "dec + unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec64" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DD(-3.0),
                    d + BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(-8.0));

        if (veryVeryVerbose) bsl::cout << "dec + dec32" << bsl::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
                    d + BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) + d,
                    BDLDFP_DECIMAL_DF(-3.0) + d ==
                    BDLDFP_DECIMAL_DD(-8.0));
    }

    if (veryVerbose) bsl::cout << "operator-" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec - int" << bsl::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DD( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DD( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long int" << bsl::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DD( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DD( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DD( 6.0));

        if (veryVeryVerbose) bsl::cout << "dec - unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DD( 15.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec64" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DD(-3.0),
                    d - BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(-2.0));

        if (veryVeryVerbose) bsl::cout << "dec - dec32" << bsl::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
                    d - BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) - d,
                    BDLDFP_DECIMAL_DF(-3.0) - d ==
                    BDLDFP_DECIMAL_DD( 2.0));
    }

    if (veryVerbose) bsl::cout << "operator*" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec * int" << bsl::endl;
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long int" << bsl::endl;
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) bsl::cout << "dec * unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec64" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DD(-3.0),
                    d * BDLDFP_DECIMAL_DD(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) bsl::cout << "dec * dec32" << bsl::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
                    d * BDLDFP_DECIMAL_DF(-3.0) ==
                    BDLDFP_DECIMAL_DD(15.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) * d,
                    BDLDFP_DECIMAL_DF(-3.0) * d ==
                    BDLDFP_DECIMAL_DD(15.0));
    }

    if (veryVerbose) bsl::cout << "operator/" << bsl::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) bsl::cout << "dec / int" << bsl::endl;
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DD( -0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long int" << bsl::endl;
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DD( -0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) bsl::cout << "dec / unsigned long long int"
                                       << bsl::endl;
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DD( -0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) bsl::cout << "dec / dec64" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DD(-50.0),
                    d / BDLDFP_DECIMAL_DD(-50.0) ==
                    BDLDFP_DECIMAL_DD(0.1));

        if (veryVeryVerbose) bsl::cout << "dec / dec32" << bsl::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
                    d / BDLDFP_DECIMAL_DF(-50.0) ==
                    BDLDFP_DECIMAL_DD(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-50.0) / d,
                    BDLDFP_DECIMAL_DF(-50.0) / d ==
                    BDLDFP_DECIMAL_DD(10.0));
    }

    if (veryVerbose) bsl::cout << "Create test objects" << bsl::endl;

    BDEC::Decimal32        d32  = BDEC::Decimal32();
    BDEC::Decimal64        d64  = BDEC::Decimal64();
    const BDEC::Decimal64  c64  = BDEC::Decimal64();

    if (veryVerbose) bsl::cout << "Check return types" << bsl::endl;

    checkType<BDEC::Decimal64&>(++d64);
    checkType<BDEC::Decimal64>(d64++);
    checkType<BDEC::Decimal64&>(--d64);
    checkType<BDEC::Decimal64>(d64--);
    checkType<BDEC::Decimal64&>(d64 += static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal128>(1));

    checkType<BDEC::Decimal64>(+d64);
    checkType<BDEC::Decimal64>(-d64);

    checkType<BDEC::Decimal64>(d64 + d64);
    checkType<BDEC::Decimal64>(d64 - d64);
    checkType<BDEC::Decimal64>(d64 * d64);
    checkType<BDEC::Decimal64>(d64 / d64);
    checkType<bool>(d64 == d64);
    checkType<bool>(d32 == d64);
    checkType<bool>(d64 == d32);
    checkType<bool>(d64 != d64);
    checkType<bool>(d32 != d64);
    checkType<bool>(d64 != d32);
    checkType<bool>(d64 < d64);
    checkType<bool>(d32 < d64);
    checkType<bool>(d64 < d32);
    checkType<bool>(d64 <= d64);
    checkType<bool>(d32 <= d64);
    checkType<bool>(d64 <= d32);
    checkType<bool>(d64 > d64);
    checkType<bool>(d32 > d64);
    checkType<bool>(d64 > d32);
    checkType<bool>(d64 >= d64);
    checkType<bool>(d32 >= d64);
    checkType<bool>(d64 >= d32);

    {
        bsl::istringstream  in(pa);
        bsl::wistringstream win(pa);
        bsl::ostringstream  out(pa);
        bsl::wostringstream wout(pa);

        checkType<bsl::istream&>(in >> d64);
        checkType<bsl::wistream&>(win >> d64);
        checkType<bsl::ostream&>(out << c64);
        checkType<bsl::wostream&>(wout << c64);
    }

    typedef bsl::numeric_limits<BDEC::Decimal64> d64_limits;
    checkType<bool>(d64_limits::is_specialized);
    checkType<BDEC::Decimal64>(d64_limits::min());
    checkType<BDEC::Decimal64>(d64_limits::max());
    checkType<int>(d64_limits::digits);
    checkType<int>(d64_limits::digits10);
    checkType<int>(d64_limits::max_digits10);
    checkType<bool>(d64_limits::is_signed);
    checkType<bool>(d64_limits::is_integer);
    checkType<bool>(d64_limits::is_exact);
    checkType<int>(d64_limits::radix);
    checkType<BDEC::Decimal64>(d64_limits::epsilon());
    checkType<BDEC::Decimal64>(d64_limits::round_error());
    checkType<int>(d64_limits::min_exponent);
    checkType<int>(d64_limits::min_exponent10);
    checkType<int>(d64_limits::max_exponent);
    checkType<int>(d64_limits::max_exponent10);
    checkType<bool>(d64_limits::has_infinity);
    checkType<bool>(d64_limits::has_quiet_NaN);
    checkType<bool>(d64_limits::has_signaling_NaN);
    checkType<bsl::float_denorm_style>(d64_limits::has_denorm);
    checkType<bool>(d64_limits::has_denorm_loss);
    checkType<BDEC::Decimal64>(d64_limits::infinity());
    checkType<BDEC::Decimal64>(d64_limits::quiet_NaN());
    checkType<BDEC::Decimal64>(d64_limits::signaling_NaN());
    checkType<BDEC::Decimal64>(d64_limits::denorm_min());
    checkType<bool>(d64_limits::is_iec559);
    checkType<bool>(d64_limits::is_bounded);
    checkType<bool>(d64_limits::is_modulo);
    checkType<bool>(d64_limits::traps);
    checkType<bool>(d64_limits::tinyness_before);
    checkType<bsl::float_round_style>(d64_limits::round_style);
}

void TestDriver::testCase1()
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal32'
    //   Ensure that 'Decimal32' functions correctly
    //
    // Concerns:
    //: 1 'Decimal32' operations are properly forwarded to implementation
    //:
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will be
    //:   tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal32'
    // ------------------------------------------------------------------------
    if (verbose) bsl::cout << bsl::endl
                           << "Testing class Decimal32" << bsl::endl
                           << "=======================" << bsl::endl;

    if (veryVerbose) bsl::cout << "Constructors" << bsl::endl;

    if (veryVeryVerbose) bsl::cout << "Copy/convert" << bsl::endl;
    {
        const BDEC::Decimal32  c32  = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64  = BDEC::Decimal64(64);

        LOOP2_ASSERT(BDLDFP_DECIMAL_DF( 32.0),   BDEC::Decimal32(c32),
                     BDLDFP_DECIMAL_DF( 32.0) == BDEC::Decimal32(c32));
        LOOP2_ASSERT(BDLDFP_DECIMAL_DF( 64.0),   BDEC::Decimal32(c64),
                     BDLDFP_DECIMAL_DF( 64.0) == BDEC::Decimal32(c64));
        // TODO: Conversions from Decimal128 to Decimal32.
        // ASSERT(BDLDFP_DECIMAL_DF(128.0) == BDEC::Decimal32(c128));
    }

    if (veryVeryVerbose) bsl::cout << "Integral" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(  0.0) == BDEC::Decimal32());  // default
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42)); // int
    ASSERT(BDLDFP_DECIMAL_DF( 42.0) == BDEC::Decimal32(42u)); // unsigned
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42l)); // long
    ASSERT(BDLDFP_DECIMAL_DF( 42.0) == BDEC::Decimal32(42ul)); // ulong
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42ll)); // longlong
    ASSERT(BDLDFP_DECIMAL_DF( 42.0) == BDEC::Decimal32(42ull)); // ulongl

    BDEC::Decimal32 cDefault;
    BDEC::Decimal32 cExpectedDefault = BDLDFP_DECIMAL_DF(0e-101);
    ASSERTV(cDefault, cExpectedDefault,
            0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal32 cZero(0);
    BDEC::Decimal32 cExpectedZero = BDLDFP_DECIMAL_DF(0e0);
    ASSERTV(cZero, cExpectedZero,
            0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    ASSERTV(cDefault, cZero, 0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) bsl::cout << "Binary FP" << bsl::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    ASSERT(BDLDFP_DECIMAL_DF(4.25) == BDEC::Decimal32(4.25f)); // float
    ASSERT(BDLDFP_DECIMAL_DF(4.25) == BDEC::Decimal32(4.25)); // double

    if (veryVeryVerbose) bsl::cout << "Decimal FP" << bsl::endl;

    // Decimal64
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDLDFP_DECIMAL_DD(-42.0));
    ASSERT(BDLDFP_DECIMAL_DF(42.0) == BDLDFP_DECIMAL_DD(42.0));
    ASSERT(BDLDFP_DECIMAL_DF(4.2) == BDLDFP_DECIMAL_DD(4.2));
    ASSERT(BDLDFP_DECIMAL_DF(4.2e9) == BDLDFP_DECIMAL_DD(4.2e9));

    // Dec128
    ASSERT(BDLDFP_DECIMAL_DF(-42.0) == BDLDFP_DECIMAL_DL(-42.0));
    ASSERT(BDLDFP_DECIMAL_DF(42.0) == BDLDFP_DECIMAL_DL(42.0));
    ASSERT(BDLDFP_DECIMAL_DF(4.2) == BDLDFP_DECIMAL_DL(4.2));
    ASSERT(BDLDFP_DECIMAL_DF(4.2e9) == BDLDFP_DECIMAL_DL(4.2e9));

    if (veryVerbose) bsl::cout << "Propriatery accessors" << bsl::endl;
    {
        BDEC::Decimal32 d32(42);
        ASSERT((void*)d32.data() == (void*)&d32);

        const BDEC::Decimal32 cd32(42);
        ASSERT((const void*)d32.data() == (const void*)&d32);

        ASSERT(BDEC::Decimal32(cd32.value()) == BDEC::Decimal32(42));
    }

    if (veryVerbose) bsl::cout << "Operator==" << bsl::endl;

    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));

    if (veryVerbose) bsl::cout << "Operator!=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(4.0) != BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) !=
              BDLDFP_DECIMAL_DF(-9.345e27)));

    if (veryVerbose) bsl::cout << "Operator<" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(4.0) <
           BDLDFP_DECIMAL_DF(5.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) <
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DF(4.0)));

    if (veryVerbose) bsl::cout << "Operator>" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DF(4.0));
    ASSERT(! (BDLDFP_DECIMAL_DF(-9.345e27) >
              BDLDFP_DECIMAL_DF(-9.345e27)));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DF(5.0)));

    if (veryVerbose) bsl::cout << "Operator<=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DF(4.0)));

    if (veryVerbose) bsl::cout << "Operator>=" << bsl::endl;

    ASSERT(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    ASSERT(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    ASSERT(! (BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DF(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.  Expecting to see all
    // digits is wrong because that is not how the stream output should
    // behave: it should print with the default precision as it would for
    // binary floating point *or* with the implied precision of the cohort
    // if that is larger.  AFAIU

    if (veryVerbose) bsl::cout << "Test stream out" << bsl::endl;
    {
        bsl::ostringstream out(pa);

        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-8.327457e-24));
        out << d1;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP2_ASSERT(s, decLower(s), decLower(s) == "-8.327457e-24");
    }

    if (veryVerbose) bsl::cout << "Test stream in" << bsl::endl;
    {
        bsl::istringstream in(pa);
        bsl::string ins("-8.327457e-24", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DF(-8.327457e-24));
    }

    if (veryVerbose) bsl::cout << "Test stream in NaN" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("NaN", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 != d1);
    }

    if (veryVerbose) bsl::cout << "Test stream in Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("Inf", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 > bsl::numeric_limits<BDEC::Decimal32>::max());
    }

    if (veryVerbose) bsl::cout << "Test stream in -Infinity" << bsl::endl;
    {
        bsl::istringstream  in(pa);
        bsl::string ins("-Inf", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 < -bsl::numeric_limits<BDEC::Decimal32>::max());
    }

    if (veryVerbose) bsl::cout << "Test wide stream out" << bsl::endl;
    {
        bsl::wostringstream  out(pa);
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-8.327457e-24));
        out << d1;
        bsl::wstring s(pa);
        getStringFromStream(out, &s);
        ASSERT(decLower(s) == L"-8.327457e-24");
    }

    if (veryVerbose) bsl::cout << "Test wide stream in" << bsl::endl;
    {
        bsl::wistringstream  in(pa);
        bsl::wstring ins(L"-8.327457e-24", pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        ASSERT(d1 == BDLDFP_DECIMAL_DF(-8.327457e-24));
    }

    if (veryVerbose) bsl::cout << "Unary-" << bsl::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2(BDLDFP_DECIMAL_DF( 5.0));
        ASSERT(BDLDFP_DECIMAL_DF( 5.0) == -d1);
        ASSERT(BDLDFP_DECIMAL_DF(-5.0) == -d2);

        // Unary - must make +0 into -0

        bsl::ostringstream out(pa);
        out << -BDLDFP_DECIMAL_DF(0.0);
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        ASSERT(-BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32(0)) // and 0
    }

    if (veryVerbose) bsl::cout << "Unary+" << bsl::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2(BDLDFP_DECIMAL_DF( 5.0));
        ASSERT(BDLDFP_DECIMAL_DF(-5.0) == +d1);
        ASSERT(BDLDFP_DECIMAL_DF( 5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DF(0.0));
        bsl::ostringstream out(pa);
        out << +negzero;
        bsl::string s(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        ASSERT(-BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32(0)) // and 0
    }


    if (veryVerbose) bsl::cout << "Create test objects" << bsl::endl;

    BDEC::Decimal32        d32  = BDEC::Decimal32();
    const BDEC::Decimal32  c32  = BDEC::Decimal32();

    if (veryVerbose) bsl::cout << "Check return types" << bsl::endl;

    checkType<bool>(d32 == d32);
    checkType<bool>(d32 != d32);
    checkType<bool>(d32 < d32);
    checkType<bool>(d32 <= d32);
    checkType<bool>(d32 > d32);
    checkType<bool>(d32 >= d32);

    checkType<BDEC::Decimal32>(+d32);
    checkType<BDEC::Decimal32>(-d32);

    {
        bsl::istringstream  in(pa);
        bsl::ostringstream  out(pa);
        bsl::wistringstream win(pa);
        bsl::wostringstream wout(pa);

        checkType<bsl::istream&>(in >> d32);
        checkType<bsl::wistream&>(win >> d32);
        checkType<bsl::ostream&>(out << c32);
        checkType<bsl::wostream&>(wout << c32);
    }

    typedef bsl::numeric_limits<BDEC::Decimal32> d32_limits;
    checkType<bool>(d32_limits::is_specialized);
    checkType<BDEC::Decimal32>(d32_limits::min());
    checkType<BDEC::Decimal32>(d32_limits::max());
    checkType<int>(d32_limits::digits);
    checkType<int>(d32_limits::digits10);
    checkType<int>(d32_limits::max_digits10);
    checkType<bool>(d32_limits::is_signed);
    checkType<bool>(d32_limits::is_integer);
    checkType<bool>(d32_limits::is_exact);
    checkType<int>(d32_limits::radix);
    checkType<BDEC::Decimal32>(d32_limits::epsilon());
    checkType<BDEC::Decimal32>(d32_limits::round_error());
    checkType<int>(d32_limits::min_exponent);
    checkType<int>(d32_limits::min_exponent10);
    checkType<int>(d32_limits::max_exponent);
    checkType<int>(d32_limits::max_exponent10);
    checkType<bool>(d32_limits::has_infinity);
    checkType<bool>(d32_limits::has_quiet_NaN);
    checkType<bool>(d32_limits::has_signaling_NaN);
    checkType<bsl::float_denorm_style>(d32_limits::has_denorm);
    checkType<bool>(d32_limits::has_denorm_loss);
    checkType<BDEC::Decimal32>(d32_limits::infinity());
    checkType<BDEC::Decimal32>(d32_limits::quiet_NaN());
    checkType<BDEC::Decimal32>(d32_limits::signaling_NaN());
    checkType<BDEC::Decimal32>(d32_limits::denorm_min());
    checkType<bool>(d32_limits::is_iec559);
    checkType<bool>(d32_limits::is_bounded);
    checkType<bool>(d32_limits::is_modulo);
    checkType<bool>(d32_limits::traps);
    checkType<bool>(d32_limits::tinyness_before);
    checkType<bsl::float_round_style>(d32_limits::round_style);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    pa = &ta;

    cout.precision(35);

    switch (test) { case 0:
      case 7: {
        TestDriver::testCase7();
      } break;
      case 6: {
        TestDriver::testCase6();
      } break;
      case 5: {
        TestDriver::testCase5();
      } break;
      case 4: {
        TestDriver::testCase4();
      } break;
      case 3: {
        TestDriver::testCase3();
      } break;
      case 2: {
        TestDriver::testCase2();
      } break;
      case 1: {
        TestDriver::testCase1();
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test, defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

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
