// bdldfp_decimalimplutil.t.cpp                                       -*-C++-*-
#include <bdldfp_decimalimplutil.h>

#include <bdldfp_uint128.h>
#include <bdls_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <typeinfo>

extern "C" {
#include <decSingle.h>  // Even in hardware modes, we need decNumber functions.
}

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::hex;
using bsl::atoi;
using bsl::stringstream;


// FIXME: For now, the test is disabled, on intel -- must fix

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP

int
main()
{
    return -1;
}

#else

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bdldfp_decimalimplutil' implements parsing and creation of 32-, 64-, and
// 128- bit floating point values.
//
// Global Concerns:
//: o The rules of the 754-2008 IEEE Standard for Floating-Point Arithmetic,
//:   the decNumber C library, and the Rationale for TR 24732 Extension to the
//:   programming language C Decimal Floating-Point Arithmetic
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] makeDecimalRaw32 (int mantissa,                int exponent);
// [ 2] makeDecimalRaw64 (long long mantissa,          int exponent);
// [ 2] makeDecimalRaw64 (unsigned long long mantissa, int exponent);
// [ 2] makeDecimalRaw64 (int mantissa,                int exponent);
// [ 2] makeDecimalRaw64 (unsigned int mantissa,       int exponent);
// [ 2] makeDecimalRaw128(long long mantissa,          int exponent);
// [ 2] makeDecimalRaw128(unsigned long long mantissa, int exponent);
// [ 2] makeDecimalRaw128(int mantissa,                int exponent);
// [ 2] makeDecimalRaw128(unsigned int mantissa,       int exponent);
// [ 3] parseInt (char *s, int *i,                Allocator *ma);
// [ 3] parseUInt(char *s, unsigned int *i,       Allocator *ma);
// [ 3] parseLL  (char *s, long long *i,          Allocator *ma);
// [ 3] parseULL (char *s, unsigned long long *i, Allocator *ma);
// [ 3] normalize(long long *mantissa, int *exponent)
// [ 4] equals(ValueType32  lhs, ValueType32  rhs);
// [ 4] equals(ValueType32  lhs, ValueType64  rhs);
// [ 4] equals(ValueType32  lhs, ValueType128 rhs);
// [ 4] equals(ValueType64  lhs, ValueType32  rhs);
// [ 4] equals(ValueType64  lhs, ValueType64  rhs);
// [ 4] equals(ValueType64  lhs, ValueType128 rhs);
// [ 4] equals(ValueType128 lhs, ValueType32  rhs);
// [ 4] equals(ValueType128 lhs, ValueType64  rhs);
// [ 4] equals(ValueType128 lhs, ValueType128 rhs);
// [ 5] parse32 (char *s);
// [ 5] parse64 (char *s);
// [ 5] parse128(char *s);
// [ 6] makeDecimal64(long long mantissa,          int exponent);
// [ 6] makeDecimal64(unsigned long long mantissa, int exponent);
// [ 6] makeDecimal64(int mantissa,                int exponent);
// [ 6] makeDecimal64(unsigned int mantissa,       int exponent);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
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

//=========================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P   BDLS_TESTUTIL_P   // Print identifier and value.
#define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

typedef BDEC::DecimalImplUtil Util;

static bslma::Allocator *ia = bslma::Default::globalAllocator();

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

// Normalize the specified 'mantissa' by dividing by powers of 10 if
// divisible. Add to the specified 'exponent' the powers of 10 removed from
// the mantissa. If 'mantissa' is zero, set 'exponent' to zero, giving a
// canonical zero value.
void normalize(long long *mantissa, int *exponent)
{
    // Canonicalize zeros.
    if (!(*mantissa)) {
        *exponent = 0;
        return;                                                       // RETURN
    }

    while (!(*mantissa % 10)) {
        *mantissa/= 10;
        ++*exponent;
    }
}

// Compare two mantissa and exponent pairs, with normalization.
bool decimalNormalizedCompare(long long lhs_mantissa, int lhs_exponent,
                              long long rhs_mantissa, int rhs_exponent )
{
    normalize(&lhs_mantissa, &lhs_exponent);
    normalize(&rhs_mantissa, &rhs_exponent);

    return lhs_mantissa == rhs_mantissa && lhs_exponent == rhs_exponent;
}

//-----------------------------------------------------------------------------

template <class EXPECT, class RECEIVED>
bool sameType(const RECEIVED&)
{
    return typeid(EXPECT) == typeid(RECEIVED);
}

                          // Stream buffer helpers

template <int SIZE>
struct BufferBuf : bsl::streambuf {
    BufferBuf() { reset(); }
    void reset() { this->setp(this->d_buf, this->d_buf + SIZE); }
    const char *str() { *this->pptr() =0; return this->pbase(); }
    char d_buf[SIZE + 1];
};

struct PtrInputBuf : bsl::streambuf {
    PtrInputBuf(const char *s)
    {
        char *x = const_cast<char *>(s);
        this->setg(x, x, x + strlen(x));
    }
};

struct NulBuf : bsl::streambuf {
    char d_dummy[64];
    virtual int overflow(int c)
    {
        setp( d_dummy, d_dummy + sizeof(d_dummy));
        return traits_type::not_eof(c);
    }
};

//-----------------------------------------------------------------------------

                                // ===
                                // D32
                                // ===

class D32 {
    Util::ValueType32 d_data;

    void set(int exponent, const unsigned char *bcd, int sign)
    {
        decSingle d_data_s;
        decSingleFromBCD(&d_data_s, exponent, bcd, sign);
        memcpy(&d_data, &d_data_s, sizeof(d_data));
    }
  public:
    template <unsigned S>
    D32(int exponent, const unsigned char (&bcd)[S], int sign)
    {
        if (S >= DECSINGLE_Pmax) {
            unsigned char buff[DECSINGLE_Pmax];
            memcpy(&buff, &bcd[S - DECSINGLE_Pmax], DECSINGLE_Pmax);
            set(exponent, buff, sign<0?DECFLOAT_Sign:0);
        } else {
            unsigned char buff[DECSINGLE_Pmax];
            memset(buff, 0, DECSINGLE_Pmax);
            for (unsigned i = S, j = DECSINGLE_Pmax - 1; i > 0; --i, --j) {
                buff[j] = bcd[i - 1];
            }
            set(exponent, buff, sign < 0 ? DECFLOAT_Sign : 0);
        }
    }

    D32(Util::ValueType32 data) : d_data(data)
    {
    }

    bool isEqual(const D32& other) const
    {
        // compare the memory for now.  It is wrong in general, sufficient here
        return memcmp(&d_data, &other.d_data, sizeof(d_data)) == 0;
    }

    bsl::ostream &printHex(bsl::ostream &o) const
    {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
        char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
        sprintf(buffer, "%#.7HG", &d_data);
        return o << bsl::hex << buffer << bsl::dec;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
        return o << bsl::hex << *d_data.words << bsl::dec;
#endif
    }
};

bool operator==(const D32& lhs, const D32& rhs)
{
    return lhs.isEqual(rhs);
}

bsl::ostream &operator<<(bsl::ostream &o, const D32& d)
{
    return d.printHex(o);
}

bsl::ostream &operator<<(bsl::ostream &o, const Util::ValueType32& d)
{
    return operator<<(o, D32(d));
}
                                // ===
                                // D64
                                // ===

class D64 {
    Util::ValueType64 d_data;

    void set(int exponent, const unsigned char *bcd, int sign)
    {
        decDouble d_data_s;
        decDoubleFromBCD(&d_data_s, exponent, bcd, sign);
        memcpy(&d_data, &d_data_s, sizeof(d_data));
    }
  public:
    template <unsigned S>
    D64(int exponent, const unsigned char (&bcd)[S], int sign)
    {
        if (S >= DECDOUBLE_Pmax) {
            unsigned char buff[DECDOUBLE_Pmax];
            memcpy(&buff, &bcd[S - DECDOUBLE_Pmax], DECDOUBLE_Pmax);
            set(exponent, buff, sign<0?DECFLOAT_Sign:0);
        } else {
            unsigned char buff[DECDOUBLE_Pmax];
            memset(buff, 0, DECDOUBLE_Pmax);
            for (unsigned i = S, j = DECDOUBLE_Pmax - 1; i > 0; --i, --j) {
                buff[j] = bcd[i - 1];
            }
            set(exponent, buff, sign < 0 ? DECFLOAT_Sign : 0);
        }
    }

    D64(Util::ValueType64 data) : d_data(data)
    {
    }

    bool isEqual(const D64& other) const
    {
        // compare the memory for now.  It is wrong in general, sufficient here
        return memcmp(&d_data, &other.d_data, sizeof(d_data)) == 0;
    }

    bsl::ostream &printHex(bsl::ostream &o) const
    {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
        char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
        sprintf(buffer, "%.16DG", &d_data);
        return o << bsl::hex << buffer << bsl::dec;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
        return o << bsl::hex << *d_data.words << bsl::dec;
#endif
    }
};

bool operator==(const D64& lhs, const D64& rhs)
{
    return lhs.isEqual(rhs);
}

bsl::ostream &operator<<(bsl::ostream &o, const D64& d)
{
    return d.printHex(o);
}

bsl::ostream &operator<<(bsl::ostream &o, const Util::ValueType64& d)
{
    return operator<<(o, D64(d));
}

                                // ====
                                // D128
                                // ====

class D128 {
    Util::ValueType128 d_data;

    void set(int exponent, const unsigned char *bcd, int sign)
    {
        decQuad d_data_s;
        decQuadFromBCD(&d_data_s, exponent, bcd, sign);
        memcpy(&d_data, &d_data_s, sizeof(d_data));
    }
  public:
    template <unsigned S>
    D128(int exponent, const unsigned char (&bcd)[S], int sign)
    {
        if (S >= DECQUAD_Pmax) {
            unsigned char buff[DECQUAD_Pmax];
            memcpy(&buff, &bcd[S - DECQUAD_Pmax], DECQUAD_Pmax);
            set(exponent, buff, sign<0?DECFLOAT_Sign:0);
        } else {
            unsigned char buff[DECQUAD_Pmax];
            memset(buff, 0, DECQUAD_Pmax);
            for (unsigned i = S, j = DECQUAD_Pmax - 1; i > 0; --i, --j) {
                buff[j] = bcd[i - 1];
            }
            set(exponent, buff, sign < 0 ? DECFLOAT_Sign : 0);
        }
    }

    D128(Util::ValueType128 data) : d_data(data)
    {
    }

    bool isEqual(const D128& other) const
    {
        // compare the memory for now.  It is wrong in general, sufficient here
        return memcmp(&d_data, &other.d_data, sizeof(d_data)) == 0;
    }

    bsl::ostream &printHex(bsl::ostream &o) const
    {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
        char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
        sprintf(buffer, "%.34DDG", &d_data);
        return o << bsl::hex << buffer << bsl::dec;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
        return o << bsl::hex << *d_data.words << bsl::dec;
#endif
    }
};

bool operator==(const D128& lhs, const D128& rhs)
{
    return lhs.isEqual(rhs);
}

bsl::ostream &operator<<(bsl::ostream &o, const D128& d)
{
    return d.printHex(o);
}

