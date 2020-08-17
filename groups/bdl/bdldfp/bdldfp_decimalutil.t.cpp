// bdldfp_decimalutil.t.cpp                                           -*-C++-*-
#include <bdldfp_decimalutil.h>

#include <bdldfp_decimal.h>
#include <bdldfp_uint128.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

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
// TBD:
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// TRAITS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

typedef bdldfp::DecimalFormatConfig         Config;
typedef bdldfp::DecimalUtil                 Util;
typedef bdldfp::DecimalImpUtil              ImpUtil;

#if defined(BSLS_PLATFORM_OS_WINDOWS) && !defined(FP_NAN)

// MS does not provide standard floating-point classification so we do

// First, make sure the environment is sane

#if defined(FP_NORMAL) || defined(FP_INFINITE) || defined(FP_ZERO) || \
    defined(FP_SUBNORMAL)
#error Standard FP_ macros are not defined properly.
#endif

// Make it look like stiff MS has in math.h

#define FP_SUBNORMAL (-2)
#define FP_NORMAL    (-1)
#define FP_ZERO        0
#define FP_INFINITE    1
#define FP_NAN         2

#endif

//=============================================================================
//                      GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

//=============================================================================
//                          PERFORMANCE TEST HELPERS
//-----------------------------------------------------------------------------

// Accumulate data per symbol.
struct SymbolDataDecimal64 {
    BDEC::Decimal64    d_low;
    BDEC::Decimal64    d_high;
    BDEC::Decimal64    d_valueTraded;
    BDEC::Decimal64    d_vwap;
    unsigned long long d_volume;
};

struct SymbolDataDouble {
    double             d_low;
    double             d_high;
    double             d_valueTraded;
    double             d_vwap;
    unsigned long long d_volume;
};

struct TradeDataPoint {
    bsl::string        d_symbol;
    unsigned long long d_mantissa;
    int                d_exponent;
    bsl::string        d_price;
    unsigned long long d_quantity;
};

void parseDecimal(unsigned long long *mantissa,
                  int                *exponent,
                  const bsl::string&  input)
{
    // Load into the specified 'mantissa' and 'exponent' values corresponding
    // to the decimal number represented by the specified 'input'. The
    // behavior is undefined if 's' is not a valid decimal number. For
    // example, 123.45 = 12345 * 10 ^ -2, and thus the string '123.45' would
    // return a 'mantissa' of '12345' and an 'exponent' to '-2'.
    bsl::string::const_iterator pos =
                                    bsl::find(input.begin(), input.end(), '.');
    *exponent = -static_cast<int>(bsl::distance(pos, input.end()));

    bsl::string mantissa_str(input.begin(), pos);
    mantissa_str += bsl::string(pos + 1, input.end());

    *mantissa = atoi(mantissa_str.c_str());
}