bsl::ostream &operator<<(bsl::ostream &o, const Util::ValueType128& d)
{
    return operator<<(o, D128(d));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

// Special constants we wish to test.
template <int Size>
struct DecConsts;

 // TODO
template<>
struct DecConsts<128>
{
    static const Util::ValueType128 pZero()
    {
        Util::ValueType128 v;
        BDEC::Uint128 e(0x2208000000000000ULL, 0);
        memcpy(&v, &e, 16);
        return v;
    }

    static const Util::ValueType128 nZero()
    {
        Util::ValueType128 v;
        BDEC::Uint128 e(0xa208000000000000ULL, 0);
        memcpy(&v, &e, 16);
        return v;
    }

    static const Util::ValueType128 pInf()
    {
        Util::ValueType128 v;
        BDEC::Uint128 e(0x7800000000000000ULL, 0);
        memcpy(&v, &e, 16);
        return v;
    }

    static const Util::ValueType128 nInf()
    {
        Util::ValueType128 v;
        BDEC::Uint128 e(0xf800000000000000ULL, 0);
        memcpy(&v, &e, 16);
        return v;
    }

    static const Util::ValueType128 sNan()
    {
        Util::ValueType128 v;
        BDEC::Uint128 e(0x7e00000000000000ULL, 0);
        memcpy(&v, &e, 16);
        return v;
    }

    static const Util::ValueType128 qNan()
    {
        Util::ValueType128 v;
        BDEC::Uint128 e(0x7c00000000000000ULL, 0);
        memcpy(&v, &e, 16);
        return v;
    }
};

template <>
struct DecConsts<64>
{
    union ValueType64Raw {
        unsigned long long bits;
        Util::ValueType64 value;
    };

    static const Util::ValueType64 pZero()
    {
        ValueType64Raw pz;
        pz.bits = 0x2238000000000000ULL;
        return pz.value;
    }
    static const Util::ValueType64 nZero()
    {
        ValueType64Raw pz;
        pz.bits = 0xa238000000000000ULL;
        return pz.value;
    }
    static const Util::ValueType64 pInf()
    {
        ValueType64Raw pz;
        pz.bits = 0x7800000000000000ULL;
        return pz.value;
    }
    static const Util::ValueType64 nInf()
    {
        ValueType64Raw pz;
        pz.bits = 0xf800000000000000ULL;
        return pz.value;
    }
    static const Util::ValueType64 sNan()
    {
        ValueType64Raw pz;
        pz.bits = 0x7e00000000000000ULL;
        return pz.value;
    }
    static const Util::ValueType64 qNan()
    {
        ValueType64Raw pz;
        pz.bits = 0x7c00000000000000ULL;
        return pz.value;
    }
};

template <>
struct DecConsts<32>
{
    union ValueType32Raw {
        unsigned int bits;
        Util::ValueType32 value;
    };

    static const Util::ValueType32 pZero()
    {
        ValueType32Raw pz;
        pz.bits = 0x22500000u;
        return pz.value;
    }
    static const Util::ValueType32 nZero()
    {
        ValueType32Raw pz;
        pz.bits = 0xa2500000u;
        return pz.value;
    }
    static const Util::ValueType32 pInf()
    {
        ValueType32Raw pz;
        pz.bits = 0x78000000u;
        return pz.value;
    }
    static const Util::ValueType32 nInf()
    {
        ValueType32Raw pz;
        pz.bits = 0xf8000000u;
        return pz.value;
    }
    static const Util::ValueType32 sNan()
    {
        ValueType32Raw pz;
        pz.bits = 0x7e000000u;
        return pz.value;
    }
    static const Util::ValueType32 qNan()
    {
        ValueType32Raw pz;
        pz.bits = 0x7c000000u;
        return pz.value;
    }
};

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;

    using bsls::AssertFailureHandlerGuard;

    bslma::TestAllocator defaultAllocator("default");
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global");
    bslma::Default::setGlobalAllocator(&globalAllocator);


    cout << "TEST " << __FILE__ << " CASE " << test << endl;;


    cout.precision(35);

    static const long long TEST_NONZERO_MANTISSAS[] = {
        // These numbers will test equality of decimal floating point values
        // of different quanta.
        2LL,
        7LL,
        20LL,
        70LL,
        200LL,
        700LL,
        2000LL,
        7000LL,
        20000LL,
        70000LL,
        200000LL,
        700000LL,
        2000000LL,
        7000000LL,
        20000000LL,
        70000000LL,
        200000000LL,
        700000000LL,
        2000000000LL,
        7000000000LL,
        20000000000LL,
        70000000000LL,
        200000000000LL,
        700000000000LL,
        2000000000000LL,
        7000000000000LL,
        20000000000000LL,
        70000000000000LL,
        200000000000000LL,
        700000000000000LL,
        2000000000000000LL,
        7000000000000000LL,
        20000000000000000LL,
        70000000000000000LL,
        200000000000000000LL,
        700000000000000000LL,

        // These number ensure that we can handle digits in all positions of
        // the mantissa.
        12LL,
        123LL,
        1234LL,
        12345LL,
        123456LL,
        1234567LL,
        12345678LL,
        123456789LL,
        1234567890LL,
        12345678901LL,
        123456789012LL,
        1234567890123LL,
        12345678901234LL,
        123456789012345LL,
        1234567890123456LL,
        12345678901234567LL,
        123456789012345678LL,
        1234567890123456789LL,

        // These numbers test the behavior of rounding to 16 significant
        // figures (e.g., in makeDecimal64.)
        44444444444444444LL,
        44444444444444445LL,
        44444444444444455LL,
        444444444444444444LL,
        444444444444444445LL,
        444444444444444455LL,
        4444444444444444445LL,
        4444444444444444455LL,
        4444444444444444555LL,
        5555555555555555555LL,

        // Negative variations of these test numbers.
        -2LL,
        -7LL,
        -20LL,
        -70LL,
        -200LL,
        -700LL,
        -2000LL,
        -7000LL,
        -20000LL,
        -70000LL,
        -200000LL,
        -700000LL,
        -2000000LL,
        -7000000LL,
        -20000000LL,
        -70000000LL,
        -200000000LL,
        -700000000LL,
        -2000000000LL,
        -7000000000LL,
        -20000000000LL,
        -70000000000LL,
        -200000000000LL,
        -700000000000LL,
        -2000000000000LL,
        -7000000000000LL,
        -20000000000000LL,
        -70000000000000LL,
        -200000000000000LL,
        -700000000000000LL,
        -2000000000000000LL,
        -7000000000000000LL,
        -20000000000000000LL,
        -70000000000000000LL,
        -200000000000000000LL,
        -700000000000000000LL,
        -12LL,
        -123LL,
        -1234LL,
        -12345LL,
        -123456LL,
        -1234567LL,
        -12345678LL,
        -123456789LL,
        -1234567890LL,
        -12345678901LL,
        -123456789012LL,
        -1234567890123LL,
        -12345678901234LL,
        -123456789012345LL,
        -1234567890123456LL,
        -12345678901234567LL,
        -123456789012345678LL,
        -1234567890123456789LL,

        -44444444444444444LL,
        -44444444444444445LL,
        -44444444444444455LL,
        -444444444444444444LL,
        -444444444444444445LL,
        -444444444444444455LL,
        -4444444444444444445LL,
        -4444444444444444455LL,
        -4444444444444444555LL,
        -5555555555555555555LL,

    };

    const int NUM_TEST_NONZERO_MANTISSAS =
                sizeof TEST_NONZERO_MANTISSAS / sizeof *TEST_NONZERO_MANTISSAS;

    static const int TEST_EXPONENTS[] = {
        -4064,
        -399,
        -398,
        -355,
        -44,
        -42,
        -13,
        -4,
        -3,
        -2,
        -1,
        0,
        1,
        2,
        3,
        4,
        13,
        42,
        44,
        355,

        // These exponents exercise the rounding behavior of makeDecimal64.
        369,
        370,
        371,
        372,
        373,
        374,
        375,
        376,
        377,
        378,
        379,
        380,
        381,
        382,
        383,
        384,
        385,

        4064
    };

    const int NUM_TEST_EXPONENTS =
                                sizeof TEST_EXPONENTS / sizeof *TEST_EXPONENTS;

    switch (test) { case 0:
      case 7: {
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Parse a decimal floating point literal
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Decimal floating points are used to represent numbers for which the exact
// decimal representation is known and we wish to avoid rounding errors that
// are introduced when the number is represented in binary.  It is sometimes of
// interest to create a decimal floating point value from its string
// representation.  The functions of this component allow us to work with
// types for which we may create representations for 32-, 64-, and 128-bit
// decimal floating point values.
//
// As an example, one mile is equal to '1.6093' kilometers exactly.  First, we
// create the 64-bit pattern for the 64-bit decimal floating point value of
// '1.6093'.
//..
BloombergLP::bdldfp::DecimalImplUtil::ValueType64 kilometersPerMile =
    BloombergLP::bdldfp::DecimalImplUtil::parse64("1.6093");
//..
// Now, if a mantissa and an exponent within range of the decimal floating
// point type are given, we can also create the decimal floating point value of
// interest with the functions of ''makeDecimalRawXX.  For example, the 32-bit
// decimal floating point type can store 7 mantissa digits and has an exponent
// range of '-101' to '90' inclusive.  It therefore may fit a mantissa of
// '16093' and an exponent of '-4', representing the value of '1.6093'
// ('16093' times ten to the power of '-4').  We create this value using
// 'makeDecimalRaw32'.
//..
BloombergLP::bdldfp::DecimalImplUtil::ValueType32
    kilometersPerMileMakeDecimal =
        BloombergLP::bdldfp::DecimalImplUtil::makeDecimalRaw32(16093, -4);
//..
// Finally, we verify that we obtain the same result without introducing
// rounding errors with both implementations.
//..
ASSERT(BloombergLP::bdldfp::DecimalImplUtil::equals(kilometersPerMile,
     kilometersPerMileMakeDecimal));
//..
//
///Example 2: Create a 64-bit decimal floating point with possible adjustment
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A mantissa and an exponent are given. The mantissa and/or the exponent may
// be out of range of the precision represented by the 64-bit decimal floating
// point type.  The 64-bit decimal floating point type represents a mantissa
// of up to 16 figures, and an exponent between '-6176' and '6111' inclusive.
// For example, on February 13, 2014, the US National Debt in Japanese Yen is
// approximately '1,765,002,361,949,679.79' Japanese Yen.  This number can be
// represented with a mantissa of '176500236194967979' and an exponent of
// '-2'. First, we create this number.
//..
BloombergLP::bdldfp::DecimalImplUtil::ValueType64 usNationalDebtInJpy =
    BloombergLP::bdldfp::DecimalImplUtil::makeDecimal64(
        176500236194967979ull, -2);
//..
// Notice that '176500236194967979' contains 18 significant figures, more
// than can be represented by a 64-bit decimal floating point type.
//
// Now, the number can also be parsed as a string.
//..
BloombergLP::bdldfp::DecimalImplUtil::ValueType64
    usNationalDebtInJpyParse =
        BloombergLP::bdldfp::DecimalImplUtil::parse64("1765002361949679.79");
//..
// Finally, we verify that we obtain the same result.
//..
ASSERT(BloombergLP::bdldfp::DecimalImplUtil::equals(usNationalDebtInJpy,
     usNationalDebtInJpyParse));
//..
      }
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'makeDecimal64'
        //
        // Concerns:
        //: 1 'makeDecimal64' passes its arguments to 'makeDecimal64Raw' and
        //:   'makeDecimal128Raw' correctly.
        //: 2 The correct rounding behavior is performed when the specified
        //:   mantissa has a magnitude too great to be represented by a 64-bit
        //:   decimal floating point value.
        //: 3 The correct rounding behavior is performed when the magnitude
        //:   of the specified number is too great to be represented by a
        //:   64-bit decimal floating point value.
        //: 4 The correct sign is preserved when rounding to zero or infinite
        //:   values.
        //: 5 If the mantissa and exponent are in range for 64-bit decimal
        //:   floating point numbers (i.e.,
        //:   'abs(mantissa) <= 9,999,999,999,999,999' and
        //:   '-398 <= exponent <= 369', the 64-bit decimal floating number
        //:   consisting of the mantissa and exponent is returned without
        //:   rounding.
        //: 6 The four overloads of 'makeDecimal64' perform properly.
        //
        // Plan:
        //: 1 Test that 'makeDecimal64' returns a value that is 'equals' to
        //:   the value given by 'parse64' called on a string containing the
        //:   specified 'mantissa' and 'exponent'.
        //: 2 Test 'makeDecimal64' on values for which both the mantissa and
        //:   exponent are in the range of 64-bit decimal floating point. The
        //:   value returned should be identical in bits to that returned by
        //:   'makeDecimal64Raw' called on 'mantissa' and 'exponent'.
        //: 3 For each mantissa, exponent pair, test 'makeDecimal64' for all
        //:   overloads for which the mantissa fits.
        //
        // Testing:
        //   makeDecimal64(long long mantissa,          int exponent);
        //   makeDecimal64(unsigned long long mantissa, int exponent);
        //   makeDecimal64(int mantissa,                int exponent);
        //   makeDecimal64(unsigned int mantissa,       int exponent);
        // --------------------------------------------------------------------
          if (verbose) bsl::cout << bsl::endl
                                  << "TESTING 'makeDecimal64'" << bsl::endl
                                  << "=======================" << bsl::endl;
          {
              for (int mi = 0; mi < NUM_TEST_NONZERO_MANTISSAS; ++mi) {
                  for (int ei = 0; ei < NUM_TEST_EXPONENTS; ++ei) {
                      const long long MANTISSA = TEST_NONZERO_MANTISSAS[mi];
                      const int EXPONENT = TEST_EXPONENTS[ei];

                      char TEST_STRING[100];
                      sprintf(TEST_STRING, "%llde%d", MANTISSA, EXPONENT);

                      Util::ValueType64 EXPECTED64 =
                          Util::parse64(TEST_STRING);
                      LOOP2_ASSERT(MANTISSA, EXPONENT,
                                   Util::equals(Util::makeDecimal64(
                                             MANTISSA, EXPONENT), EXPECTED64));

                      if (MANTISSA >= 0) {
                          LOOP2_ASSERT(MANTISSA, EXPONENT,
                                       Util::equals(Util::makeDecimal64(
                              static_cast<unsigned long long>(
                                            MANTISSA), EXPONENT), EXPECTED64));
                      }
                      if ((MANTISSA >= bsl::numeric_limits<int>::min()) &&
                          (MANTISSA <= bsl::numeric_limits<int>::max())) {
                          LOOP2_ASSERT(MANTISSA, EXPONENT,
                                       Util::equals(Util::makeDecimal64(
                              static_cast<int>(MANTISSA), EXPONENT),
                                                                  EXPECTED64));
                      }
                      if ((MANTISSA >=
                           bsl::numeric_limits<unsigned int>::min()) &&
                          (MANTISSA <=
                           bsl::numeric_limits<unsigned int>::max())) {
                          LOOP2_ASSERT(MANTISSA, EXPONENT, Util::equals(
                                           Util::makeDecimal64(
                              static_cast<unsigned int>(
                                            MANTISSA), EXPONENT), EXPECTED64));
                      }

                      bool exponentInRange =
                                       (EXPONENT >= -398) && (EXPONENT <= 369);
                      bool mantissaInRange =
                          (MANTISSA >= -9999999999999999LL) &&
                                              (MANTISSA <= 9999999999999999LL);

                      if (exponentInRange & mantissaInRange) {
                          LOOP2_ASSERT(MANTISSA, EXPONENT,
                                       Util::makeDecimal64(
                                           MANTISSA, EXPONENT) ==
                                   Util::makeDecimalRaw64(MANTISSA, EXPONENT));
                          if (MANTISSA >= 0) {
                              LOOP2_ASSERT(MANTISSA, EXPONENT,
                                           Util::makeDecimal64(
                                         static_cast<unsigned long long>(
                                                        MANTISSA), EXPONENT) ==
                                           Util::makeDecimalRaw64(
                                               static_cast<unsigned long long>(
                                                         MANTISSA), EXPONENT));
                          }
                          if ((MANTISSA >= bsl::numeric_limits<int>::min()) &&
                              (MANTISSA <= bsl::numeric_limits<int>::max())) {
                              LOOP2_ASSERT(MANTISSA, EXPONENT,
                                           Util::makeDecimal64(
                                         static_cast<int>(
                                                        MANTISSA), EXPONENT) ==
                                     Util::makeDecimalRaw64(
                                         static_cast<int>(
                                                         MANTISSA), EXPONENT));
                          }
                          if ((MANTISSA >=
                               bsl::numeric_limits<unsigned int>::min()) &&
                              (MANTISSA <=
                               bsl::numeric_limits<unsigned int>::max())) {
                              LOOP2_ASSERT(MANTISSA, EXPONENT,
                                           Util::makeDecimal64(
                                         static_cast<unsigned int>(
                                                        MANTISSA), EXPONENT) ==
                                     Util::makeDecimalRaw64(
                                         static_cast<unsigned int>(
                                                         MANTISSA), EXPONENT));
                          }
                      }
                  }
              }
          }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHODS
        //   'parse32', 'parse64', 'parse128'
        //
        // Concerns:
        //: 1 'parseXX' passes its arguments to the 'decNumber' library, or to
        //:   scanf in the context of a hardware implementation, correctly.
        //: 2 The correct rounding behavior is performed when the specified
        //:   mantissa has a magnitude too great to be represented by a 64-bit
        //:   decimal floating point value.
        //: 3 The correct rounding behavior is performed when the magnitude
        //:   of the specified number is too great to be represented by a
        //:   64-bit decimal floating point value.
        //: 4 The correct sign is preserved when rounding to zero or infinite
        //:   values.
        //
        // Plan:
        //: 1 Test the parsing function on a set of mantissa and exponents in
        //:   which rounding is not needed. This can be done using an array of
        //:   mantissa and exponents from which strings to be parsed are
        //:   generated.
        //: 2 Test the rounding behavior when the mantissa has a magnitude too
        //:   large to be represented by the particular variant of decimal
        //:   floating point.
        //
        // Testing:
        //   parse32 (char *s);
        //   parse64 (char *s);
        //   parse128(char *s);
        // --------------------------------------------------------------------
          {
              if (verbose) bsl::cout
                                     << bsl::endl
                                     << "TESTING PARSE FUNCTIONS" << bsl::endl
                                     << "=======================" << bsl::endl;

              if (veryVerbose) bsl::cout << "\nTesting non-rounding cases."
                                      << bsl::endl;
              {
                  for (int mi = 0; mi < NUM_TEST_NONZERO_MANTISSAS; ++mi) {
                      for (int ei = 0; ei < NUM_TEST_EXPONENTS; ++ei) {
                          const long long MANTISSA =
                                                    TEST_NONZERO_MANTISSAS[mi];
                          const int EXPONENT = TEST_EXPONENTS[ei];

                          char TEST_STRING[100];
                          sprintf(TEST_STRING, "%llde%d", MANTISSA, EXPONENT);

                          Util::ValueType128 EXPECTED128 =
                                   Util::makeDecimalRaw128(MANTISSA, EXPONENT);
                          Util::ValueType128 ACTUAL128 =
                                                   Util::parse128(TEST_STRING);

                          ASSERT(Util::equals(ACTUAL128, EXPECTED128));

                          const bool fits64 =
                              (-9999999999999999LL <= MANTISSA) &&
                              (MANTISSA <= 9999999999999999LL) &&
                              (-398 <= EXPONENT) &&
                              (EXPONENT <= 369);

                          const bool fits32 =
                              (-9999999LL <= MANTISSA) &&
                              (MANTISSA <= 9999999LL) &&
                              (-101 <= EXPONENT) &&
                              (EXPONENT <= 90);

                          if (fits64) {
                              const Util::ValueType64 ACTUAL64 =
                                                    Util::parse64(TEST_STRING);
                              const Util::ValueType64 EXPECTED64 =
                                    Util::makeDecimalRaw64(MANTISSA, EXPONENT);
                              ASSERT(Util::equals(ACTUAL64, EXPECTED64));
                          }

                          if (fits32) {
                              int mantissa = static_cast<int>(MANTISSA);
                              ASSERT(mantissa == MANTISSA);

                              const Util::ValueType32 ACTUAL32 =
                                                    Util::parse32(TEST_STRING);
                              const Util::ValueType32 EXPECTED32 =
                                    Util::makeDecimalRaw32(mantissa, EXPONENT);
                              ASSERT(Util::equals(ACTUAL32, EXPECTED32));
                          }

                          if (MANTISSA >= 0) {
                              // Test with optional leading '+'.
                              char TEST_STRING[100];
                              sprintf(TEST_STRING, "+%llde%d",
                                                           MANTISSA, EXPONENT);

                              Util::ValueType128 EXPECTED128 =
                                   Util::makeDecimalRaw128(MANTISSA, EXPONENT);
                              Util::ValueType128 ACTUAL128 =
                                                   Util::parse128(TEST_STRING);

                              ASSERT(Util::equals(ACTUAL128, EXPECTED128));

                              if (fits64) {
                                  const Util::ValueType64 ACTUAL64 =
                                                    Util::parse64(TEST_STRING);
                                  const Util::ValueType64 EXPECTED64 =
                                    Util::makeDecimalRaw64(MANTISSA, EXPONENT);
                                  ASSERT(Util::equals(ACTUAL64, EXPECTED64));
                              }

                              if (fits32) {
                                  int mantissa = static_cast<int>(MANTISSA);
                                  ASSERT(mantissa == MANTISSA);

                                  const Util::ValueType32 ACTUAL32 =
                                                    Util::parse32(TEST_STRING);
                                  const Util::ValueType32 EXPECTED32 =
                                    Util::makeDecimalRaw32(mantissa, EXPONENT);
                                  ASSERT(Util::equals(ACTUAL32, EXPECTED32));
                              }
                          }

                          // Test with capital 'E'.
                          {
                              char TEST_STRING[100];
                              sprintf(
                                   TEST_STRING, "%llde%d", MANTISSA, EXPONENT);

                              Util::ValueType128 EXPECTED128 =
                                   Util::makeDecimalRaw128(MANTISSA, EXPONENT);
                              Util::ValueType128 ACTUAL128 =
                                                   Util::parse128(TEST_STRING);

                              ASSERT(Util::equals(ACTUAL128, EXPECTED128));

                              const bool fits64 =
                                  (-9999999999999999LL <= MANTISSA) &&
                                  (MANTISSA <= 9999999999999999LL) &&
                                  (-398 <= EXPONENT) &&
                                  (EXPONENT <= 369);

                              const bool fits32 =
                                  (-9999999LL <= MANTISSA) &&
                                  (MANTISSA <= 9999999LL) &&
                                  (-101 <= EXPONENT) &&
                                  (EXPONENT <= 90);

                              if (fits64) {
                                  const Util::ValueType64 ACTUAL64 =
                                                    Util::parse64(TEST_STRING);
                                  const Util::ValueType64 EXPECTED64 =
                                    Util::makeDecimalRaw64(MANTISSA, EXPONENT);
                                  ASSERT(Util::equals(ACTUAL64, EXPECTED64));
                              }

                              if (fits32) {
                                  int mantissa = static_cast<int>(MANTISSA);
                                  ASSERT(mantissa == MANTISSA);

                                  const Util::ValueType32 ACTUAL32 =
                                                    Util::parse32(TEST_STRING);
                                  const Util::ValueType32 EXPECTED32 =
                                    Util::makeDecimalRaw32(mantissa, EXPONENT);
                                  ASSERT(Util::equals(ACTUAL32, EXPECTED32));
                              }
                          }
                      }
                  }
              }

              if (veryVerbose) bsl::cout << bsl::endl
                                         << "Test cases involving combination "
                                         << "field" << bsl::endl;
              {
                  for (unsigned char LEADING_DIGIT = 1; LEADING_DIGIT <= 9;
                       ++LEADING_DIGIT) {
                      for (int EXPONENT = -6176; EXPONENT <= 6111;
                                                                  ++EXPONENT) {
                          // Create the strong formed by the leading digit
                          // followed by 33 zeros, followed by "e" and the
                          // exponent.
                          char STRING_VAL[50];
                          sprintf(STRING_VAL, "%d", LEADING_DIGIT);

                          int i;
                          for (i = 1; i <= 33; ++i) {
                              STRING_VAL[i] = '0';
                          }
                          STRING_VAL[i++] = 'e';
                          sprintf(&STRING_VAL[i], "%d", EXPONENT);

                          Util::ValueType128 ACTUAL =
                                                    Util::parse128(STRING_VAL);
                          unsigned char iBCD[34] = {LEADING_DIGIT, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                    0, 0};
                          const D128 EXPECTED = D128(EXPONENT, iBCD, 1);
                          LOOP2_ASSERT(LEADING_DIGIT,
                                       EXPONENT,
                                       ACTUAL == EXPECTED);
                      }
                  }

                  for (unsigned char LEADING_DIGIT = 1; LEADING_DIGIT <= 9;
                       ++LEADING_DIGIT) {
                      for (int EXPONENT = -6176; EXPONENT <= 6111;
                                                                  ++EXPONENT) {
                          // Create the string formed by the leading digit
                          // followed by eleven occurrances of the 3-digit
                          // pattern "255", followed by "e" and the exponent.
                          char STRING_VAL[50];
                          sprintf(STRING_VAL,
                                  "%d255255255255255255255255255255255e%d",
                                  LEADING_DIGIT, EXPONENT);

                          Util::ValueType128 ACTUAL =
                                                    Util::parse128(STRING_VAL);
                          unsigned char iBCD[34] = {LEADING_DIGIT, 2, 5, 5, 2,
                                                    5, 5, 2, 5, 5, 2, 5, 5, 2,
                                                    5, 5, 2, 5, 5, 2, 5, 5, 2,
                                                    5, 5, 2, 5, 5, 2, 5, 5, 2,
                                                    5, 5};
                          const D128 EXPECTED = D128(EXPONENT, iBCD, 1);
                          LOOP2_ASSERT(LEADING_DIGIT,
                                       EXPONENT,
                                       ACTUAL == EXPECTED);
                      }
                  }

                  for (unsigned char LEADING_DIGIT = 1; LEADING_DIGIT <= 9;
                       ++LEADING_DIGIT) {
                      for (int EXPONENT = -6176; EXPONENT <= 6111;
                                                                  ++EXPONENT) {
                          // Create the strong formed by the leading digit
                          // followed by eleven occurrances of the 3-digit
                          // pattern "582", followed by "e" and the exponent.

                          char STRING_VAL[50];
                          sprintf(STRING_VAL,
                                  "%d582582582582582582582582582582582e%d",
                                  LEADING_DIGIT, EXPONENT);

                          Util::ValueType128 ACTUAL =
                                                    Util::parse128(STRING_VAL);
                          unsigned char iBCD[34] = {LEADING_DIGIT, 5, 8, 2, 5,
                                                    8, 2, 5, 8, 2, 5, 8, 2, 5,
                                                    8, 2, 5, 8, 2, 5, 8, 2, 5,
                                                    8, 2, 5, 8, 2, 5, 8, 2, 5,
                                                    8, 2};
                          const D128 EXPECTED = D128(EXPONENT, iBCD, 1);
                          LOOP2_ASSERT(LEADING_DIGIT,
                                       EXPONENT,
                                       ACTUAL == EXPECTED);
                      }
                  }
              }

              if (veryVerbose) bsl::cout << "\nTesting rounding cases"
                                      << bsl::endl;
              {
                  // Test the parse function for cases in which rounding is
                  // needed.
                  static const struct {
                      int         d_lineNum;
                      const char *d_stringVal;
                      long long   d_expectedMantissa;
                      int         d_expectedExponent;
                  } PARSE64_TEST_DATA[] = {
              { L_,            "12345678901234567",  1234567890123457LL,   1 },
              { L_,           "123456789012345678",  1234567890123457LL,   2 },
              { L_,          "1234567890123456789",  1234567890123457LL,   3 },
              { L_,         "12345678901234567890",  1234567890123457LL,   4 },
              { L_,        "123456789012345678901",  1234567890123457LL,   5 },
              { L_,       "1234567890123456789012",  1234567890123457LL,   6 },
              { L_,      "12345678901234567890123",  1234567890123457LL,   7 },
              { L_,         "12345678901234567e50",  1234567890123457LL,  51 },
              { L_,        "123456789012345678e50",  1234567890123457LL,  52 },
              { L_,       "1234567890123456789e50",  1234567890123457LL,  53 },
              { L_,      "12345678901234567890e50",  1234567890123457LL,  54 },
              { L_,     "123456789012345678901e50",  1234567890123457LL,  55 },
              { L_,    "1234567890123456789012e50",  1234567890123457LL,  56 },
              { L_,   "12345678901234567890123e50",  1234567890123457LL,  57 },
              { L_,        "12345678901234567e-50",  1234567890123457LL, -49 },
              { L_,       "123456789012345678e-50",  1234567890123457LL, -48 },
              { L_,      "1234567890123456789e-50",  1234567890123457LL, -47 },
              { L_,     "12345678901234567890e-50",  1234567890123457LL, -46 },
              { L_,    "123456789012345678901e-50",  1234567890123457LL, -45 },
              { L_,   "1234567890123456789012e-50",  1234567890123457LL, -44 },
              { L_,  "12345678901234567890123e-50",  1234567890123457LL, -43 },

              { L_,            "44444444444444445",  4444444444444444LL,   1 },
              { L_,            "44444444444444455",  4444444444444446LL,   1 },
              { L_,         "44444444444444445e50",  4444444444444444LL,  51 },
              { L_,         "44444444444444455e50",  4444444444444446LL,  51 },
              { L_,        "44444444444444445e-50",  4444444444444444LL, -49 },
              { L_,        "44444444444444455e-50",  4444444444444446LL, -49 },
              { L_,  "444444444444444444444444445",  4444444444444444LL,  11 },

              { L_,                        "1e370",                10LL, 369 },
              { L_,                       "12e371",              1200LL, 369 },
              { L_,                      "123e372",            123000LL, 369 },
              { L_,                     "1234e373",          12340000LL, 369 },
              { L_,                    "12345e374",        1234500000LL, 369 },
              { L_,                   "123456e375",      123456000000LL, 369 },
              { L_,                  "1234567e376",    12345670000000LL, 369 },
              { L_,                 "12345678e377",  1234567800000000LL, 369 },

              { L_,           "+12345678901234567",  1234567890123457LL,   1 },
              { L_,          "+123456789012345678",  1234567890123457LL,   2 },
              { L_,         "+1234567890123456789",  1234567890123457LL,   3 },
              { L_,        "+12345678901234567890",  1234567890123457LL,   4 },
              { L_,       "+123456789012345678901",  1234567890123457LL,   5 },
              { L_,      "+1234567890123456789012",  1234567890123457LL,   6 },
              { L_,     "+12345678901234567890123",  1234567890123457LL,   7 },
              { L_,        "+12345678901234567e50",  1234567890123457LL,  51 },
              { L_,       "+123456789012345678e50",  1234567890123457LL,  52 },
              { L_,      "+1234567890123456789e50",  1234567890123457LL,  53 },
              { L_,     "+12345678901234567890e50",  1234567890123457LL,  54 },
              { L_,    "+123456789012345678901e50",  1234567890123457LL,  55 },
              { L_,   "+1234567890123456789012e50",  1234567890123457LL,  56 },
              { L_,  "+12345678901234567890123e50",  1234567890123457LL,  57 },
              { L_,       "+12345678901234567e-50",  1234567890123457LL, -49 },
              { L_,      "+123456789012345678e-50",  1234567890123457LL, -48 },
              { L_,     "+1234567890123456789e-50",  1234567890123457LL, -47 },
              { L_,    "+12345678901234567890e-50",  1234567890123457LL, -46 },
              { L_,   "+123456789012345678901e-50",  1234567890123457LL, -45 },
              { L_,  "+1234567890123456789012e-50",  1234567890123457LL, -44 },
              { L_, "+12345678901234567890123e-50",  1234567890123457LL, -43 },

              { L_,           "+44444444444444445",  4444444444444444LL,   1 },
              { L_,           "+44444444444444455",  4444444444444446LL,   1 },
              { L_,        "+44444444444444445e50",  4444444444444444LL,  51 },
              { L_,        "+44444444444444455e50",  4444444444444446LL,  51 },
              { L_,       "+44444444444444445e-50",  4444444444444444LL, -49 },
              { L_,       "+44444444444444455e-50",  4444444444444446LL, -49 },
              { L_, "+444444444444444444444444445",  4444444444444444LL,  11 },

              { L_,                       "+1e370",                10LL, 369 },
              { L_,                      "+12e371",              1200LL, 369 },
              { L_,                     "+123e372",            123000LL, 369 },
              { L_,                    "+1234e373",          12340000LL, 369 },
              { L_,                   "+12345e374",        1234500000LL, 369 },
              { L_,                  "+123456e375",      123456000000LL, 369 },
              { L_,                 "+1234567e376",    12345670000000LL, 369 },
              { L_,                "+12345678e377",  1234567800000000LL, 369 },

              { L_,           "-12345678901234567", -1234567890123457LL,   1 },
              { L_,          "-123456789012345678", -1234567890123457LL,   2 },
              { L_,         "-1234567890123456789", -1234567890123457LL,   3 },
              { L_,        "-12345678901234567890", -1234567890123457LL,   4 },
              { L_,       "-123456789012345678901", -1234567890123457LL,   5 },
              { L_,      "-1234567890123456789012", -1234567890123457LL,   6 },
              { L_,     "-12345678901234567890123", -1234567890123457LL,   7 },
              { L_,        "-12345678901234567e50", -1234567890123457LL,  51 },
              { L_,       "-123456789012345678e50", -1234567890123457LL,  52 },
              { L_,      "-1234567890123456789e50", -1234567890123457LL,  53 },
              { L_,     "-12345678901234567890e50", -1234567890123457LL,  54 },
              { L_,    "-123456789012345678901e50", -1234567890123457LL,  55 },
              { L_,   "-1234567890123456789012e50", -1234567890123457LL,  56 },
              { L_,  "-12345678901234567890123e50", -1234567890123457LL,  57 },
              { L_,       "-12345678901234567e-50", -1234567890123457LL, -49 },
              { L_,      "-123456789012345678e-50", -1234567890123457LL, -48 },
              { L_,     "-1234567890123456789e-50", -1234567890123457LL, -47 },
              { L_,    "-12345678901234567890e-50", -1234567890123457LL, -46 },
              { L_,   "-123456789012345678901e-50", -1234567890123457LL, -45 },
              { L_,  "-1234567890123456789012e-50", -1234567890123457LL, -44 },
              { L_, "-12345678901234567890123e-50", -1234567890123457LL, -43 },

              { L_,                       "-1e370",               -10LL, 369 },
              { L_,                      "-12e371",             -1200LL, 369 },
              { L_,                     "-123e372",           -123000LL, 369 },
              { L_,                    "-1234e373",         -12340000LL, 369 },
              { L_,                   "-12345e374",       -1234500000LL, 369 },
              { L_,                  "-123456e375",     -123456000000LL, 369 },
              { L_,                 "-1234567e376",   -12345670000000LL, 369 },
              { L_,                "-12345678e377", -1234567800000000LL, 369 },

              { L_,           "-44444444444444445", -4444444444444444LL,   1 },
              { L_,           "-44444444444444455", -4444444444444446LL,   1 },
              { L_,        "-44444444444444445e50", -4444444444444444LL,  51 },
              { L_,        "-44444444444444455e50", -4444444444444446LL,  51 },
              { L_,       "-44444444444444445e-50", -4444444444444444LL, -49 },
              { L_,       "-44444444444444455e-50", -4444444444444446LL, -49 },
              { L_, "-444444444444444444444444445", -4444444444444444LL,  11 },
                  };

                  const int NUM_PARSE64_TESTS =
                          sizeof PARSE64_TEST_DATA / sizeof *PARSE64_TEST_DATA;

                  for (int ti = 0; ti < NUM_PARSE64_TESTS; ++ti) {
                      const int LINE = PARSE64_TEST_DATA[ti].d_lineNum;
                      const char *STRING_VAL =
                                             PARSE64_TEST_DATA[ti].d_stringVal;
                      const long long mantissa =
                                      PARSE64_TEST_DATA[ti].d_expectedMantissa;
                      const int exponent =
                                      PARSE64_TEST_DATA[ti].d_expectedExponent;
                      Util::ValueType64 EXPECTED =
                                    Util::makeDecimalRaw64(mantissa, exponent);
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      LOOP_ASSERT(LINE, Util::equals(ACTUAL, EXPECTED));
                  }

                  // Test very long numbers.
                  // 1 followed by lots of 0's.
                  for (int e = 0; e <= 369; ++e) {
                      // Create 1 followed by e 0's.
                      char STRING_VAL[500];
                      STRING_VAL[0] = '1';
                      for (int i = 1; i < e + 1; ++i) {
                          STRING_VAL[i] = '0';
                      }
                      STRING_VAL[e + 1] = 0;
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      Util::ValueType64 EXPECTED =
                                                Util::makeDecimalRaw64(1LL, e);
                      LOOP_ASSERT(e, Util::equals(ACTUAL, EXPECTED));

                      // Create +1 followed by e 0's.
                      char POS_STRING_VAL[500];
                      POS_STRING_VAL[0] = '+';
                      POS_STRING_VAL[1] = '1';
                      for (int i = 2; i < e + 2; ++i) {
                          POS_STRING_VAL[i] = '0';
                      }
                      POS_STRING_VAL[e + 2] = 0;
                      Util::ValueType64 POS_ACTUAL =
                                                 Util::parse64(POS_STRING_VAL);
                      LOOP_ASSERT(e, Util::equals(POS_ACTUAL, EXPECTED));

                      // Create -1 followed by e 0's.
                      char NEG_STRING_VAL[500];
                      NEG_STRING_VAL[0] = '-';
                      NEG_STRING_VAL[1] = '1';
                      for (int i = 2; i < e + 2; ++i) {
                          NEG_STRING_VAL[i] = '0';
                      }
                      NEG_STRING_VAL[e + 2] = 0;
                      Util::ValueType64 NEG_ACTUAL =
                                                 Util::parse64(NEG_STRING_VAL);
                      Util::ValueType64 NEG_EXPECTED =
                                               Util::makeDecimalRaw64(-1LL, e);
                      LOOP_ASSERT(e, Util::equals(NEG_ACTUAL, NEG_EXPECTED));
                  }

                  // Test very long numbers with rounding.

                  // Lots of 4's followed by a 5.
                  for (int e = 18; e < 369; ++e) {
                      char STRING_VAL_ROUND[500];
                      for (int i = 0; i < e - 1; ++i) {
                          STRING_VAL_ROUND[i] = '4';
                      }
                      STRING_VAL_ROUND[e - 1] = '5';
                      STRING_VAL_ROUND[e] = 0;
                      Util::ValueType64 ACTUAL =
                                               Util::parse64(STRING_VAL_ROUND);
                      Util::ValueType64 EXPECTED =
                            Util::makeDecimalRaw64(4444444444444444LL, e - 16);
                      LOOP_ASSERT(e, Util::equals(ACTUAL, EXPECTED));
                  }

                  // 16 4s followed by a 5, folllowed by 4s. Round up.
                  for (int e = 18; e <= 369; ++e) {
                      char STRING_VAL_ROUND[500];

                      for (int i = 0; i < 16; ++i) {
                          STRING_VAL_ROUND[i] = '4';
                      }
                      STRING_VAL_ROUND[16] = '5';
                      for (int i = 17; i < e; ++i) {
                          STRING_VAL_ROUND[i] = '4';
                      }
                      STRING_VAL_ROUND[e] = 0;

                      Util::ValueType64 ACTUAL =
                                               Util::parse64(STRING_VAL_ROUND);
                      Util::ValueType64 EXPECTED =
                            Util::makeDecimalRaw64(4444444444444445LL, e - 16);
                      LOOP_ASSERT(STRING_VAL_ROUND, Util::equals(ACTUAL,
                                                                 EXPECTED));
                  }
              }

              if (veryVerbose) bsl::cout << "\nTesting infinity cases."
                                      << bsl::endl;
              {
                  // All of these string should parse to plus or minus
                  // infinity.
                  static const struct {
                      int         d_lineNum;
                      const char *d_stringVal;
                      int         d_expectedSignOfInfinity;
                      // 1 for +inf, -1 for -inf.
                  } PARSE64_INFINITY_TEST_DATA[] = {
                      { L_,                    "1e386",  1},
                      { L_,                   "12e385",  1},
                      { L_,                  "123e384",  1},
                      { L_,                 "1234e383",  1},
                      { L_,                "12345e382",  1},
                      { L_,               "123456e381",  1},
                      { L_,              "1234567e380",  1},
                      { L_,             "12345678e379",  1},
                      { L_,            "123456789e378",  1},
                      { L_,           "1234567890e377",  1},
                      { L_,          "12345678901e376",  1},
                      { L_,         "123456789012e375",  1},
                      { L_,        "1234567890123e374",  1},
                      { L_,       "12345678901234e373",  1},
                      { L_,      "123456789012345e372",  1},
                      { L_,     "1234567890123456e371",  1},
                      { L_,    "12345678901234567e370",  1},
                      { L_,   "123456789012345678e369",  1},
                      { L_,  "1234567890123456789e368",  1},
                      { L_,                   "+1e386",  1},
                      { L_,                  "+12e385",  1},
                      { L_,                 "+123e384",  1},
                      { L_,                "+1234e383",  1},
                      { L_,               "+12345e382",  1},
                      { L_,              "+123456e381",  1},
                      { L_,             "+1234567e380",  1},
                      { L_,            "+12345678e379",  1},
                      { L_,           "+123456789e378",  1},
                      { L_,          "+1234567890e377",  1},
                      { L_,         "+12345678901e376",  1},
                      { L_,        "+123456789012e375",  1},
                      { L_,       "+1234567890123e374",  1},
                      { L_,      "+12345678901234e373",  1},
                      { L_,     "+123456789012345e372",  1},
                      { L_,    "+1234567890123456e371",  1},
                      { L_,   "+12345678901234567e370",  1},
                      { L_,  "+123456789012345678e369",  1},
                      { L_, "+1234567890123456789e368",  1},
                      { L_,                      "inf",  1},
                      { L_,                      "inF",  1},
                      { L_,                      "iNf",  1},
                      { L_,                      "iNF",  1},
                      { L_,                      "Inf",  1},
                      { L_,                      "InF",  1},
                      { L_,                      "INf",  1},
                      { L_,                      "INF",  1},
                      { L_,                     "+inf",  1},
                      { L_,                     "+inF",  1},
                      { L_,                     "+iNf",  1},
                      { L_,                     "+iNF",  1},
                      { L_,                     "+Inf",  1},
                      { L_,                     "+InF",  1},
                      { L_,                     "+INf",  1},
                      { L_,                     "+INF",  1},

                      { L_,                   "-1e386", -1},
                      { L_,                  "-12e385", -1},
                      { L_,                 "-123e384", -1},
                      { L_,                "-1234e383", -1},
                      { L_,               "-12345e382", -1},
                      { L_,              "-123456e381", -1},
                      { L_,             "-1234567e380", -1},
                      { L_,            "-12345678e379", -1},
                      { L_,           "-123456789e378", -1},
                      { L_,          "-1234567890e377", -1},
                      { L_,         "-12345678901e376", -1},
                      { L_,        "-123456789012e375", -1},
                      { L_,       "-1234567890123e374", -1},
                      { L_,      "-12345678901234e373", -1},
                      { L_,     "-123456789012345e372", -1},
                      { L_,    "-1234567890123456e371", -1},
                      { L_,   "-12345678901234567e370", -1},
                      { L_,  "-123456789012345678e369", -1},
                      { L_, "-1234567890123456789e368", -1},
                      { L_,                     "-inf", -1},
                      { L_,                     "-inF", -1},
                      { L_,                     "-iNf", -1},
                      { L_,                     "-iNF", -1},
                      { L_,                     "-Inf", -1},
                      { L_,                     "-InF", -1},
                      { L_,                     "-INf", -1},
                      { L_,                     "-INF", -1},

                  };

                  const int NUM_PARSE64_INFINITY_TESTS =
                      sizeof PARSE64_INFINITY_TEST_DATA /
                                            sizeof *PARSE64_INFINITY_TEST_DATA;

                  for (int ti = 0; ti < NUM_PARSE64_INFINITY_TESTS; ++ti) {
                      const int LINE =
                                      PARSE64_INFINITY_TEST_DATA[ti].d_lineNum;
                      const char *STRING_VAL =
                                    PARSE64_INFINITY_TEST_DATA[ti].d_stringVal;
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      Util::ValueType64 EXPECTED =
                          PARSE64_INFINITY_TEST_DATA[ti].
                          d_expectedSignOfInfinity >= 0 ?
                          DecConsts<64>::pInf() : DecConsts<64>::nInf();
                      LOOP_ASSERT(LINE, Util::equals(ACTUAL, EXPECTED));
                  }

                  // Test 256 capitalizations of "infinity".
                  for (int i = 0; i < 256; i++) {
                      char STRING_VAL[9];
                      STRING_VAL[0] = (i &   1) ? 'I' : 'i';
                      STRING_VAL[1] = (i &   2) ? 'N' : 'n';
                      STRING_VAL[2] = (i &   4) ? 'F' : 'f';
                      STRING_VAL[3] = (i &   8) ? 'I' : 'i';
                      STRING_VAL[4] = (i &  16) ? 'N' : 'n';
                      STRING_VAL[5] = (i &  32) ? 'I' : 'i';
                      STRING_VAL[6] = (i &  64) ? 'T' : 't';
                      STRING_VAL[7] = (i & 128) ? 'Y' : 'y';
                      STRING_VAL[8] = 0;
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      Util::ValueType64 EXPECTED = DecConsts<64>::pInf();
                      LOOP_ASSERT(STRING_VAL, ACTUAL == EXPECTED);
                  }

                  // Test 256 capitalizations of "+infinity".
                  for (int i = 0; i < 256; i++) {
                      char STRING_VAL[10];
                      STRING_VAL[0] = '+';
                      STRING_VAL[1] = (i &   1) ? 'I' : 'i';
                      STRING_VAL[2] = (i &   2) ? 'N' : 'n';
                      STRING_VAL[3] = (i &   4) ? 'F' : 'f';
                      STRING_VAL[4] = (i &   8) ? 'I' : 'i';
                      STRING_VAL[5] = (i &  16) ? 'N' : 'n';
                      STRING_VAL[6] = (i &  32) ? 'I' : 'i';
                      STRING_VAL[7] = (i &  64) ? 'T' : 't';
                      STRING_VAL[8] = (i & 128) ? 'Y' : 'y';
                      STRING_VAL[9] = 0;
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      Util::ValueType64 EXPECTED = DecConsts<64>::pInf();
                      LOOP_ASSERT(STRING_VAL, ACTUAL == EXPECTED);
                  }

                  // Test 256 capitalizations of "-infinity".
                  for (int i = 0; i < 256; i++) {
                      char STRING_VAL[10];
                      STRING_VAL[0] = '-';
                      STRING_VAL[1] = (i &   1) ? 'I' : 'i';
                      STRING_VAL[2] = (i &   2) ? 'N' : 'n';
                      STRING_VAL[3] = (i &   4) ? 'F' : 'f';
                      STRING_VAL[4] = (i &   8) ? 'I' : 'i';
                      STRING_VAL[5] = (i &  16) ? 'N' : 'n';
                      STRING_VAL[6] = (i &  32) ? 'I' : 'i';
                      STRING_VAL[7] = (i &  64) ? 'T' : 't';
                      STRING_VAL[8] = (i & 128) ? 'Y' : 'y';
                      STRING_VAL[9] = 0;
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      Util::ValueType64 EXPECTED = DecConsts<64>::nInf();
                      LOOP_ASSERT(STRING_VAL, ACTUAL == EXPECTED);
                  }
              }
              if (veryVerbose) bsl::cout << "\nTesting NaN cases."
                                      << bsl::endl;
              {
                  // All of these string should parse to sNan or qNan.
                  static const struct {
                      int         d_lineNum;
                      const char *d_stringVal;
                      // true for sNan, false for qNan.
                  } PARSE64_NAN_TEST_DATA[] = {
                      { L_, "nan" },
                      { L_, "naN" },
                      { L_, "nAn" },
                      { L_, "nAN" },
                      { L_, "Nan" },
                      { L_, "NaN" },
                      { L_, "NAn" },
                      { L_, "NAN" },
                  };

                  const int NUM_PARSE64_NAN_TESTS =
                      sizeof PARSE64_NAN_TEST_DATA /
                                                 sizeof *PARSE64_NAN_TEST_DATA;

                  for (int ti = 0; ti < NUM_PARSE64_NAN_TESTS; ++ti) {
                      const int LINE = PARSE64_NAN_TEST_DATA[ti].d_lineNum;
                      const char *STRING_VAL =
                                         PARSE64_NAN_TEST_DATA[ti].d_stringVal;
                      Util::ValueType64 ACTUAL = Util::parse64(STRING_VAL);
                      Util::ValueType64 EXPECTED_QNAN = DecConsts<64>::qNan();
                      Util::ValueType64 EXPECTED_SNAN = DecConsts<64>::sNan();

                      LOOP_ASSERT(LINE,
                                  !memcmp(&ACTUAL, &EXPECTED_QNAN,
                                          sizeof(Util::ValueType64)) ||
                                  !memcmp(&ACTUAL, &EXPECTED_SNAN,
                                          sizeof(Util::ValueType64)));
                  }
              }
          }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING Equality comparison
        //
        // Concerns:
        //: 1 'equals' passes its arguments to the decNumber library.
        //: 2 'equals' correctly compares numbers in which the quanta differ.
        //
        // Plan:
        //: 1 Test the equality comparison of numbers generated by an array of
        //:   mantissa and exponents.
        //:
        //: 2 Use the testing apparatus, in particular 'normalize', to
        //:   determine if the correct equality result is returned.
        //
        // Testing:
        //  equals(ValueType32  lhs, ValueType32  rhs)
        //  equals(ValueType32  lhs, ValueType64  rhs)
        //  equals(ValueType32  lhs, ValueType128 rhs)
        //  equals(ValueType64  lhs, ValueType32  rhs)
        //  equals(ValueType64  lhs, ValueType64  rhs)
        //  equals(ValueType64  lhs, ValueType128 rhs)
        //  equals(ValueType128 lhs, ValueType32  rhs)
        //  equals(ValueType128 lhs, ValueType64  rhs)
        //  equals(ValueType128 lhs, ValueType128 rhs)
        // --------------------------------------------------------------------
          {
              if (verbose) bsl::cout
                                 << bsl::endl
                                 << "TESTING EQUALITY COMPARISON" << bsl::endl
                                 << "===========================" << bsl::endl;


              for (int m1i = 0; m1i < NUM_TEST_NONZERO_MANTISSAS; ++m1i) {
                  for (int e1i = 0; e1i < NUM_TEST_EXPONENTS; ++e1i) {
                      for (int m2i = 0; m2i < NUM_TEST_NONZERO_MANTISSAS;
                                                                       ++m2i) {
                          for (int e2i = 0; e2i < NUM_TEST_EXPONENTS; ++e2i) {


                              const long long mantissa1 =
                                                   TEST_NONZERO_MANTISSAS[m1i];
                              const int exponent1 = TEST_EXPONENTS[e1i];
                              const long long mantissa2 =
                                                   TEST_NONZERO_MANTISSAS[m2i];
                              const int exponent2 = TEST_EXPONENTS[e2i];

                              const Util::ValueType128 FIRST128 =
                                 Util::makeDecimalRaw128(mantissa1, exponent1);
                              const Util::ValueType128 SECOND128 =
                                 Util::makeDecimalRaw128(mantissa2, exponent2);

                              const bool EXPECTED = decimalNormalizedCompare(
                                   mantissa1, exponent1, mantissa2, exponent2);

                              LOOP2_ASSERT(FIRST128, SECOND128,
                                           Util::equals(FIRST128, SECOND128) ==
                                                                     EXPECTED);

                              const bool num1Fits64 =
                                  (-9999999999999999LL <= mantissa1) &&
                                  (mantissa1 <= 9999999999999999LL) &&
                                  (-398 <= exponent1) &&
                                  (exponent1 <= 369);

                              const bool num2Fits64 =
                                  (-9999999999999999LL <= mantissa2) &&
                                  (mantissa2 <= 9999999999999999LL) &&
                                  (-398 <= exponent2) &&
                                  (exponent2 <= 369);

                              const bool num1Fits32 =
                                  (-9999999LL <= mantissa1) &&
                                  (mantissa1 <= 9999999LL) &&
                                  (-101 <= exponent1) &&
                                  (exponent1 <= 90);

                              const bool num2Fits32 =
                                  (-9999999LL <= mantissa2) &&
                                  (mantissa2 <= 9999999LL) &&
                                  (-101 <= exponent2) &&
                                  (exponent2 <= 90);

                              if (num1Fits64) {
                                  const Util::ValueType64 FIRST64 =
                                      Util::makeDecimalRaw64(mantissa1,
                                                                    exponent1);
                                  LOOP2_ASSERT(FIRST64, SECOND128,
                                      Util::equals(FIRST64, SECOND128) ==
                                                                     EXPECTED);

                                  if (num2Fits64) {
                                      const Util::ValueType64 SECOND64 =
                                          Util::makeDecimalRaw64(mantissa2,
                                                                    exponent2);
                                      LOOP2_ASSERT(FIRST64, SECOND64,
                                          Util::equals(FIRST64, SECOND64) ==
                                                                     EXPECTED);

                                  }
                                  if (num2Fits32) {
                                      int tMantissa2 =
                                                 static_cast<int>(mantissa2);
                                      ASSERT(tMantissa2 == mantissa2);

                                      const Util::ValueType32 SECOND32 =
                                          Util::makeDecimalRaw32(tMantissa2,
                                                                    exponent2);
                                      LOOP2_ASSERT(FIRST64, SECOND32,
                                          Util::equals(FIRST64, SECOND32) ==
                                                                     EXPECTED);

                                  }
                              }

                              if (num1Fits32) {
                                  int tMantissa1 = static_cast<int>(mantissa1);
                                  ASSERT(tMantissa1 == mantissa1);

                                  const Util::ValueType32 FIRST32 =
                                Util::makeDecimalRaw32(tMantissa1, exponent1);
                                  LOOP2_ASSERT(FIRST32, SECOND128,
                                      Util::equals(FIRST32, SECOND128) ==
                                                                     EXPECTED);

                                  if (num2Fits64) {
                                      const Util::ValueType64 SECOND64 =
                                          Util::makeDecimalRaw64(mantissa2,
                                                                    exponent2);
                                      LOOP2_ASSERT(FIRST32, SECOND64,
                                          Util::equals(FIRST32, SECOND64) ==
                                                                     EXPECTED);

                                  }
                                  if (num2Fits32) {
                                      int tMantissa2 =
                                                 static_cast<int>(mantissa2);
                                      ASSERT(tMantissa2 == mantissa2);

                                      const Util::ValueType32 SECOND32 =
                                          Util::makeDecimalRaw32(tMantissa2,
                                                                    exponent2);
                                      LOOP2_ASSERT(FIRST32, SECOND32,
                                          Util::equals(FIRST32, SECOND32) ==
                                                                     EXPECTED);

                                  }
                              }
                          }
                      }
                  }
              }


              // None of the (mantissa, exponent) pairs should compare equal
              // to 0's, Inf's, or Nan's.
              for (int mi = 0; mi < NUM_TEST_NONZERO_MANTISSAS; ++mi) {
                  for (int ei = 0; ei < NUM_TEST_EXPONENTS; ++ei) {

                      const long long mantissa = TEST_NONZERO_MANTISSAS[mi];
                      const int exponent = TEST_EXPONENTS[ei];

                      const Util::ValueType128 TEST =
                          Util::makeDecimalRaw128(mantissa, exponent);

                      const bool fits64 =
                          (-9999999999999999LL <= mantissa) &&
                          (mantissa <= 9999999999999999LL) &&
                          (-398 <= exponent) &&
                          (exponent <= 369);

                      const bool fits32 =
                          (-9999999LL <= mantissa) &&
                          (mantissa <= 9999999LL) &&
                          (-101 <= exponent) &&
                          (exponent <= 90);

                      LOOP_ASSERT(TEST, !Util::equals(
                                               TEST, DecConsts<128>::pZero()));
                      LOOP_ASSERT(TEST, !Util::equals(
                                               TEST, DecConsts<128>::nZero()));
                      LOOP_ASSERT(TEST, !Util::equals(
                                               TEST,  DecConsts<128>::pInf()));
                      LOOP_ASSERT(TEST, !Util::equals(
                                               TEST,  DecConsts<128>::nInf()));
                      LOOP_ASSERT(TEST, !Util::equals(
                                               TEST,  DecConsts<128>::sNan()));
                      LOOP_ASSERT(TEST, !Util::equals(
                                               TEST,  DecConsts<128>::qNan()));

                      LOOP_ASSERT(TEST, !Util::equals(
                                      DecConsts<128>::pZero(), TEST));
                      LOOP_ASSERT(TEST, !Util::equals(
                                      DecConsts<128>::nZero(), TEST));
                      LOOP_ASSERT(TEST, !Util::equals(
                                      DecConsts<128>::pInf(),  TEST));
                      LOOP_ASSERT(TEST, !Util::equals(
                                      DecConsts<128>::nInf(),  TEST));
                      LOOP_ASSERT(TEST, !Util::equals(
                                      DecConsts<128>::sNan(),  TEST));
                      LOOP_ASSERT(TEST, !Util::equals(
                                      DecConsts<128>::qNan(),  TEST));

                      if (fits64) {
                          const Util::ValueType64 TEST64 =
                                    Util::makeDecimalRaw64(mantissa, exponent);
                          LOOP_ASSERT(TEST64, !Util::equals(TEST64,
                                                      DecConsts<64>::pZero()));
                          LOOP_ASSERT(TEST64, !Util::equals(TEST64,
                                                      DecConsts<64>::nZero()));
                          LOOP_ASSERT(TEST64, !Util::equals(TEST64,
                                                       DecConsts<64>::pInf()));
                          LOOP_ASSERT(TEST64, !Util::equals(TEST64,
                                                       DecConsts<64>::nInf()));
                          LOOP_ASSERT(TEST64, !Util::equals(TEST64,
                                                       DecConsts<64>::sNan()));
                          LOOP_ASSERT(TEST64, !Util::equals(TEST64,
                                                       DecConsts<64>::qNan()));

                          LOOP_ASSERT(TEST64, !Util::equals(
                                              DecConsts<64>::pZero(), TEST64));
                          LOOP_ASSERT(TEST64, !Util::equals(
                                              DecConsts<64>::nZero(), TEST64));
                          LOOP_ASSERT(TEST64, !Util::equals(
                                              DecConsts<64>::pInf(),  TEST64));
                          LOOP_ASSERT(TEST64, !Util::equals(
                                              DecConsts<64>::nInf(),  TEST64));
                          LOOP_ASSERT(TEST64, !Util::equals(
                                              DecConsts<64>::sNan(),  TEST64));
                          LOOP_ASSERT(TEST64, !Util::equals(
                                              DecConsts<64>::qNan(),  TEST64));
                      }

                      if (fits32) {
                          int tMantissa = static_cast<int>(mantissa);
                          ASSERT(tMantissa == mantissa);

                          const Util::ValueType32 TEST32 =
                                   Util::makeDecimalRaw32(tMantissa, exponent);

                          LOOP_ASSERT(TEST32, !Util::equals(
                                              DecConsts<32>::pZero(), TEST32));
                          LOOP_ASSERT(TEST32, !Util::equals(
                                              DecConsts<32>::nZero(), TEST32));
                          LOOP_ASSERT(TEST32, !Util::equals(
                                              DecConsts<32>::pInf(),  TEST32));
                          LOOP_ASSERT(TEST32, !Util::equals(
                                              DecConsts<32>::nInf(),  TEST32));
                          LOOP_ASSERT(TEST32, !Util::equals(
                                              DecConsts<32>::sNan(),  TEST32));
                          LOOP_ASSERT(TEST32, !Util::equals(
                                              DecConsts<32>::qNan(),  TEST32));

                          LOOP_ASSERT(TEST32, !Util::equals(TEST32,
                                                      DecConsts<32>::pZero()));
                          LOOP_ASSERT(TEST32, !Util::equals(TEST32,
                                                      DecConsts<32>::nZero()));
                          LOOP_ASSERT(TEST32, !Util::equals(TEST32,
                                                       DecConsts<32>::pInf()));
                          LOOP_ASSERT(TEST32, !Util::equals(TEST32,
                                                       DecConsts<32>::nInf()));
                          LOOP_ASSERT(TEST32, !Util::equals(TEST32,
                                                       DecConsts<32>::sNan()));
                          LOOP_ASSERT(TEST32, !Util::equals(TEST32,
                                                       DecConsts<32>::qNan()));
                      }
                  }
              }

              // Test comparisons between zero and infinite values and Nan
              // constants.
              ASSERT(Util::equals(DecConsts<128>::pZero(),
                                                     DecConsts<128>::pZero()));
              ASSERT(Util::equals(DecConsts<128>::pZero(),
                                                     DecConsts<128>::nZero()));
              ASSERT(!Util::equals(DecConsts<128>::pZero(),
                                                      DecConsts<128>::pInf()));
              ASSERT(!Util::equals(DecConsts<128>::pZero(),
                                                      DecConsts<128>::nInf()));
              ASSERT(!Util::equals(DecConsts<128>::pZero(),
                                                      DecConsts<128>::sNan()));
              ASSERT(!Util::equals(DecConsts<128>::pZero(),
                                                      DecConsts<128>::qNan()));

              ASSERT(Util::equals(DecConsts<128>::nZero(),
                                                     DecConsts<128>::pZero()));
              ASSERT(Util::equals(DecConsts<128>::nZero(),
                                                     DecConsts<128>::nZero()));
              ASSERT(!Util::equals(DecConsts<128>::nZero(),
                                                      DecConsts<128>::pInf()));
              ASSERT(!Util::equals(DecConsts<128>::nZero(),
                                                      DecConsts<128>::nInf()));
              ASSERT(!Util::equals(DecConsts<128>::nZero(),
                                                      DecConsts<128>::sNan()));
              ASSERT(!Util::equals(DecConsts<128>::nZero(),
                                                      DecConsts<128>::qNan()));

              ASSERT(!Util::equals(DecConsts<128>::pInf(),
                                                     DecConsts<128>::pZero()));
              ASSERT(!Util::equals(DecConsts<128>::pInf(),
                                                     DecConsts<128>::nZero()));
              ASSERT(Util::equals(DecConsts<128>::pInf(),
                                                      DecConsts<128>::pInf()));
              ASSERT(!Util::equals(DecConsts<128>::pInf(),
                                                      DecConsts<128>::nInf()));
              ASSERT(!Util::equals(DecConsts<128>::pInf(),
                                                      DecConsts<128>::sNan()));
              ASSERT(!Util::equals(DecConsts<128>::pInf(),
                                                      DecConsts<128>::qNan()));

              ASSERT(!Util::equals(DecConsts<128>::nInf(),
                                                     DecConsts<128>::pZero()));
              ASSERT(!Util::equals(DecConsts<128>::nInf(),
                                                     DecConsts<128>::nZero()));
              ASSERT(!Util::equals(DecConsts<128>::nInf(),
                                                      DecConsts<128>::pInf()));
              ASSERT(Util::equals(DecConsts<128>::nInf(),
                                                      DecConsts<128>::nInf()));
              ASSERT(!Util::equals(DecConsts<128>::nInf(),
                                                      DecConsts<128>::sNan()));
              ASSERT(!Util::equals(DecConsts<128>::nInf(),
                                                      DecConsts<128>::qNan()));

              ASSERT(!Util::equals(DecConsts<128>::sNan(),
                                                     DecConsts<128>::pZero()));
              ASSERT(!Util::equals(DecConsts<128>::sNan(),
                                                     DecConsts<128>::nZero()));
              ASSERT(!Util::equals(DecConsts<128>::sNan(),
                                                      DecConsts<128>::pInf()));
              ASSERT(!Util::equals(DecConsts<128>::sNan(),
                                                      DecConsts<128>::nInf()));
              ASSERT(!Util::equals(DecConsts<128>::sNan(),
                                                      DecConsts<128>::sNan()));
              ASSERT(!Util::equals(DecConsts<128>::sNan(),
                                                      DecConsts<128>::qNan()));

              ASSERT(!Util::equals(DecConsts<128>::qNan(),
                                                     DecConsts<128>::pZero()));
              ASSERT(!Util::equals(DecConsts<128>::qNan(),
                                                     DecConsts<128>::nZero()));
              ASSERT(!Util::equals(DecConsts<128>::qNan(),
                                                      DecConsts<128>::pInf()));
              ASSERT(!Util::equals(DecConsts<128>::qNan(),
                                                      DecConsts<128>::nInf()));
              ASSERT(!Util::equals(DecConsts<128>::qNan(),
                                                      DecConsts<128>::sNan()));
              ASSERT(!Util::equals(DecConsts<128>::qNan(),
                                                      DecConsts<128>::qNan()));

              ASSERT(Util::equals(DecConsts<64>::pZero(),
                                                      DecConsts<64>::pZero()));
              ASSERT(Util::equals(DecConsts<64>::pZero(),
                                                      DecConsts<64>::nZero()));
              ASSERT(!Util::equals(DecConsts<64>::pZero(),
                                                       DecConsts<64>::pInf()));
              ASSERT(!Util::equals(DecConsts<64>::pZero(),
                                                       DecConsts<64>::nInf()));
              ASSERT(!Util::equals(DecConsts<64>::pZero(),
                                                       DecConsts<64>::sNan()));
              ASSERT(!Util::equals(DecConsts<64>::pZero(),
                                                       DecConsts<64>::qNan()));

              ASSERT(Util::equals(DecConsts<64>::nZero(),
                                                      DecConsts<64>::pZero()));
              ASSERT(Util::equals(DecConsts<64>::nZero(),
                                                      DecConsts<64>::nZero()));
              ASSERT(!Util::equals(DecConsts<64>::nZero(),
                                                       DecConsts<64>::pInf()));
              ASSERT(!Util::equals(DecConsts<64>::nZero(),
                                                       DecConsts<64>::nInf()));
              ASSERT(!Util::equals(DecConsts<64>::nZero(),
                                                       DecConsts<64>::sNan()));
              ASSERT(!Util::equals(DecConsts<64>::nZero(),
                                                       DecConsts<64>::qNan()));

              ASSERT(!Util::equals(DecConsts<64>::pInf(),
                                                      DecConsts<64>::pZero()));
              ASSERT(!Util::equals(DecConsts<64>::pInf(),
                                                      DecConsts<64>::nZero()));
              ASSERT(Util::equals(DecConsts<64>::pInf(),
                                                       DecConsts<64>::pInf()));
              ASSERT(!Util::equals(DecConsts<64>::pInf(),
                                                       DecConsts<64>::nInf()));
              ASSERT(!Util::equals(DecConsts<64>::pInf(),
                                                       DecConsts<64>::sNan()));
              ASSERT(!Util::equals(DecConsts<64>::pInf(),
                                                       DecConsts<64>::qNan()));

              ASSERT(!Util::equals(DecConsts<64>::nInf(),
                                                      DecConsts<64>::pZero()));
              ASSERT(!Util::equals(DecConsts<64>::nInf(),
                                                      DecConsts<64>::nZero()));
              ASSERT(!Util::equals(DecConsts<64>::nInf(),
                                                       DecConsts<64>::pInf()));
              ASSERT(Util::equals(DecConsts<64>::nInf(),
                                                       DecConsts<64>::nInf()));
              ASSERT(!Util::equals(DecConsts<64>::nInf(),
                                                       DecConsts<64>::sNan()));
              ASSERT(!Util::equals(DecConsts<64>::nInf(),
                                                       DecConsts<64>::qNan()));

              ASSERT(!Util::equals(DecConsts<64>::sNan(),
                                                      DecConsts<64>::pZero()));
              ASSERT(!Util::equals(DecConsts<64>::sNan(),
                                                      DecConsts<64>::nZero()));
              ASSERT(!Util::equals(DecConsts<64>::sNan(),
                                                       DecConsts<64>::pInf()));
              ASSERT(!Util::equals(DecConsts<64>::sNan(),
                                                       DecConsts<64>::nInf()));
              ASSERT(!Util::equals(DecConsts<64>::sNan(),
                                                       DecConsts<64>::sNan()));
              ASSERT(!Util::equals(DecConsts<64>::sNan(),
                                                       DecConsts<64>::qNan()));

              ASSERT(!Util::equals(DecConsts<64>::qNan(),
                                                      DecConsts<64>::pZero()));
              ASSERT(!Util::equals(DecConsts<64>::qNan(),
                                                      DecConsts<64>::nZero()));
              ASSERT(!Util::equals(DecConsts<64>::qNan(),
                                                       DecConsts<64>::pInf()));
              ASSERT(!Util::equals(DecConsts<64>::qNan(),
                                                       DecConsts<64>::nInf()));
              ASSERT(!Util::equals(DecConsts<64>::qNan(),
                                                       DecConsts<64>::sNan()));
              ASSERT(!Util::equals(DecConsts<64>::qNan(),
                                                       DecConsts<64>::qNan()));

              ASSERT(Util::equals(DecConsts<32>::pZero(),
                                                      DecConsts<32>::pZero()));
              ASSERT(Util::equals(DecConsts<32>::pZero(),
                                                      DecConsts<32>::nZero()));
              ASSERT(!Util::equals(DecConsts<32>::pZero(),
                                                      DecConsts<32>::pInf()));
              ASSERT(!Util::equals(DecConsts<32>::pZero(),
                                                      DecConsts<32>::nInf()));
              ASSERT(!Util::equals(DecConsts<32>::pZero(),
                                                      DecConsts<32>::sNan()));
              ASSERT(!Util::equals(DecConsts<32>::pZero(),
                                                      DecConsts<32>::qNan()));

              ASSERT(Util::equals(DecConsts<32>::nZero(),
                                                      DecConsts<32>::pZero()));
              ASSERT(Util::equals(DecConsts<32>::nZero(),
                                                      DecConsts<32>::nZero()));
              ASSERT(!Util::equals(DecConsts<32>::nZero(),
                                                       DecConsts<32>::pInf()));
              ASSERT(!Util::equals(DecConsts<32>::nZero(),
                                                       DecConsts<32>::nInf()));
              ASSERT(!Util::equals(DecConsts<32>::nZero(),
                                                       DecConsts<32>::sNan()));
              ASSERT(!Util::equals(DecConsts<32>::nZero(),
                                                       DecConsts<32>::qNan()));

              ASSERT(!Util::equals(DecConsts<32>::pInf(),
                                                      DecConsts<32>::pZero()));
              ASSERT(!Util::equals(DecConsts<32>::pInf(),
                                                      DecConsts<32>::nZero()));
              ASSERT(Util::equals(DecConsts<32>::pInf(),
                                                       DecConsts<32>::pInf()));
              ASSERT(!Util::equals(DecConsts<32>::pInf(),
                                                       DecConsts<32>::nInf()));
              ASSERT(!Util::equals(DecConsts<32>::pInf(),
                                                       DecConsts<32>::sNan()));
              ASSERT(!Util::equals(DecConsts<32>::pInf(),
                                                       DecConsts<32>::qNan()));

              ASSERT(!Util::equals(DecConsts<32>::nInf(),
                                                      DecConsts<32>::pZero()));
              ASSERT(!Util::equals(DecConsts<32>::nInf(),
                                                      DecConsts<32>::nZero()));
              ASSERT(!Util::equals(DecConsts<32>::nInf(),
                                                       DecConsts<32>::pInf()));
              ASSERT(Util::equals(DecConsts<32>::nInf(),
                                                       DecConsts<32>::nInf()));
              ASSERT(!Util::equals(DecConsts<32>::nInf(),
                                                       DecConsts<32>::sNan()));
              ASSERT(!Util::equals(DecConsts<32>::nInf(),
                                                       DecConsts<32>::qNan()));

              ASSERT(!Util::equals(DecConsts<32>::sNan(),
                                                      DecConsts<32>::pZero()));
              ASSERT(!Util::equals(DecConsts<32>::sNan(),
                                                      DecConsts<32>::nZero()));
              ASSERT(!Util::equals(DecConsts<32>::sNan(),
                                                       DecConsts<32>::pInf()));
              ASSERT(!Util::equals(DecConsts<32>::sNan(),
                                                       DecConsts<32>::nInf()));
              ASSERT(!Util::equals(DecConsts<32>::sNan(),
                                                       DecConsts<32>::sNan()));
              ASSERT(!Util::equals(DecConsts<32>::sNan(),
                                                       DecConsts<32>::qNan()));

              ASSERT(!Util::equals(DecConsts<32>::qNan(),
                                                      DecConsts<32>::pZero()));
              ASSERT(!Util::equals(DecConsts<32>::qNan(),
                                                      DecConsts<32>::nZero()));
              ASSERT(!Util::equals(DecConsts<32>::qNan(),
                                                       DecConsts<32>::pInf()));
              ASSERT(!Util::equals(DecConsts<32>::qNan(),
                                                       DecConsts<32>::nInf()));
              ASSERT(!Util::equals(DecConsts<32>::qNan(),
                                                       DecConsts<32>::sNan()));
              ASSERT(!Util::equals(DecConsts<32>::qNan(),
                                                       DecConsts<32>::qNan()));
          }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing:
        //   Simple test apparatus
        //
        // Concerns:
        //: 1 The normalization of mantissa, exponent pairs, such that the
        //:   value of 'mantissa * pow(10, exponent)' is preserved and 10 does
        //:   not divide the adjusted 'mantissa'.
        //
        // Plan:
        //: 1 Test the normalizing of numbers of each length in base 10 and
        //:   with powers of ten of each size.
        //
        // Testing:
        //   normalize(long long *mantissa,            int *exponent)
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                               << "TESTING SIMPLE TEST APPARATUS" << bsl::endl
                               << "=============================" << bsl::endl;

        if (veryVerbose) bsl::cout << "\nTesting helper parsing function for "
                                   "comparing mantissa/exponent pairs."
                                << bsl::endl;
        {
            static const struct {
                int       d_lineNum;
                long long d_mantissa;
                int       d_exponent;
                long long d_expectedMantissa;
                int       d_expectedExponent;
            } NORMALIZE_TEST_DATA[] = {
                {L_,                   1LL,  -10,                   1LL,  -10},
                {L_,                   1LL,    0,                   1LL,    0},
                {L_,                   1LL,   10,                   1LL,   10},
                {L_,  100000000000000001LL,  -10,  100000000000000001LL,  -10},
                {L_,  100000000000000001LL,    0,  100000000000000001LL,    0},
                {L_,  100000000000000001LL,   10,  100000000000000001LL,   10},
                {L_,                  -1LL,  -10,                  -1LL,  -10},
                {L_,                  -1LL,    0,                  -1LL,    0},
                {L_,                  -1LL,   10,                  -1LL,   10},
                {L_, -100000000000000001LL,  -10, -100000000000000001LL,  -10},
                {L_, -100000000000000001LL,    0, -100000000000000001LL,    0},
                {L_, -100000000000000001LL,   10, -100000000000000001LL,   10},

                {L_,                  10LL,  100,                   1LL,  101},
                {L_,                1200LL,  105,                  12LL,  107},
                {L_,              123000LL,  110,                 123LL,  113},
                {L_,            12340000LL,  115,                1234LL,  119},
                {L_,          1234500000LL,  120,               12345LL,  125},
                {L_,             5000000LL,  125,                   5LL,  131},
                {L_,           540000000LL,  130,                  54LL,  137},
                {L_,         54300000000LL,  135,                 543LL,  143},
                {L_,       5432000000000LL,  140,                5432LL,  149},
                {L_,     543210000000000LL,  145,               54321LL,  155},
                {L_,     987600000000000LL,  150,                9876LL,  161},
                {L_,     876000000000000LL,  155,                 876LL,  167},
                {L_,     760000000000000LL,  160,                  76LL,  173},
                {L_,     600000000000000LL,  165,                   6LL,  179},
                {L_,    6000000000000000LL,  170,                   6LL,  185},
                {L_,   50000000000000000LL,  175,                   5LL,  191},
                {L_,  400000000000000000LL,  180,                   4LL,  197},
                {L_, 3000000000000000000LL,  185,                   3LL,  203},

                {L_,                  10LL, -100,                   1LL,  -99},
                {L_,                1200LL, -105,                  12LL, -103},
                {L_,              123000LL, -110,                 123LL, -107},
                {L_,            12340000LL, -115,                1234LL, -111},
                {L_,          1234500000LL, -120,               12345LL, -115},
                {L_,             5000000LL, -125,                   5LL, -119},
                {L_,           540000000LL, -130,                  54LL, -123},
                {L_,         54300000000LL, -135,                 543LL, -127},
                {L_,       5432000000000LL, -140,                5432LL, -131},
                {L_,     543210000000000LL, -145,               54321LL, -135},
                {L_,     987600000000000LL, -150,                9876LL, -139},
                {L_,     876000000000000LL, -155,                 876LL, -143},
                {L_,     760000000000000LL, -160,                  76LL, -147},
                {L_,     600000000000000LL, -165,                   6LL, -151},
                {L_,    6000000000000000LL, -170,                   6LL, -155},
                {L_,   50000000000000000LL, -175,                   5LL, -159},
                {L_,  400000000000000000LL, -180,                   4LL, -163},
                {L_, 3000000000000000000LL, -185,                   3LL, -167},

                {L_,                 -10LL,  100,                  -1LL,  101},
                {L_,               -1200LL,  105,                 -12LL,  107},
                {L_,             -123000LL,  110,                -123LL,  113},
                {L_,           -12340000LL,  115,               -1234LL,  119},
                {L_,         -1234500000LL,  120,              -12345LL,  125},
                {L_,            -5000000LL,  125,                  -5LL,  131},
                {L_,          -540000000LL,  130,                 -54LL,  137},
                {L_,        -54300000000LL,  135,                -543LL,  143},
                {L_,      -5432000000000LL,  140,               -5432LL,  149},
                {L_,    -543210000000000LL,  145,              -54321LL,  155},
                {L_,    -987600000000000LL,  150,               -9876LL,  161},
                {L_,    -876000000000000LL,  155,                -876LL,  167},
                {L_,    -760000000000000LL,  160,                 -76LL,  173},
                {L_,    -600000000000000LL,  165,                  -6LL,  179},
                {L_,   -6000000000000000LL,  170,                  -6LL,  185},
                {L_,  -50000000000000000LL,  175,                  -5LL,  191},
                {L_, -400000000000000000LL,  180,                  -4LL,  197},
                {L_,-3000000000000000000LL,  185,                  -3LL,  203},

                {L_,                 -10LL, -100,                  -1LL,  -99},
                {L_,               -1200LL, -105,                 -12LL, -103},
                {L_,             -123000LL, -110,                -123LL, -107},
                {L_,           -12340000LL, -115,               -1234LL, -111},
                {L_,         -1234500000LL, -120,              -12345LL, -115},
                {L_,            -5000000LL, -125,                  -5LL, -119},
                {L_,          -540000000LL, -130,                 -54LL, -123},
                {L_,        -54300000000LL, -135,                -543LL, -127},
                {L_,      -5432000000000LL, -140,               -5432LL, -131},
                {L_,    -543210000000000LL, -145,              -54321LL, -135},
                {L_,    -987600000000000LL, -150,               -9876LL, -139},
                {L_,    -876000000000000LL, -155,                -876LL, -143},
                {L_,    -760000000000000LL, -160,                 -76LL, -147},
                {L_,    -600000000000000LL, -165,                  -6LL, -151},
                {L_,   -6000000000000000LL, -170,                  -6LL, -155},
                {L_,  -50000000000000000LL, -175,                  -5LL, -159},
                {L_, -400000000000000000LL, -180,                  -4LL, -163},
                {L_,-3000000000000000000LL, -185,                  -3LL, -167},
            };
            const int NUM_NORMALIZE_TEST_DATA =
                sizeof NORMALIZE_TEST_DATA / sizeof *NORMALIZE_TEST_DATA;

            for (int ti = 0; ti < NUM_NORMALIZE_TEST_DATA; ++ti) {
                const int LINE = NORMALIZE_TEST_DATA[ti].d_lineNum;

                const long long MANTISSA = NORMALIZE_TEST_DATA[ti].d_mantissa;
                const int EXPONENT = NORMALIZE_TEST_DATA[ti].d_exponent;
                const long long EXPECTED_MANTISSA
                                  = NORMALIZE_TEST_DATA[ti].d_expectedMantissa;
                const int EXPECTED_EXPONENT
                                  = NORMALIZE_TEST_DATA[ti].d_expectedExponent;

                long long ACTUAL_NORMALIZED_MANTISSA = MANTISSA;
                int ACTUAL_NORMALIZED_EXPONENT = EXPONENT;

                normalize(&ACTUAL_NORMALIZED_MANTISSA,
                                                  &ACTUAL_NORMALIZED_EXPONENT);
                LOOP_ASSERT(LINE,
                              EXPECTED_MANTISSA == ACTUAL_NORMALIZED_MANTISSA);
                LOOP_ASSERT(LINE,
                              EXPECTED_EXPONENT == ACTUAL_NORMALIZED_EXPONENT);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING Primary manipulators
        //
        // Concerns:
        //: 1 'makeDecimalRawXX' constructs a decimal floating point value
        //:   having the specified mantissa and exponent, with no rounding.
        //:
        //: 2 The correctness of the conversion of each 3-digit group (or
        //:   declet) into a 10-bit binary-encoded declet.
        //:
        //: 3 The conversion of each 3-digit declet into a 10-bit
        //:   binary-encoded declet does not intefere each other.
        //:
        //: 4 The combination field is correctly computed given the leading
        //:   digit of the mantissa and the exponent.
        //:
        //: 5 'makeDecimalRawXX' asserts in the right build modes for undefined
        //:   behavior.
        //
        // Plan:
        //: 1 Test 'makeDecimalRawXX' contract using
        //:   'AssertFailureHandlerGuard' and 'BSLS_ASSERTTEST_ASSERT_OPT_FAIL'
        //:
        //: 2 Test each overload of each 'makeDecimalRawXX' by representing
        //:   the mantissa in a string format and parsing in each intergral
        //:   type.
        //:
        //: 3 Test a mantissa of each length in base 10.
        //:
        //: 4 Test that 'makeDecimalRawXX' returns the same value as the
        //:   decimal created by invoking the decNumber constructors using
        //:   binary-coded decimals.
        //
        // Testing:
        //   makeDecimalRaw32 (int mantissa,                int exponent);
        //   makeDecimalRaw64 (long long mantissa,          int exponent);
        //   makeDecimalRaw64 (unsigned long long mantissa, int exponent);
        //   makeDecimalRaw64 (int mantissa,                int exponent);
        //   makeDecimalRaw64 (unsigned int mantissa,       int exponent);
        //   makeDecimalRaw128(long long mantissa,          int exponent);
        //   makeDecimalRaw128(unsigned long long mantissa, int exponent);
        //   makeDecimalRaw128(int mantissa,                int exponent);
        //   makeDecimalRaw128(unsigned int mantissa,       int exponent);
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                                << "PRIMARY MANIPULATORS"
                                << "====================" << bsl::endl;

        // Test that 'makeDecimalRaw32' enforces undefined behavior in the
        // right build mode
        {
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(42, -102));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(42, -101));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(42, 90));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(42, 91));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(-10000000,
                                                                          42));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(- 9999999,
                                                                          42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32( 9999999,
                                                                          42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(10000000,
                                                                          42));
        }

        // Test that 'makeDecimalRaw64' variants enforce undefined behavior in
        // the right build mode
        {
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Test 'makeDecimalRaw64(int, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42, -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42, -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42, 369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42, 370));

            // 'makeDecimalRaw64(int, int)' needs no test for mantissa bounds
            // because 9,999,999,999,999,999 is unrepresentable as an int.

            // Test 'makeDecimalRaw64(unsigned int, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42u, -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42u, -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42u, 369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42u, 370));

            // 'makeDecimalRaw64(unsigned int, int)' needs no test for mantissa
            // bounds because 9,999,999,999,999,999 is unrepresentable as an
            // int.

            // Test 'makeDecimalRaw64(long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ll,
                                                                        -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ll,
                                                                        -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ll, 369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ll, 370));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(
                                                    -10000000000000000ll, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(
                                                    - 9999999999999999ll, 42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(
                                                      9999999999999999ll, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(
                                                     10000000000000000ll, 42));

            // Test 'makeDecimalRaw64(unsigned long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ull,
                                                                        -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ull,
                                                                        -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ull,
                                                                         369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ull,
                                                                         370));

            // Unsigned numbers cannot be negative, so we do not have a lower
            // bound test.

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(
                                                     9999999999999999ull, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(
                                                    10000000000000000ull, 42));
        }

        // Test that 'makeDecimalRaw128' variants enforce undefined behavior in
        // the right build mode
        {
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Test 'makeDecimalRaw128(int, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42,
                                                                       -6176));
            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42, 6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42, 6112));

            // Test 'makeDecimalRaw128(unsigned, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42u,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42u,
                                                                       -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42u,
                                                                        6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42u,
                                                                        6112));

            // Test 'makeDecimalRaw128(long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ll,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ll,
                                                                       -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ll,
                                                                        6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ll,
                                                                        6112));

            // Test 'makeDecimalRaw128(unsigned long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ull,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ull,
                                                                       -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ull,
                                                                        6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ull,
                                                                        6112));
        }


        if (veryVerbose) bsl::cout << "Test the correctness of the declets "
                                   "table in this implementation by parsing "
                                   "each number between 0 and 999, "
                                   "inclusive." << bsl::endl;
        {
            for (int NUM = 0; NUM < 1000; ++NUM) {
                const Util::ValueType32 ACTUAL =
                    Util::makeDecimalRaw32(NUM, 0);
                const unsigned char iBCD[7] = {
                    0,
                    0,
                    0,
                    0,
                    static_cast<unsigned char>( NUM / 100),
                    static_cast<unsigned char>((NUM / 10) % 10),
                    static_cast<unsigned char>( NUM % 10)
                };

                const D32 EXPECTED = D32(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int NUM = 0; NUM < 1000; ++NUM) {
                const Util::ValueType64 ACTUAL =
                    Util::makeDecimalRaw64(NUM, 0);
                const unsigned char iBCD[7] = {
                    0,
                    0,
                    0,
                    0,
                    static_cast<unsigned char>( NUM / 100),
                    static_cast<unsigned char>((NUM / 10) % 10),
                    static_cast<unsigned char>( NUM % 10)
                };

                const D64 EXPECTED = D64(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int NUM = 0; NUM < 1000; ++NUM) {
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(NUM, 0);
                const unsigned char iBCD[7] = {
                    0,
                    0,
                    0,
                    0,
                    static_cast<unsigned char>( NUM / 100),
                    static_cast<unsigned char>((NUM / 10) % 10),
                    static_cast<unsigned char>( NUM % 10)
                };
                const D128 EXPECTED = D128(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }
        }
        if (veryVerbose) bsl::cout << "Test that each declet can be correctly "
                                   "placed into each 10-bit shift pattern."
                                << bsl::endl;
        {
            // Test that declets can be properly shifted to each position.
            for (int thousands_pow = 1; thousands_pow <= 1; ++thousands_pow) {
                for (int raw_num = 1; raw_num < 1000; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned int NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType32 ACTUAL =
                        Util::makeDecimalRaw32(NUM, 0);
                    unsigned char iBCD[7] = {0, 0, 0, 0, 0, 0, 0};

                    iBCD[6 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num % 10);

                    iBCD[5 - 3 * thousands_pow] =
                        static_cast<unsigned char>((raw_num / 10) % 10);

                    iBCD[4 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num / 100);

                    const D32 EXPECTED = D32(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }

            for (int thousands_pow = 1; thousands_pow <= 4; ++thousands_pow) {
                for (int raw_num = 1; raw_num < 1000; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType64 ACTUAL =
                        Util::makeDecimalRaw64(NUM, 0);
                    unsigned char iBCD[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                                0, 0, 0, 0, 0};

                    iBCD[15 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num % 10);
                    iBCD[14 - 3 * thousands_pow] =
                        static_cast<unsigned char>((raw_num / 10) % 10);
                    iBCD[13 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num / 100);

                    const D64 EXPECTED = D64(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }

            for (int thousands_pow = 1; thousands_pow <= 5; ++thousands_pow) {
                for (int raw_num = 1; raw_num < 1000; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType128 ACTUAL =
                        Util::makeDecimalRaw128(NUM, 0);
                    unsigned char iBCD[34] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

                    iBCD[33 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num % 10);
                    iBCD[32 - 3 * thousands_pow] =
                        static_cast<unsigned char>((raw_num / 10) % 10);
                    iBCD[31 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num / 100);

                    const D128 EXPECTED = D128(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }
            for (int thousands_pow = 6; thousands_pow <= 6; ++thousands_pow) {
                for (int raw_num = 1; raw_num <= 18; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType128 ACTUAL =
                        Util::makeDecimalRaw128(NUM, 0);
                    unsigned char iBCD[34] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

                    iBCD[33 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num % 10);
                    iBCD[32 - 3 * thousands_pow] =
                        static_cast<unsigned char>((raw_num / 10) % 10);
                    iBCD[31 - 3 * thousands_pow] =
                        static_cast<unsigned char>( raw_num / 100);

                    const D128 EXPECTED = D128(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }
        }

        if (veryVerbose) bsl::cout << "Test some special cases to prove the "
                                   "correcness of declet bit arithmetic."
                                << bsl::endl;
        {
            // The bit pattern of the declet '255' is '0b0101010101' and that
            // of '582' is '0b1010101010'. If the bit arithmetic is off by a
            // character, it will cause problems in adjacent declets.
            const Util::ValueType64 v255 = Util::makeDecimal64(255, 0);
            const Util::ValueType64 v582 = Util::makeDecimal64(582, 0);
            const unsigned long long v255x = 0x2238000000000155ull;
            const unsigned long long v582x = 0x22380000000002aaull;
            ASSERT(!memcmp(&v255, &v255x, 8));
            ASSERT(!memcmp(&v582, &v582x, 8));

            for (int i = 0; i < 4; i++) {
                int NUM =
                    (i & 1 ?    255 :    582) +
                    (i & 2 ? 255000 : 582000);

                const Util::ValueType32 ACTUAL =
                    Util::makeDecimalRaw32(NUM, 0);

                unsigned char iBCD[7] = {0, 0, 0, 0, 0, 0, 0};

                if (i & 1) {
                    iBCD[4] = 2;
                    iBCD[5] = 5;
                    iBCD[6] = 5;
                }
                else {
                    iBCD[4] = 5;
                    iBCD[5] = 8;
                    iBCD[6] = 2;
                }

                if (i & 2) {
                    iBCD[1] = 2;
                    iBCD[2] = 5;
                    iBCD[3] = 5;
                }
                else {
                    iBCD[1] = 5;
                    iBCD[2] = 8;
                    iBCD[3] = 2;
                }

                const D32 EXPECTED = D32(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int i = 0; i < 32; i++) {
                unsigned long long NUM =
                    (i &  1 ?             255ull :             582ull) +
                    (i &  2 ?          255000ull :          582000ull) +
                    (i &  4 ?       255000000ull :       582000000ull) +
                    (i &  8 ?    255000000000ull :    582000000000ull) +
                    (i & 16 ? 255000000000000ull : 582000000000000ull);

                const Util::ValueType64 ACTUAL =
                    Util::makeDecimalRaw64(NUM, 0);

                unsigned char iBCD[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                                   0, 0, 0, 0};

                if (i & 1) {
                    iBCD[13] = 2;
                    iBCD[14] = 5;
                    iBCD[15] = 5;
                }
                else {
                    iBCD[13] = 5;
                    iBCD[14] = 8;
                    iBCD[15] = 2;
                }
                if (i & 2) {
                    iBCD[10] = 2;
                    iBCD[11] = 5;
                    iBCD[12] = 5;
                }
                else {
                    iBCD[10] = 5;
                    iBCD[11] = 8;
                    iBCD[12] = 2;
                }
                if (i & 4) {
                    iBCD[7] = 2;
                    iBCD[8] = 5;
                    iBCD[9] = 5;
                }
                else {
                    iBCD[7] = 5;
                    iBCD[8] = 8;
                    iBCD[9] = 2;
                }
                if (i & 8) {
                    iBCD[4] = 2;
                    iBCD[5] = 5;
                    iBCD[6] = 5;
                }
                else {
                    iBCD[4] = 5;
                    iBCD[5] = 8;
                    iBCD[6] = 2;
                }
                if (i & 16) {
                    iBCD[1] = 2;
                    iBCD[2] = 5;
                    iBCD[3] = 5;
                }
                else {
                    iBCD[1] = 5;
                    iBCD[2] = 8;
                    iBCD[3] = 2;
                }

                const D64 EXPECTED = D64(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int i = 0; i < 64; i++) {
                unsigned long long NUM =
                    (i &  1 ?                255ull :                582ull) +
                    (i &  2 ?             255000ull :             582000ull) +
                    (i &  4 ?          255000000ull :          582000000ull) +
                    (i &  8 ?       255000000000ull :       582000000000ull) +
                    (i & 16 ?    255000000000000ull :    582000000000000ull) +
                    (i & 32 ? 255000000000000000ull : 582000000000000000ull);

                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(NUM, 0);

                unsigned char iBCD[34] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                if (i & 1) {
                    iBCD[31] = 2;
                    iBCD[32] = 5;
                    iBCD[33] = 5;
                }
                else {
                    iBCD[31] = 5;
                    iBCD[32] = 8;
                    iBCD[33] = 2;
                }
                if (i & 2) {
                    iBCD[28] = 2;
                    iBCD[29] = 5;
                    iBCD[30] = 5;
                }
                else {
                    iBCD[28] = 5;
                    iBCD[29] = 8;
                    iBCD[30] = 2;
                }
                if (i & 4) {
                    iBCD[25] = 2;
                    iBCD[26] = 5;
                    iBCD[27] = 5;
                }
                else {
                    iBCD[25] = 5;
                    iBCD[26] = 8;
                    iBCD[27] = 2;
                }
                if (i & 8) {
                    iBCD[22] = 2;
                    iBCD[23] = 5;
                    iBCD[24] = 5;
                }
                else {
                    iBCD[22] = 5;
                    iBCD[23] = 8;
                    iBCD[24] = 2;
                }
                if (i & 16) {
                    iBCD[19] = 2;
                    iBCD[20] = 5;
                    iBCD[21] = 5;
                }
                else {
                    iBCD[19] = 5;
                    iBCD[20] = 8;
                    iBCD[21] = 2;
                }
                if (i & 32) {
                    iBCD[16] = 2;
                    iBCD[17] = 5;
                    iBCD[18] = 5;
                }
                else {
                    iBCD[16] = 5;
                    iBCD[17] = 8;
                    iBCD[18] = 2;
                }

                const D128 EXPECTED = D128(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }
        }

        if (veryVerbose) bsl::cout << "Test the correctness of the "
                                   << "computation of the combination field."
                                   << bsl::endl;
        {
            // Test each of the 9 possible leading digits with each exponent
            // value.
            for (unsigned char leadingDigit = 1; leadingDigit <= 9;
                 ++leadingDigit) {
                for (int EXPONENT = -101; EXPONENT <= 90; ++EXPONENT) {
                    unsigned int MANTISSA = leadingDigit * 1000000u + 255255u;
                    unsigned char iBCD[7] = {leadingDigit, 2, 5, 5, 2, 5, 5};

                    const Util::ValueType32 ACTUAL =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);

                    const D32 EXPECTED = D32(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }
            for (unsigned char leadingDigit = 1; leadingDigit <= 9;
                 ++leadingDigit) {
                for (int EXPONENT = -101; EXPONENT <= 90; ++EXPONENT) {
                    unsigned int MANTISSA = leadingDigit * 1000000u + 582582u;
                    unsigned char iBCD[7] = {leadingDigit, 5, 8, 2, 5, 8, 2};

                    const Util::ValueType32 ACTUAL =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);

                    const D32 EXPECTED = D32(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }

            for (unsigned char leadingDigit = 1; leadingDigit <= 9;
                 ++leadingDigit) {
                for (int EXPONENT = -398; EXPONENT <= 369; ++EXPONENT) {
                    unsigned long long MANTISSA =
                      leadingDigit * 1000000000000000ull + 255255255255255ull;
                    unsigned char iBCD[16] = {leadingDigit, 2, 5, 5, 2, 5, 5,
                                              2, 5, 5, 2, 5, 5, 2, 5, 5};

                    const Util::ValueType64 ACTUAL =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);

                    const D64 EXPECTED = D64(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }
            for (unsigned char leadingDigit = 1; leadingDigit <= 9;
                 ++leadingDigit) {
                for (int EXPONENT = -398; EXPONENT <= 369; ++EXPONENT) {
                    unsigned long long MANTISSA =
                      leadingDigit * 1000000000000000ull + 582582582582582ull;
                    unsigned char iBCD[16] = {leadingDigit, 5, 8, 2, 5, 8, 2,
                                              5, 8, 2, 5, 8, 2, 5, 8, 2};

                    const Util::ValueType64 ACTUAL =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);

                    const D64 EXPECTED = D64(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }
        }

        if (veryVerbose) bsl::cout << "Test makeDecimalRaw32, "
                                   << "makeDecimalRaw64, and makeDecimal128."
                                   << bsl::endl;
        static const struct {
            int           d_lineNum;
            long long     d_mantissa;
            int           d_exponent;
            int           d_expectedExponent;
            unsigned char d_expectedBCD[34];
            int           d_expectedSign;
        } MAKE_DECIMAL_RAW_TESTS[] = {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            { L_,                    0ll,     0, 0,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                    0ll,    10, 0,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                    0ll,   -10, 0,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
#else
            { L_,                    0ll,     0, 0,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                    0ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                    0ll,   -10, -10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
#endif
            { L_,                  100ll,    -2, -2,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 0, 0}, 1},
            { L_,                    1ll,     2, 2,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    1ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   12ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2}, 1},
            { L_,                  123ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3}, 1},
            { L_,                 1234ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4}, 1},
            { L_,                12345ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5}, 1},
            { L_,               123456ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6}, 1},
            { L_,              1234567ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7}, 1},
            { L_,             12345678ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, 1},
            { L_,            123456789ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, 1},
            { L_,           1234567890ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0}, 1},
            { L_,           2147483647ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 7}, 1},
            { L_,           4294967295ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 4, 2, 9,
               4, 9, 6, 7, 2, 9, 5}, 1},
            { L_,          12345678901ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2, 3, 4,
               5, 6, 7, 8, 9, 0, 1}, 1},
            { L_,         123456789012ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3, 4, 5,
               6, 7, 8, 9, 0, 1, 2}, 1},
            { L_,        1234567890123ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4, 5, 6,
               7, 8, 9, 0, 1, 2, 3}, 1},
            { L_,       12345678901234ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5, 6, 7,
               8, 9, 0, 1, 2, 3, 4}, 1},
            { L_,      123456789012345ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6, 7, 8,
               9, 0, 1, 2, 3, 4, 5}, 1},
            { L_,     1234567890123456ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7, 8, 9,
               0, 1, 2, 3, 4, 5, 6}, 1},
            { L_,    12345678901234567ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8, 9, 0,
               1, 2, 3, 4, 5, 6, 7}, 1},
            { L_,   123456789012345678ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, 1},
            { L_,  1234567890123456789ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, 1},
            { L_,  9223372036854775807ll,    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
               7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
               5, 8, 0, 7}, 1},
            { L_,                    -1ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   -12ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2}, -1},
            { L_,                  -123ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3}, -1},
            { L_,                 -1234ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4}, -1},
            { L_,                -12345ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5}, -1},
            { L_,               -123456ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6}, -1},
            { L_,              -1234567ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7}, -1},
            { L_,             -12345678ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, -1},
            { L_,            -123456789ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, -1},
            { L_,           -1234567890ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0}, -1},
            { L_,           -2147483647ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 7}, -1},
            { L_,           -2147483648ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 8}, -1},
            { L_,          -12345678901ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2, 3, 4,
               5, 6, 7, 8, 9, 0, 1}, -1},
            { L_,         -123456789012ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3, 4, 5,
               6, 7, 8, 9, 0, 1, 2}, -1},
            { L_,        -1234567890123ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4, 5, 6,
               7, 8, 9, 0, 1, 2, 3}, -1},
            { L_,       -12345678901234ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5, 6, 7,
               8, 9, 0, 1, 2, 3, 4}, -1},
            { L_,      -123456789012345ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6, 7, 8,
               9, 0, 1, 2, 3, 4, 5}, -1},
            { L_,     -1234567890123456ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7, 8, 9,
               0, 1, 2, 3, 4, 5, 6}, -1},
            { L_,    -12345678901234567ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8, 9, 0,
               1, 2, 3, 4, 5, 6, 7}, -1},
            { L_,   -123456789012345678ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, -1},
            { L_,  -1234567890123456789ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, -1},
            { L_,  -9223372036854775807ll,   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
               7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
               5, 8, 0, 7}, -1},
            { L_, bsl::numeric_limits<long long>::min(),
                                             10, 10,
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
                 7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
                 5, 8, 0, 8}, -1},

            { L_,                    1ll,  -101, -101,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    1ll,    90, 90,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   -1ll,  -101, -101,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   -1ll,    90, 90,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},

            { L_,                    1ll,  -398, -398,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    1ll,   369, 369,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   -1ll,  -398, -398,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   -1ll,   369, 369,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},

            { L_,                    1ll, -6176, -6176,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    1ll,  6111,  6111,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   -1ll, -6176, -6176,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   -1ll,  6111,  6111,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
        };

        const int NUM_MAKE_DECIMAL_RAW_TESTS =
            sizeof MAKE_DECIMAL_RAW_TESTS / sizeof *MAKE_DECIMAL_RAW_TESTS;

        for (int ti = 0; ti < NUM_MAKE_DECIMAL_RAW_TESTS; ++ti) {
            const int LINE = MAKE_DECIMAL_RAW_TESTS[ti].d_lineNum;

            const long long MANTISSA =
                MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa;
            const int EXPONENT = MAKE_DECIMAL_RAW_TESTS[ti].d_exponent;

            bool inULL = (MANTISSA >= 0);
            bool inUInt = (MANTISSA >= 0 &&
                           MANTISSA <=
                           bsl::numeric_limits<unsigned int>::max());
            bool inInt = (MANTISSA >= bsl::numeric_limits<int>::min() &&
                          MANTISSA <= bsl::numeric_limits<int>::max());

            const D128 EXPECTED =
                D128(MAKE_DECIMAL_RAW_TESTS[ti].d_expectedExponent,
                     MAKE_DECIMAL_RAW_TESTS[ti].d_expectedBCD,
                     MAKE_DECIMAL_RAW_TESTS[ti].d_expectedSign);

            const Util::ValueType128 ACTUAL =
                Util::makeDecimalRaw128(MANTISSA, EXPONENT);
            LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);

            if (inULL) {
                // Try 'unsigned long long'.
                const unsigned long long MANTISSA =
                    static_cast<unsigned long long>(
                        MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(MANTISSA, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (inUInt) {
                // Try 'unsigned int'.
                const unsigned int MANTISSA =
                    static_cast<unsigned int>(
                        MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(MANTISSA, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (inInt) {
                // Try 'int'.
                const int MANTISSA =
                    static_cast<int>(
                        MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(MANTISSA, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            // Check if within 64-bit decimal floating-point range.
            if ((-9999999999999999ll <= MANTISSA) &&
                (MANTISSA <= 9999999999999999ll) &&
                (-398 <= EXPONENT) && (EXPONENT <= 369)) {
                const D64 EXPECTED64 =
                    D64(MAKE_DECIMAL_RAW_TESTS[ti].d_expectedExponent,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedBCD,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedSign);

                const Util::ValueType64 ACTUAL64 =
                    Util::makeDecimalRaw64(MANTISSA, EXPONENT);

                LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                                       ACTUAL64 == EXPECTED64);

                if (inULL) {
                    // Try 'unsigned long long'.
                    const unsigned long long MANTISSA =
                        static_cast<unsigned long long>(
                            MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                                       ACTUAL64 == EXPECTED64);
                }

                if (inUInt) {
                    // Try 'unsigned int'.
                    const unsigned int MANTISSA =
                        static_cast<unsigned int>(
                            MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                                       ACTUAL64 == EXPECTED64);
                }

                if (inInt) {
                    // Try 'int'.
                    const int MANTISSA =
                        static_cast<int>(
                            MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                                       ACTUAL64 == EXPECTED64);
                }
            }

            // Check if within 32-bit decimal floating-point range.
            if ((-9999999 <= MANTISSA) && (MANTISSA <= 9999999) &&
                (-98 <= EXPONENT) && (EXPONENT <= 90)) {
                const D32 EXPECTED32 =
                    D32(MAKE_DECIMAL_RAW_TESTS[ti].d_expectedExponent,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedBCD,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedSign);

                const int tMantissa = static_cast<int>(MANTISSA);
                ASSERT(tMantissa == MANTISSA);

                const Util::ValueType32 ACTUAL32 =
                    Util::makeDecimalRaw32(tMantissa, EXPONENT);

                LOOP3_ASSERT(LINE, ACTUAL32, EXPECTED32,
                                                       ACTUAL32 == EXPECTED32);

                if (inULL) {
                    // Try 'unsigned long long'.
                    const unsigned int MANTISSA =
                        static_cast<unsigned int>(
                            MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                    ASSERT(MANTISSA == MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);

                    const Util::ValueType32 ACTUAL32 =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL32, EXPECTED32,
                                                       ACTUAL32 == EXPECTED32);
                }

                if (inUInt) {
                    // Try 'unsigned int'.
                    const unsigned int MANTISSA =
                        static_cast<unsigned int>(
                            MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                    const Util::ValueType32 ACTUAL32 =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL32, EXPECTED32,
                                                       ACTUAL32 == EXPECTED32);
                }

                if (inInt) {
                    // Try 'int'.
                    const int MANTISSA =
                        static_cast<int>(
                            MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa);
                    const Util::ValueType32 ACTUAL32 =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL32, EXPECTED32,
                                                       ACTUAL32 == EXPECTED32);
                }
            }
        }

        {
            // Test makeDecimalRaw128 on the case of
            // 'mantissa == bsl::numeric_limits<unsigned long long>::max()'.

            const Util::ValueType128 ACTUAL128 =
                Util::makeDecimalRaw128(
                           bsl::numeric_limits<unsigned long long>::max(), 10);
            const unsigned char EXPECTED_MANTISSA[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 1, 8, 4, 4, 6, 7,
                4, 4, 0, 7, 3, 7, 0, 9, 5, 5,
                1, 6, 1, 5};
            const D128 EXPECTED128 = D128(10, EXPECTED_MANTISSA, 1);
            LOOP2_ASSERT(ACTUAL128, EXPECTED128, ACTUAL128 == EXPECTED128);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   test all functions
        //
        // Concerns:
        //: 1 Forwarding to the right routines
        //
        // Plan:
        //: 1 Try all operations see if basics work
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                                << "BREATHING TEST" << bsl::endl
                                << "==============" << bsl::endl;

        if (veryVerbose) bsl::cout << "Types" << bsl::endl;

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
        ASSERT(sameType<_Decimal32 >(Util::ValueType32()));
        ASSERT(sameType<_Decimal64 >(Util::ValueType64()));
        ASSERT(sameType<_Decimal128>(Util::ValueType128()));
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
        ASSERT(sameType<decSingle>(Util::ValueType32() ));
        ASSERT(sameType<decDouble>(Util::ValueType64() ));
        ASSERT(sameType<decQuad  >(Util::ValueType128()));
#endif

        if (veryVerbose) bsl::cout << "Helper functions" << bsl::endl;

#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER

    ASSERT(sameType<decContext*>(Util::getDecNumberContext()));

    ASSERT(Util::getDecNumberContext() == Util::getDecNumberContext());

#endif

        if (veryVerbose) bsl::cout << "Parsers" << bsl::endl;

        // While checking the parsers we cannot use our BDLDFP_DECIMALIMPL_Dx
        // macros to provide portable decimal floating point literals, since
        // they may use the 'parse' functions in the background.  Therefore,
        // for the decNumber based implementation, I will have to recreate the
        // parse function here, using another implementation, otherwise we
        // won't be able to create the expected values that we can compare to.

        {
            ASSERT(sameType<Util::ValueType32>(Util::parse32("1")));

            static const unsigned char bcd42[] = {0x4, 0x2};
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            _Decimal32 expected = 42.df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            D32 expected(0, bcd42, 1);
#endif
            LOOP2_ASSERT(Util::parse32("42"), expected,
                         Util::parse32("42") == expected);

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            expected = -42e1df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            expected = D32(1, bcd42, -1);
#endif
            LOOP2_ASSERT(Util::parse32("-42e1"), expected,
                         Util::parse32("-42e1") == expected);

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            expected = -4.2df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            expected = D32(-1, bcd42, -1);
#endif
            ASSERT(Util::parse32("-4.2") == expected);
        }

        {
            ASSERT(sameType<Util::ValueType64>(Util::parse64("1")));

            static const unsigned char bcd42[] = {0x4, 0x2};
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            _Decimal64 expected = 42.dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            D64 expected(0, bcd42, 1);
#endif
            ASSERT(Util::parse64("42") == expected);

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            expected = -42e1dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            expected = D64(1, bcd42, -1);
#endif
            ASSERT(Util::parse64("-42e1") == expected);

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            expected = -4.2dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            expected = D64(-1, bcd42, -1);
#endif
            ASSERT(Util::parse64("-4.2") == expected);
        }

        {
            ASSERT(sameType<Util::ValueType128>(Util::parse128("1")));

            static const unsigned char bcd42[] = {0x4, 0x2};
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            _Decimal128 expected = 42.dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            D128 expected(0, bcd42, 1);
#endif
            ASSERT(Util::parse128("42") == expected);

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            expected = -42e1dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            expected = D128(1, bcd42, -1);
#endif
            ASSERT(Util::parse128("-42e1") == expected);

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            expected = -4.2dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            expected = D128(-1, bcd42, -1);
#endif
            ASSERT(Util::parse128("-4.2") == expected);
        }

        if (veryVerbose) bsl::cout << "Literal macros" << bsl::endl;

        {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            ASSERT(sameType<_Decimal32>(BDLDFP_DECIMALIMPLUTIL_DF(42.)));
            ASSERT(42.df == BDLDFP_DECIMALIMPLUTIL_DF(42.));
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            ASSERT(sameType<decSingle>(BDLDFP_DECIMALIMPLUTIL_DF(42.)));
            const D32 expected = Util::parse32("42");
            ASSERT(expected.isEqual(BDLDFP_DECIMALIMPLUTIL_DF(42.)));
#endif
        }

        {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            ASSERT(sameType<_Decimal64>(BDLDFP_DECIMALIMPLUTIL_DD(42.)));
            ASSERT(42.dd == BDLDFP_DECIMALIMPLUTIL_DD(42.));
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            ASSERT(sameType<decDouble>(BDLDFP_DECIMALIMPLUTIL_DD(42.)));
            const D64 expected = Util::parse64("42");
            ASSERT(expected.isEqual(BDLDFP_DECIMALIMPLUTIL_DD(42.)));
#endif
        }

        {
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
            ASSERT(sameType<_Decimal128>(BDLDFP_DECIMALIMPLUTIL_DL(42.)));
            ASSERT(42.dl == BDLDFP_DECIMALIMPLUTIL_DL(42.));
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
            ASSERT(sameType<decQuad>(BDLDFP_DECIMALIMPLUTIL_DL(42.)));
            const D128 expected = Util::parse128("42");
            ASSERT(expected.isEqual(BDLDFP_DECIMALIMPLUTIL_DL(42.)));
#endif
        }
      } break;

      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
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

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
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