unsigned int split(bsl::vector<bsl::string>& strs,
                   const bsl::string&        input,
                   char                      ch)
{
    // Split the specified 'input' by a separator character given by the
    // specified 'ch', storing each section of 'input' in the order they
    // appear into the specified 'strs'.
    bsl::string::const_iterator pos =
                                     bsl::find(input.begin(), input.end(), ch);

    bsl::string::const_iterator initialPos = input.begin();
    strs.clear();

    // Decompose statement
    while (pos != input.end()) {
        strs.push_back(bsl::string(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = bsl::find(initialPos, input.end(), ch);
    }

    // Add the last one
    strs.push_back(bsl::string(initialPos,
                                 bsl::min(pos, input.end()) - initialPos + 1));

    return static_cast<unsigned int>(strs.size());
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
  // TBD
}  // close namespace UsageExample

// TODO: Find out why the 17 digit variants are failing tests.
const long long mantissas[] = {
//                            -12345678901234567ll,
                            - 2345678901234567ll,
                            - 1234567890123456ll,
                            -  234567890123456ll,
                            -  123456789012345ll,
                            -   23456789012345ll,
                            -   12345678901234ll,
                            -    2345678901234ll,
                            -    1234567890123ll,
                            -     234567890123ll,
                            -     123456789012ll,
                            -      23456789012ll,
                            -      12345678901ll,
                            -       2345678901ll,
                            -       1234567890ll,
                            -        234567890ll,
                            -        123456789ll,
                            -         23456789ll,
                            -         12345678ll,
                            -          2345678ll,
                            -          1234567ll,
                            -           234567ll,
                            -           123456ll,
                            -            23456ll,
                            -            12345ll,
                            -             6721ll,
                            -             5723ll,
                            -              317ll,
                            -              100ll,
                            -               83ll,
                            -               27ll,
                            -                9ll,
                            -                5ll,
                                             0ll,
                                             5ll,
                                             9ll,
                                            27ll,
                                            83ll,
                                           100ll,
                                           317ll,
                                          5723ll,
                                          6721ll,
                                         12345ll,
                                         23456ll,
                                        123456ll,
                                        234567ll,
                                       1234567ll,
                                       2345678ll,
                                      12345678ll,
                                      23456789ll,
                                     123456789ll,
                                     234567890ll,
                                    1234567890ll,
                                    2345678901ll,
                                   12345678901ll,
                                   23456789012ll,
                                  123456789012ll,
                                  234567890123ll,
                                 1234567890123ll,
                                 2345678901234ll,
                                12345678901234ll,
                                23456789012345ll,
                               123456789012345ll,
                               234567890123456ll,
                              1234567890123456ll,
                              2345678901234567ll,
//                             12345678901234567ll,
                        };
const int numMantissas = static_cast<int>(sizeof(mantissas) /
                                          sizeof(*mantissas));

const int exps[] = {
                 -321,
                 -129,
                 - 23,
                 - 10,
                 -  7,
                 -  2,
                    0,
                    2,
                    7,
                   10,
                   23,
                  129,
                  321,
};
const int numExps = static_cast<int>(sizeof(exps) / sizeof(*exps));

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

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

template <class TYPE>
struct NumberMaker;

template <>
struct NumberMaker<BDEC::Decimal32>
{
    BDEC::Decimal32 operator()(long long mantissa, int exponent) const
    {
        return Util::makeDecimalRaw32(static_cast<int>(mantissa), exponent);
    }
};

template <>
struct NumberMaker<BDEC::Decimal64>
{
    BDEC::Decimal64 operator()(long long mantissa, int exponent) const
    {
        return Util::makeDecimalRaw64(mantissa, exponent);
    }
};

template <>
struct NumberMaker<BDEC::Decimal128>
{
    BDEC::Decimal128 operator()(long long mantissa, int exponent) const
    {
        return Util::makeDecimalRaw128(mantissa, exponent);
    }
};


                          // Stream buffer helpers

template <int Size>
struct BufferBuf : bsl::streambuf {
    BufferBuf() { reset(); }
    const char *str() { *this->pptr() =0; return this->pbase(); }
    void reset() { this->setp(this->d_buf, this->d_buf + Size); }
    char d_buf[Size + 1];
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

                          // Concurrency helpers

extern "C" void doQuantize()
{
    typedef BDEC::DecimalUtil Util;
    const BDEC::Decimal64 d_1_005 = BDLDFP_DECIMAL_DD(1.005);
    const BDEC::Decimal64 d_1_00 = BDLDFP_DECIMAL_DD(1.00);
    const BDEC::Decimal64 d_0_01 = bdldfp::DecimalUtil::makeDecimal64(1, -2);
    const int k_REPETITIONS = 1000000;
    for (int repetition = 0; repetition < k_REPETITIONS; ++repetition) {
        const BDEC::Decimal64 q = Util::quantize(d_1_005, d_0_01);
        if (d_1_00 != q) {
            ASSERTV(repetition, d_1_00, q, d_1_00 == q);
            break;
        }
    }
}

extern "C" void doRound()
{
    typedef BDEC::DecimalUtil Util;
    const BDEC::Decimal64 d_1_005 = BDLDFP_DECIMAL_DD(1.005);
    const BDEC::Decimal64 d_1_01 = BDLDFP_DECIMAL_DD(1.01);
    const BDEC::Decimal64 d_100_0 = BDLDFP_DECIMAL_DD(100.0);
    const int k_REPETITIONS = 1000000;
    for (int repetition = 0; repetition < k_REPETITIONS; ++repetition) {
        const BDEC::Decimal64 r = Util::round(d_1_005 * d_100_0) / d_100_0;
        if (d_1_01 != r) {
            ASSERTV(repetition, d_1_01, r, d_1_01 == r);
            break;
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;  // always the last


    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    bslma::TestAllocator *pa = &ta;

    typedef BDEC::DecimalUtil Util;

    cout.precision(35);


    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'format'
        //
        // Concerns:
        //: 1 That if 'cfg' is not defined then 'format' outputs decimal value
        //:   in scientific notation with a precision sufficient to produce
        //:   all available digits.
        //:
        //: 2 That if 'cfg' is not defined then precision provided by default
        //:   configuration object is sufficient to produce all available
        //:   digits.
        //
        // Plan:
        //: 1 Test that style is default to scientific notation  if 'cfg' is
        //:   not defined.  (C-1)
        //:
        //: 2 Test that style is default to scientific notation  if 'cfg' is
        //:   not defined.  (C-2)
        //:
        //: 3 Repeat P1-2 for Decimal64 and Decimal128 types.
        //
        // Testing:
        //   int format(char *, int, Decimal32,  const DecimalFormatConfig&);
        //   int format(char *, int, Decimal64,  const DecimalFormatConfig&);
        //   int format(char *, int, Decimal128, const DecimalFormatConfig&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST FORMAT METHODS" << endl
                          << "===================" << endl;

        if (verbose) cout << endl
                          << "Test Decimal32" << endl
                          << "--------------" << endl;

#define DEC(X) BDLDFP_DECIMAL_DF(X)

        {
            typedef bdldfp::Decimal32 Type;

            const int  k_BUFFER_SIZE = 256;
            char       buffer[k_BUFFER_SIZE];

            const Type  VALUE = DEC(1.234567E+0);
            int         len = Util::format(buffer,
                                           k_BUFFER_SIZE,
                                           VALUE);

            const bsl::string DEFAULT(buffer, len, pa);

            {
                // Test that 'format' produces the same resultant string
                // when invoked with 'MAX_DIGIT_CONFIG' configuration.
                const bsl::string EXPECTED("1.234567", pa);

                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                const Config MAX_DIGIT_CONFIG(
                    bsl::numeric_limits<Type>::digits10 - 1);
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   MAX_DIGIT_CONFIG);

                const bsl::string RESULT(buffer, len, pa);
                LOOP3_ASSERT(L_,
                             RESULT,
                             EXPECTED,
                             RESULT == EXPECTED);
                LOOP3_ASSERT(L_,
                             RESULT,
                             DEFAULT,
                             RESULT == DEFAULT);
            }
            {
                // Test that 'format' output a decimal value in notation
                // distinct from to 'DEFAULT' string when invoked with a style
                // distinct from the one provided by default configuration
                // object.
                const bsl::string EXPECTED("1.234567e+00", pa);
                const Config CONFIG(6, Config::e_SCIENTIFIC);
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   CONFIG);

                const bsl::string RESULT(buffer, len, pa);
                LOOP3_ASSERT(L_,
                             RESULT,
                             EXPECTED,
                             RESULT == EXPECTED);
                LOOP3_ASSERT(L_,
                             RESULT,
                             DEFAULT,
                             RESULT != DEFAULT);
            }
        }

#undef DEC
        if (verbose) cout << endl
                          << "Test Decimal64" << endl
                          << "--------------" << endl;
#define DEC(X) BDLDFP_DECIMAL_DD(X)
        {
            typedef bdldfp::Decimal64 Type;

            const int  k_BUFFER_SIZE = 256;
            char       buffer[k_BUFFER_SIZE];

            const Type  VALUE = DEC(1.234567890123456E+0);
            int         len = Util::format(buffer,
                                           k_BUFFER_SIZE,
                                           VALUE);

            const bsl::string DEFAULT(buffer, len, pa);
            {
                // Test that 'format' produces the same resultant string
                // when invoked with 'MAX_DAGIT_CONFIG' configuration.
                const bsl::string EXPECTED("1.234567890123456", pa);

                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                const Config MAX_DAGIT_CONFIG(
                                  bsl::numeric_limits<Type>::digits10 - 1);
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   MAX_DAGIT_CONFIG);

                const bsl::string RESULT(buffer, len, pa);
                LOOP3_ASSERT(L_,
                             RESULT,
                             EXPECTED,
                             RESULT == EXPECTED);
                LOOP3_ASSERT(L_,
                             RESULT,
                             DEFAULT,
                             RESULT == DEFAULT);
            }
            {
                // Test that 'format' output a decimal value in notation
                // distinct from 'DEFAULT' string when invoked with a style
                // distinct from the one provided by default configuration
                // object.
                const bsl::string EXPECTED("1.234567890123456e+00", pa);
                const Config CONFIG(15, Config::e_SCIENTIFIC);
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   CONFIG);

                const bsl::string RESULT(buffer, len, pa);
                LOOP3_ASSERT(L_,
                             RESULT,
                             EXPECTED,
                             RESULT == EXPECTED);
                LOOP3_ASSERT(L_,
                             RESULT,
                             DEFAULT,
                             RESULT != DEFAULT);
            }
        }
#undef DEC

        if (verbose) cout << endl
                          << "Test Decimal128" << endl
                          << "---------------" << endl;

#define DEC(X) BDLDFP_DECIMAL_DL(X)

        typedef bdldfp::Decimal128 Type;

        const int  k_BUFFER_SIZE = 10000;
        char       buffer[k_BUFFER_SIZE];

        {
            const Type  VALUE = DEC(1.234567890123456789012345678901234E+0);
            int         len = Util::format(buffer,
                                           k_BUFFER_SIZE,
                                           VALUE);

            const bsl::string DEFAULT(buffer, len, pa);

            {
                // Test that 'format' produces the same resultant string
                // when invoked with 'MAX_DAGIT_CONFIG' configuration.
                const bsl::string EXPECTED(
                                 "1.234567890123456789012345678901234", pa);

                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                const Config MAX_DAGIT_CONFIG(
                                      bsl::numeric_limits<Type>::digits10 - 1);
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   MAX_DAGIT_CONFIG);

                const bsl::string RESULT(buffer, len, pa);
                LOOP3_ASSERT(L_,
                             RESULT,
                             EXPECTED,
                             RESULT == EXPECTED);
                LOOP3_ASSERT(L_,
                             RESULT,
                             DEFAULT,
                             RESULT == DEFAULT);
            }
            {
                // Test that 'format' output a decimal value in notation
                // distinct from 'DEFAULT' string when invoked with a style
                // distinct from the one provided by default configuration
                // object.
                const bsl::string EXPECTED(
                                "1.234567890123456789012345678901234e+00", pa);
                const Config CONFIG(33, Config::e_SCIENTIFIC);
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   CONFIG);

                const bsl::string RESULT(buffer, len, pa);
                LOOP3_ASSERT(L_,
                             RESULT,
                             EXPECTED,
                             RESULT == EXPECTED);
                LOOP3_ASSERT(L_,
                             RESULT,
                             DEFAULT,
                             RESULT != DEFAULT);
            }
#undef DEC
        }
      } break;
    case 14: {
    // ------------------------------------------------------------------------
    // TESTING math functions
    //
    // Concerns:
    //: 1 Forwarding to the right routines.
    //:
    //: 2 That decimal arguments with higher precision 'Decimal64/128' are
    //:   not rounded to fit into the decimal numbers with lower precision
    //:   'Decimal32'/64' when passed into underlying library's functions.
    //
    // Plan:
    //:  1 Using table-driven technique:
    //:
    //:    1 Try with several variations and combinations of decimal floats
    //:      (different mantissas and exponents, both positive and negative).
    //:      C-1.
    //:
    //:    2 For functions that accept decimal numbers with higher precision
    //:      select arguments that precision exceeds the lower precision
    //:      decimal types.  Ensure the result of function execution equals the
    //:      expected value.  C-1,2.
    //
    // Testing:
    //   Decimal32     copySign  (Decimal32);
    //   Decimal64     copySign  (Decimal64);
    //   Decimal128    copySign  (Decimal128);
    //   Decimal32     exp       (Decimal32);
    //   Decimal64     exp       (Decimal64);
    //   Decimal128    exp       (Decimal128);
    //   Decimal32     log       (Decimal32);
    //   Decimal64     log       (Decimal64);
    //   Decimal128    log       (Decimal128);
    //   Decimal32     logB      (Decimal32);
    //   Decimal64     logB      (Decimal64);
    //   Decimal128    logB      (Decimal128);
    //   Decimal32     log10     (Decimal32);
    //   Decimal64     log10     (Decimal64);
    //   Decimal128    log10     (Decimal128);
    //   Decimal32     fmod      (Decimal32,  Decimal32);
    //   Decimal64     fmod      (Decimal64,  Decimal64);
    //   Decimal128    fmod      (Decimal128, Decimal128);
    //   Decimal32     remainder (Decimal32,  Decimal32);
    //   Decimal64     remainder (Decimal64,  Decimal64);
    //   Decimal128    remainder (Decimal128, Decimal128);
    //   long int      lrint     (Decimal32);
    //   long int      lrint     (Decimal64);
    //   long int      lrint     (Decimal128);
    //   long long int llrint    (Decimal32);
    //   long long int llrint    (Decimal64);
    //   long long int llrint    (Decimal128);
    //   Decimal32     nextafter (Decimal32,  Decimal32);
    //   Decimal64     nextafter (Decimal64,  Decimal64);
    //   Decimal128    nextafter (Decimal128, Decimal128);
    //   Decimal32     nexttoward(Decimal32,  Decimal32);
    //   Decimal64     nexttoward(Decimal64,  Decimal64);
    //   Decimal128    nexttoward(Decimal128, Decimal128);
    //   Decimal32     pow       (Decimal32);
    //   Decimal64     pow       (Decimal64);
    //   Decimal128    pow       (Decimal128);
    //   long int      lround    (Decimal32);
    //   long int      lround    (Decimal64);
    //   long int      lround    (Decimal128);
    //   Decimal32     sqrt      (Decimal32);
    //   Decimal64     sqrt      (Decimal64);
    //   Decimal128    sqrt      (Decimal128);
    // --------------------------------------------------------------------

    if (verbose) bsl::cout << "\nTESTING MATH METHODS"
                           << "\n===================="
                           << bsl::endl;

    typedef BDEC::DecimalUtil Util;

#define DEC(X) BDLDFP_DECIMAL_DF(X)
    if (veryVerbose) bsl::cout << "\nDecimal32"
                               << "\n---------"
                               << bsl::endl;
    {
        typedef BDEC::Decimal32 Obj;

        const Obj NAN_P   = bsl::numeric_limits<Obj>::quiet_NaN();
        const Obj NAN_N   = -NAN_P;
        const Obj INF_P   = bsl::numeric_limits<Obj>::infinity();
        const Obj INF_N   = -INF_P;
        const Obj ZERO_P  = DEC( 0.0);
        const Obj ZERO_N  = DEC(-0.0);

        if (veryVerbose) { T_ bsl::cout << "copySign()" << bsl::endl; }
        {
            const Obj DEC_X  = DEC(1234567.0);
            const Obj DEC_Y  = DEC(0.1234567);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //---------------------------------
            // LINE |  X    |    Y   | EXPECTED
            //---------------------------------
                { L_, DEC_X,   DEC_Y,   DEC_X },
                { L_, DEC_X,   DEC_Y,   DEC_X },
                { L_, DEC_Y,  -DEC_X,  -DEC_Y },
                { L_, DEC_Y,  -DEC_X,  -DEC_Y },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::copySign(X, Y);

                LOOP_ASSERT(LINE, RESULT ==  EXPECTED);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "exp()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //------------------------------------------
            // LINE |     X         | EXPECTED
            //------------------------------------------
                { L_, DEC( 12.0),     DEC(1.627548e+5) },
                { L_, DEC(-11.0),     DEC(1.670170e-5) },
                { L_, DEC(9999999.0), INF_P            },
                { L_, NAN_P,          NAN_P            },
                { L_, NAN_N,          NAN_N            },
                { L_, INF_P,          INF_P            },
                { L_, INF_N,          ZERO_P           },
                { L_, ZERO_P,         DEC(1.0)         },
                { L_, ZERO_N,         DEC(1.0)         },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::exp(X);

                if (Util::isNan(X)) {
                    LOOP2_ASSERT(LINE, EXPECTED, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "log()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |     X     | EXPECTED
            //-----------------------------------
                { L_, DEC(  1.0), ZERO_P        },
                { L_, DEC( 12.0), DEC(2.484907) },
                { L_, DEC(-11.0), NAN_N         },
                { L_, NAN_P,      NAN_P         },
                { L_, NAN_N,      NAN_N         },
                { L_, INF_P,      INF_P         },
                { L_, INF_N,      NAN_N         },
                { L_, ZERO_P,     INF_N         },
                { L_, ZERO_N,     INF_N         },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::log(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "logB()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //------------------------------
            // LINE |     X     | EXPECTED
            //------------------------------
                { L_, DEC(  1.0),  ZERO_P   },
                { L_, DEC( 10.0),  DEC(1.0) },
                { L_, DEC( 12.0),  DEC(1.0) },
                { L_, DEC(-11.0),  DEC(1.0) },
                { L_, DEC(-100.0), DEC(2.0) },
                { L_, NAN_P,       NAN_P    },
                { L_, NAN_N,       NAN_N    },
                { L_, INF_P,       INF_P    },
                { L_, INF_N,       INF_P    },
                { L_, ZERO_P,      INF_N    },
                { L_, ZERO_N,      INF_N    },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::logB(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "log10()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |     X     | EXPECTED
            //-----------------------------------
                { L_, DEC(  1.0), ZERO_P        },
                { L_, DEC( 12.0), DEC(1.079181) },
                { L_, DEC(-11.0), NAN_N         },
                { L_, NAN_P,      NAN_P         },
                { L_, NAN_N,      NAN_N         },
                { L_, INF_P,      INF_P         },
                { L_, INF_N,      NAN_N         },
                { L_, ZERO_P,     INF_N         },
                { L_, ZERO_N,     INF_N         },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::log10(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "fmod()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------------
            // LINE |     X    |     Y   | EXPECTED
            //-----------------------------------------
                { L_, DEC( 5.1), DEC(3.0), DEC( 2.1)  },
                { L_, DEC( 5.1), DEC(3.0), DEC( 2.1)  },
                { L_, DEC(-5.1), DEC(3.0), DEC(-2.1)  },
                { L_, DEC( 5.1), NAN_P,    NAN_P      },
                { L_, NAN_P,     DEC(3.0), NAN_P      },
                { L_, INF_P,     DEC(3.0), NAN_P      },
                { L_, INF_P,     INF_P,    NAN_P      },
                { L_, INF_P,     ZERO_P,   NAN_P      },
                { L_, ZERO_P,    ZERO_P,   NAN_P      },
                { L_, DEC(5.1),  ZERO_P,   NAN_P      },
                { L_, ZERO_P,    DEC(3.0), ZERO_P     },
                { L_, DEC( 5.1), INF_P,    DEC( 5.1)  },
                { L_, DEC(-5.1), INF_N,    DEC(-5.1)  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::fmod(X, Y);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "remainder()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------------
            // LINE |     X    |     Y   | EXPECTED
            //-----------------------------------------
                { L_, DEC( 5.1), DEC(3.0), DEC(-0.9)  },
                { L_, DEC( 5.1), DEC(3.0), DEC(-0.9)  },
                { L_, DEC(-5.1), DEC(3.0), DEC( 0.9)  },
                { L_, DEC( 5.1), NAN_P,    NAN_P      },
                { L_, DEC( 5.1), NAN_N,    NAN_P      },
                { L_, DEC( 5.1), INF_P,    DEC( 5.1)  },
                { L_, DEC(-5.1), INF_N,    DEC(-5.1)  },
                { L_, DEC( 5.1), ZERO_P,   NAN_P      },
                { L_, DEC( 5.1), ZERO_N,   NAN_P      },
                { L_, NAN_P,     DEC(3.0), NAN_P      },
                { L_, INF_P,     DEC(3.0), NAN_P      },
                { L_, ZERO_P,    DEC(3.0), ZERO_P     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::remainder(X, Y);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "lrint()" << bsl::endl; }
        {
            const long int NaN = ~(-1ul >> 1);

            struct {
                int      d_line;
                Obj      d_x;
                long int d_expected;
            } DATA[] = {
            //---------------------------
            // LINE |     X    | EXPECTED
            //---------------------------
                { L_, DEC( 2.3),  2  },
                { L_, DEC( 2.5),  2  },
                { L_, DEC( 3.5),  4  },
                { L_, DEC(-2.3), -2  },
                { L_, DEC(-2.5), -2  },
                { L_, DEC(-3.5), -4  },
                { L_, NAN_P,     NaN },
                { L_, NAN_N,     NaN },
                { L_, INF_P,     NaN },
                { L_, INF_N,     NaN },
                { L_, ZERO_P,     0  },
                { L_, ZERO_N,     0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE     = DATA[ti].d_line;
                const Obj&      X        = DATA[ti].d_x;
                const long int& EXPECTED = DATA[ti].d_expected;
                const long int  RESULT   = Util::lrint(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "llrint()" << bsl::endl; }
        {
            const long long int NaN = ~(-1ull >> 1);
            struct {
                int           d_line;
                Obj           d_x;
                long long int d_expected;
            } DATA[] = {
            //---------------------------
            // LINE |     X    | EXPECTED
            //---------------------------
                { L_, DEC( 2.3),  2  },
                { L_, DEC( 2.5),  2  },
                { L_, DEC( 3.5),  4  },
                { L_, DEC(-2.3), -2  },
                { L_, DEC(-2.5), -2  },
                { L_, DEC(-3.5), -4  },
                { L_, NAN_P,     NaN },
                { L_, NAN_N,     NaN },
                { L_, INF_P,     NaN },
                { L_, INF_N,     NaN },
                { L_, ZERO_P,     0  },
                { L_, ZERO_N,     0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE     = DATA[ti].d_line;
                const Obj&           X        = DATA[ti].d_x;
                const long long int& EXPECTED = DATA[ti].d_expected;
                const long long int  RESULT   = Util::llrint(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "nextafter()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_from;
                Obj d_to;
                Obj d_expected;
            } DATA[] = {
            //-------------------------------------------------
            // LINE |   FROM  |     TO   | EXPECTED
            //-------------------------------------------------
                { L_, DEC(0.0),  DEC(1.0), DEC(1.0e-101)      },
                { L_, DEC(1.0),  DEC(2.0), DEC(1.000001)      },
                { L_, DEC(1.0),  DEC(0.0), DEC(9.999999e-1)   },
                { L_, DEC(0.0),  DEC(0.0), DEC(0.0)           },
                { L_, NAN_P,     NAN_P,    NAN_P              },
                { L_, NAN_N,     NAN_N,    NAN_N              },
                { L_, INF_P,     INF_P,    INF_P              },
                { L_, INF_N,     INF_N,    INF_N              },
                { L_, INF_P,     DEC(2.0), DEC( 9.999999e+96) },
                { L_, INF_N,     DEC(2.0), DEC(-9.999999e+96) },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& FROM     = DATA[ti].d_from;
                const Obj& TO       = DATA[ti].d_to;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::nextafter(FROM, TO);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "nexttoward()" << bsl::endl; }
        {
            const Obj SUBN_32   = bsl::numeric_limits<Obj>::denorm_min();
            const Obj NAN_S_32  = bsl::numeric_limits<Obj>::signaling_NaN();
            const Obj NAN_Q_32  = bsl::numeric_limits<Obj>::quiet_NaN();

            typedef BDEC::Decimal128 Obj128;

            const Obj128 SUBN_128  =
                                  bsl::numeric_limits<Obj128>::denorm_min();
            const Obj128 NAN_Q_128 =
                                  bsl::numeric_limits<Obj128>::quiet_NaN();
            const Obj128 NAN_S_128 =
                                  bsl::numeric_limits<Obj128>::signaling_NaN();
            const Obj128 INF_128   =
                                  bsl::numeric_limits<Obj128>::infinity();

            struct {
                int    d_line;
                Obj    d_from;
                Obj128 d_to;
                Obj    d_expected;
            } DATA[] = {
             //-----------------------------------------
             //LINE |  FROM    |    TO     | EXPECTED
             //-----------------------------------------
                { L_,  DEC(0.0),  SUBN_128,   SUBN_32  },
                { L_,  DEC(0.0), -SUBN_128,  -SUBN_32  },
                { L_,  NAN_Q_32,  NAN_Q_128,  NAN_Q_32 },
                { L_,  NAN_S_32,  NAN_S_128,  NAN_S_32 },
                { L_,  INF_P,     INF_128,    INF_P    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE     = DATA[ti].d_line;
                const Obj&    FROM     = DATA[ti].d_from;
                const Obj128& TO       = DATA[ti].d_to;
                const Obj&    EXPECTED = DATA[ti].d_expected;
                const Obj     RESULT   = Util::nexttoward(FROM, TO);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "pow()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_base;
                Obj d_exponent;
                Obj d_expected;
            } DATA[] = {

            //-------------------------------------------------
            // LINE |   BASE   |  EXPONENT |     EXPECTED
            //-------------------------------------------------
                { L_, DEC( 2.0),  DEC(10.0), DEC(1024.0     ) },
                { L_, DEC( 2.0),  DEC( 0.5), DEC(   1.414214) },
                { L_, DEC(-2.0),  DEC(-3.0), DEC(  -0.125   ) },
                { L_, DEC(-1.0),  NAN_P,            NAN_P     },
                { L_, DEC( 1.0),  NAN_P,     DEC(   1.0     ) },
                { L_, INF_P,      DEC( 2.0),        INF_P     },
                { L_, INF_P,      DEC(-1.0), DEC(   0.0     ) },
                { L_, DEC(-1.0),  DEC(0.3333333),   NAN_N     },
                { L_, ZERO_N,     DEC(-3.0),        INF_N     },
                { L_, NAN_P,      DEC( 2.0),        NAN_P     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& BASE     = DATA[ti].d_base;
                const Obj& EXPONENT = DATA[ti].d_exponent;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::pow(BASE, EXPONENT);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "lround()" << bsl::endl; }
        {
            const long int NaN = ~(-1ul >> 1);

            struct {
                int      d_line;
                Obj      d_x;
                long int d_expected;
            } DATA[] = {
            //---------------------------
            // LINE |     X    | EXPECTED
            //---------------------------
                { L_, DEC( 2.3),  2  },
                { L_, DEC( 2.5),  3  },
                { L_, DEC(-2.3), -2  },
                { L_, DEC(-2.5), -3  },
                { L_, NAN_P,     NaN },
                { L_, NAN_N,     NaN },
                { L_, INF_P,     NaN },
                { L_, INF_N,     NaN },
                { L_, ZERO_P,     0  },
                { L_, ZERO_N,     0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE     = DATA[ti].d_line;
                const Obj&      X        = DATA[ti].d_x;
                const long int& EXPECTED = DATA[ti].d_expected;
                const long int  RESULT   = Util::lround(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ T_ bsl::cout << "round(x, p)" << bsl::endl; }
        {
            struct {
                int          d_line;
                Obj          d_x;
                unsigned int d_precision;
                Obj          d_expected;
                int          d_errno;
            } DATA[] = {
            //--------------------------------------------------------------
            // LINE |      X          | PRECISION | EXPECTED        | ERRNO
            //--------------------------------------------------------------
            //--------------------------------------------------------------
            //                          Test normal values
            //--------------------------------------------------------------
                { L_, DEC( 1234567e-4),          3, DEC( 123457e-3),     0 },
                { L_, DEC(-1234567e-5),          4, DEC(-123457e-4),     0 },
                { L_, DEC(7654321e-101),        96, DEC( 77e-96),        0 },
                { L_, DEC(-765432e-101),        97, DEC(-77e-97),        0 },

            //--------------------------------------------------------------
            //                          Test special values
            //--------------------------------------------------------------
                { L_, ZERO_P,                    0, ZERO_P,              0 },
                { L_, ZERO_N,                    0, ZERO_N,              0 },
                { L_, INF_P,                     0, INF_P,               0 },
                { L_, INF_N,                     0, INF_N,               0 },
                { L_, NAN_P,                     0, NAN_P,               0 },
                { L_, NAN_N,                     0, NAN_N,               0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const Obj&          X         = DATA[ti].d_x;
                const unsigned int& PRECISION = DATA[ti].d_precision;
                const Obj&          EXPECTED  = DATA[ti].d_expected;
                const int&          ERRNO     = DATA[ti].d_errno;

                errno = 0;
                const Obj RESULT = Util::round(X, PRECISION);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                }
                LOOP3_ASSERT(LINE, ERRNO, errno, ERRNO == errno);
            }
        }

        if (veryVerbose) { T_ T_ bsl::cout << "trunc(x, p)" << bsl::endl; }
        {
            struct {
                int          d_line;
                Obj          d_x;
                unsigned int d_precision;
                Obj          d_expected;
                int          d_errno;
            } DATA[] = {
            //--------------------------------------------------------------
            // LINE |      X          | PRECISION | EXPECTED        | ERRNO
            //--------------------------------------------------------------
            //--------------------------------------------------------------
            //                          Test normal values
            //--------------------------------------------------------------
                { L_, DEC( 1234567e-4),          0, DEC( 123e-0),        0 },
                { L_, DEC( 1234567e-4),          1, DEC( 1234e-1),       0 },
                { L_, DEC( 1234567e-4),          2, DEC( 12345e-2),      0 },
                { L_, DEC( 1234567e-4),          3, DEC( 123456e-3),     0 },
                { L_, DEC( 1234567e-4),          4, DEC( 1234567e-4),    0 },
                { L_, DEC( 1234567e-4),          5, DEC( 1234567e-4),    0 },
                { L_, DEC( 1234567e-4),          6, DEC( 1234567e-4),    0 },

                { L_, DEC(-1234567e-5),          0, DEC(-12e-0),         0 },
                { L_, DEC(-1234567e-5),          1, DEC(-123e-1),        0 },
                { L_, DEC(-1234567e-5),          2, DEC(-1234e-2),       0 },
                { L_, DEC(-1234567e-5),          3, DEC(-12345e-3),      0 },
                { L_, DEC(-1234567e-5),          4, DEC(-123456e-4),     0 },
                { L_, DEC(-1234567e-5),          5, DEC(-1234567e-5),    0 },
                { L_, DEC(-1234567e-5),          6, DEC(-1234567e-5),    0 },

                { L_, DEC( 1234567e-6),          0, DEC( 1e-0),          0 },
                { L_, DEC( 1234567e-6),          1, DEC( 12e-1),         0 },
                { L_, DEC( 1234567e-6),          2, DEC( 123e-2),        0 },
                { L_, DEC( 1234567e-6),          3, DEC( 1234e-3),       0 },
                { L_, DEC( 1234567e-6),          4, DEC( 12345e-4),      0 },
                { L_, DEC( 1234567e-6),          5, DEC( 123456e-5),     0 },
                { L_, DEC( 1234567e-6),          6, DEC( 1234567e-6),    0 },

                { L_, DEC(-1234567e-7),          0, DEC( 0e-0),          0 },
                { L_, DEC(-1234567e-7),          1, DEC(-1e-1),          0 },
                { L_, DEC(-1234567e-7),          2, DEC(-12e-2),         0 },
                { L_, DEC(-1234567e-7),          3, DEC(-123e-3),        0 },
                { L_, DEC(-1234567e-7),          4, DEC(-1234e-4),       0 },
                { L_, DEC(-1234567e-7),          5, DEC(-12345e-5),      0 },
                { L_, DEC(-1234567e-7),          6, DEC(-123456e-6),     0 },

                { L_, DEC( 1234567e-8),          0, DEC( 0e-0),          0 },
                { L_, DEC( 1234567e-8),          1, DEC( 0e-0),          0 },
                { L_, DEC( 1234567e-8),          2, DEC( 1e-2),          0 },
                { L_, DEC( 1234567e-8),          3, DEC( 12e-3),         0 },
                { L_, DEC( 1234567e-8),          4, DEC( 123e-4),        0 },
                { L_, DEC( 1234567e-8),          5, DEC( 1234e-5),       0 },
                { L_, DEC( 1234567e-8),          6, DEC( 12345e-6),      0 },

                { L_, DEC( 3456000e-5),          0, DEC( 34e-0),         0 },
                { L_, DEC( 3456000e-5),          1, DEC( 345e-1),        0 },
                { L_, DEC( 3456000e-5),          2, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          3, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          4, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          5, DEC( 3456e-2),       0 },

                { L_, DEC(7654321e-101),       102, DEC( 7654321e-101),  0 },
                { L_, DEC(7654321e-101),       101, DEC( 7654321e-101),  0 },
                { L_, DEC(7654321e-101),       100, DEC( 765432e-100),   0 },
                { L_, DEC(7654321e-101),        99, DEC( 76543e-99),     0 },
                { L_, DEC(7654321e-101),        98, DEC( 7654e-98),      0 },
                { L_, DEC(7654321e-101),        97, DEC( 765e-97),       0 },
                { L_, DEC(7654321e-101),        96, DEC( 76e-96),        0 },
                { L_, DEC(7654321e-101),        95, DEC( 7e-95),         0 },
                { L_, DEC(7654321e-101),        94, DEC( 0e-0),          0 },
                { L_, DEC(7654321e-101),        93, DEC( 0e-0),          0 },

                { L_, DEC( 3456e+0),             0, DEC( 3456e+0),       0 },
                { L_, DEC( 3456e+1),             0, DEC( 3456e+1),       0 },
                { L_, DEC( 3456e+2),             0, DEC( 3456e+2),       0 },

                { L_, DEC( 3456e+0),           101, DEC( 3456e+0),       0 },
                { L_, DEC( 3456e+1),           101, DEC( 3456e+1),       0 },
                { L_, DEC( 3456e+2),           101, DEC( 3456e+2),       0 },
            //--------------------------------------------------------------
            //                          Test subnormal values
            //--------------------------------------------------------------
                { L_, DEC(-765432e-101),       102, DEC(-765432e-101),   0 },
                { L_, DEC(-765432e-101),       101, DEC(-765432e-101),   0 },
                { L_, DEC(-765432e-101),       100, DEC(-76543e-100),    0 },
                { L_, DEC(-765432e-101),        99, DEC(-7654e-99),      0 },
                { L_, DEC(-765432e-101),        98, DEC(-765e-98),       0 },
                { L_, DEC(-765432e-101),        97, DEC(-76e-97),        0 },
                { L_, DEC(-765432e-101),        96, DEC(-7e-96),         0 },
                { L_, DEC(-765432e-101),        95, DEC(-0e-0),          0 },
                { L_, DEC(-765432e-101),        94, DEC( 0e-0),          0 },
            //--------------------------------------------------------------
            //                          Test special values
            //--------------------------------------------------------------
                { L_, ZERO_P,                    0, ZERO_P,              0 },
                { L_, ZERO_N,                    0, ZERO_N,              0 },
                { L_, INF_P,                     0, INF_P,               0 },
                { L_, INF_N,                     0, INF_N,               0 },
                { L_, NAN_P,                     0, NAN_P,               0 },
                { L_, NAN_N,                     0, NAN_N,               0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const Obj&          X         = DATA[ti].d_x;
                const unsigned int& PRECISION = DATA[ti].d_precision;
                const Obj&          EXPECTED  = DATA[ti].d_expected;
                const int&          ERRNO     = DATA[ti].d_errno;

                errno = 0;
                const Obj RESULT = Util::trunc(X, PRECISION);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                }
                LOOP3_ASSERT(LINE, ERRNO, errno, ERRNO == errno);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "sqrt()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |     X     |   EXPECTED
            //-----------------------------------
                { L_, DEC(100.0), DEC(10.0)     },
                { L_, DEC(  2.0), DEC(1.414214) },
                { L_, DEC(-1.0),  NAN_P         },
                { L_, INF_P,      INF_P         },
                { L_, INF_N,      NAN_P         },
                { L_, NAN_P,      NAN_P         },
                { L_, NAN_N,      NAN_N         },
                { L_, ZERO_P,     ZERO_P        },
                { L_, ZERO_N,     ZERO_N        },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::sqrt(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }
#undef DEC
    }
#define DEC(X) BDLDFP_DECIMAL_DD(X)
    if (veryVerbose) bsl::cout << "\nDecimal64"
                               << "\n---------"
                               << bsl::endl;
    {
        typedef BDEC::Decimal64 Obj;

        const Obj NAN_P   = bsl::numeric_limits<Obj>::quiet_NaN();
        const Obj NAN_N   = -NAN_P;
        const Obj INF_P   = bsl::numeric_limits<Obj>::infinity();
        const Obj INF_N   = -INF_P;
        const Obj ZERO_P  = DEC( 0.0);
        const Obj ZERO_N  = DEC(-0.0);

        if (veryVerbose) { T_ bsl::cout << "copySign()" << bsl::endl; }
        {
            const Obj DEC_X  = DEC(1234567890123456.0);
            const Obj DEC_Y  = DEC(0.1234567890123456);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //---------------------------------
            // LINE |  X    |    Y   | EXPECTED
            //---------------------------------
                { L_, DEC_X,   DEC_Y,   DEC_X },
                { L_, DEC_X,   DEC_Y,   DEC_X },
                { L_, DEC_Y,  -DEC_X,  -DEC_Y },
                { L_, DEC_Y,  -DEC_X,  -DEC_Y },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::copySign(X, Y);

                LOOP_ASSERT(LINE, RESULT ==  EXPECTED);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "exp()" << bsl::endl; }
        {
            const Obj DEC_X1 = DEC( 12.000046);
            const Obj DEC_X2 = DEC(-11.000046);
            const Obj RES_1  = DEC(1.627622783116064e+5);
            const Obj RES_2  = DEC(1.670093252967944e-5);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------------------
            // LINE |     X     |      EXPECTED
            //-----------------------------------------------
                { L_, DEC_X1,     RES_1    },
                { L_, DEC_X2,     RES_2    },
                { L_, NAN_P,      NAN_P    },
                { L_, NAN_N,      NAN_N    },
                { L_, INF_P,      INF_P    },
                { L_, INF_N,      ZERO_P   },
                { L_, ZERO_P,     DEC(1.0) },
                { L_, ZERO_N,     DEC(1.0) },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::exp(X);

                if (Util::isNan(X)) {
                    LOOP2_ASSERT(LINE, EXPECTED, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "log()" << bsl::endl; }
        {
            const Obj DEC_X = DEC(12.000046);
            const Obj RES_1 = DEC( 2.484910483113986);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //------------------------------
            // LINE |     X     | EXPECTED
            //------------------------------
                { L_, DEC(  1.0), ZERO_P   },
                { L_, DEC_X,      RES_1    },
                { L_, DEC(-11.0), NAN_N    },
                { L_, NAN_P,      NAN_P    },
                { L_, NAN_N,      NAN_N    },
                { L_, INF_P,      INF_P    },
                { L_, INF_N,      NAN_N    },
                { L_, ZERO_P,     INF_N    },
                { L_, ZERO_N,     INF_N    },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::log(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "logB()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |       X        | EXPECTED
            //-----------------------------------
                { L_, DEC(  1.0),      ZERO_P   },
                { L_, DEC( 99.999999), DEC(1.0) },
                { L_, DEC(-11.0),      DEC(1.0) },
                { L_, NAN_P,           NAN_P    },
                { L_, NAN_N,           NAN_N    },
                { L_, INF_P,           INF_P    },
                { L_, INF_N,           INF_P    },
                { L_, ZERO_P,          INF_N    },
                { L_, ZERO_N,          INF_N    },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::logB(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "log10()" << bsl::endl; }
        {
            const Obj DEC_X = DEC(12.000046);
            const Obj RES_1 = DEC(1.079182910839948);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //----------------------------
            // LINE |     X     | EXPECTED
            //----------------------------
                { L_, DEC(  1.0), ZERO_P },
                { L_, DEC_X,      RES_1  },
                { L_, DEC(-11.0), NAN_N  },
                { L_, NAN_P,      NAN_P  },
                { L_, NAN_N,      NAN_N  },
                { L_, INF_P,      INF_P  },
                { L_, INF_N,      NAN_N  },
                { L_, ZERO_P,     INF_N  },
                { L_, ZERO_N,     INF_N  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::log10(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "fmod()" << bsl::endl; }
        {
            const Obj DEC_X   = DEC( 5.0000002);
            const Obj DEC_Y   = DEC( 3.0000001);
            const Obj DEC_R   = DEC( 2.0000001);
            const Obj DEC_X_N = DEC(-5.0000002);
            const Obj DEC_Y_N = DEC(-3.0000001);
            const Obj DEC_R_N = DEC(-2.0000001);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |    X   |    Y   | EXPECTED
            //-----------------------------------
                { L_, DEC_X,   DEC_Y,   DEC_R   },
                { L_, DEC_X_N, DEC_Y,   DEC_R_N },
                { L_, DEC_X,   DEC_Y_N, DEC_R   },
                { L_, DEC_X_N, DEC_Y_N, DEC_R_N },
                { L_, DEC_X,   NAN_P,   NAN_P   },
                { L_, DEC_X,   NAN_N,   NAN_P   },
                { L_, DEC_X,   INF_P,   DEC_X   },
                { L_, DEC_X,   INF_N,   DEC_X   },
                { L_, DEC_X,   ZERO_P,  NAN_P   },
                { L_, DEC_X,   ZERO_N,  NAN_P   },
                { L_, NAN_P,   DEC_Y,   NAN_P   },
                { L_, INF_P,   DEC_Y,   NAN_P   },
                { L_, ZERO_P,  DEC_Y,   ZERO_P  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::fmod(X, Y);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "remainder()" << bsl::endl; }
        {
            const Obj DEC_X   = DEC( 5.0000003);
            const Obj DEC_Y   = DEC( 3.0000001);
            const Obj DEC_R   = DEC( 0.9999999);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |    X   |    Y   | EXPECTED
            //-----------------------------------
                { L_,  DEC_X,   DEC_Y,  -DEC_R  },
                { L_, -DEC_X,   DEC_Y,   DEC_R  },
                { L_,  DEC_X,  -DEC_Y,  -DEC_R  },
                { L_, -DEC_X,  -DEC_Y,   DEC_R  },
                { L_,  DEC_X,   NAN_P,   NAN_P  },
                { L_,  DEC_X,   NAN_N,   NAN_P  },
                { L_,  DEC_X,   INF_P,   DEC_X  },
                { L_, -DEC_X,   INF_N,  -DEC_X  },
                { L_,  DEC_X,   ZERO_P,  NAN_P  },
                { L_,  DEC_X,   ZERO_N,  NAN_P  },
                { L_,  NAN_P,   DEC_Y,   NAN_P  },
                { L_,  INF_P,   DEC_Y,   NAN_P  },
                { L_,  ZERO_P,  DEC_Y,   ZERO_P },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::remainder(X, Y);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "lrint()" << bsl::endl; }
        {
            const long int NaN = ~(-1ul >> 1);

            struct {
                int      d_line;
                Obj      d_x;
                long int d_expected;
            } DATA[] = {
            //---------------------------------
            // LINE |       X        | EXPECTED
            //---------------------------------
                { L_, DEC( 2.3),        2    },
                { L_, DEC( 2.5),        2    },
                { L_, DEC( 3.5),        4    },
                { L_, DEC(-2.3),       -2    },
                { L_, DEC(-2.5),       -2    },
                { L_, DEC(-3.4999999), -3    },
                { L_, DEC(-3.5),       -4    },
                { L_, NAN_P,           NaN   },
                { L_, NAN_N,           NaN   },
                { L_, INF_P,           NaN   },
                { L_, INF_N,           NaN   },
                { L_, ZERO_P,           0    },
                { L_, ZERO_N,           0    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE     = DATA[ti].d_line;
                const Obj&      X        = DATA[ti].d_x;
                const long int& EXPECTED = DATA[ti].d_expected;
                const long int  RESULT   = Util::lrint(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "llrint()" << bsl::endl; }
        {
            const long long int NaN = ~(-1ull >> 1);

            struct {
                int           d_line;
                Obj           d_x;
                long long int d_expected;
            } DATA[] = {
            //---------------------------------
            // LINE |       X        | EXPECTED
            //---------------------------------
                { L_, DEC( 2.3),        2    },
                { L_, DEC( 2.5),        2    },
                { L_, DEC( 3.5),        4    },
                { L_, DEC(-2.3),       -2    },
                { L_, DEC(-2.5),       -2    },
                { L_, DEC(-3.4999999), -3    },
                { L_, DEC(-3.5),       -4    },
                { L_, NAN_P,           NaN   },
                { L_, NAN_N,           NaN   },
                { L_, INF_P,           NaN   },
                { L_, INF_N,           NaN   },
                { L_, ZERO_P,           0    },
                { L_, ZERO_N,           0    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE     = DATA[ti].d_line;
                const Obj&           X        = DATA[ti].d_x;
                const long long int& EXPECTED = DATA[ti].d_expected;
                const long long int  RESULT   = Util::llrint(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "nextafter()" << bsl::endl; }
        {
            const Obj SUBN_P =  bsl::numeric_limits<Obj>::denorm_min();
            const Obj MAX_P  =  bdldfp::DecimalImpUtil::parse64(
                                                     "9.999999999999999e+384");
            const Obj MAX_N  =  bdldfp::DecimalImpUtil::parse64(
                                                    "-9.999999999999999e+384");

            struct {
                int d_line;
                Obj d_from;
                Obj d_to;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------------------------------
            // LINE |   FROM  |     TO   |       EXPECTED
            //-----------------------------------------------------------
                { L_, DEC(0.0),  DEC(1.0), SUBN_P                       },
                { L_, DEC(1.0),  DEC(2.0), DEC( 1.000000000000001)      },
                { L_, DEC(1.0),  DEC(0.0), DEC( 9.999999999999999e-1)   },
                { L_, DEC(0.0),  DEC(0.0), DEC( 0.0)                    },
                { L_, NAN_P,     NAN_P,    NAN_P                        },
                { L_, NAN_N,     NAN_N,    NAN_N                        },
                { L_, INF_P,     INF_P,    INF_P                        },
                { L_, INF_N,     INF_N,    INF_N                        },
                { L_, INF_P,     DEC(2.0), MAX_P                        },
                { L_, INF_N,     DEC(2.0), MAX_N                        },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& FROM     = DATA[ti].d_from;
                const Obj& TO       = DATA[ti].d_to;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::nextafter(FROM, TO);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "nexttoward()" << bsl::endl; }
        {
            const Obj SUBN_64   = bsl::numeric_limits<Obj>::denorm_min();
            const Obj NAN_S_64  = bsl::numeric_limits<Obj>::signaling_NaN();
            const Obj NAN_Q_64  = bsl::numeric_limits<Obj>::quiet_NaN();

            typedef BDEC::Decimal128 Obj128;

            const Obj128 SUBN_128  =
                                  bsl::numeric_limits<Obj128>::denorm_min();
            const Obj128 NAN_Q_128 =
                                  bsl::numeric_limits<Obj128>::quiet_NaN();
            const Obj128 NAN_S_128 =
                                  bsl::numeric_limits<Obj128>::signaling_NaN();
            const Obj128 INF_128   =
                                  bsl::numeric_limits<Obj128>::infinity();

            struct {
                int    d_line;
                Obj    d_from;
                Obj128 d_to;
                Obj    d_expected;
            } DATA[] = {
             //-----------------------------------------
             //LINE |  FROM    |    TO     | EXPECTED
             //-----------------------------------------
                { L_,  DEC(0.0),  SUBN_128,   SUBN_64  },
                { L_,  DEC(0.0), -SUBN_128,  -SUBN_64  },
                { L_,  NAN_Q_64,  NAN_Q_128,  NAN_Q_64 },
                { L_,  NAN_S_64,  NAN_S_128,  NAN_S_64 },
                { L_,  INF_P,     INF_128,    INF_P    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE     = DATA[ti].d_line;
                const Obj&    FROM     = DATA[ti].d_from;
                const Obj128& TO       = DATA[ti].d_to;
                const Obj&    EXPECTED = DATA[ti].d_expected;
                const Obj     RESULT   = Util::nexttoward(FROM, TO);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose)  { T_ bsl::cout << "pow()" << bsl::endl; }
        {
            const Obj SQRT_2 = DEC(1.414213562373095);
            const Obj DEC_Y  = DEC(0.333333333333333);

            struct {
                int d_line;
                Obj d_base;
                Obj d_exponent;
                Obj d_expected;
            } DATA[] = {
            //----------------------------------------------
            // LINE |   BASE   |  EXPONENT |   EXPECTED
            //----------------------------------------------
                { L_, DEC( 2.0),  DEC(10.0), DEC(1024.0)   },
                { L_, DEC( 2.0),  DEC( 0.5), SQRT_2        },
                { L_, DEC(-2.0),  DEC(-3.0), DEC(  -0.125) },
                { L_, DEC(-1.0),  NAN_P,     NAN_P         },
                { L_, DEC( 1.0),  NAN_P,     DEC(   1.0)   },
                { L_, INF_P,      DEC( 2.0), INF_P         },
                { L_, INF_P,      DEC(-1.0), DEC(   0.0)   },
                { L_, DEC(-1.0),  DEC_Y,     NAN_N         },
                { L_, ZERO_N,     DEC(-3.0), INF_N         },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& BASE     = DATA[ti].d_base;
                const Obj& EXPONENT = DATA[ti].d_exponent;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::pow(BASE, EXPONENT);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "lround()" << bsl::endl; }
        {
            const long int NaN = ~(-1ul >> 1);

            struct {
                int      d_line;
                Obj      d_x;
                long int d_expected;
            } DATA[] = {
            //---------------------------------
            // LINE |     X          | EXPECTED
            //---------------------------------
                { L_, DEC( 2.3),        2  },
                { L_, DEC( 2.4999999),  2  },
                { L_, DEC( 2.5),        3  },
                { L_, DEC(-2.3),       -2  },
                { L_, DEC(-2.5),       -3  },
                { L_, NAN_P,           NaN },
                { L_, NAN_N,           NaN },
                { L_, INF_P,           NaN },
                { L_, INF_N,           NaN },
                { L_, ZERO_P,           0  },
                { L_, ZERO_N,           0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE     = DATA[ti].d_line;
                const Obj&      X        = DATA[ti].d_x;
                const long int& EXPECTED = DATA[ti].d_expected;
                const long int  RESULT   = Util::lround(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ T_ bsl::cout << "round(x, p)" << bsl::endl; }
        {
            struct {
                int          d_line;
                Obj          d_x;
                unsigned int d_precision;
                Obj          d_expected;
                int          d_errno;
            } DATA[] = {
            //--------------------------------------------------------------
            // LINE |      X          | PRECISION | EXPECTED        | ERRNO
            //--------------------------------------------------------------
            //--------------------------------------------------------------
            //                        Test normal values
            //--------------------------------------------------------------
                { L_, DEC(-1234567e-5),          4, DEC(-123457e-4),     0 },
                { L_, DEC( 1234567e-6),          5, DEC( 123457e-5),     0 },

            //--------------------------------------------------------------
            //                          Test special values
            //--------------------------------------------------------------
                { L_, ZERO_P,                    0, ZERO_P,              0 },
                { L_, ZERO_N,                    0, ZERO_N,              0 },
                { L_, INF_P,                     0, INF_P,               0 },
                { L_, INF_N,                     0, INF_N,               0 },
                { L_, NAN_P,                     0, NAN_P,               0 },
                { L_, NAN_N,                     0, NAN_N,               0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const Obj&          X         = DATA[ti].d_x;
                const unsigned int& PRECISION = DATA[ti].d_precision;
                const Obj&          EXPECTED  = DATA[ti].d_expected;
                const int&          ERRNO     = DATA[ti].d_errno;

                errno = 0;
                const Obj RESULT = Util::round(X, PRECISION);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                }
                LOOP3_ASSERT(LINE, ERRNO, errno, ERRNO == errno);
            }
        }

        if (veryVerbose) { T_ T_ bsl::cout << "trunc(x, p)" << bsl::endl; }
        {
            struct {
                int          d_line;
                Obj          d_x;
                unsigned int d_precision;
                Obj          d_expected;
                int          d_errno;
            } DATA[] = {
            //--------------------------------------------------------------
            // LINE |      X          | PRECISION | EXPECTED        | ERRNO
            //--------------------------------------------------------------
            //--------------------------------------------------------------
            //                        Test normal values
            //--------------------------------------------------------------
                { L_, DEC( 1234567e-4),          0, DEC( 123e-0),        0 },
                { L_, DEC( 1234567e-4),          1, DEC( 1234e-1),       0 },
                { L_, DEC( 1234567e-4),          2, DEC( 12345e-2),      0 },
                { L_, DEC( 1234567e-4),          3, DEC( 123456e-3),     0 },
                { L_, DEC( 1234567e-4),          4, DEC( 1234567e-4),    0 },
                { L_, DEC( 1234567e-4),          5, DEC( 1234567e-4),    0 },
                { L_, DEC( 1234567e-4),          6, DEC( 1234567e-4),    0 },

                { L_, DEC(-1234567e-5),          0, DEC(-12e-0),         0 },
                { L_, DEC(-1234567e-5),          1, DEC(-123e-1),        0 },
                { L_, DEC(-1234567e-5),          2, DEC(-1234e-2),       0 },
                { L_, DEC(-1234567e-5),          3, DEC(-12345e-3),      0 },
                { L_, DEC(-1234567e-5),          4, DEC(-123456e-4),     0 },
                { L_, DEC(-1234567e-5),          5, DEC(-1234567e-5),    0 },
                { L_, DEC(-1234567e-5),          6, DEC(-1234567e-5),    0 },

                { L_, DEC( 1234567e-6),          0, DEC( 1e-0),          0 },
                { L_, DEC( 1234567e-6),          1, DEC( 12e-1),         0 },
                { L_, DEC( 1234567e-6),          2, DEC( 123e-2),        0 },
                { L_, DEC( 1234567e-6),          3, DEC( 1234e-3),       0 },
                { L_, DEC( 1234567e-6),          4, DEC( 12345e-4),      0 },
                { L_, DEC( 1234567e-6),          5, DEC( 123456e-5),     0 },
                { L_, DEC( 1234567e-6),          6, DEC( 1234567e-6),    0 },

                { L_, DEC(-1234567e-7),          0, DEC( 0e-0),          0 },
                { L_, DEC(-1234567e-7),          1, DEC(-1e-1),          0 },
                { L_, DEC(-1234567e-7),          2, DEC(-12e-2),         0 },
                { L_, DEC(-1234567e-7),          3, DEC(-123e-3),        0 },
                { L_, DEC(-1234567e-7),          4, DEC(-1234e-4),       0 },
                { L_, DEC(-1234567e-7),          5, DEC(-12345e-5),      0 },
                { L_, DEC(-1234567e-7),          6, DEC(-123456e-6),     0 },

                { L_, DEC( 1234567e-8),          0, DEC( 0e-0),          0 },
                { L_, DEC( 1234567e-8),          1, DEC( 0e-0),          0 },
                { L_, DEC( 1234567e-8),          2, DEC( 1e-2),          0 },
                { L_, DEC( 1234567e-8),          3, DEC( 12e-3),         0 },
                { L_, DEC( 1234567e-8),          4, DEC( 123e-4),        0 },
                { L_, DEC( 1234567e-8),          5, DEC( 1234e-5),       0 },
                { L_, DEC( 1234567e-8),          6, DEC( 12345e-6),      0 },

                { L_, DEC( 3456000e-5),          0, DEC( 34e-0),         0 },
                { L_, DEC( 3456000e-5),          1, DEC( 345e-1),        0 },
                { L_, DEC( 3456000e-5),          2, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          3, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          4, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          5, DEC( 3456e-2),       0 },

                { L_, DEC( 3456e+0),             0, DEC( 3456e+0),       0 },
                { L_, DEC( 3456e+1),             0, DEC( 3456e+1),       0 },
                { L_, DEC( 3456e+2),             0, DEC( 3456e+2),       0 },


                { L_, DEC( 3456e+0),           101, DEC( 3456e+0),       0 },
                { L_, DEC( 3456e+1),           101, DEC( 3456e+1),       0 },
                { L_, DEC( 3456e+2),           101, DEC( 3456e+2),       0 },
            //--------------------------------------------------------------
            //                          Test subnormal values
            //--------------------------------------------------------------
                { L_, DEC(-7654321e-101),      102, DEC(-7654321e-101),  0 },
                { L_, DEC(-7654321e-101),      101, DEC(-7654321e-101),  0 },
                { L_, DEC(-7654321e-101),      100, DEC(-765432e-100),   0 },
                { L_, DEC(-7654321e-101),       99, DEC(-76543e-99),     0 },
                { L_, DEC(-7654321e-101),       98, DEC(-7654e-98),      0 },
                { L_, DEC(-7654321e-101),       97, DEC(-765e-97),       0 },
                { L_, DEC(-7654321e-101),       96, DEC(-76e-96),        0 },
                { L_, DEC(-7654321e-101),       95, DEC(-7e-95),         0 },
                { L_, DEC(-7654321e-101),       94, DEC(-0e-0),          0 },
                { L_, DEC(-7654321e-101),       93, DEC(-0e-0),          0 },
            //--------------------------------------------------------------
            //                          Test special values
            //--------------------------------------------------------------
                { L_, ZERO_P,                    0, ZERO_P,              0 },
                { L_, ZERO_N,                    0, ZERO_N,              0 },
                { L_, INF_P,                     0, INF_P,               0 },
                { L_, INF_N,                     0, INF_N,               0 },
                { L_, NAN_P,                     0, NAN_P,               0 },
                { L_, NAN_N,                     0, NAN_N,               0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const Obj&          X         = DATA[ti].d_x;
                const unsigned int& PRECISION = DATA[ti].d_precision;
                const Obj&          EXPECTED  = DATA[ti].d_expected;
                const int&          ERRNO     = DATA[ti].d_errno;

                errno = 0;
                const Obj RESULT = Util::trunc(X, PRECISION);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                }
                LOOP3_ASSERT(LINE, ERRNO, errno, ERRNO == errno);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "sqrt()" << bsl::endl; }
        {
            const Obj SQRT_2 = DEC(1.414213562373095);
            const Obj DEC_1  = DEC(8.999999999999996);
            const Obj RES_1  = DEC(2.999999999999999);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |     X     |   EXPECTED
            //-----------------------------------
                { L_, DEC(100.0),  DEC(10.0) },
                { L_, DEC_1,       RES_1     },
                { L_, DEC(  2.0),  SQRT_2    },
                { L_, DEC( -1.0),  NAN_P     },
                { L_, INF_P,       INF_P     },
                { L_, INF_N,       NAN_P     },
                { L_, NAN_P,       NAN_P     },
                { L_, NAN_N,       NAN_N     },
                { L_, ZERO_P,      ZERO_P    },
                { L_, ZERO_N,      ZERO_N    },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::sqrt(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

#undef DEC
    }
#define DEC(X) ImpUtil::parse128(#X)
    if (veryVerbose) bsl::cout << "\nDecimal128"
                               << "\n----------"
                               << bsl::endl;
    {
        typedef BDEC::Decimal128 Obj;

        const Obj NAN_P   = bsl::numeric_limits<Obj>::quiet_NaN();
        const Obj NAN_N   = -NAN_P;
        const Obj INF_P   = bsl::numeric_limits<Obj>::infinity();
        const Obj INF_N   = -INF_P;
        const Obj ZERO_P  = DEC( 0.0);
        const Obj ZERO_N  = DEC(-0.0);

        if (veryVerbose) { T_ bsl::cout << "copySign()" << bsl::endl; }
        {
            const Obj DEC_X  = DEC(1234567890123456789012345678901234.0);
            const Obj DEC_Y  = DEC(0.1234567890123456789012345678901234);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //---------------------------------
            // LINE |  X    |    Y   | EXPECTED
            //---------------------------------
                { L_, DEC_X,   DEC_Y,   DEC_X },
                { L_, DEC_X,   DEC_Y,   DEC_X },
                { L_, DEC_Y,  -DEC_X,  -DEC_Y },
                { L_, DEC_Y,  -DEC_X,  -DEC_Y },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::copySign(X, Y);

                LOOP_ASSERT(LINE, RESULT ==  EXPECTED);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "exp()" << bsl::endl; }
        {
            const Obj DEC_X1 = DEC( 12.000000000000046);
            const Obj DEC_X2 = DEC(-11.000000000000046);
            const Obj RES_1  = DEC(1.627547914190114075284104792510385e+5);
            const Obj RES_2  = DEC(1.670170079024489103439916607792290e-5);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //------------------------------
            // LINE |     X     | EXPECTED
            //------------------------------
                { L_, DEC_X1,         RES_1    },
                { L_, DEC_X2,         RES_2    },
                { L_, DEC(9999999.0), INF_P    },
                { L_, NAN_P,          NAN_P    },
                { L_, NAN_N,          NAN_N    },
                { L_, INF_P,          INF_P    },
                { L_, INF_N,          ZERO_P   },
                { L_, ZERO_P,         DEC(1.0) },
                { L_, ZERO_N,         DEC(1.0) },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::exp(X);

                if (Util::isNan(X)) {
                    LOOP2_ASSERT(LINE, EXPECTED, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "log()" << bsl::endl; }
        {
            const Obj DEC_X = DEC(12.000000000000046);
            const Obj RES_1 = DEC(2.484906649788004143563042813164865);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //----------------------------
            // LINE |     X     | EXPECTED
            //----------------------------
                { L_, DEC(  1.0), ZERO_P },
                { L_, DEC_X,      RES_1  },
                { L_, DEC(-11.0), NAN_N  },
                { L_, NAN_P,      NAN_P  },
                { L_, NAN_N,      NAN_N  },
                { L_, INF_P,      INF_P  },
                { L_, INF_N,      NAN_N  },
                { L_, ZERO_P,     INF_N  },
                { L_, ZERO_N,     INF_N  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::log(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "logB()" << bsl::endl; }
        {
            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //---------------------------------------------
            // LINE |       X                  | EXPECTED
            //---------------------------------------------
                { L_, DEC(  1.0),                ZERO_P   },
                { L_, DEC( 99.9999999999999999), DEC(1.0) },
                { L_, DEC(-11.0),                DEC(1.0) },
                { L_, NAN_P,                     NAN_P    },
                { L_, NAN_N,                     NAN_N    },
                { L_, INF_P,                     INF_P    },
                { L_, INF_N,                     INF_P    },
                { L_, ZERO_P,                    INF_N    },
                { L_, ZERO_N,                    INF_N    },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::logB(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "log10()" << bsl::endl; }
        {
            const Obj DEC_X = DEC(12.000000000000046);
            const Obj RES_1 = DEC(1.079181246047626492518019655166250);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //----------------------------
            // LINE |     X     | EXPECTED
            //----------------------------
                { L_, DEC(  1.0), ZERO_P },
                { L_, DEC_X,      RES_1  },
                { L_, DEC(-11.0), NAN_N  },
                { L_, NAN_P,      NAN_P  },
                { L_, NAN_N,      NAN_N  },
                { L_, INF_P,      INF_P  },
                { L_, INF_N,      NAN_N  },
                { L_, ZERO_P,     INF_N  },
                { L_, ZERO_N,     INF_N  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::log10(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "fmod()" << bsl::endl; }
        {
            const Obj DEC_X   = DEC( 5.0000000000000002);
            const Obj DEC_Y   = DEC( 3.0000000000000001);
            const Obj DEC_R   = DEC( 2.0000000000000001);
            const Obj DEC_X_N = DEC(-5.0000000000000002);
            const Obj DEC_Y_N = DEC(-3.0000000000000001);
            const Obj DEC_R_N = DEC(-2.0000000000000001);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |    X   |    Y   | EXPECTED
            //-----------------------------------
                { L_, DEC_X,   DEC_Y,   DEC_R   },
                { L_, DEC_X_N, DEC_Y,   DEC_R_N },
                { L_, DEC_X,   DEC_Y_N, DEC_R   },
                { L_, DEC_X_N, DEC_Y_N, DEC_R_N },
                { L_, DEC_X,   NAN_P,   NAN_P   },
                { L_, DEC_X,   NAN_N,   NAN_P   },
                { L_, DEC_X,   INF_P,   DEC_X   },
                { L_, DEC_X,   INF_N,   DEC_X   },
                { L_, DEC_X,   ZERO_P,  NAN_P   },
                { L_, DEC_X,   ZERO_N,  NAN_P   },
                { L_, NAN_P,   DEC_Y,   NAN_P   },
                { L_, INF_P,   DEC_Y,   NAN_P   },
                { L_, ZERO_P,  DEC_Y,   ZERO_P  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::fmod(X, Y);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "remainder()" << bsl::endl; }
        {
            const Obj DEC_X   = DEC( 5.0000000000000003);
            const Obj DEC_Y   = DEC( 3.0000000000000001);
            const Obj DEC_R   = DEC( 0.9999999999999999);

            struct {
                int d_line;
                Obj d_x;
                Obj d_y;
                Obj d_expected;
            } DATA[] = {
            //-----------------------------------
            // LINE |    X   |    Y   | EXPECTED
            //-----------------------------------
                { L_,  DEC_X,   DEC_Y,  -DEC_R  },
                { L_, -DEC_X,   DEC_Y,   DEC_R  },
                { L_,  DEC_X,  -DEC_Y,  -DEC_R  },
                { L_, -DEC_X,  -DEC_Y,   DEC_R  },
                { L_,  DEC_X,   NAN_P,   NAN_P  },
                { L_,  DEC_X,   NAN_N,   NAN_P  },
                { L_,  DEC_X,   INF_P,   DEC_X  },
                { L_, -DEC_X,   INF_N,  -DEC_X  },
                { L_,  DEC_X,   ZERO_P,  NAN_P  },
                { L_,  DEC_X,   ZERO_N,  NAN_P  },
                { L_,  NAN_P,   DEC_Y,   NAN_P  },
                { L_,  INF_P,   DEC_Y,   NAN_P  },
                { L_,  ZERO_P,  DEC_Y,   ZERO_P },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& Y        = DATA[ti].d_y;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::remainder(X, Y);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "lrint()" << bsl::endl; }
        {
            const long int NaN = ~(-1ul >> 1);

            struct {
                int      d_line;
                Obj      d_x;
                long int d_expected;
            } DATA[] = {
            //------------------------------------------
            // LINE |     X                   | EXPECTED
            //------------------------------------------
                { L_, DEC( 2.3),                 2  },
                { L_, DEC( 2.5),                 2  },
                { L_, DEC( 3.5),                 4  },
                { L_, DEC(-2.3),                -2  },
                { L_, DEC(-2.5),                -2  },
                { L_, DEC(-3.4999999999999999), -3  },
                { L_, DEC(-3.5),                -4  },
                { L_, NAN_P,                    NaN },
                { L_, NAN_N,                    NaN },
                { L_, INF_P,                    NaN },
                { L_, INF_N,                    NaN },
                { L_, ZERO_P,                    0  },
                { L_, ZERO_N,                    0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE     = DATA[ti].d_line;
                const Obj&      X        = DATA[ti].d_x;
                const long int& EXPECTED = DATA[ti].d_expected;
                const long int  RESULT   = Util::lrint(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "llrint()" << bsl::endl; }
        {
            const long long int NaN = ~(-1ull >> 1);
            struct {
                int           d_line;
                Obj           d_x;
                long long int d_expected;
            } DATA[] = {
            //------------------------------------------
            // LINE |       X                 | EXPECTED
            //------------------------------------------
                { L_, DEC( 2.3),                 2  },
                { L_, DEC( 2.5),                 2  },
                { L_, DEC( 3.5),                 4  },
                { L_, DEC(-2.3),                -2  },
                { L_, DEC(-2.5),                -2  },
                { L_, DEC(-3.4999999999999999), -3  },
                { L_, DEC(-3.5),                -4  },
                { L_, NAN_P,                    NaN },
                { L_, NAN_N,                    NaN },
                { L_, INF_P,                    NaN },
                { L_, INF_N,                    NaN },
                { L_, ZERO_P,                    0  },
                { L_, ZERO_N,                    0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE     = DATA[ti].d_line;
                const Obj&           X        = DATA[ti].d_x;
                const long long int& EXPECTED = DATA[ti].d_expected;
                const long long int  RESULT   = Util::llrint(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "nextafter()" << bsl::endl; }
        {
            const Obj DEC_1  = DEC( 1.000000000000000000000000000000001);
            const Obj DEC_9  = DEC( 9.999999999999999999999999999999999e-1);
            const Obj MAX_P  = bsl::numeric_limits<Obj>::max();
            const Obj MAX_N  = -MAX_P;
            const Obj SUBN_P = bsl::numeric_limits<Obj>::denorm_min();

            struct {
                int d_line;
                Obj d_from;
                Obj d_to;
                Obj d_expected;
            } DATA[] = {
            //---------------------------------------
            // LINE |   FROM  |     TO   | EXPECTED
            //---------------------------------------
                { L_, DEC(0.0),  DEC(1.0), SUBN_P   },
                { L_, DEC(1.0),  DEC(2.0), DEC_1    },
                { L_, DEC(1.0),  DEC(0.0), DEC_9    },
                { L_, DEC(0.0),  DEC(0.0), DEC(0.0) },
                { L_, NAN_P,     NAN_P,    NAN_P    },
                { L_, NAN_N,     NAN_N,    NAN_N    },
                { L_, INF_P,     INF_P,    INF_P    },
                { L_, INF_N,     INF_N,    INF_N    },
                { L_, INF_P,     DEC(1.0), MAX_P    },
                { L_, INF_N,     DEC(1.0), MAX_N    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& FROM     = DATA[ti].d_from;
                const Obj& TO       = DATA[ti].d_to;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::nextafter(FROM, TO);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "nexttoward()" << bsl::endl; }
        {
            const Obj SUBN  = bsl::numeric_limits<Obj>::denorm_min();
            const Obj NAN_Q = bsl::numeric_limits<Obj>::quiet_NaN();
            const Obj NAN_S = bsl::numeric_limits<Obj>::signaling_NaN();

            struct {
                int d_line;
                Obj d_from;
                Obj d_to;
                Obj d_expected;
            } DATA[] = {
             //-----------------------------------
             //LINE |  FROM     | TO   | EXPECTED
             //-----------------------------------
                { L_,  DEC(0.0),  SUBN,   SUBN  },
                { L_,  DEC(0.0), -SUBN,  -SUBN  },
                { L_,  NAN_Q,     NAN_Q,  NAN_Q },
                { L_,  NAN_S,     NAN_S,  NAN_S },
                { L_,  INF_P,     INF_P,  INF_P     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& FROM     = DATA[ti].d_from;
                const Obj& TO       = DATA[ti].d_to;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::nexttoward(FROM, TO);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }

        if (veryVerbose) { T_ bsl::cout << "pow()" << bsl::endl; }
        {
            const Obj SQRT_2 = DEC(1.414213562373095048801688724209698);
            const Obj DEC_Y1 = DEC(2.9999999999999999);
            const Obj DEC_Y2 = DEC(0.3333333333333333);
            const Obj DEC_R1 = DEC(7.999999999999999445482255552043771);

            struct {
                int d_line;
                Obj d_base;
                Obj d_exponent;
                Obj d_expected;
            } DATA[] = {
            //----------------------------------------------
            // LINE |   BASE   |  EXPONENT |   EXPECTED
            //----------------------------------------------
                { L_, DEC( 2.0),  DEC(10.0),  DEC( 1024.0) },
                { L_, DEC( 2.0),  DEC_Y1,     DEC_R1       },
                { L_, DEC( 2.0),  DEC( 0.5),  SQRT_2       },
                { L_, DEC(-2.0),  DEC(-3.0),  DEC(-0.125)  },
                { L_, DEC(-1.0),  NAN_P,      NAN_P        },
                { L_, DEC( 1.0),  NAN_P,      DEC( 1.0)    },
                { L_, INF_P,      DEC( 2.0),  INF_P        },
                { L_, INF_P,      DEC(-1.0),  DEC( 0.0)    },
                { L_, DEC(-1.0),  DEC_Y2,     NAN_N        },
                { L_, ZERO_N,     DEC(-3.0),  INF_N        },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& BASE     = DATA[ti].d_base;
                const Obj& EXPONENT = DATA[ti].d_exponent;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::pow(BASE, EXPONENT);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }
        if (veryVerbose) { T_ bsl::cout << "lround()" << bsl::endl; }
        {
            const long int NaN = ~(-1ul >> 1);
            const Obj      DEC_1 = DEC(2.4999999999999999);

            struct {
                int      d_line;
                Obj      d_x;
                long int d_expected;
            } DATA[] = {
            //---------------------------
            // LINE |    X     | EXPECTED
            //---------------------------
                { L_, DEC( 2.3),  2  },
                { L_, DEC_1,      2  },
                { L_, DEC( 2.5),  3  },
                { L_, DEC(-2.3), -2  },
                { L_, DEC(-2.5), -3  },
                { L_, NAN_P,     NaN },
                { L_, NAN_N,     NaN },
                { L_, INF_P,     NaN },
                { L_, INF_N,     NaN },
                { L_, ZERO_P,     0  },
                { L_, ZERO_N,     0  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE     = DATA[ti].d_line;
                const Obj&      X        = DATA[ti].d_x;
                const long int& EXPECTED = DATA[ti].d_expected;
                const long int  RESULT   = Util::lround(X);

                LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (veryVerbose) { T_ T_ bsl::cout << "round(x, p)" << bsl::endl; }
        {
            struct {
                int          d_line;
                Obj          d_x;
                unsigned int d_precision;
                Obj          d_expected;
                int          d_errno;
            } DATA[] = {
            //--------------------------------------------------------------
            // LINE |      X          | PRECISION | EXPECTED        | ERRNO
            //--------------------------------------------------------------
            //--------------------------------------------------------------
            //                          Test normal values
            //--------------------------------------------------------------
                { L_, DEC( 1234567e-4),          3, DEC( 123457e-3),     0 },
                { L_, DEC(-1234567e-5),          4, DEC(-123457e-4),     0 },

            //--------------------------------------------------------------
            //                          Test special values
            //--------------------------------------------------------------
                { L_, ZERO_P,                    0, ZERO_P,              0 },
                { L_, ZERO_N,                    0, ZERO_N,              0 },
                { L_, INF_P,                     0, INF_P,               0 },
                { L_, INF_N,                     0, INF_N,               0 },
                { L_, NAN_P,                     0, NAN_P,               0 },
                { L_, NAN_N,                     0, NAN_N,               0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const Obj&          X         = DATA[ti].d_x;
                const unsigned int& PRECISION = DATA[ti].d_precision;
                const Obj&          EXPECTED  = DATA[ti].d_expected;
                const int&          ERRNO     = DATA[ti].d_errno;

                errno = 0;
                const Obj RESULT = Util::round(X, PRECISION);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                }
                LOOP3_ASSERT(LINE, ERRNO, errno, ERRNO == errno);
            }
        }

        if (veryVerbose) { T_ T_ bsl::cout << "trunc(x, p)" << bsl::endl; }
        {
            struct {
                int          d_line;
                Obj          d_x;
                unsigned int d_precision;
                Obj          d_expected;
                int          d_errno;
            } DATA[] = {
            //--------------------------------------------------------------
            // LINE |      X          | PRECISION | EXPECTED        | ERRNO
            //--------------------------------------------------------------
            //--------------------------------------------------------------
            //                          Test normal values
            //--------------------------------------------------------------
                { L_, DEC( 1234567e-4),          0, DEC( 123e-0),        0 },
                { L_, DEC( 1234567e-4),          1, DEC( 1234e-1),       0 },
                { L_, DEC( 1234567e-4),          2, DEC( 12345e-2),      0 },
                { L_, DEC( 1234567e-4),          3, DEC( 123456e-3),     0 },
                { L_, DEC( 1234567e-4),          4, DEC( 1234567e-4),    0 },
                { L_, DEC( 1234567e-4),          5, DEC( 1234567e-4),    0 },
                { L_, DEC( 1234567e-4),          6, DEC( 1234567e-4),    0 },

                { L_, DEC(-1234567e-5),          0, DEC(-12e-0),         0 },
                { L_, DEC(-1234567e-5),          1, DEC(-123e-1),        0 },
                { L_, DEC(-1234567e-5),          2, DEC(-1234e-2),       0 },
                { L_, DEC(-1234567e-5),          3, DEC(-12345e-3),      0 },
                { L_, DEC(-1234567e-5),          4, DEC(-123456e-4),     0 },
                { L_, DEC(-1234567e-5),          5, DEC(-1234567e-5),    0 },
                { L_, DEC(-1234567e-5),          6, DEC(-1234567e-5),    0 },

                { L_, DEC( 1234567e-6),          0, DEC( 1e-0),          0 },
                { L_, DEC( 1234567e-6),          1, DEC( 12e-1),         0 },
                { L_, DEC( 1234567e-6),          2, DEC( 123e-2),        0 },
                { L_, DEC( 1234567e-6),          3, DEC( 1234e-3),       0 },
                { L_, DEC( 1234567e-6),          4, DEC( 12345e-4),      0 },
                { L_, DEC( 1234567e-6),          5, DEC( 123456e-5),     0 },
                { L_, DEC( 1234567e-6),          6, DEC( 1234567e-6),    0 },

                { L_, DEC(-1234567e-7),          0, DEC( 0e-0),          0 },
                { L_, DEC(-1234567e-7),          1, DEC(-1e-1),          0 },
                { L_, DEC(-1234567e-7),          2, DEC(-12e-2),         0 },
                { L_, DEC(-1234567e-7),          3, DEC(-123e-3),        0 },
                { L_, DEC(-1234567e-7),          4, DEC(-1234e-4),       0 },
                { L_, DEC(-1234567e-7),          5, DEC(-12345e-5),      0 },
                { L_, DEC(-1234567e-7),          6, DEC(-123456e-6),     0 },

                { L_, DEC( 1234567e-8),          0, DEC( 0e-0),          0 },
                { L_, DEC( 1234567e-8),          1, DEC( 0e-0),          0 },
                { L_, DEC( 1234567e-8),          2, DEC( 1e-2),          0 },
                { L_, DEC( 1234567e-8),          3, DEC( 12e-3),         0 },
                { L_, DEC( 1234567e-8),          4, DEC( 123e-4),        0 },
                { L_, DEC( 1234567e-8),          5, DEC( 1234e-5),       0 },
                { L_, DEC( 1234567e-8),          6, DEC( 12345e-6),      0 },

                { L_, DEC( 3456000e-5),          0, DEC( 34e-0),         0 },
                { L_, DEC( 3456000e-5),          1, DEC( 345e-1),        0 },
                { L_, DEC( 3456000e-5),          2, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          3, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          4, DEC( 3456e-2),       0 },
                { L_, DEC( 3456000e-5),          5, DEC( 3456e-2),       0 },

                { L_, DEC( 3456e+0),             0, DEC( 3456e+0),       0 },
                { L_, DEC( 3456e+1),             0, DEC( 3456e+1),       0 },
                { L_, DEC( 3456e+2),             0, DEC( 3456e+2),       0 },

                { L_, DEC( 3456e+0),          6111, DEC( 3456e+0),       0 },
                { L_, DEC( 3456e+1),          6111, DEC( 3456e+1),       0 },
                { L_, DEC( 3456e+2),          6111, DEC( 3456e+2),       0 },
            //--------------------------------------------------------------
            //                          Test subnormal values
            //--------------------------------------------------------------
                { L_, DEC(-765432e-6176),     6177, DEC(-765432e-6176),  0 },
                { L_, DEC(-765432e-6176),     6176, DEC(-765432e-6176),  0 },
                { L_, DEC(-765432e-6176),     6175, DEC(-76543e-6175),   0 },
                { L_, DEC(-765432e-6176),     6174, DEC(-7654e-6174),    0 },
                { L_, DEC(-765432e-6176),     6173, DEC(-765e-6173),     0 },
                { L_, DEC(-765432e-6176),     6172, DEC(-76e-6172),      0 },
                { L_, DEC(-765432e-6176),     6171, DEC(-7e-6171),       0 },
                { L_, DEC(-765432e-6176),     6170, DEC( 0e-0),          0 },
                { L_, DEC(-765432e-6176),     6169, DEC( 0e-0),          0 },
            //--------------------------------------------------------------
            //                          Test special values
            //--------------------------------------------------------------
                { L_, ZERO_P,                    0, ZERO_P,              0 },
                { L_, ZERO_N,                    0, ZERO_N,              0 },
                { L_, INF_P,                     0, INF_P,               0 },
                { L_, INF_N,                     0, INF_N,               0 },
                { L_, NAN_P,                     0, NAN_P,               0 },
                { L_, NAN_N,                     0, NAN_N,               0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE      = DATA[ti].d_line;
                const Obj&          X         = DATA[ti].d_x;
                const unsigned int& PRECISION = DATA[ti].d_precision;
                const Obj&          EXPECTED  = DATA[ti].d_expected;
                const int&          ERRNO     = DATA[ti].d_errno;

                errno = 0;
                const Obj RESULT = Util::trunc(X, PRECISION);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                }
                LOOP3_ASSERT(LINE, ERRNO, errno, ERRNO == errno);
            }
        }

        if (veryVerbose) { T_ bsl::cout << "sqrt()" << bsl::endl; }
        {
            const Obj SQRT_2 = DEC(1.414213562373095048801688724209698);
            const Obj DEC_1  = DEC(8.999999999999999999999999999999996);
            const Obj RES_1  = DEC(2.999999999999999999999999999999999);

            struct {
                int d_line;
                Obj d_x;
                Obj d_expected;
            } DATA[] = {
            //--------------------------------
            // LINE |     X     |   EXPECTED
            //--------------------------------
                { L_, DEC(100.0),  DEC(10.0) },
                { L_, DEC_1,       RES_1     },
                { L_, DEC(  2.0),  SQRT_2    },
                { L_, DEC( -1.0),  NAN_P     },
                { L_, INF_P,       INF_P     },
                { L_, INF_N,       NAN_P     },
                { L_, NAN_P,       NAN_P     },
                { L_, NAN_N,       NAN_N     },
                { L_, ZERO_P,      ZERO_P    },
                { L_, ZERO_N,      ZERO_N    },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const Obj& X        = DATA[ti].d_x;
                const Obj& EXPECTED = DATA[ti].d_expected;
                const Obj  RESULT   = Util::sqrt(X);

                if (Util::isNan(EXPECTED)) {
                    LOOP2_ASSERT(LINE, RESULT, Util::isNan(RESULT));
                } else {
                    LOOP2_ASSERT(LINE, EXPECTED, EXPECTED == RESULT);
                }
            }
        }
#undef DEC
    }
    } break;
    case 13: {
    // ------------------------------------------------------------------------
    // TESTING decompose
    // Concerns:
    //: 1 Finite decimal value decomposed using 'decompose' functions can be
    //:   reconstructed to the value equal to the original one using
    //:   'sign', 'significand' and 'exponent' values.
    //:
    //: 2 Special encoding values having the 2 bits after the sign set to '11'
    //:   are decomposed either into finite value if at least one of the next
    //:   two bits are set to '0' or decomposed into 'Inf' or 'NaN' type
    //:   values otherwise.
    //
    // Plan:
    //: 1 Try decomposing a variety of different valid values into sign,
    //:   significand and exponent compartments.  Including minimum and maximum
    //:   values and special values such as 'NaN' and 'Inf'.  Restore the value
    //:   using 'sign', significand' and 'exponent' parts and check that if the
    //:   value is not a special (infinity or NaNs), it is restored into the
    //:   value equal to the original one.
    //
    // Testing:
    //   int decompose(int *, unsigned int *,        int *, Decimal32);
    //   int decompose(int *, bsls::Types::Uint64 *, int *, Decimal64);
    //   int decompose(int *, Uint128 *,             int *, Decimal128);
    // --------------------------------------------------------------------
    if (verbose) bsl::cout << "\nTESTING DECOMPOSE METHODS"
                           << "\n=========================="
                           << bsl::endl;

    typedef BDEC::DecimalUtil Util;

#define DEC(X) BDLDFP_DECIMAL_DF(X)
    {
        if (veryVerbose) bsl::cout << "\nDecimal32"
                                   << "\n---------"
                                   << bsl::endl;

        typedef bdldfp::Decimal32 Type;

        Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
        Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
        Type INF_P   =  bsl::numeric_limits<Type>::infinity();
        Type INF_N   = -bsl::numeric_limits<Type>::infinity();
        Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
        Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
        Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
        Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();
        Type MAX     = DEC(9.999999e+96);
        Type MIN     = DEC(1e-95);

        // the most left 4 bits of the significand are in the range 8 to 9.
        Type SPE_8_P = DEC( 8.999999e+0);
        Type SPE_8_N = DEC(-8.999999e+0);
        Type SPE_9_P = DEC( 9.000001e+10);
        Type SPE_9_N = DEC(-9.000001e+10);

        static const struct {
            int          d_line;
            Type         d_decimalValue;
            int          d_class;         // classification
            int          d_sign;
            unsigned int d_significand;
            int          d_exponent;
        } DATA[] = {
            // L    NUMBER       CLASS       SIGN  SIGNIFICAND  EXPONENT
            // ---  ----------   -------     ----  -----------  --------

            // Test zero values
            {  L_,  DEC( 0.0),   FP_ZERO,       1,           0,   -1 },
            {  L_,  DEC(-0.0),   FP_ZERO,      -1,           0,   -1 },

            // Test special encoding values
            {  L_,   INF_P,      FP_INFINITE,   1,    0x800000, 0xc0 },
            {  L_,   INF_N,      FP_INFINITE,  -1,    0x800000, 0xc0 },
            {  L_,   NAN_Q_P,    FP_NAN,        1,    0x800000, 0xe0 },
            {  L_,   NAN_Q_N,    FP_NAN,       -1,    0x800000, 0xe0 },
            {  L_,   NAN_S_P,    FP_NAN,        1,    0x800000, 0xf0 },
            {  L_,   NAN_S_N,    FP_NAN,       -1,    0x800000, 0xf0 },
            {  L_,   SUBN_P,     FP_SUBNORMAL,  1,           1, -101 },
            {  L_,   SUBN_N,     FP_SUBNORMAL, -1,           1, -101 },
            {  L_,   SPE_8_P,    FP_NORMAL,     1,     8999999,   -6 },
            {  L_,   SPE_8_N,    FP_NORMAL,    -1,     8999999,   -6 },
            {  L_,   SPE_9_P,    FP_NORMAL,     1,     9000001,    4 },
            {  L_,   SPE_9_N,    FP_NORMAL,    -1,     9000001,    4 },

            // Test boundary values
            {  L_,   MIN,        FP_NORMAL,     1,           1,  -95 },
            {  L_,  -MIN,        FP_NORMAL,    -1,           1,  -95 },
            {  L_,   MAX,        FP_NORMAL,     1,     9999999,   90 },
            {  L_,  -MAX,        FP_NORMAL,    -1,     9999999,   90 },

            // Test arbitrary values
            {  L_,   DEC( 1.0),  FP_NORMAL,     1,          10,   -1 },
            {  L_,   DEC(-1.0),  FP_NORMAL,    -1,          10,   -1 },
            {  L_,   DEC( 0.1),  FP_NORMAL,     1,           1,   -1 },
            {  L_,   DEC(-0.1),  FP_NORMAL,    -1,           1,   -1 },
            {  L_,   DEC( 4.25), FP_NORMAL,     1,         425,   -2 },
            {  L_,   DEC(-4.25), FP_NORMAL,    -1,         425,   -2 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE        = DATA[ti].d_line;
            const Type          DECIMAL32   = DATA[ti].d_decimalValue;
            const int           CLASS       = DATA[ti].d_class;
            const int           SIGN        = DATA[ti].d_sign;
            const unsigned int  SIGNIFICAND = DATA[ti].d_significand;
            const int           EXPONENT    = DATA[ti].d_exponent;

            if (veryVerbose) {
                P_(LINE); P_(CLASS); P_(SIGN); P_(SIGNIFICAND); P_(EXPONENT);
                P(DECIMAL32);
            }

            // Test with Decimal32.
            {
                const Type VALUE(DECIMAL32);

                int ACTUAL_SIGN;
                unsigned int ACTUAL_SIGNIFICAND;
                int ACTUAL_EXPONENT;
                int ACTUAL_CLASS = Util::decompose(&ACTUAL_SIGN,
                                                   &ACTUAL_SIGNIFICAND,
                                                   &ACTUAL_EXPONENT,
                                                   VALUE);

                ASSERTV(LINE, ACTUAL_CLASS, CLASS, ACTUAL_CLASS == CLASS);
                ASSERTV(LINE, ACTUAL_SIGN,   SIGN, ACTUAL_SIGN  == SIGN);
                ASSERTV(LINE,
                        ACTUAL_SIGNIFICAND,
                        SIGNIFICAND,
                        ACTUAL_SIGNIFICAND == SIGNIFICAND);
                ASSERTV(LINE,
                        ACTUAL_EXPONENT,
                        EXPONENT,
                        ACTUAL_EXPONENT == EXPONENT);

                if (Util::isFinite(VALUE)) {
                    Type INPUT =
                        ImpUtil::makeDecimalRaw32(
                                              ACTUAL_SIGN * ACTUAL_SIGNIFICAND,
                                              ACTUAL_EXPONENT);

                    ASSERTV(LINE, INPUT, VALUE, INPUT == VALUE);
                }
            }
        }
    }
#undef DEC
#define DEC(X) BDLDFP_DECIMAL_DD(X)
    {
        if (veryVerbose) bsl::cout << "\nDecimal64"
                                   << "\n---------"
                                   << bsl::endl;

        typedef bdldfp::Decimal64 Type;

        Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
        Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
        Type INF_P   =  bsl::numeric_limits<Type>::infinity();
        Type INF_N   = -bsl::numeric_limits<Type>::infinity();
        Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
        Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
        Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
        Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();
        Type MAX     =  bdldfp::DecimalImpUtil::parse64(
                                                     "9.999999999999999e+384");
        Type MIN     =  bdldfp::DecimalImpUtil::parse64("1e-383");

        // the most left 4 bits of the significand are in the range 8 to 9.
        Type SPE_8_P = DEC( 8.999999999999999e+0);
        Type SPE_8_N = DEC(-8.999999999999999e+0);
        Type SPE_9_P = DEC( 9.000000000000001e+10);
        Type SPE_9_N = DEC(-9.000000000000001e+10);

        static const struct {
            int                 d_line;
            Type                d_decimalValue;
            int                 d_class;         // classification
            int                 d_sign;
            bsls::Types::Uint64 d_significand;
            int                 d_exponent;
        } DATA[] = {
            // L   NUMBER     CLASS       SIGN   SIGNIFICAND          EXPONENT
            // --- ---------- -------     ----   -------------------- --------

            // Test zero values
            {  L_, DEC( 0.0), FP_ZERO,       1,                    0,     -1 },
            {  L_, DEC(-0.0), FP_ZERO,      -1,                    0,     -1 },

            // Test special encoding values
            {  L_,  INF_P,    FP_INFINITE,   1,  0x20000000000000ull,  0x300 },
            {  L_,  INF_N,    FP_INFINITE,  -1,  0x20000000000000ull,  0x300 },
            {  L_,  NAN_Q_P,  FP_NAN,        1,  0x20000000000000ull,  0x380 },
            {  L_,  NAN_Q_N,  FP_NAN,       -1,  0x20000000000000ull,  0x380 },
            {  L_,  NAN_S_P,  FP_NAN,        1,  0x20000000000000ull,  0x3c0 },
            {  L_,  NAN_S_N,  FP_NAN,       -1,  0x20000000000000ull,  0x3c0 },
            {  L_,  SUBN_P,   FP_SUBNORMAL,  1,                    1,   -398 },
            {  L_,  SUBN_N,   FP_SUBNORMAL, -1,                    1,   -398 },
            {  L_,  SPE_8_P,  FP_NORMAL,     1,  8999999999999999ull,    -15 },
            {  L_,  SPE_8_N,  FP_NORMAL,    -1,  8999999999999999ull,    -15 },
            {  L_,  SPE_9_P,  FP_NORMAL,     1,  9000000000000001ull,     -5 },
            {  L_,  SPE_9_N,  FP_NORMAL,    -1,  9000000000000001ull,     -5 },

            // Test boundary values
            {  L_,   MIN,     FP_NORMAL,     1,                    1,   -383 },
            {  L_,  -MIN,     FP_NORMAL,    -1,                    1,   -383 },
            {  L_,   MAX,     FP_NORMAL,     1,  9999999999999999ull,    369 },
            {  L_,  -MAX,     FP_NORMAL,    -1,  9999999999999999ull,    369 },

            // Test arbitrary values
            {  L_,   DEC( 1.0),  FP_NORMAL,  1,                   10,     -1 },
            {  L_,   DEC(-1.0),  FP_NORMAL, -1,                   10,     -1 },
            {  L_,   DEC( 0.1),  FP_NORMAL,  1,                    1,     -1 },
            {  L_,   DEC(-0.1),  FP_NORMAL, -1,                    1,     -1 },
            {  L_,   DEC( 4.25), FP_NORMAL,  1,                  425,     -2 },
            {  L_,   DEC(-4.25), FP_NORMAL, -1,                  425,     -2 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE         = DATA[ti].d_line;
            const Type               DECIMAL64    = DATA[ti].d_decimalValue;
            const int                CLASS        = DATA[ti].d_class;
            const int                SIGN         = DATA[ti].d_sign;
            const bsls::Types::Uint64 SIGNIFICAND = DATA[ti].d_significand;
            const int                EXPONENT     = DATA[ti].d_exponent;

            if (veryVerbose) {
                P_(LINE); P_(CLASS); P_(SIGN); P_(SIGNIFICAND); P_(EXPONENT);
                P(DECIMAL64);
            }

            // Test with Decimal64.
            {
                const Type VALUE(DECIMAL64);

                int                 ACTUAL_SIGN;
                bsls::Types::Uint64 ACTUAL_SIGNIFICAND;
                int                 ACTUAL_EXPONENT;
                int                 ACTUAL_CLASS =
                                           Util::decompose(&ACTUAL_SIGN,
                                                           &ACTUAL_SIGNIFICAND,
                                                           &ACTUAL_EXPONENT,
                                                           VALUE);

                ASSERTV(LINE, ACTUAL_CLASS, CLASS, ACTUAL_CLASS == CLASS);
                ASSERTV(LINE, ACTUAL_SIGN,   SIGN, ACTUAL_SIGN  == SIGN);
                ASSERTV(LINE,
                        ACTUAL_SIGNIFICAND,
                        SIGNIFICAND,
                        ACTUAL_SIGNIFICAND == SIGNIFICAND);
                ASSERTV(LINE,
                        ACTUAL_EXPONENT,
                        EXPONENT,
                        ACTUAL_EXPONENT == EXPONENT);

                if (Util::isFinite(VALUE)) {
                    Type INPUT = ACTUAL_SIGN
                                 * Util::multiplyByPowerOf10(
                                                      Type(ACTUAL_SIGNIFICAND),
                                                      ACTUAL_EXPONENT);

                    ASSERTV(LINE, INPUT, VALUE, INPUT == VALUE);
                }
            }
        }
    }
#undef DEC
#define DEC(X) BDLDFP_DECIMAL_DL(X)
    {
        if (veryVerbose) bsl::cout << "\nDecimal128"
                                   << "\n----------"
                                   << bsl::endl;

        typedef bdldfp::Decimal128 Type;

        Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
        Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
        Type INF_P   =  bsl::numeric_limits<Type>::infinity();
        Type INF_N   = -bsl::numeric_limits<Type>::infinity();
        Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
        Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
        Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
        Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();
        Type MAX     =  bsl::numeric_limits<Type>::max();
        Type MIN     =  bsl::numeric_limits<Type>::min();
        Type V1      = Type(0xFFFFFFFFFFFFFFFFull) + 1;

        static const struct {
            int                 d_line;
            Type                d_decimalValue;
            int                 d_class;         // classification
            int                 d_sign;
            bsls::Types::Uint64 d_significandH;
            bsls::Types::Uint64 d_significandL;
            int                 d_exponent;
        } DATA[] = {
        // L   NUMBER      CLASS        SIGN  SIGNIFICAND HIGH,    EXPONENT
        //                                    SIGNIFICAND LOW
        // --- ----------  -------      ----  -------------------  ---------

        // Test zero values
        {  L_, DEC( 0.0),  FP_ZERO,       1,                     0,
                                                                 0,       -1 },
        {  L_, DEC(-0.0),  FP_ZERO,      -1,                     0,
                                                                 0,       -1 },

        // Test special encoding values
        {  L_,  INF_P,     FP_INFINITE,   1,   0x20000000000000ull,
                                                                 0,   0x3000 },
        {  L_,  INF_N,     FP_INFINITE,  -1,   0x20000000000000ull,
                                                                 0,   0x3000 },
        {  L_,  NAN_Q_P,   FP_NAN,        1,   0x20000000000000ull,
                                                                 0,   0x3800 },
        {  L_,  NAN_Q_N,   FP_NAN,       -1,   0x20000000000000ull,
                                                                 0,   0x3800 },
        {  L_,  NAN_S_P,   FP_NAN,        1,   0x20000000000000ull,
                                                                 0,   0x3c00 },
        {  L_,  NAN_S_N,   FP_NAN,       -1,   0x20000000000000ull,
                                                                 0,   0x3c00 },
        {  L_,  SUBN_P,    FP_SUBNORMAL,  1,                     0,
                                                                 1,    -6176 },
        {  L_,  SUBN_N,    FP_SUBNORMAL, -1,                     0,
                                                                 1,    -6176 },

            // // Test boundary values
        {  L_,   MIN,      FP_NORMAL,     1,                     0,
                                                                 1,    -6143 },
        {  L_,  -MIN,      FP_NORMAL,    -1,                     0,
                                                                 1,    -6143 },
        {  L_,   MAX,      FP_NORMAL,     1,    0x1ED09BEAD87C0ull,
                                             0x378D8E63FFFFFFFFull,     6111 },
        {  L_,  -MAX,      FP_NORMAL,    -1,    0x1ED09BEAD87C0ull,
                                             0x378D8E63FFFFFFFFull,     6111 },
            // // Test arbitrary values
        {  L_, DEC( 1.0),  FP_NORMAL,     1,                     0,
                                                                10,       -1 },
        {  L_, DEC(-1.0),  FP_NORMAL,    -1,                     0,
                                                                10,       -1 },
        {  L_, DEC( 0.1),  FP_NORMAL,     1,                     0,
                                                                 1,       -1 },
        {  L_, DEC(-0.1),  FP_NORMAL,    -1,                     0,
                                                                 1,       -1 },
        {  L_, DEC( 4.25), FP_NORMAL,     1,                     0,
                                                               425,       -2 },
        {  L_, DEC(-4.25), FP_NORMAL,    -1,                     0,
                                                               425,       -2 },
        {  L_, V1,         FP_NORMAL,     1,                     1,
                                                                 0,        0 },
        {  L_, -V1,        FP_NORMAL,    -1,                     1,
                                                                 0,        0 },
        {  L_, V1 * 0x10 + 0x50,
                           FP_NORMAL,     1,                  0x10,
                                                              0x50,        0 },
        {  L_, -(V1 * 0x10 + 0x50),
                           FP_NORMAL,    -1,                  0x10,
                                                              0x50,        0 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof * DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                 LINE          = DATA[ti].d_line;
            const Type                DECIMAL128    = DATA[ti].d_decimalValue;
            const int                 CLASS         = DATA[ti].d_class;
            const int                 SIGN          = DATA[ti].d_sign;
            const bsls::Types::Uint64 SIGNIFICAND_H = DATA[ti].d_significandH;
            const bsls::Types::Uint64 SIGNIFICAND_L = DATA[ti].d_significandL;
            const int                 EXPONENT      = DATA[ti].d_exponent;

            if (veryVerbose) {
                P_(LINE); P_(CLASS); P_(SIGN);
                P_(SIGNIFICAND_H); P_(SIGNIFICAND_L);
                P_(EXPONENT); P(DECIMAL128);
            }

            // Test with Decimal64.
            {
                const Type VALUE(DECIMAL128);

                int           ACTUAL_SIGN;
                BDEC::Uint128 ACTUAL_SIGNIFICAND;
                int           ACTUAL_EXPONENT;
                int           ACTUAL_CLASS = Util::decompose(
                                                           &ACTUAL_SIGN,
                                                           &ACTUAL_SIGNIFICAND,
                                                           &ACTUAL_EXPONENT,
                                                           VALUE);

                ASSERTV(LINE, ACTUAL_CLASS, CLASS, ACTUAL_CLASS == CLASS);
                ASSERTV(LINE, ACTUAL_SIGN,   SIGN, ACTUAL_SIGN  == SIGN);
                ASSERTV(LINE,
                        ACTUAL_SIGNIFICAND.high(),
                        SIGNIFICAND_H,
                        ACTUAL_SIGNIFICAND.high() == SIGNIFICAND_H);
                ASSERTV(LINE,
                        ACTUAL_SIGNIFICAND.low(),
                        SIGNIFICAND_L,
                        ACTUAL_SIGNIFICAND.low() == SIGNIFICAND_L);
                ASSERTV(LINE,
                        ACTUAL_EXPONENT,
                        EXPONENT,
                        ACTUAL_EXPONENT == EXPONENT);

                if (Util::isFinite(VALUE)) {
                    const Type MOVE_LEFT_64 = Type(0xFFFFFFFFFFFFFFFFull) + 1;
                    const Type SIGNIFICAND =
                        Type(ACTUAL_SIGNIFICAND.high()) * MOVE_LEFT_64
                        + Type(ACTUAL_SIGNIFICAND.low());
                    const Type INPUT =
                           ACTUAL_SIGN * Util::multiplyByPowerOf10(
                                                              SIGNIFICAND,
                                                              ACTUAL_EXPONENT);

                    ASSERTV(LINE, INPUT, VALUE, INPUT == VALUE);
                }

            }
        }
    }
#undef DEC
    } break;
    case 12: {
      // ----------------------------------------------------------------------
      // CONCURRENCY
      //
      // Concerns:
      //:  1 Concurrent use of round and quantize is correct.
      //
      // Plan:
      //:  1 Create several threads that call either quantize or round in loops
      //:    and verify that the correct values are always returned.
      //
      // Testing:
      //   void DecimalUtil::quantize;
      //   void DecimalUtil::round;
      // ----------------------------------------------------------------------

      if (verbose) bsl::cout << "\nCONCURRENCY"
                                "\n===========\n";

      const int                 k_NUM_THREADS = 10;
      bslmt::ThreadUtil::Handle q[k_NUM_THREADS];
      bslmt::ThreadUtil::Handle r[k_NUM_THREADS];
      for (int i = 0; i < k_NUM_THREADS; ++i) {
          if (veryVerbose) { cout << "Begin quantize thread " << i << "\n"; }
          bslmt::ThreadUtil::createWithAllocator(&q[i], &doQuantize, &ta);
          if (veryVerbose) { cout << "Begin round thread " << i << "\n"; }
          bslmt::ThreadUtil::createWithAllocator(&r[i], &doRound, &ta);
      }
      for (int i = 0; i < k_NUM_THREADS; ++i) {
          if (veryVerbose) {
              cout << "Wait for quantize thread " << i << "\n";
          }
          bslmt::ThreadUtil::join(q[i]);
          if (veryVerbose) { cout << "Wait for round thread " << i << "\n"; }
          bslmt::ThreadUtil::join(r[i]);
      }
    } break;
    case 11: {
        // --------------------------------------------------------------------
        // TESTING parseDecimal
        // Concerns: the parse functions correctly parses decimal strings.
        // Plan: Try parsing a variety of different valid and invalid
        //       strings. Including minimum and maximum values.
        //       decimal floats
        //       positive and negative.), and different powers of 10 to
        //       multiply by (both positive and negative.)
        // Testing: parseDecimal32, parseDecimal64, parseDecimal128
        // --------------------------------------------------------------------

        {
            typedef bdldfp::Decimal32 Type;

            const Type ERROR_VALUE = BDLDFP_DECIMAL_DF(999.0);

            Type inf  = bsl::numeric_limits<Type>::infinity();
            Type qnan = bsl::numeric_limits<Type>::quiet_NaN();
            Type snan = bsl::numeric_limits<Type>::signaling_NaN();
            Type max  = BDLDFP_DECIMAL_DF(9.999999e+96);
            Type min  = BDLDFP_DECIMAL_DF(-1e-95);

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
                // line      input   exp           isValid
                // ----      -----   ---           -------
                {  L_,             "0",     BDLDFP_DECIMAL_DF(0.0),  true },
                {  L_,             "1",     BDLDFP_DECIMAL_DF(1.0),  true },
                {  L_,        "9.3E23",  BDLDFP_DECIMAL_DF(9.3E23),  true },
                {  L_,            "-1",    BDLDFP_DECIMAL_DF(-1.0),  true },
                {  L_,          "-0.1",    BDLDFP_DECIMAL_DF(-0.1),  true },
                {  L_,  "9.999999e+96",                        max,  true },
                {  L_,        "-1e-95",                        min,  true },
                {  L_,         "1e+97",                        inf,  true },
                {  L_,           "NaN",                       qnan,  true },
                {  L_,          "sNaN",                       snan,  true },
                {  L_,           "nan",                       qnan,  true },
                {  L_,           "INF",                        inf,  true },
                {  L_,          "-INF",                       -inf,  true },
                {  L_,      "infinity",                        inf,  true },
                {  L_,     "-infinity",                       -inf,  true },

                {  L_,        "nanb", ERROR_VALUE,  false },
                {  L_,       "snane", ERROR_VALUE,  false },
                {  L_,       "snan ", ERROR_VALUE,  false },
                {  L_,   "infinity ", ERROR_VALUE,  false },

                {  L_,         "-",   ERROR_VALUE,  false },
                {  L_,       "E-1",   ERROR_VALUE,  false },

                {  L_,  "3Z4.56e1",   ERROR_VALUE,  false },
                {  L_,      "1.1}",   ERROR_VALUE,  false },
                {  L_,  "DEADBEEF",   ERROR_VALUE,  false },
                {  L_,      "JUNK",   ERROR_VALUE,  false },
            };
            const int NUM_DATA = static_cast<int>(sizeof(DATA) /
                                                  sizeof(*DATA));

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char* INPUT    = DATA[i].d_input_p;
                const Type  EXP      = DATA[i].d_exp;
                const bool  IS_VALID = DATA[i].d_isValid;
                      Type  value    = ERROR_VALUE;

                const int rc = Util::parseDecimal32(&value, INPUT);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }

                if (Util::isNan(EXP)) {
                    LOOP3_ASSERT(LINE, EXP, value, Util::isNan(value));
                } else {
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
                }
            }
        }
        {
            typedef bdldfp::Decimal64 Type;

            const Type ERROR_VALUE = BDLDFP_DECIMAL_DD(999.0);

            Type inf  = bsl::numeric_limits<Type>::infinity();
            Type qnan = bsl::numeric_limits<Type>::quiet_NaN();
            Type snan = bsl::numeric_limits<Type>::signaling_NaN();
            Type max  = bdldfp::DecimalImpUtil::parse64(
                                                     "9.999999999999999e+384");
            Type min  = bdldfp::DecimalImpUtil::parse64("1e-383");

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
                // line       input   exp           isValid
                // ----       -----   ---           -------
                {  L_,             "0",     BDLDFP_DECIMAL_DF(0.0),  true },
                {  L_,             "1",     BDLDFP_DECIMAL_DF(1.0),  true },
                {  L_,        "9.3E23",  BDLDFP_DECIMAL_DF(9.3E23),  true },
                {  L_,            "-1",    BDLDFP_DECIMAL_DF(-1.0),  true },
                {  L_,          "-0.1",    BDLDFP_DECIMAL_DF(-0.1),  true },
                {  L_,  "9.999999999999999e+384",              max,  true },
                {  L_,        "1e-383",                        min,  true },
                {  L_,        "1e+385",                        inf,  true },

                {  L_,           "NaN",                       qnan,  true },
                {  L_,          "sNaN",                       snan,  true },
                {  L_,           "nan",                       qnan,  true },
                {  L_,           "INF",                        inf,  true },
                {  L_,          "-INF",                       -inf,  true },
                {  L_,      "infinity",                        inf,  true },
                {  L_,     "-infinity",                       -inf,  true },

                {  L_,        "nanb", ERROR_VALUE,  false },
                {  L_,       "snane", ERROR_VALUE,  false },
                {  L_,       "snan ", ERROR_VALUE,  false },
                {  L_,   "infinity ", ERROR_VALUE,  false },

                {  L_,         "-",   ERROR_VALUE,  false },
                {  L_,       "E-1",   ERROR_VALUE,  false },

                {  L_,  "3Z4.56e1",   ERROR_VALUE,  false },
                {  L_,      "1.1}",   ERROR_VALUE,  false },
                {  L_,  "DEADBEEF",   ERROR_VALUE,  false },
                {  L_,      "JUNK",   ERROR_VALUE,  false },
            };
            const int NUM_DATA = static_cast<int>(sizeof(DATA) /
                                                  sizeof(*DATA));

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char* INPUT    = DATA[i].d_input_p;
                const Type  EXP      = DATA[i].d_exp;
                const bool  IS_VALID = DATA[i].d_isValid;
                      Type  value    = ERROR_VALUE;

                const int rc = Util::parseDecimal64(&value, INPUT);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                if (Util::isNan(EXP)) {
                    LOOP3_ASSERT(LINE, EXP, value, Util::isNan(value));
                } else {
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
                }
            }
        }
        {
            typedef bdldfp::Decimal128 Type;

            const Type ERROR_VALUE = BDLDFP_DECIMAL_DL(999.0);
            Type inf  = bsl::numeric_limits<Type>::infinity();
            Type qnan = bsl::numeric_limits<Type>::quiet_NaN();
            Type snan = bsl::numeric_limits<Type>::signaling_NaN();

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
                // line       input   exp           isValid
                // ----       -----   ---           -------
                {  L_,             "0",     BDLDFP_DECIMAL_DF(0.0),  true },
                {  L_,             "1",     BDLDFP_DECIMAL_DF(1.0),  true },
                {  L_,        "9.3E23",  BDLDFP_DECIMAL_DF(9.3E23),  true },
                {  L_,            "-1",    BDLDFP_DECIMAL_DF(-1.0),  true },
                {  L_,          "-0.1",    BDLDFP_DECIMAL_DF(-0.1),  true },

                {  L_,   "9.999999999999999e+500",
                   bdldfp::DecimalImpUtil::parse128(
                                         "9.999999999999999e+500"),  true },
                {  L_,       "-1e-500",
                       bdldfp::DecimalImpUtil::parse128("-1e-500"),  true },

                {  L_,           "NaN",                       qnan,  true },
                {  L_,          "sNaN",                       snan,  true },
                {  L_,           "nan",                       qnan,  true },
                {  L_,           "INF",                        inf,  true },
                {  L_,          "-INF",                       -inf,  true },
                {  L_,      "infinity",                        inf,  true },
                {  L_,     "-infinity",                       -inf,  true },

                {  L_,        "nanb", ERROR_VALUE,  false },
                {  L_,       "snane", ERROR_VALUE,  false },
                {  L_,       "snan ", ERROR_VALUE,  false },
                {  L_,   "infinity ", ERROR_VALUE,  false },

                {  L_,         "-",   ERROR_VALUE,  false },
                {  L_,       "E-1",   ERROR_VALUE,  false },

                {  L_,  "3Z4.56e1",   ERROR_VALUE,  false },
                {  L_,      "1.1}",   ERROR_VALUE,  false },
                {  L_,  "DEADBEEF",   ERROR_VALUE,  false },
                {  L_,      "JUNK",   ERROR_VALUE,  false },
            };
            const int NUM_DATA = static_cast<int>(sizeof(DATA) /
                                                  sizeof(*DATA));

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char* INPUT    = DATA[i].d_input_p;
                const Type  EXP      = DATA[i].d_exp;
                const bool  IS_VALID = DATA[i].d_isValid;
                      Type  value    = ERROR_VALUE;

                const int rc = Util::parseDecimal128(&value, INPUT);
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, rc, 0 == rc);
                }
                else {
                    LOOP2_ASSERT(LINE, rc, rc);
                }
                if (Util::isNan(EXP)) {
                    LOOP3_ASSERT(LINE, EXP, value, Util::isNan(value));
                } else {
                    LOOP3_ASSERT(LINE, EXP, value, EXP == value);
                }
            }
        }
    } break;
    case 10: {
        // --------------------------------------------------------------------
        // TESTING multiplyByPowerOf10
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.), and different powers of 10 to
        //       multiply by (both positive and negative.)
        // Testing: multiplyByPowerOf10
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "multiplyByPowerOf10 tests..." << bsl::endl;
        {
            if (veryVerbose) bsl::cout << "multiplyByPowerOf10 "
                                 "tests on 'int' powers..." << bsl::endl;

            for (int mi = 0; mi < numMantissas; ++mi) {
                for (int ei = 0; ei < numExps; ++ei) {
                    for (int ei2 = 0; ei2 < numExps; ++ei2) {

                        const long long MANTISSA = mantissas[mi];
                        const int EXP            = exps[ei];
                        const int POW            = exps[ei2];

                        {
                            const BDEC::Decimal64 VALUE = Util::makeDecimal64(
                                MANTISSA, EXP);
                            const BDEC::Decimal64 ACTUAL_RESULT =
                                Util::multiplyByPowerOf10(VALUE, POW);
                            const BDEC::Decimal64 EXPECTED_RESULT =
                                Util::makeDecimal64(MANTISSA, EXP + POW);

                            LOOP5_ASSERT(MANTISSA, EXP, POW,
                                         ACTUAL_RESULT, EXPECTED_RESULT,
                                         ACTUAL_RESULT == EXPECTED_RESULT);
                        }
                    }
                }
            }
        }
    } break;
    case 9: {
        // --------------------------------------------------------------------
        // TESTING fabs
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: fabs
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "fabs Decimal64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for quantum, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test that all special cases work correctly.
            // o sNaN
            ASSERT(Util::isNan(Util::fabs(sNaN)));

            // o qNaN
            ASSERT(Util::isNan(Util::fabs(qNaN)));

            // o +Inf
            ASSERT(Util::fabs(pInf) == pInf);

            // o -Inf
            ASSERT(Util::fabs(nInf) == pInf);


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                        makeNumber(mantissas[tiM], exps[tiE]);

                    // Test the value of what quantum returns:
                    ASSERT(Util::fabs(value) ==
                           makeNumber(
                               bsl::max(mantissas[tiM], -mantissas[tiM]),
                               exps[tiE]));
                }
            }
        }
        if (verbose) bsl::cout << "fabs Decimal128 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for quantum, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test that all special cases work correctly.
            // o sNaN
            ASSERT(Util::isNan(Util::fabs(sNaN)));

            // o qNaN
            ASSERT(Util::isNan(Util::fabs(qNaN)));

            // o +Inf
            ASSERT(Util::fabs(pInf) == pInf);

            // o -Inf
            ASSERT(Util::fabs(nInf) == pInf);


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test the value of what quantum returns:
                    ASSERT(Util::fabs(value) ==
                          makeNumber(bsl::max(mantissas[tiM], -mantissas[tiM]),
                                                                   exps[tiE]));
                }
            }
        }
    } break;
    case 8: {
        // --------------------------------------------------------------------
        // TESTING quantize
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        //
        // Testing:
        //   Decimal32  quantize(Decimal32,    Decimal32);
        //   Decimal64  quantize(Decimal64,    Decimal64);
        //   Decimal128 quantize(Decimal128,   Decimal128);
        //   Decimal32  quantize(Decimal32,    int);
        //   Decimal64  quantize(Decimal64,    int);
        //   Decimal128 quantize(Decimal128,   int);
        //   int        quantizeEqual(Decimal32  *, Decimal32,  int);
        //   int        quantizeEqual(Decimal64  *, Decimal64,  int);
        //   int        quantizeEqual(Decimal128 *, Decimal128, int);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "quantize Decimal32 tests..." << bsl::endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DF(X)

            typedef BDEC::Decimal32           TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;

            const TYPE sNaN(NumLim::signaling_NaN());
            const TYPE qNaN(NumLim::quiet_NaN());
            const TYPE pInf(NumLim::infinity());
            const TYPE nInf(-pInf);

            struct {
                int  d_line;
                int  d_significand;
                int  d_exponent;
                int  d_quantum;
                TYPE d_expected;
                int  d_retValue;
            } DATA[] = {
        //--------------------------------------------------------------
        // LINE | SIGNIFICAND | EXP | QUANTUM | EXPECTED      | RETVALUE
        //--------------------------------------------------------------
            { L_,    123456,     0,     -2,     qNaN,             -1   },
            { L_,    123456,     0,     -1,     DEC(123456e+0),    0   },
            { L_,    123456,     0,      0,     DEC(123456e+0),    0   },
            { L_,    123456,     0,      1,     DEC(123460e+0),   -1   },
            { L_,    123456,     0,      2,     DEC(123500e+0),   -1   },
            { L_,    123456,     0,      3,     DEC(123000e+0),   -1   },
            { L_,    123456,     0,      4,     DEC(120000e+0),   -1   },
            { L_,    123456,     0,      5,     DEC(100000e+0),   -1   },
            { L_,    123456,     0,      6,     DEC(0e+0),        -1   },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            NumberMaker<TYPE> makeDecimal;

            // All special case values:
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE        = DATA[ti].d_line;
                const long long int& SIGNIFICAND = DATA[ti].d_significand;
                const int&           EXPONENT    = DATA[ti].d_exponent;
                const int&           QUANTUM     = DATA[ti].d_quantum;

                const TYPE V = makeDecimal(SIGNIFICAND, EXPONENT);
                const TYPE E = makeDecimal(          1, QUANTUM);

                {  //: o TYPE quantize(TYPE, TYPE);

                    const TYPE EXPECTED = DATA[ti].d_expected;
                    const TYPE RESULT   = Util::quantize(V, E);

                    if (Util::isNan(EXPECTED)) {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, Util::isNan(RESULT));
                    } else {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                        LOOP_ASSERT(LINE, Util::sameQuantum(RESULT, E));
                    }

                    ASSERT(Util::isNan(Util::quantize(   V, sNaN)));
                    ASSERT(Util::isNan(Util::quantize(   V, qNaN)));
                    ASSERT(Util::isNan(Util::quantize(   V, pInf)));
                    ASSERT(Util::isNan(Util::quantize(   V, nInf)));
                    ASSERT(Util::isNan(Util::quantize(sNaN,    E)));
                    ASSERT(Util::isNan(Util::quantize(qNaN,    E)));
                    ASSERT(Util::isNan(Util::quantize(pInf,    E)));
                    ASSERT(Util::isNan(Util::quantize(nInf,    E)));
                }

                {  //: o TYPE quantize(TYPE, int);

                    const TYPE& EXPECTED = DATA[ti].d_expected;
                    const TYPE  RESULT   = Util::quantize(V, QUANTUM);

                    if (Util::isNan(EXPECTED)) {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, Util::isNan(RESULT));
                    } else {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                        LOOP_ASSERT(LINE, Util::sameQuantum(RESULT, E));
                    }

                    ASSERT(Util::isNan(Util::quantize(sNaN, 0)));
                    ASSERT(Util::isNan(Util::quantize(qNaN, 0)));
                    ASSERT(Util::isNan(Util::quantize(pInf, 0)));
                    ASSERT(Util::isNan(Util::quantize(nInf, 0)));
                }

                {  //: o int quantizeEqual(TYPE *, TYPE, int);

                    TYPE X(V); TYPE *X_P(&X);
                    TYPE Y(V);

                    const int& EXPECTED = DATA[ti].d_retValue;
                    const int  RESULT   = Util::quantizeEqual(X_P, Y, QUANTUM);

                    LOOP_ASSERT(LINE, RESULT == EXPECTED);
                    LOOP_ASSERT(LINE, X == V);
                    if (0 == RESULT) {
                        LOOP_ASSERT(LINE, Util::sameQuantum(X, E));
                    }

                    ASSERT(-1 == Util::quantizeEqual(X_P, sNaN, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, qNaN, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, pInf, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, nInf, 0));
                }
            }

            if (verbose) cout << "\tNegative Testing." << endl;
            {
                bsls::AssertTestHandlerGuard hG;

                const int   VALID_EXPONENT1 = -101;
                const int   VALID_EXPONENT2 =   90;
                const int INVALID_EXPONENT1 = -102;
                const int INVALID_EXPONENT2 =   91;

                TYPE O; TYPE *O_P(&O);

                ASSERT_PASS(Util::quantize(O,     VALID_EXPONENT1));
                ASSERT_PASS(Util::quantize(O,     VALID_EXPONENT2));
                ASSERT_FAIL(Util::quantize(O,   INVALID_EXPONENT1));
                ASSERT_FAIL(Util::quantize(O,   INVALID_EXPONENT2));

                ASSERT_PASS(Util::quantizeEqual(O_P, O,   VALID_EXPONENT1));
                ASSERT_PASS(Util::quantizeEqual(O_P, O,   VALID_EXPONENT2));
                ASSERT_FAIL(Util::quantizeEqual(O_P, O, INVALID_EXPONENT1));
                ASSERT_FAIL(Util::quantizeEqual(O_P, O, INVALID_EXPONENT2));
            }
#undef  DEC
        }

        if (verbose) bsl::cout << "quantize Decimal64 tests..." << bsl::endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)
            typedef BDEC::Decimal64           TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;

            const TYPE sNaN(NumLim::signaling_NaN());
            const TYPE qNaN(NumLim::quiet_NaN());
            const TYPE pInf(NumLim::infinity());
            const TYPE nInf(-pInf);

            struct {
                int           d_line;
                long long int d_significand;
                int           d_exponent;
                int           d_quantum;
                TYPE          d_expected;
                int           d_retValue;
            } DATA[] = {
      //----------------------------------------------------------------------
      // LINE |  SIGNIFICAND    | EXP | QUANTUM | EXPECTED              | RV
      //----------------------------------------------------------------------
          { L_, 12345678901234ll,  0,     -3,     qNaN,                   -1 },
          { L_, 12345678901234ll,  0,     -2,     DEC(12345678901234e+0),  0 },
          { L_, 12345678901234ll,  0,     -1,     DEC(12345678901234e+0),  0 },
          { L_, 12345678901234ll,  0,      0,     DEC(12345678901234e+0),  0 },
          { L_, 12345678901234ll,  0,      1,     DEC(12345678901230e+0), -1 },
          { L_, 12345678901234ll,  0,      2,     DEC(12345678901200e+0), -1 },
          { L_, 12345678901234ll,  0,      3,     DEC(12345678901000e+0), -1 },
          { L_, 12345678901234ll,  0,      4,     DEC(12345678900000e+0), -1 },
          { L_, 12345678901234ll,  0,      5,     DEC(12345678900000e+0), -1 },
          { L_, 12345678901234ll,  0,      6,     DEC(12345679000000e+0), -1 },
          { L_, 12345678901234ll,  0,      7,     DEC(12345680000000e+0), -1 },
          { L_, 12345678901234ll,  0,      8,     DEC(12345700000000e+0), -1 },
          { L_, 12345678901234ll,  0,      9,     DEC(12346000000000e+0), -1 },
          { L_, 12345678901234ll,  0,     10,     DEC(12350000000000e+0), -1 },
          { L_, 12345678901234ll,  0,     11,     DEC(12300000000000e+0), -1 },
          { L_, 12345678901234ll,  0,     12,     DEC(12000000000000e+0), -1 },
          { L_, 12345678901234ll,  0,     13,     DEC(10000000000000e+0), -1 },
          { L_, 12345678901234ll,  0,     14,     DEC(0e+0),              -1 },
          };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            NumberMaker<TYPE> makeDecimal;

            // All special case values:
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE        = DATA[ti].d_line;
                const long long int& SIGNIFICAND = DATA[ti].d_significand;
                const int&           EXPONENT    = DATA[ti].d_exponent;
                const int&           QUANTUM     = DATA[ti].d_quantum;

                const TYPE V = makeDecimal(SIGNIFICAND, EXPONENT);
                const TYPE E = makeDecimal(          1, QUANTUM);

                {  //: o TYPE quantize(TYPE, TYPE);

                    const TYPE EXPECTED = DATA[ti].d_expected;
                    const TYPE RESULT   = Util::quantize(V, E);

                    if (Util::isNan(EXPECTED)) {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, Util::isNan(RESULT));
                    } else {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                        LOOP_ASSERT(LINE, Util::sameQuantum(RESULT, E));
                    }

                    ASSERT(Util::isNan(Util::quantize(   V, sNaN)));
                    ASSERT(Util::isNan(Util::quantize(   V, qNaN)));
                    ASSERT(Util::isNan(Util::quantize(   V, pInf)));
                    ASSERT(Util::isNan(Util::quantize(   V, nInf)));
                    ASSERT(Util::isNan(Util::quantize(sNaN,    E)));
                    ASSERT(Util::isNan(Util::quantize(qNaN,    E)));
                    ASSERT(Util::isNan(Util::quantize(pInf,    E)));
                    ASSERT(Util::isNan(Util::quantize(nInf,    E)));
                }

                {  //: o TYPE quantize(TYPE, int);

                    const TYPE& EXPECTED = DATA[ti].d_expected;
                    const TYPE  RESULT   = Util::quantize(V, QUANTUM);

                    if (Util::isNan(EXPECTED)) {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, Util::isNan(RESULT));
                    } else {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                        LOOP_ASSERT(LINE, Util::sameQuantum(RESULT, E));
                    }

                    ASSERT(Util::isNan(Util::quantize(sNaN, 0)));
                    ASSERT(Util::isNan(Util::quantize(qNaN, 0)));
                    ASSERT(Util::isNan(Util::quantize(pInf, 0)));
                    ASSERT(Util::isNan(Util::quantize(nInf, 0)));
                }

                {  //: o int quantizeEqual(TYPE *, TYPE, int);

                    TYPE X(V); TYPE *X_P(&X);
                    TYPE Y(V);

                    const int& EXPECTED = DATA[ti].d_retValue;
                    const int  RESULT   = Util::quantizeEqual(X_P, Y, QUANTUM);

                    LOOP_ASSERT(LINE, RESULT == EXPECTED);
                    LOOP_ASSERT(LINE, X == V);
                    if (0 == RESULT) {
                        LOOP_ASSERT(LINE, Util::sameQuantum(X, E));
                    }

                    ASSERT(-1 == Util::quantizeEqual(X_P, sNaN, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, qNaN, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, pInf, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, nInf, 0));
                }
            }

            if (verbose) cout << "\tNegative Testing." << endl;
            {
                bsls::AssertTestHandlerGuard hG;

                const int   VALID_EXPONENT1 = -398;
                const int   VALID_EXPONENT2 =  369;
                const int INVALID_EXPONENT1 = -399;
                const int INVALID_EXPONENT2 =  370;

                TYPE O; TYPE *O_P(&O);

                ASSERT_PASS(Util::quantize(O,     VALID_EXPONENT1));
                ASSERT_PASS(Util::quantize(O,     VALID_EXPONENT2));
                ASSERT_FAIL(Util::quantize(O,   INVALID_EXPONENT1));
                ASSERT_FAIL(Util::quantize(O,   INVALID_EXPONENT2));

                ASSERT_PASS(Util::quantizeEqual(O_P, O,   VALID_EXPONENT1));
                ASSERT_PASS(Util::quantizeEqual(O_P, O,   VALID_EXPONENT2));
                ASSERT_FAIL(Util::quantizeEqual(O_P, O, INVALID_EXPONENT1));
                ASSERT_FAIL(Util::quantizeEqual(O_P, O, INVALID_EXPONENT2));
            }
#undef DEC
        }

        if (verbose) bsl::cout << "quantize Decimal128 tests..." << bsl::endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DL(X)
            typedef BDEC::Decimal128          TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;

            const TYPE sNaN(NumLim::signaling_NaN());
            const TYPE qNaN(NumLim::quiet_NaN());
            const TYPE pInf(NumLim::infinity());
            const TYPE nInf(-pInf);

            struct {
                int           d_line;
                long long int d_significand;
                int           d_exponent;
                int           d_quantum;
                TYPE          d_expected;
                int           d_retValue;
            } DATA[] = {
      //----------------------------------------------------------------------
      // LINE |  SIGNIFICAND    | EXP | QUANTUM | EXPECTED              | RV
      //----------------------------------------------------------------------
            { L_, 123456789ll,     0,     -26,     qNaN,              -1 },
            { L_, 123456789ll,     0,     -25,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,     -24,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,     -20,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,     -16,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,     -14,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,     -10,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,      -6,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,      -2,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,      -1,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,       0,     DEC(123456789e+0),  0 },
            { L_, 123456789ll,     0,       1,     DEC(123456790e+0), -1 },
            { L_, 123456789ll,     0,       2,     DEC(123456800e+0), -1 },
            { L_, 123456789ll,     0,       3,     DEC(123457000e+0), -1 },
            { L_, 123456789ll,     0,       4,     DEC(123460000e+0), -1 },
            { L_, 123456789ll,     0,       5,     DEC(123500000e+0), -1 },
            { L_, 123456789ll,     0,       6,     DEC(123000000e+0), -1 },
            { L_, 123456789ll,     0,       7,     DEC(120000000e+0), -1 },
            { L_, 123456789ll,     0,       8,     DEC(100000000e+0), -1 },
            { L_, 123456789ll,     0,       9,     DEC(0.0),          -1 },
        };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            NumberMaker<TYPE> makeDecimal;

            // All special case values:
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE        = DATA[ti].d_line;
                const long long int& SIGNIFICAND = DATA[ti].d_significand;
                const int&           EXPONENT    = DATA[ti].d_exponent;
                const int&           QUANTUM     = DATA[ti].d_quantum;

                const TYPE V = makeDecimal(SIGNIFICAND, EXPONENT);
                const TYPE E = makeDecimal(          1, QUANTUM);

                {  //: o TYPE quantize(TYPE, TYPE);

                    const TYPE EXPECTED = DATA[ti].d_expected;
                    const TYPE RESULT   = Util::quantize(V, E);

                    if (Util::isNan(EXPECTED)) {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, Util::isNan(RESULT));
                    } else {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                        LOOP_ASSERT(LINE, Util::sameQuantum(RESULT, E));
                    }

                    ASSERT(Util::isNan(Util::quantize(   V, sNaN)));
                    ASSERT(Util::isNan(Util::quantize(   V, qNaN)));
                    ASSERT(Util::isNan(Util::quantize(   V, pInf)));
                    ASSERT(Util::isNan(Util::quantize(   V, nInf)));
                    ASSERT(Util::isNan(Util::quantize(sNaN,    E)));
                    ASSERT(Util::isNan(Util::quantize(qNaN,    E)));
                    ASSERT(Util::isNan(Util::quantize(pInf,    E)));
                    ASSERT(Util::isNan(Util::quantize(nInf,    E)));
                }

                {  //: o TYPE quantize(TYPE, int);

                    const TYPE& EXPECTED = DATA[ti].d_expected;
                    const TYPE  RESULT   = Util::quantize(V, QUANTUM);

                    if (Util::isNan(EXPECTED)) {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, Util::isNan(RESULT));
                    } else {
                        LOOP3_ASSERT(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
                        LOOP_ASSERT(LINE, Util::sameQuantum(RESULT, E));
                    }

                    ASSERT(Util::isNan(Util::quantize(sNaN, 0)));
                    ASSERT(Util::isNan(Util::quantize(qNaN, 0)));
                    ASSERT(Util::isNan(Util::quantize(pInf, 0)));
                    ASSERT(Util::isNan(Util::quantize(nInf, 0)));
                }

                {  //: o int quantizeEqual(TYPE *, TYPE, int);

                    TYPE X(V); TYPE *X_P(&X);
                    TYPE Y(V);

                    const int& EXPECTED = DATA[ti].d_retValue;
                    const int  RESULT   = Util::quantizeEqual(X_P, Y, QUANTUM);

                    LOOP_ASSERT(LINE, RESULT == EXPECTED);
                    LOOP_ASSERT(LINE, X == V);
                    if (0 == RESULT) {
                        LOOP_ASSERT(LINE, Util::sameQuantum(X, E));
                    }

                    ASSERT(-1 == Util::quantizeEqual(X_P, sNaN, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, qNaN, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, pInf, 0));
                    ASSERT(-1 == Util::quantizeEqual(X_P, nInf, 0));
                }
            }

            if (verbose) cout << "\tNegative Testing." << endl;
            {
                bsls::AssertTestHandlerGuard hG;

                const int   VALID_EXPONENT1 = -6176;
                const int   VALID_EXPONENT2 =  6111;
                const int INVALID_EXPONENT1 = -6177;
                const int INVALID_EXPONENT2 =  6112;

                TYPE O; TYPE *O_P(&O);

                ASSERT_PASS(Util::quantize(O,     VALID_EXPONENT1));
                ASSERT_PASS(Util::quantize(O,     VALID_EXPONENT2));
                ASSERT_FAIL(Util::quantize(O,   INVALID_EXPONENT1));
                ASSERT_FAIL(Util::quantize(O,   INVALID_EXPONENT2));

                ASSERT_PASS(Util::quantizeEqual(O_P, O,   VALID_EXPONENT1));
                ASSERT_PASS(Util::quantizeEqual(O_P, O,   VALID_EXPONENT2));
                ASSERT_FAIL(Util::quantizeEqual(O_P, O, INVALID_EXPONENT1));
                ASSERT_FAIL(Util::quantizeEqual(O_P, O, INVALID_EXPONENT2));
            }

#undef DEC
        }
    } break;
    case 7: {
        // --------------------------------------------------------------------
        // TESTING quantum
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: quantum
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "quantum Decimal64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for quantum, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                bsls::AssertTestHandlerGuard g;
                // o sNaN
                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(sNaN));

                // o qNaN
                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(qNaN));

                // o +Inf
                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(pInf));

                // o -Inf
                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                        makeNumber(mantissas[tiM], exps[tiE]);

                    // Test the value of what quantum returns:
                    LOOP6_ASSERT(tiM, tiE, value, mantissas[tiM], exps[tiE],
                                 Util::quantum(value),
                                 Util::quantum(value) == exps[tiE]);
                }
            }
        }
        if (verbose) bsl::cout << "quantum Decimal128 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for quantum, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            //: o signaling NaN     (sNaN)

            const TYPE sNaN(NumLim::signaling_NaN());

            //: o quiet NaN         (qNaN)

            const TYPE qNaN(NumLim::quiet_NaN());

            //: o positive Infinity (+Inf)

            const TYPE pInf(NumLim::infinity());

            //: o negative Infinity (-Inf)

            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                bsls::AssertTestHandlerGuard g;

                //: o sNaN

                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(sNaN));

                //: o qNaN

                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(qNaN));

                //: o +Inf

                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(pInf));

                //: o -Inf

                BSLS_ASSERTTEST_ASSERT_FAIL(Util::quantum(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                        makeNumber(mantissas[tiM], exps[tiE]);
                    const int quantum = Util::quantum(value);

                    // Test the value of what quantum returns:
                    LOOP6_ASSERT(tiM, tiE, mantissas[tiM], exps[tiE],
                                 value, quantum,
                                 quantum == exps[tiE]);
                }
            }
        }
    } break;
    case 6: {
        // --------------------------------------------------------------------
        // TESTING sameQuantum
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: sameQuantum
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "sameQuantum Decimal64 tests..."
                                << bsl::endl;

        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for sameQuantum, which depends upon the strict, narrow
            // contract for makeDecimalRaw.


            // All special case values:
            //: o signaling NaN     (sNaN)

            const TYPE sNaN(NumLim::signaling_NaN());

            //: o quiet NaN         (qNaN)

            const TYPE qNaN(NumLim::quiet_NaN());

            //: o positive Infinity (+Inf)

            const TYPE pInf(NumLim::infinity());

            //: o negative Infinity (-Inf)

            const TYPE nInf(-pInf);

            // Test all special cases with each other, organized by LHS.

            //: o sNaN

            ASSERT( Util::sameQuantum(sNaN, sNaN));
            ASSERT( Util::sameQuantum(sNaN, qNaN));
            ASSERT(!Util::sameQuantum(sNaN, pInf));
            ASSERT(!Util::sameQuantum(sNaN, nInf));

            //: o qNaN

            ASSERT( Util::sameQuantum(qNaN, sNaN));
            ASSERT( Util::sameQuantum(qNaN, qNaN));
            ASSERT(!Util::sameQuantum(qNaN, pInf));
            ASSERT(!Util::sameQuantum(qNaN, nInf));

            // Note that +Inf compares equal to all inf values
            //: o +Inf

            ASSERT(!Util::sameQuantum(pInf, sNaN));
            ASSERT(!Util::sameQuantum(pInf, qNaN));
            ASSERT( Util::sameQuantum(pInf, pInf));
            ASSERT( Util::sameQuantum(pInf, nInf));

            // Note that -Inf compares equal to all inf values
            //: o -Inf

            ASSERT(!Util::sameQuantum(nInf, sNaN));
            ASSERT(!Util::sameQuantum(nInf, qNaN));
            ASSERT( Util::sameQuantum(nInf, pInf));
            ASSERT( Util::sameQuantum(nInf, nInf));


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                        makeNumber(mantissas[tiM], exps[tiE]);

                    // Test all special cases on both sides:
                    //: o sNaN
                    //: o qNaN
                    //: o +Inf
                    //: o -Inf
                    ASSERT(!Util::sameQuantum(value, sNaN));
                    ASSERT(!Util::sameQuantum(value, qNaN));
                    ASSERT(!Util::sameQuantum(value, pInf));
                    ASSERT(!Util::sameQuantum(value, nInf));

                    ASSERT(!Util::sameQuantum(sNaN, value));
                    ASSERT(!Util::sameQuantum(qNaN, value));
                    ASSERT(!Util::sameQuantum(pInf, value));
                    ASSERT(!Util::sameQuantum(nInf, value));
                }
            }

            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            // These will be compared with other values created from
            // the same table, in the same way.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE lhs =
                        makeNumber(mantissas[tiM], exps[tiE]);

                    for (long long tjM = 0; tjM < numMantissas; ++tjM) {
                        for (  int tjE = 0; tjE < numExps;      ++tjE) {
                            const TYPE rhs =
                                makeNumber(mantissas[tjM], exps[tjE]);

                            LOOP4_ASSERT(mantissas[tiM], exps[tiE],
                                         mantissas[tjM], exps[tjE],
                                         (tiE == tjE) ==
                                         Util::sameQuantum(lhs, rhs));
                        }
                    }
                }
            }
        }

        if (verbose) bsl::cout << "sameQuantum Decimal128 tests..."
                                << bsl::endl;

        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for sameQuantum, which depends upon the strict, narrow
            // contract for makeDecimalRaw.


            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test all special cases with each other,
            // organized by LHS.

            // o sNaN
            ASSERT( Util::sameQuantum(sNaN, sNaN));
            ASSERT( Util::sameQuantum(sNaN, qNaN));
            ASSERT(!Util::sameQuantum(sNaN, pInf));
            ASSERT(!Util::sameQuantum(sNaN, nInf));

            // o qNaN
            ASSERT( Util::sameQuantum(qNaN, sNaN));
            ASSERT( Util::sameQuantum(qNaN, qNaN));
            ASSERT(!Util::sameQuantum(qNaN, pInf));
            ASSERT(!Util::sameQuantum(qNaN, nInf));

            // Note that +Inf compares equal to all inf values
            // o +Inf
            ASSERT(!Util::sameQuantum(pInf, sNaN));
            ASSERT(!Util::sameQuantum(pInf, qNaN));
            ASSERT( Util::sameQuantum(pInf, pInf));
            ASSERT( Util::sameQuantum(pInf, nInf));

            // Note that -Inf compares equal to all inf values
            // o -Inf
            ASSERT(!Util::sameQuantum(nInf, sNaN));
            ASSERT(!Util::sameQuantum(nInf, qNaN));
            ASSERT( Util::sameQuantum(nInf, pInf));
            ASSERT( Util::sameQuantum(nInf, nInf));


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                        makeNumber(mantissas[tiM], exps[tiE]);

                    // Test all special cases on both sides:
                    // o sNaN
                    // o qNaN
                    // o +Inf
                    // o -Inf
                    ASSERT(!Util::sameQuantum(value, sNaN));
                    ASSERT(!Util::sameQuantum(value, qNaN));
                    ASSERT(!Util::sameQuantum(value, pInf));
                    ASSERT(!Util::sameQuantum(value, nInf));

                    ASSERT(!Util::sameQuantum(sNaN, value));
                    ASSERT(!Util::sameQuantum(qNaN, value));
                    ASSERT(!Util::sameQuantum(pInf, value));
                    ASSERT(!Util::sameQuantum(nInf, value));
                }
            }

            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            // These will be compared with other values created from
            // the same table, in the same way.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE lhs =
                               makeNumber(mantissas[tiM], exps[tiE]);

                    for (long long tjM = 0; tjM < numMantissas; ++tjM) {
                        for (  int tjE = 0; tjE < numExps;      ++tjE) {
                            const TYPE rhs =
                               makeNumber(mantissas[tjM], exps[tjE]);

                            // Quanta is unspecified if 'mantissa == 0'.
                            LOOP4_ASSERT(mantissas[tiM], exps[tiE],
                                         mantissas[tjM], exps[tjE],
                                  (tiE == tjE) == Util::sameQuantum(lhs, rhs));
                        }
                    }
                }
            }
        }
    } break;
    case 5: {
        // --------------------------------------------------------------------
        // TESTING isFinite
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: isFinite
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "isFinite Decimal64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isFinite, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                // o sNaN
                ASSERT(!Util::isFinite(sNaN));

                // o qNaN
                ASSERT(!Util::isFinite(qNaN));

                // o +Inf
                ASSERT(!Util::isFinite(pInf));

                // o -Inf
                ASSERT(!Util::isFinite(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test that any real value isn't Inf
                    ASSERT(Util::isFinite(value));
                }
            }
        }
        if (verbose) bsl::cout << "isFinite Decimal128 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isNan, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            //: o signaling NaN     (sNaN)

            const TYPE sNaN(NumLim::signaling_NaN());

            //: o quiet NaN         (qNaN)

            const TYPE qNaN(NumLim::quiet_NaN());

            //: o positive Infinity (+Inf)

            const TYPE pInf(NumLim::infinity());

            //: o negative Infinity (-Inf)

            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                //: o sNaN

                ASSERT(!Util::isFinite(sNaN));

                //: o qNaN

                ASSERT(!Util::isFinite(qNaN));

                //: o +Inf

                ASSERT(!Util::isFinite(pInf));

                //: o -Inf

                ASSERT(!Util::isFinite(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test that any real value isn't NaN
                    ASSERT(Util::isFinite(value));
                }
            }
        }
    } break;
    case 4: {
        // --------------------------------------------------------------------
        // TESTING isInf
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: isInf
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "isInf Decimal64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isInf, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            //: o signaling NaN     (sNaN)

            const TYPE sNaN(NumLim::signaling_NaN());

            //: o quiet NaN         (qNaN)

            const TYPE qNaN(NumLim::quiet_NaN());

            //: o positive Infinity (+Inf)

            const TYPE pInf(NumLim::infinity());

            //: o negative Infinity (-Inf)

            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                //: o sNaN

                ASSERT(!Util::isInf(sNaN));

                //: o qNaN

                ASSERT(!Util::isInf(qNaN));

                //: o +Inf

                ASSERT( Util::isInf(pInf));

                //: o -Inf

                ASSERT( Util::isInf(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test that any real value isn't Inf
                    ASSERT(!Util::isInf(value));
                }
            }
        }
        if (verbose) bsl::cout << "isInf Decimal128 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isNan, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:

            //: o signaling NaN     (sNaN)

            const TYPE sNaN(NumLim::signaling_NaN());

            //: o quiet NaN         (qNaN)

            const TYPE qNaN(NumLim::quiet_NaN());

            //: o positive Infinity (+Inf)

            const TYPE pInf(NumLim::infinity());

            //: o negative Infinity (-Inf)

            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                // o sNaN
                ASSERT(!Util::isInf(sNaN));

                // o qNaN
                ASSERT(!Util::isInf(qNaN));

                // o +Inf
                ASSERT( Util::isInf(pInf));

                // o -Inf
                ASSERT( Util::isInf(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test that any real value isn't NaN
                    ASSERT(!Util::isInf(value));
                }
            }
        }
    } break;
    case 3: {
        // --------------------------------------------------------------------
        // TESTING isUnordered
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: isUnordered
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "isUnordered Decimal64 tests..."
                                << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isNan, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            //: o signaling NaN     (sNaN)

            const TYPE sNaN(NumLim::signaling_NaN());

            //: o quiet NaN         (qNaN)

            const TYPE qNaN(NumLim::quiet_NaN());

            //: o positive Infinity (+Inf)

            const TYPE pInf(NumLim::infinity());

            //: o negative Infinity (-Inf)

            const TYPE nInf(-pInf);

            {
                // Test all special cases with each other,
                // organized by LHS.

                //: o sNaN

                ASSERT( Util::isUnordered(sNaN, sNaN));
                ASSERT( Util::isUnordered(sNaN, qNaN));
                ASSERT( Util::isUnordered(sNaN, pInf));
                ASSERT( Util::isUnordered(sNaN, nInf));

                //: o qNaN

                ASSERT( Util::isUnordered(qNaN, sNaN));
                ASSERT( Util::isUnordered(qNaN, qNaN));
                ASSERT( Util::isUnordered(qNaN, pInf));
                ASSERT( Util::isUnordered(qNaN, nInf));

                // Note that +Inf compares equal to all inf values
                //: o +Inf

                ASSERT( Util::isUnordered(pInf, sNaN));
                ASSERT( Util::isUnordered(pInf, qNaN));
                ASSERT(!Util::isUnordered(pInf, pInf));
                ASSERT(!Util::isUnordered(pInf, nInf));

                // Note that -Inf compares equal to all inf values

                //: o -Inf

                ASSERT( Util::isUnordered(nInf, sNaN));
                ASSERT( Util::isUnordered(nInf, qNaN));
                ASSERT(!Util::isUnordered(nInf, pInf));
                ASSERT(!Util::isUnordered(nInf, nInf));
            }


            // Iterate through all possible pairings of mantissa and exponent,
            // and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test all special cases on both sides:
                    //
                    //: o sNaN
                    //: o qNaN
                    //: o +Inf
                    //: o -Inf

                    ASSERT( Util::isUnordered(value, sNaN));
                    ASSERT( Util::isUnordered(value, qNaN));
                    ASSERT(!Util::isUnordered(value, pInf));
                    ASSERT(!Util::isUnordered(value, nInf));

                    ASSERT( Util::isUnordered(sNaN, value));
                    ASSERT( Util::isUnordered(qNaN, value));
                    ASSERT(!Util::isUnordered(pInf, value));
                    ASSERT(!Util::isUnordered(nInf, value));
                }
            }
        }

        if (verbose) bsl::cout << "isUnordered Decimal128 tests..."
                                << bsl::endl;
        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isNan, which depends upon the strict, narrow contract
            // for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            {
                // Test all special cases with each other,
                // organized by LHS.

                // o sNaN
                ASSERT( Util::isUnordered(sNaN, sNaN));
                ASSERT( Util::isUnordered(sNaN, qNaN));
                ASSERT( Util::isUnordered(sNaN, pInf));
                ASSERT( Util::isUnordered(sNaN, nInf));

                // o qNaN
                ASSERT( Util::isUnordered(qNaN, sNaN));
                ASSERT( Util::isUnordered(qNaN, qNaN));
                ASSERT( Util::isUnordered(qNaN, pInf));
                ASSERT( Util::isUnordered(qNaN, nInf));

                // Note that +Inf compares equal to all inf values
                // o +Inf
                ASSERT( Util::isUnordered(pInf, sNaN));
                ASSERT( Util::isUnordered(pInf, qNaN));
                ASSERT(!Util::isUnordered(pInf, pInf));
                ASSERT(!Util::isUnordered(pInf, nInf));

                // Note that -Inf compares equal to all inf values
                // o -Inf
                ASSERT( Util::isUnordered(nInf, sNaN));
                ASSERT( Util::isUnordered(nInf, qNaN));
                ASSERT(!Util::isUnordered(nInf, pInf));
                ASSERT(!Util::isUnordered(nInf, nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test all special cases on both sides:
                    // o sNaN
                    // o qNaN
                    // o +Inf
                    // o -Inf
                    ASSERT( Util::isUnordered(value, sNaN));
                    ASSERT( Util::isUnordered(value, qNaN));
                    ASSERT(!Util::isUnordered(value, pInf));
                    ASSERT(!Util::isUnordered(value, nInf));

                    ASSERT( Util::isUnordered(sNaN, value));
                    ASSERT( Util::isUnordered(qNaN, value));
                    ASSERT(!Util::isUnordered(pInf, value));
                    ASSERT(!Util::isUnordered(nInf, value));
                }
            }
        }
    } break;
    case 2: {
        // --------------------------------------------------------------------
        // TESTING isNan
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: isNan
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "isNan Decimal64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isNan, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                // o sNaN
                ASSERT( Util::isNan(sNaN));

                // o qNaN
                ASSERT( Util::isNan(qNaN));

                // o +Inf
                ASSERT(!Util::isNan(pInf));

                // o -Inf
                ASSERT(!Util::isNan(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test that any real value isn't NaN
                    ASSERT(!Util::isNan(value));
                }
            }
        }
        if (verbose) bsl::cout << "isNan Decimal128 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal128 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for isNan, which depends upon the strict, narrow
            // contract for makeDecimalRaw.

            // All special case values:
            // o signaling NaN     (sNaN)
            const TYPE sNaN(NumLim::signaling_NaN());
            // o quiet NaN         (qNaN)
            const TYPE qNaN(NumLim::quiet_NaN());
            // o positive Infinity (+Inf)
            const TYPE pInf(NumLim::infinity());
            // o negative Infinity (-Inf)
            const TYPE nInf(-pInf);

            // Test that all special cases fail.
            {
                // o sNaN
                ASSERT( Util::isNan(sNaN));

                // o qNaN
                ASSERT( Util::isNan(qNaN));

                // o +Inf
                ASSERT(!Util::isNan(pInf));

                // o -Inf
                ASSERT(!Util::isNan(nInf));
            }


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE value =
                            makeNumber(mantissas[tiM], exps[tiE]);

                    // Test that any real value isn't NaN
                    ASSERT(!Util::isNan(value));
                }
            }
        }
    } break;
    case 1: {
        // --------------------------------------------------------------------
        // TESTING Breathing test
        // Concerns: Forwarding to the right routines
        // Plan: Try all operations see if basics work
        // Testing: all functions
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                               << "Breathing Test" << bsl::endl
                               << "==============" << bsl::endl;

        if (veryVerbose) bsl::cout << "makeDecimalNNRaw functions"
                                   << bsl::endl;

        if (veryVeryVerbose) bsl::cout << "makeDecimalRaw32" << bsl::endl;

        bdldfp::Decimal32 mdr = Util::makeDecimalRaw32(314159, -5);
        bdldfp::Decimal32 lit = BDLDFP_DECIMAL_DF(3.14159);
        LOOP3_ASSERT(mdr, lit, Util::makeDecimalRaw32(314159, -5), mdr == lit);

        mdr = Util::makeDecimalRaw32(314159, 0);
        lit = BDLDFP_DECIMAL_DF(314159.);
        LOOP3_ASSERT(mdr, lit, Util::makeDecimalRaw32(314159, 0), mdr == lit);

        if (veryVeryVerbose) bsl::cout << "makeDecimalRaw64" << bsl::endl;

        ASSERT(Util::makeDecimalRaw64(314159, -5) ==
               BDLDFP_DECIMAL_DD(3.14159));
        ASSERT(Util::makeDecimalRaw64(314159u, -5) ==
               BDLDFP_DECIMAL_DD(3.14159));
        ASSERT(Util::makeDecimalRaw64(314159ll, -5) ==
               BDLDFP_DECIMAL_DD(3.14159));
        ASSERT(Util::makeDecimalRaw64(314159ull, -5) ==
               BDLDFP_DECIMAL_DD(3.14159));

        if (veryVeryVerbose) bsl::cout << "Testing the quanta of 0 value"
                                       << bsl::endl;
        // {DRQS 154126434}
        for (int i = -398; i <= 369; ++i) {

            bdldfp::Decimal64 d64 = Util::makeDecimalRaw64(0, i);

            int                 sign;
            bsls::Types::Uint64 significand;
            int                 exponent;

            int classification = Util::decompose(&sign,
                                                 &significand,
                                                 &exponent,
                                                 d64);

            LOOP2_ASSERT(i, classification, FP_ZERO == classification);
            LOOP2_ASSERT(i, sign,           1       == sign);
            LOOP2_ASSERT(i, significand,    0       == significand);
            LOOP2_ASSERT(i, exponent,       i       == exponent);
        }

        if (veryVeryVerbose) bsl::cout << "makeDecimalRaw128" << bsl::endl;

        // XLC versions prior to 12.0 incorrectly pass decimal128 values in
        // some contexts (0x0c00 -> 12.00)
#if defined(BSLS_PLATFORM_CMP_IBM) && (BSLS_PLATFORM_CMP_VERSION >= 0x0c00)
        ASSERT(Util::makeDecimalRaw128(314159, -5) ==
               BDLDFP_DECIMAL_DL(3.14159));
        ASSERT(Util::makeDecimalRaw128(314159u, -5) ==
               BDLDFP_DECIMAL_DL(3.14159));
        ASSERT(Util::makeDecimalRaw128(314159ll, -5) ==
               BDLDFP_DECIMAL_DL(3.14159));
        ASSERT(Util::makeDecimalRaw128(314159ull, -5) ==
               BDLDFP_DECIMAL_DL(3.14159));
#endif

        if (veryVeryVerbose) bsl::cout << "Testing the quanta of 0 value"
                                       << bsl::endl;
        // {DRQS 154126434}
        for (int i = -6176; i <= 6111; ++i) {

            bdldfp::Decimal128 d128 = Util::makeDecimalRaw128(0, i);

            int           sign;
            BDEC::Uint128 significand;
            int     exponent;

            int classification = Util::decompose(&sign,
                                                 &significand,
                                                 &exponent,
                                                 d128);

            LOOP2_ASSERT(i, classification,     FP_ZERO == classification);
            LOOP2_ASSERT(i, sign,               1       == sign);
            LOOP2_ASSERT(i, significand.high(), 0       == significand.high());
            LOOP2_ASSERT(i, significand.low(),  0       == significand.low());
            LOOP2_ASSERT(i, exponent,           i       == exponent);
        }

        if (veryVerbose) bsl::cout << "makeDecimalNN functions" << bsl::endl;

        if (veryVeryVerbose) bsl::cout << "makeDecimal64" << bsl::endl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
#endif
        // Test some zero-rounded values.

        // Note that the following code does (and MUST) generate warnings on
        // gcc.  Unfortunately, the pragma to disable that warning (above), is
        // ignored.

        ASSERT(Util::makeDecimal64(-1234567890123456ll, -382-16+1) ==
               bdldfp::DecimalImpUtil::parse64("-1.234567890123456e-382"));
        ASSERT(Util::makeDecimal64(1234567890123456ull, -382-16+1) ==
               bdldfp::DecimalImpUtil::parse64("1.234567890123456e-382"));

        // {DRQS 118092561}
        ASSERT(Util::makeDecimal64(bsl::numeric_limits<int32_t>::min(), -3) ==
               bdldfp::DecimalImpUtil::parse64("-2147483.648"));

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        // Test some simple values.

        ASSERT(Util::makeDecimal64(314159, -5) ==
               BDLDFP_DECIMAL_DD(3.14159));
        ASSERT(Util::makeDecimal64(314159u, -5) ==
               BDLDFP_DECIMAL_DD(3.14159));

        if (veryVerbose) bsl::cout << "parseDecimalNN functions" << bsl::endl;

        {
            BDEC::Decimal32 result;
            ASSERT(Util::parseDecimal32(&result, "1234567") == 0);
            ASSERT(BDLDFP_DECIMAL_DF(1234567.0) == result);
        }
        {
            BDEC::Decimal64 result;
            ASSERT(Util::parseDecimal64(&result, "1234567890123456") == 0);
            LOOP2_ASSERT(BDLDFP_DECIMAL_DD(1234567890123456.0),   result,
                         BDLDFP_DECIMAL_DD(1234567890123456.0) == result);
        }
        {
            BDEC::Decimal128 result;
            ASSERT(Util::parseDecimal128(&result,
                                         "1234567890123456789012345678901234")
                   == 0);
            ASSERT(BDLDFP_DECIMAL_DL(1234567890123456789012345678901234.0)
                   == result);
        }

        {
            BDEC::Decimal32 result;
            const bsl::string str("1234567", pa);
            ASSERT(Util::parseDecimal32(&result, str) == 0);
            ASSERT(BDLDFP_DECIMAL_DF(1234567.0) == result);
        }
        {
            BDEC::Decimal64 result;
            const bsl::string str("1234567890123456", pa);
            ASSERT(Util::parseDecimal64(&result, str) == 0);
            ASSERT(BDLDFP_DECIMAL_DD(1234567890123456.0) == result);
        }
        {
            BDEC::Decimal128 result;
            const bsl::string str("1234567890123456789012345678901234", pa);
            ASSERT(Util::parseDecimal128(&result,str) == 0);
            ASSERT(BDLDFP_DECIMAL_DL(1234567890123456789012345678901234.0)
                   == result);
        }

        if (veryVerbose) bsl::cout << "fma functions" << bsl::endl;

        // TODO TBD - How to test if fma really does not round too early?

        ASSERT(Util::fma(BDLDFP_DECIMAL_DD(3.),
                         BDLDFP_DECIMAL_DD(4.),
                         BDLDFP_DECIMAL_DD(5.)) == BDLDFP_DECIMAL_DD(17.));

        ASSERT(Util::fma(BDLDFP_DECIMAL_DL(3.),
                         BDLDFP_DECIMAL_DL(4.),
                         BDLDFP_DECIMAL_DL(5.)) == BDLDFP_DECIMAL_DL(17.));

        if (veryVerbose) bsl::cout << "fabs functions" << bsl::endl;

        ASSERT(Util::fabs(BDLDFP_DECIMAL_DF(-1.234567e-94))
               == BDLDFP_DECIMAL_DF(1.234567e-94));

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
#endif

        // Test using zero-rounded numbers

        // Note that the following code does (and MUST) generate warnings on
        // gcc.  Unfortunately, the pragma to disable that warning (above), is
        // ignored.

        ASSERT(Util::fabs(
                    bdldfp::DecimalImpUtil::parse64("-1.234567890123456e-382"))
               == bdldfp::DecimalImpUtil::parse64("1.234567890123456e-382"));
        ASSERT(Util::fabs(
                   bdldfp::DecimalImpUtil::parse128("-1.234567890123456e-382"))
               == bdldfp::DecimalImpUtil::parse128("1.234567890123456e-382"));

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        if (veryVerbose) bsl::cout << "Classification functions" << bsl::endl;

        if (veryVeryVerbose) bsl::cout << "Decimal32" << bsl::endl;
        {
            typedef BDEC::Decimal32 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DF(x)

            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            ASSERT(Util::classify(oNaNq) == FP_NAN);
            ASSERT(Util::classify(oNaNs) == FP_NAN);

            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);
            ASSERT(Util::classify(oInfP) == FP_INFINITE);
            ASSERT(Util::classify(oInfN) == FP_INFINITE);

            const Tested oNorm1(NumLim::max());
            const Tested oNorm2(NumLim::min());
            ASSERT(Util::classify(oNorm1) == FP_NORMAL);
            ASSERT(Util::classify(oNorm2) == FP_NORMAL);

            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            ASSERT(Util::classify(oZeroP) == FP_ZERO);
            ASSERT(Util::classify(oZeroN) == FP_ZERO);

            const Tested oDenmP(NumLim::denorm_min());
            const Tested oDenmN(-oDenmP);
            ASSERT(Util::classify(oDenmP) == FP_SUBNORMAL);
            ASSERT(Util::classify(oDenmN) == FP_SUBNORMAL);

            ASSERT(Util::isNan(oNaNq)  == true);
            ASSERT(Util::isNan(oNaNs)  == true);
            ASSERT(Util::isNan(oInfP)  == false);
            ASSERT(Util::isNan(oInfN)  == false);
            ASSERT(Util::isNan(oNorm1) == false);
            ASSERT(Util::isNan(oNorm2) == false);
            ASSERT(Util::isNan(oZeroP) == false);
            ASSERT(Util::isNan(oZeroN) == false);
            ASSERT(Util::isNan(oDenmP) == false);
            ASSERT(Util::isNan(oDenmN) == false);

            ASSERT(Util::isInf(oNaNq)  == false);
            ASSERT(Util::isInf(oNaNs)  == false);
            ASSERT(Util::isInf(oInfP)  == true);
            ASSERT(Util::isInf(oInfN)  == true);
            ASSERT(Util::isInf(oNorm1) == false);
            ASSERT(Util::isInf(oNorm2) == false);
            ASSERT(Util::isInf(oZeroP) == false);
            ASSERT(Util::isInf(oZeroN) == false);
            ASSERT(Util::isInf(oDenmP) == false);
            ASSERT(Util::isInf(oDenmN) == false);

            ASSERT(Util::isFinite(oNaNq)  == false);
            ASSERT(Util::isFinite(oNaNs)  == false);
            ASSERT(Util::isFinite(oInfP)  == false);
            ASSERT(Util::isFinite(oInfN)  == false);
            ASSERT(Util::isFinite(oNorm1) == true);
            ASSERT(Util::isFinite(oNorm2) == true);
            ASSERT(Util::isFinite(oZeroP) == true);
            ASSERT(Util::isFinite(oZeroN) == true);
            ASSERT(Util::isFinite(oDenmP) == true);
            ASSERT(Util::isFinite(oDenmN) == true);

            ASSERT(Util::isNormal(oNaNq)  == false);
            ASSERT(Util::isNormal(oNaNs)  == false);
            ASSERT(Util::isNormal(oInfP)  == false);
            ASSERT(Util::isNormal(oInfN)  == false);
            ASSERT(Util::isNormal(oNorm1) == true);
            ASSERT(Util::isNormal(oNorm2) == true);
            ASSERT(Util::isNormal(oZeroP) == false);
            ASSERT(Util::isNormal(oZeroN) == false);
            ASSERT(Util::isNormal(oDenmP) == false);
            ASSERT(Util::isNormal(oDenmN) == false);

            #undef DECLIT
        }

        if (veryVeryVerbose) bsl::cout << "Decimal64" << bsl::endl;
        {
            typedef BDEC::Decimal64 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DD(x)

            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            ASSERT(Util::classify(oNaNq) == FP_NAN);
            ASSERT(Util::classify(oNaNs) == FP_NAN);

            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);
            ASSERT(Util::classify(oInfP) == FP_INFINITE);
            ASSERT(Util::classify(oInfN) == FP_INFINITE);

            const Tested oNorm1(NumLim::max());
            const Tested oNorm2(NumLim::min());
            ASSERT(Util::classify(oNorm1) == FP_NORMAL);
            ASSERT(Util::classify(oNorm2) == FP_NORMAL);

            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            ASSERT(Util::classify(oZeroP) == FP_ZERO);
            ASSERT(Util::classify(oZeroN) == FP_ZERO);

            const Tested oDenmP(NumLim::denorm_min());
            const Tested oDenmN(-oDenmP);
            ASSERT(Util::classify(oDenmP) == FP_SUBNORMAL);
            ASSERT(Util::classify(oDenmN) == FP_SUBNORMAL);

            ASSERT(Util::isNan(oNaNq)  == true);
            ASSERT(Util::isNan(oNaNs)  == true);
            ASSERT(Util::isNan(oInfP)  == false);
            ASSERT(Util::isNan(oInfN)  == false);
            ASSERT(Util::isNan(oNorm1) == false);
            ASSERT(Util::isNan(oNorm2) == false);
            ASSERT(Util::isNan(oZeroP) == false);
            ASSERT(Util::isNan(oZeroN) == false);
            ASSERT(Util::isNan(oDenmP) == false);
            ASSERT(Util::isNan(oDenmN) == false);

            ASSERT(Util::isInf(oNaNq)  == false);
            ASSERT(Util::isInf(oNaNs)  == false);
            ASSERT(Util::isInf(oInfP)  == true);
            ASSERT(Util::isInf(oInfN)  == true);
            ASSERT(Util::isInf(oNorm1) == false);
            ASSERT(Util::isInf(oNorm2) == false);
            ASSERT(Util::isInf(oZeroP) == false);
            ASSERT(Util::isInf(oZeroN) == false);
            ASSERT(Util::isInf(oDenmP) == false);
            ASSERT(Util::isInf(oDenmN) == false);

            ASSERT(Util::isFinite(oNaNq)  == false);
            ASSERT(Util::isFinite(oNaNs)  == false);
            ASSERT(Util::isFinite(oInfP)  == false);
            ASSERT(Util::isFinite(oInfN)  == false);
            ASSERT(Util::isFinite(oNorm1) == true);
            ASSERT(Util::isFinite(oNorm2) == true);
            ASSERT(Util::isFinite(oZeroP) == true);
            ASSERT(Util::isFinite(oZeroN) == true);
            ASSERT(Util::isFinite(oDenmP) == true);
            ASSERT(Util::isFinite(oDenmN) == true);

            ASSERT(Util::isNormal(oNaNq)  == false);
            ASSERT(Util::isNormal(oNaNs)  == false);
            ASSERT(Util::isNormal(oInfP)  == false);
            ASSERT(Util::isNormal(oInfN)  == false);
            ASSERT(Util::isNormal(oNorm1) == true);
            ASSERT(Util::isNormal(oNorm2) == true);
            ASSERT(Util::isNormal(oZeroP) == false);
            ASSERT(Util::isNormal(oZeroN) == false);
            ASSERT(Util::isNormal(oDenmP) == false);
            ASSERT(Util::isNormal(oDenmN) == false);

            #undef DECLIT
        }

        if (veryVeryVerbose) bsl::cout << "Decimal128" << bsl::endl;
        {
            typedef BDEC::Decimal128 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DL(x)

            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            ASSERT(Util::classify(oNaNq) == FP_NAN);
            ASSERT(Util::classify(oNaNs) == FP_NAN);

            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);
            ASSERT(Util::classify(oInfP) == FP_INFINITE);
            ASSERT(Util::classify(oInfN) == FP_INFINITE);

            const Tested oNorm1(NumLim::max());
            const Tested oNorm2(NumLim::min());
            ASSERT(Util::classify(oNorm1) == FP_NORMAL);
            ASSERT(Util::classify(oNorm2) == FP_NORMAL);

            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            ASSERT(Util::classify(oZeroP) == FP_ZERO);
            ASSERT(Util::classify(oZeroN) == FP_ZERO);

            const Tested oDenmP(NumLim::denorm_min());
            const Tested oDenmN(-oDenmP);
            ASSERT(Util::classify(oDenmP) == FP_SUBNORMAL);
            ASSERT(Util::classify(oDenmN) == FP_SUBNORMAL);

            ASSERT(Util::isNan(oNaNq)  == true);
            ASSERT(Util::isNan(oNaNs)  == true);
            ASSERT(Util::isNan(oInfP)  == false);
            ASSERT(Util::isNan(oInfN)  == false);
            ASSERT(Util::isNan(oNorm1) == false);
            ASSERT(Util::isNan(oNorm2) == false);
            ASSERT(Util::isNan(oZeroP) == false);
            ASSERT(Util::isNan(oZeroN) == false);
            ASSERT(Util::isNan(oDenmP) == false);
            ASSERT(Util::isNan(oDenmN) == false);

            ASSERT(Util::isInf(oNaNq)  == false);
            ASSERT(Util::isInf(oNaNs)  == false);
            ASSERT(Util::isInf(oInfP)  == true);
            ASSERT(Util::isInf(oInfN)  == true);
            ASSERT(Util::isInf(oNorm1) == false);
            ASSERT(Util::isInf(oNorm2) == false);
            ASSERT(Util::isInf(oZeroP) == false);
            ASSERT(Util::isInf(oZeroN) == false);
            ASSERT(Util::isInf(oDenmP) == false);
            ASSERT(Util::isInf(oDenmN) == false);

            ASSERT(Util::isFinite(oNaNq)  == false);
            ASSERT(Util::isFinite(oNaNs)  == false);
            ASSERT(Util::isFinite(oInfP)  == false);
            ASSERT(Util::isFinite(oInfN)  == false);
            ASSERT(Util::isFinite(oNorm1) == true);
            ASSERT(Util::isFinite(oNorm2) == true);
            ASSERT(Util::isFinite(oZeroP) == true);
            ASSERT(Util::isFinite(oZeroN) == true);
            ASSERT(Util::isFinite(oDenmP) == true);
            ASSERT(Util::isFinite(oDenmN) == true);

            ASSERT(Util::isNormal(oNaNq)  == false);
            ASSERT(Util::isNormal(oNaNs)  == false);
            ASSERT(Util::isNormal(oInfP)  == false);
            ASSERT(Util::isNormal(oInfN)  == false);
            ASSERT(Util::isNormal(oNorm1) == true);
            ASSERT(Util::isNormal(oNorm2) == true);
            ASSERT(Util::isNormal(oZeroP) == false);
            ASSERT(Util::isNormal(oZeroN) == false);
            ASSERT(Util::isNormal(oDenmP) == false);
            ASSERT(Util::isNormal(oDenmN) == false);

            #undef DECLIT
        }

        if (veryVerbose) bsl::cout << "Comparison functions" << bsl::endl;

        if (veryVeryVerbose) bsl::cout << "Decimal32" << bsl::endl;
        {
            typedef BDEC::Decimal32 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DF(x)

            static const unsigned unorderedValuesSize = 2;
            static const Tested unorderedValues[unorderedValuesSize] = {
                NumLim::quiet_NaN(),
                NumLim::signaling_NaN()
            };

            static const unsigned orderedValuesSize = 4;
            static const Tested orderedValues[orderedValuesSize] = {
                NumLim::infinity(),
                NumLim::max(),
                DECLIT(0.0),
                NumLim::denorm_min()
            };

            // ordered and ordered
            for (unsigned i = 0; i < orderedValuesSize; ++i) {
                for (unsigned j = 0; j < orderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(orderedValues[i],
                                             orderedValues[j]) == false);
                }
            }

            // unordered and ordered
            for (unsigned i = 0; i < unorderedValuesSize; ++i) {
                for (unsigned j = 0; j < orderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(unorderedValues[i],
                                             orderedValues[j]) == true);
                }
            }

            // ordered and unordered
            for (unsigned i = 0; i < orderedValuesSize; ++i) {
                for (unsigned j = 0; j < unorderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(orderedValues[i],
                                             unorderedValues[j]) == true);
                }
            }

            // unordered and unordered
            for (unsigned i = 0; i < unorderedValuesSize; ++i) {
                for (unsigned j = 0; j < unorderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(unorderedValues[i],
                                             unorderedValues[j]) == true);
                }
            }

            #undef DECLIT
        }

        if (veryVeryVerbose) bsl::cout << "Decimal64" << bsl::endl;
        {
            typedef BDEC::Decimal64 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DD(x)

            static const unsigned unorderedValuesSize = 2;
            static const Tested unorderedValues[unorderedValuesSize] = {
                NumLim::quiet_NaN(),
                NumLim::signaling_NaN()
            };

            static const unsigned orderedValuesSize = 4;
            static const Tested orderedValues[orderedValuesSize] = {
                NumLim::infinity(),
                NumLim::max(),
                DECLIT(0.0),
                NumLim::denorm_min()
            };

            // ordered and ordered
            for (unsigned i = 0; i < orderedValuesSize; ++i) {
                for (unsigned j = 0; j < orderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(orderedValues[i],
                                             orderedValues[j]) == false);
                }
            }

            // unordered and ordered
            for (unsigned i = 0; i < unorderedValuesSize; ++i) {
                for (unsigned j = 0; j < orderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(unorderedValues[i],
                                             orderedValues[j]) == true);
                }
            }

            // ordered and unordered
            for (unsigned i = 0; i < orderedValuesSize; ++i) {
                for (unsigned j = 0; j < unorderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(orderedValues[i],
                                             unorderedValues[j]) == true);
                }
            }

            // unordered and unordered
            for (unsigned i = 0; i < unorderedValuesSize; ++i) {
                for (unsigned j = 0; j < unorderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(unorderedValues[i],
                                             unorderedValues[j]) == true);
                }
            }

            #undef DECLIT
        }

        if (veryVeryVerbose) bsl::cout << "Decimal128" << bsl::endl;
        {
            typedef BDEC::Decimal128 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DL(x)

            static const unsigned unorderedValuesSize = 2;
            static const Tested unorderedValues[unorderedValuesSize] = {
                NumLim::quiet_NaN(),
                NumLim::signaling_NaN()
            };

            static const unsigned orderedValuesSize = 4;
            static const Tested orderedValues[orderedValuesSize] = {
                NumLim::infinity(),
                NumLim::max(),
                DECLIT(0.0),
                NumLim::denorm_min()
            };

            // ordered and ordered
            for (unsigned i = 0; i < orderedValuesSize; ++i) {
                for (unsigned j = 0; j < orderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(orderedValues[i],
                                             orderedValues[j]) == false);
                }
            }

            // unordered and ordered
            for (unsigned i = 0; i < unorderedValuesSize; ++i) {
                for (unsigned j = 0; j < orderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(unorderedValues[i],
                                             orderedValues[j]) == true);
                }
            }

            // ordered and unordered
            for (unsigned i = 0; i < orderedValuesSize; ++i) {
                for (unsigned j = 0; j < unorderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(orderedValues[i],
                                             unorderedValues[j]) == true);
                }
            }

            // unordered and unordered
            for (unsigned i = 0; i < unorderedValuesSize; ++i) {
                for (unsigned j = 0; j < unorderedValuesSize; ++j) {
                    ASSERT(Util::isUnordered(unorderedValues[i],
                                             unorderedValues[j]) == true);
                }
            }

            #undef DECLIT
        }

        if (veryVerbose) bsl::cout << "Rounding functions" << bsl::endl;

        if (veryVeryVerbose) bsl::cout << "Decimal32" << bsl::endl;
        {
            typedef BDEC::Decimal32 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DF(x)

            const Tested anInt(1234567);
            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);

            ASSERT(Util::ceil(DECLIT( 0.5)) == DECLIT(1.0));
            ASSERT(Util::ceil(DECLIT(-0.5)) == DECLIT(0.0));
            ASSERT(Util::ceil(anInt) == anInt);
            ASSERT(Util::isNan(Util::ceil(oNaNq)));
            ASSERT(Util::isNan(Util::ceil(oNaNs)));
            ASSERT(Util::ceil(oZeroP) == oZeroP);
            ASSERT(Util::ceil(oZeroN) == oZeroN);
            ASSERT(Util::ceil(oInfP) == oInfP);
            ASSERT(Util::ceil(oInfN) == oInfN);

            ASSERT(Util::floor(DECLIT( 0.5)) == DECLIT( 0.0));
            ASSERT(Util::floor(DECLIT(-0.5)) == DECLIT(-1.0));
            ASSERT(Util::floor(anInt) == anInt);
            ASSERT(Util::isNan(Util::floor(oNaNq)));
            ASSERT(Util::isNan(Util::floor(oNaNs)));
            ASSERT(Util::floor(oZeroP) == oZeroP);
            ASSERT(Util::floor(oZeroN) == oZeroN);
            ASSERT(Util::floor(oInfP) == oInfP);
            ASSERT(Util::floor(oInfN) == oInfN);

            ASSERT(Util::trunc(DECLIT( 0.5)) == DECLIT(0.0));
            ASSERT(Util::trunc(DECLIT(-0.5)) == DECLIT(0.0));
            ASSERT(Util::trunc(anInt) == anInt);
            ASSERT(Util::isNan(Util::trunc(oNaNq)));
            ASSERT(Util::isNan(Util::trunc(oNaNs)));
            ASSERT(Util::trunc(oZeroP) == oZeroP);
            ASSERT(Util::trunc(oZeroN) == oZeroN);
            ASSERT(Util::trunc(oInfP) == oInfP);
            ASSERT(Util::trunc(oInfN) == oInfN);

            ASSERT(Util::round(DECLIT( 0.5)) == DECLIT( 1.0));
            ASSERT(Util::round(DECLIT(-0.5)) == DECLIT(-1.0));
            ASSERT(Util::round(anInt) == anInt);
            ASSERT(Util::isNan(Util::round(oNaNq)));
            ASSERT(Util::isNan(Util::round(oNaNs)));
            ASSERT(Util::round(oZeroP) == oZeroP);
            ASSERT(Util::round(oZeroN) == oZeroN);
            ASSERT(Util::round(oInfP) == oInfP);
            ASSERT(Util::round(oInfN) == oInfN);

            #undef DECLIT
        }

        if (veryVeryVerbose) bsl::cout << "Decimal64" << bsl::endl;
        {
            typedef BDEC::Decimal64 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DD(x)

            const Tested anInt(1234567890123456ull);
            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);

            ASSERT(Util::ceil(DECLIT( 0.5)) == DECLIT(1.0));
            ASSERT(Util::ceil(DECLIT(-0.5)) == DECLIT(0.0));
            ASSERT(Util::ceil(anInt) == anInt);
            ASSERT(Util::isNan(Util::ceil(oNaNq)));
            ASSERT(Util::isNan(Util::ceil(oNaNs)));
            ASSERT(Util::ceil(oZeroP) == oZeroP);
            ASSERT(Util::ceil(oZeroN) == oZeroN);
            ASSERT(Util::ceil(oInfP) == oInfP);
            ASSERT(Util::ceil(oInfN) == oInfN);

            ASSERT(Util::floor(DECLIT( 0.5)) == DECLIT( 0.0));
            ASSERT(Util::floor(DECLIT(-0.5)) == DECLIT(-1.0));
            ASSERT(Util::floor(anInt) == anInt);
            ASSERT(Util::isNan(Util::floor(oNaNq)));
            ASSERT(Util::isNan(Util::floor(oNaNs)));
            ASSERT(Util::floor(oZeroP) == oZeroP);
            ASSERT(Util::floor(oZeroN) == oZeroN);
            ASSERT(Util::floor(oInfP) == oInfP);
            ASSERT(Util::floor(oInfN) == oInfN);

            ASSERT(Util::trunc(DECLIT( 0.5)) == DECLIT(0.0));
            ASSERT(Util::trunc(DECLIT(-0.5)) == DECLIT(0.0));
            ASSERT(Util::trunc(anInt) == anInt);
            ASSERT(Util::isNan(Util::trunc(oNaNq)));
            ASSERT(Util::isNan(Util::trunc(oNaNs)));
            ASSERT(Util::trunc(oZeroP) == oZeroP);
            ASSERT(Util::trunc(oZeroN) == oZeroN);
            ASSERT(Util::trunc(oInfP) == oInfP);
            ASSERT(Util::trunc(oInfN) == oInfN);

            ASSERT(Util::round(DECLIT( 0.5)) == DECLIT( 1.0));
            ASSERT(Util::round(DECLIT(-0.5)) == DECLIT(-1.0));
            ASSERT(Util::round(anInt) == anInt);
            ASSERT(Util::isNan(Util::round(oNaNq)));
            ASSERT(Util::isNan(Util::round(oNaNs)));
            ASSERT(Util::round(oZeroP) == oZeroP);
            ASSERT(Util::round(oZeroN) == oZeroN);
            ASSERT(Util::round(oInfP) == oInfP);
            ASSERT(Util::round(oInfN) == oInfN);

            #undef DECLIT
        }

        if (veryVeryVerbose) bsl::cout << "Decimal128" << bsl::endl;
        {
            typedef BDEC::Decimal128 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DL(x)

            const Tested anInt(DECLIT(1234567890123456789012345678901234.0));
            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);

            ASSERT(Util::ceil(DECLIT( 0.5)) == DECLIT(1.0));
            ASSERT(Util::ceil(DECLIT(-0.5)) == DECLIT(0.0));
            ASSERT(Util::ceil(anInt) == anInt);
            ASSERT(Util::isNan(Util::ceil(oNaNq)));
            ASSERT(Util::isNan(Util::ceil(oNaNs)));
            ASSERT(Util::ceil(oZeroP) == oZeroP);
            ASSERT(Util::ceil(oZeroN) == oZeroN);
            ASSERT(Util::ceil(oInfP) == oInfP);
            ASSERT(Util::ceil(oInfN) == oInfN);

            ASSERT(Util::floor(DECLIT( 0.5)) == DECLIT( 0.0));
            ASSERT(Util::floor(DECLIT(-0.5)) == DECLIT(-1.0));
            ASSERT(Util::floor(anInt) == anInt);
            ASSERT(Util::isNan(Util::floor(oNaNq)));
            ASSERT(Util::isNan(Util::floor(oNaNs)));
            ASSERT(Util::floor(oZeroP) == oZeroP);
            ASSERT(Util::floor(oZeroN) == oZeroN);
            ASSERT(Util::floor(oInfP) == oInfP);
            ASSERT(Util::floor(oInfN) == oInfN);

            ASSERT(Util::trunc(DECLIT( 0.5)) == DECLIT(0.0));
            ASSERT(Util::trunc(DECLIT(-0.5)) == DECLIT(0.0));
            ASSERT(Util::trunc(anInt) == anInt);
            ASSERT(Util::isNan(Util::trunc(oNaNq)));
            ASSERT(Util::isNan(Util::trunc(oNaNs)));
            ASSERT(Util::trunc(oZeroP) == oZeroP);
            ASSERT(Util::trunc(oZeroN) == oZeroN);
            ASSERT(Util::trunc(oInfP) == oInfP);
            ASSERT(Util::trunc(oInfN) == oInfN);

            ASSERT(Util::round(DECLIT( 0.5)) == DECLIT( 1.0));
            ASSERT(Util::round(DECLIT(-0.5)) == DECLIT(-1.0));
            ASSERT(Util::round(anInt) == anInt);
            ASSERT(Util::isNan(Util::round(oNaNq)));
            ASSERT(Util::isNan(Util::round(oNaNs)));
            ASSERT(Util::round(oZeroP) == oZeroP);
            ASSERT(Util::round(oZeroN) == oZeroN);
            ASSERT(Util::round(oInfP) == oInfP);
            ASSERT(Util::round(oInfN) == oInfN);

            #undef DECLIT
        }


        if (verbose) bsl::cout << "sameQuantum 64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DL(x)

            const Tested anInt(DECLIT(1234567890123456789012345678901234.0));
            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);

            ASSERT(Util::sameQuantum(anInt, anInt));

            ASSERT(Util::sameQuantum(oNaNq, oNaNq));
            ASSERT(Util::sameQuantum(oNaNs, oNaNq));
            ASSERT(Util::sameQuantum(oNaNq, oNaNs));
            ASSERT(Util::sameQuantum(oNaNs, oNaNs));

            ASSERT(Util::sameQuantum(oZeroP, oZeroP));
            ASSERT(Util::sameQuantum(oZeroN, oZeroP));
            ASSERT(Util::sameQuantum(oZeroP, oZeroN));
            ASSERT(Util::sameQuantum(oZeroN, oZeroN));

            ASSERT(Util::sameQuantum(oInfP, oInfP));
            ASSERT(Util::sameQuantum(oInfN, oInfP));
            ASSERT(Util::sameQuantum(oInfP, oInfN));
            ASSERT(Util::sameQuantum(oInfN, oInfN));

            ASSERT(!Util::sameQuantum(oInfP,  oNaNq));
            ASSERT(!Util::sameQuantum(oNaNq,  oInfP));
            ASSERT(!Util::sameQuantum(oZeroP, oInfP));
            ASSERT(!Util::sameQuantum(oInfP,  oZeroP));
            ASSERT(!Util::sameQuantum(oNaNq,  anInt));

            ASSERT(Util::sameQuantum(Util::multiplyByPowerOf10(anInt, 4),
                                     Util::multiplyByPowerOf10(anInt, 4)));

            ASSERT(!Util::sameQuantum(Util::multiplyByPowerOf10(anInt, 5),
                                      Util::multiplyByPowerOf10(anInt, 4)));

            ASSERT(Util::sameQuantum(Util::multiplyByPowerOf10(anInt, -4),
                                     Util::multiplyByPowerOf10(anInt, -4)));

            ASSERT(!Util::sameQuantum(Util::multiplyByPowerOf10(anInt, -5),
                                      Util::multiplyByPowerOf10(anInt, -4)));

            LOOP5_ASSERT(anInt, Util::multiplyByPowerOf10(anInt, 5),
                         Util::quantum(Util::multiplyByPowerOf10(anInt, 5)),
                         Util::quantum(anInt),
                         Util::quantum(anInt) + 5,
                         Util::quantum(Util::multiplyByPowerOf10(anInt, 5)) ==
                         Util::quantum(anInt) + 5);



        }

        if (verbose) bsl::cout << "sameQuantum 128 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal128 Tested;
            typedef bsl::numeric_limits<Tested> NumLim;
            #define DECLIT(x) BDLDFP_DECIMAL_DL(x)

            const Tested anInt(DECLIT(123456789012345678901234567890123.0));
            const Tested oNaNq(NumLim::quiet_NaN());
            const Tested oNaNs(NumLim::signaling_NaN());
            const Tested oZeroP(DECLIT(0.0));
            const Tested oZeroN(DECLIT(-0.0));
            const Tested oInfP(NumLim::infinity());
            const Tested oInfN(-oInfP);

            ASSERT(Util::sameQuantum(anInt, anInt));

            ASSERT(Util::sameQuantum(oNaNq, oNaNq));
            ASSERT(Util::sameQuantum(oNaNs, oNaNq));
            ASSERT(Util::sameQuantum(oNaNq, oNaNs));
            ASSERT(Util::sameQuantum(oNaNs, oNaNs));

            ASSERT(Util::sameQuantum(oZeroP, oZeroP));
            ASSERT(Util::sameQuantum(oZeroN, oZeroP));
            ASSERT(Util::sameQuantum(oZeroP, oZeroN));
            ASSERT(Util::sameQuantum(oZeroN, oZeroN));

            ASSERT(Util::sameQuantum(oInfP, oInfP));
            ASSERT(Util::sameQuantum(oInfN, oInfP));
            ASSERT(Util::sameQuantum(oInfP, oInfN));
            ASSERT(Util::sameQuantum(oInfN, oInfN));

            ASSERT(!Util::sameQuantum(oInfP,  oNaNq));
            ASSERT(!Util::sameQuantum(oNaNq,  oInfP));
            ASSERT(!Util::sameQuantum(oZeroP, oInfP));
            ASSERT(!Util::sameQuantum(oInfP,  oZeroP));
            ASSERT(!Util::sameQuantum(oNaNq,  anInt));

            ASSERT(Util::sameQuantum(Util::multiplyByPowerOf10(anInt, 4),
                                     Util::multiplyByPowerOf10(anInt, 4)));

            ASSERT(!Util::sameQuantum(Util::multiplyByPowerOf10(anInt, 5),
                                      Util::multiplyByPowerOf10(anInt, 4)));

            ASSERT(Util::quantum(Util::multiplyByPowerOf10(anInt, 5)) ==
                   Util::quantum(anInt) + 5);



        }
    } break;
    case -1: {
        // --------------------------------------------------------------------
        // TESTING: Performance Test of Random Trading Data.
        //
        // Test the performance of a real-world application of decimal
        // floating point values that is intense on calls to makeDecimal64 and
        // binary arithmetic operations such as 'operator*' and 'operator/'.
        // We test an example of aggregating data about tickers, such as the
        // lows and highs in a trading day, volume, and weighted averages.  We
        // wish to determine the number of tickers that may be processed per
        // second, using randomized data.
        // --------------------------------------------------------------------
        bsl::vector<TradeDataPoint> data;
        typedef bsl::vector<TradeDataPoint>::size_type size_type;

        int numDiffTickers =     100;  // Number of different symbols.
        int numTickerData  = 1000000;  // Number of data points.

        for (int i = 0; i < numTickerData; ++i) {
            TradeDataPoint p;
            p.d_symbol = static_cast<char>(rand() % numDiffTickers);
            p.d_exponent = (rand() % 5) - 3;
            p.d_mantissa = rand() % 100000;
            p.d_quantity = 10 * (rand() % 1000);
            data.push_back(p);
        }

        // Find all distinct symbols.
        int numSymbols = 0;
        bsl::unordered_map<bsl::string, int> symbol2Index;
        for (size_type i = 0; i < data.size(); ++i) {
            const bsl::string& symbol = data[i].d_symbol;
            if (symbol2Index.find(symbol) == symbol2Index.end()) {
                symbol2Index[symbol] = numSymbols++;
            }
        }

        // Initialize data.
        bsl::vector<SymbolDataDecimal64> symbolData;
        for (int i = 0; i < numSymbols; ++i) {
            SymbolDataDecimal64 d;
            Util::parseDecimal64(&d.d_low , "+inf");
            Util::parseDecimal64(&d.d_high, "-inf");
            d.d_valueTraded = BDEC::Decimal64(0);
            d.d_vwap = BDEC::Decimal64(0);
            d.d_volume = 0;
            symbolData.push_back(d);
        }

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (size_type i = 0; i < data.size(); ++i) {

            const bsl::string& symbol = data[i].d_symbol;
            const int index = symbol2Index[symbol];

            // Parse the price.
            BDEC::Decimal64 price =
                BloombergLP::bdldfp::DecimalUtil::makeDecimal64(
                    data[i].d_mantissa, data[i].d_exponent);

            // Update the aggregate data.
            if (price < symbolData[index].d_low) {
                symbolData[index].d_low = price;
            }
            else if (price > symbolData[index].d_high) {
                symbolData[index].d_high = price;
            }
            symbolData[index].d_valueTraded += price * data[i].d_quantity;
            symbolData[index].d_volume += data[i].d_quantity;
            symbolData[index].d_vwap =
                symbolData[index].d_valueTraded / symbolData[index].d_volume;
        }

        const double totalTime = s.accumulatedWallTime();

        const double dataPerSecond = numTickerData / totalTime;

        bsl::cout << "Performance test: " << dataPerSecond
                  << " ticker data operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -2: {
        // --------------------------------------------------------------------
        // TESTING: Performance Test of Random Trading Data.
        //
        // Test the performance of a real-world application of decimal
        // floating point values that is intense on calls to makeDecimal64 and
        // binary arithmetic operations such as 'operator*' and 'operator/'.
        // This test is performed with binary floating-point values (i.e.,
        // doubles) for comparison purposes.
        // --------------------------------------------------------------------
        bsl::vector<TradeDataPoint> data;
        typedef bsl::vector<TradeDataPoint>::size_type size_type;

        int numDiffTickers =     100;  // Number of different symbols.
        int numTickerData  = 1000000;  // Number of data points.

        for (int i = 0; i < numTickerData; ++i) {
            TradeDataPoint p;
            p.d_symbol = static_cast<char>(rand() % numDiffTickers);
            p.d_exponent = (rand() % 5) - 3;
            p.d_mantissa = rand() % 100000;
            p.d_quantity = 10 * (rand() % 1000);
            data.push_back(p);
        }

        // Find all distinct symbols.
        int numSymbols = 0;
        bsl::unordered_map<bsl::string, int> symbol2Index;
        for (size_type i = 0; i < data.size(); ++i) {
            const bsl::string& symbol = data[i].d_symbol;
            if (symbol2Index.find(symbol) == symbol2Index.end()) {
                symbol2Index[symbol] = numSymbols++;
            }
        }

        // Initialize data.
        bsl::vector<SymbolDataDouble> symbolData;
        for (int i = 0; i < numSymbols; ++i) {
            SymbolDataDouble d;
            d.d_low  = bsl::numeric_limits<double>::infinity();   //  Infinity
            d.d_high = -bsl::numeric_limits<double>::infinity();  // -Infinity
            d.d_valueTraded = 0.0;
            d.d_vwap = 0.0;
            d.d_volume = 0;
            symbolData.push_back(d);
        }

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (size_type i = 0; i < data.size(); ++i) {

            const bsl::string& symbol = data[i].d_symbol;
            const int index = symbol2Index[symbol];

            // Parse the price.
            double price = static_cast<double>(data[i].d_mantissa)
                         * pow(10.0, data[i].d_exponent);

            // Update the aggregate data.
            if (price < symbolData[index].d_low) {
                symbolData[index].d_low = price;
            }
            else if (price > symbolData[index].d_high) {
                symbolData[index].d_high = price;
            }
            symbolData[index].d_valueTraded += price
                                     * static_cast<double>(data[i].d_quantity);
            symbolData[index].d_volume += data[i].d_quantity;
            symbolData[index].d_vwap =
                (symbolData[index].d_valueTraded /
                 static_cast<double>(symbolData[index].d_volume));
        }

        const double totalTime = s.accumulatedWallTime();

        const double dataPerSecond = numTickerData / totalTime;

        bsl::cout << "Performance test: " << dataPerSecond
                  << " ticker data operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -3: {
        // --------------------------------------------------------------------
        // TESTING: Performance Test of Real-World Trading Data.
        //
        // Test the performance of a real-world application of decimal
        // floating point values that is intense on calls to makeDecimal64 and
        // binary arithmetic operations such as 'operator*' and 'operator/'.
        // We test an example of aggregating data about tickers, such as the
        // lows and highs in a trading day, volume, and weighted averages.  We
        // wish to determine the number of tickers that may be processed per
        // second, using data from an input file.
        // TBD: obtain volume information for the example file so we avoid
        // generating random numbers.  This will also give a more accurate
        // approximation for how we will be sampling the test space.
        // --------------------------------------------------------------------
        bsl::vector<TradeDataPoint> data;
        typedef bsl::vector<TradeDataPoint>::size_type size_type;

        const int numIterations = 1;

        ASSERT(argc >= 3);  // argv[2] is the filename.

        bsl::string line;
        bsl::ifstream myfile(argv[2]);
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                bsl::vector<bsl::string> seglist;
                split(seglist, line, '|');

                TradeDataPoint p;
                p.d_symbol = seglist[0];
                p.d_price = seglist[1];
                p.d_quantity = 10 * (rand() % 1000);

                parseDecimal(&p.d_mantissa, &p.d_exponent, p.d_price);

                data.push_back(p);
            }
        } else {
            cout << "Unable to open " << argv[2] << endl;
        }

        const int numData = numIterations * static_cast<int>(data.size());

        // Find all distinct symbols.
        int numSymbols = 0;
        bsl::unordered_map<bsl::string, int> symbol2Index;
        for (size_type i = 0; i < data.size(); ++i) {
            const bsl::string& symbol = data[i].d_symbol;
            if (symbol2Index.find(symbol) == symbol2Index.end()) {
                symbol2Index[symbol] = numSymbols++;
            }
        }

        // Initialize data.
        bsl::vector<SymbolDataDecimal64> symbolData;
        for (int i = 0; i < numSymbols; ++i) {
            SymbolDataDecimal64 d;
            Util::parseDecimal64(&d.d_low , "+inf");
            Util::parseDecimal64(&d.d_high, "-inf");
            d.d_valueTraded = BDEC::Decimal64(0);
            d.d_vwap = BDEC::Decimal64(0);
            d.d_volume = 0;
            symbolData.push_back(d);
        }

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iteration = 0; iteration < numIterations; ++iteration) {
            for (size_type i = 0; i < data.size(); ++i) {

                const bsl::string& symbol = data[i].d_symbol;
                const int index = symbol2Index[symbol];

                // Parse the price.
                BDEC::Decimal64 price =
                    BloombergLP::bdldfp::DecimalUtil::makeDecimal64(
                                       data[i].d_mantissa, data[i].d_exponent);

                // Update the aggregate data.
                if (price < symbolData[index].d_low) {
                    symbolData[index].d_low = price;
                }
                else if (price > symbolData[index].d_high) {
                    symbolData[index].d_high = price;
                }
                symbolData[index].d_valueTraded += price * data[i].d_quantity;
                symbolData[index].d_volume += data[i].d_quantity;
                symbolData[index].d_vwap = symbolData[index].d_valueTraded /
                                           symbolData[index].d_volume;
            }
        }

        const double totalTime = s.accumulatedWallTime();

        const double dataPerSecond = numData / totalTime;

        bsl::cout << "Performance test: " << dataPerSecond
                  << " ticker data operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -4: {
        // --------------------------------------------------------------------
        // TESTING: Performance Test of Real-World Trading Data.
        //
        // Test the performance of a real-world application of decimal
        // floating point values that is intense on calls to makeDecimal64 and
        // binary arithmetic operations such as 'operator*' and 'operator/'.
        // We test an example of aggregating data about tickers, such as the
        // lows and highs in a trading day, volume, and weighted averages.  We
        // wish to determine the number of tickers that may be processed per
        // second, using data from an input file.
        // TBD: obtain volume information for the example file so we avoid
        // generating random numbers.  This will also give a more accurate
        // approximation for how we will be sampling the test space.
        // --------------------------------------------------------------------
        bsl::vector<TradeDataPoint> data;
        typedef bsl::vector<TradeDataPoint>::size_type size_type;

        const int numIterations = 1;

        ASSERT(argc >= 3);  // argv[2] is the filename.

        bsl::string line;
        bsl::ifstream myfile(argv[2]);
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                bsl::vector<bsl::string> seglist;
                split(seglist, line, '|');

                TradeDataPoint p;
                p.d_symbol = seglist[0];
                p.d_price = seglist[1];
                p.d_quantity = 10 * (rand() % 1000);

                parseDecimal(&p.d_mantissa, &p.d_exponent, p.d_price);

                data.push_back(p);
            }
        } else {
            cout << "Unable to open " << argv[2] << endl;
        }

        const int numData = numIterations * static_cast<int>(data.size());

        // Find all distinct symbols.
        int numSymbols = 0;
        bsl::unordered_map<bsl::string, int> symbol2Index;
        for (size_type i = 0; i < data.size(); ++i) {
            const bsl::string& symbol = data[i].d_symbol;
            if (symbol2Index.find(symbol) == symbol2Index.end()) {
                symbol2Index[symbol] = numSymbols++;
            }
        }

        // Initialize data.
        bsl::vector<SymbolDataDouble> symbolData;
        for (int i = 0; i < numSymbols; ++i) {
            SymbolDataDouble d;
            d.d_low  = bsl::numeric_limits<double>::infinity();   //  Infinity
            d.d_high = -bsl::numeric_limits<double>::infinity();  // -Infinity
            d.d_valueTraded = 0.0;
            d.d_vwap = 0.0;
            d.d_volume = 0;
            symbolData.push_back(d);
        }

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iteration = 0; iteration < numIterations; ++iteration) {
            for (size_type i = 0; i < data.size(); ++i) {

                const bsl::string& symbol = data[i].d_symbol;
                const int index = symbol2Index[symbol];

                // Parse the price.
                double price = static_cast<double>(data[i].d_mantissa)
                             * pow(10.0, data[i].d_exponent);

                // Update the aggregate data.
                if (price < symbolData[index].d_low) {
                    symbolData[index].d_low = price;
                }
                else if (price > symbolData[index].d_high) {
                    symbolData[index].d_high = price;
                }
                symbolData[index].d_valueTraded += price
                                     * static_cast<double>(data[i].d_quantity);
                symbolData[index].d_volume += data[i].d_quantity;
                symbolData[index].d_vwap =
                    (symbolData[index].d_valueTraded /
                     static_cast<double>(symbolData[index].d_volume));
            }
        }

        const double totalTime = s.accumulatedWallTime();

        const double dataPerSecond = numData / totalTime;

        bsl::cout << "Performance test: " << dataPerSecond
                  << " ticker data operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -5: {
        // --------------------------------------------------------------------
        // TESTING: Performance test of 'makeDecimal64'.
        //
        // Test the performance of 'makeDecimal64' by doing a configurable
        // number of iterations of calls using a stopwatch to record the
        // elapsed time and compute the number of operations 'makeDecimal64'
        // performs per second.  An array of mantissas and exponents are used.
        // --------------------------------------------------------------------
        int numIterations = 10000;
        int numOperations = numIterations * numMantissas * numExps;

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iter = 0; iter < numIterations; ++iter) {
            for (int mi = 0; mi < numMantissas; ++mi) {
                for (int ei = 0; ei < numExps; ++ei) {
                    BDEC::Decimal64 num =
                                  Util::makeDecimal64(mantissas[mi], exps[ei]);
                    (void)num;
                }
            }
        }

        const double totalTime = s.accumulatedWallTime();
        const double operationsPerSecond = numOperations / totalTime;

        bsl::cout << "Performance test: " << operationsPerSecond
                  << " makeDecimal64 operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -6: {
        // --------------------------------------------------------------------
        // TESTING: Performance test of 'makeDecimalRaw128'.
        //
        // Test the performance of 'makeDecimalRaw128' by doing a configurable
        // number of iterations of calls using a stopwatch to record the
        // elapsed time and compute the number of operations
        // 'makeDecimalRaw128' performs per second.  An array of mantissas and
        // exponents are used.
        // --------------------------------------------------------------------
        int numIterations = 10000;
        int numOperations = numIterations * numMantissas * numExps;

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iter = 0; iter < numIterations; ++iter) {
            for (int mi = 0; mi < numMantissas; ++mi) {

                for (int ei = 0; ei < numExps; ++ei) {

                    if (exps[ei] < -6176 || exps[ei] > 6111) {
                        continue;
                    }

                    BDEC::Decimal128 num =
                              Util::makeDecimalRaw128(mantissas[mi], exps[ei]);
                    (void)num;
                }
            }
        }

        const double totalTime = s.accumulatedWallTime();
        const double operationsPerSecond = numOperations / totalTime;

        bsl::cout << "Performance test: " << operationsPerSecond
                  << " makeDecimal128 operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -7: {
        // --------------------------------------------------------------------
        // TESTING: Performance test of 'makeDecimalRaw64'.
        //
        // Test the performance of 'makeDecimalRaw64' by doing a configurable
        // number of iterations of calls using a stopwatch to record the
        // elapsed time and compute the number of operations
        // 'makeDecimalRaw64' performs per second.  An array of mantissas and
        // exponents are used.
        // --------------------------------------------------------------------
        int numIterations = 10000;
        int numOperations = numIterations * numMantissas * numExps;

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iter = 0; iter < numIterations; ++iter) {
            for (int mi = 0; mi < numMantissas; ++mi) {

                if (mantissas[mi] < -9999999999999999ll ||
                    mantissas[mi] >  9999999999999999ll) {
                    continue;
                }

                for (int ei = 0; ei < numExps; ++ei) {

                    if (exps[ei] < -398 || exps[ei] > 369) {
                        continue;
                    }

                    BDEC::Decimal64 num =
                               Util::makeDecimalRaw64(mantissas[mi], exps[ei]);
                    (void)num;
                }
            }
        }

        const double totalTime = s.accumulatedWallTime();
        const double operationsPerSecond = numOperations / totalTime;

        bsl::cout << "Performance test: " << operationsPerSecond
                  << " makeDecimal64 operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -8: {
        // --------------------------------------------------------------------
        // TESTING: Performance test of 'makeDecimalRaw32'.
        //
        // Test the performance of 'makeDecimalRaw32' by doing a configurable
        // number of iterations of calls using a stopwatch to record the
        // elapsed time and compute the number of operations
        // 'makeDecimalRaw32' performs per second.  An array of mantissas and
        // exponents are used.
        // --------------------------------------------------------------------
        int numIterations = 10000;
        int numOperations = numIterations * numMantissas * numExps;

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iter = 0; iter < numIterations; ++iter) {
            for (int mi = 0; mi < numMantissas; ++mi) {

                if (mantissas[mi] < -9999999 || mantissas[mi] > 9999999) {
                    continue;
                }

                for (int ei = 0; ei < numExps; ++ei) {

                    if (exps[ei] < -101 || exps[ei] > 90) {
                        continue;
                    }

                    BDEC::Decimal32 num =
                               Util::makeDecimalRaw32(
                                    static_cast<int>(mantissas[mi]), exps[ei]);
                    (void)num;
                }
            }
        }

        const double totalTime = s.accumulatedWallTime();
        const double operationsPerSecond = numOperations / totalTime;

        bsl::cout << "Performance test: " << operationsPerSecond
                  << " makeDecimal32 operations per second." << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;
    } break;
    case -9: {
        // --------------------------------------------------------------------
        // TESTING: Performance test of 'multiplyByPowerOf10'.
        //
        // Test the performance of 'makeDecimal64' by doing a configurable
        // number of iterations of calls using a stopwatch to record the
        // elapsed time and compute the number of operations 'makeDecimal64'
        // performs per second.  An array of mantissas and exponents are used.
        // --------------------------------------------------------------------

        // Precompute some 'Decimal64's.
        const int numDecimals = numMantissas * numExps;
        BDEC::Decimal64 decimals[numDecimals];

        for (int i = 0; i < numMantissas; ++i) {
            for (int j = 0; j < numExps; ++j) {
                decimals[i * numExps + j] =
                                    Util::makeDecimal64(mantissas[i], exps[j]);
            }
        }

        int numIterations = 1000;
        int numOperations = numIterations * numDecimals * numExps;

        // Accumulate the time elapsed in the test.
        bsls::Stopwatch s;
        s.start();

        for (int iter = 0; iter < numIterations; ++iter) {
            for (int di = 0; di < numDecimals; ++di) {
                for (int ei = 0; ei < numExps; ++ei) {
                    BDEC::Decimal64 result = Util::multiplyByPowerOf10(
                        decimals[di], exps[ei]);
                    (void)result;
                }
            }
        }

        const double totalTime = s.accumulatedWallTime();
        const double operationsPerSecond = numOperations / totalTime;

        bsl::cout << "Performance test: " << operationsPerSecond
                  << " multiplyByPowerOf10 operations per second."
                  << bsl::endl;
        bsl::cout << "Total time: " << totalTime << " seconds." << bsl::endl;

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
