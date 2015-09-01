// bdldfp_decimalutil.t.cpp                                           -*-C++-*-
#include <bdldfp_decimalutil.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalconvertutil.h>
#include <bdldfp_uint128.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

typedef bdldfp::DenselyPackedDecimalImpUtil DpdUtil;
typedef bdldfp::DecimalImpUtil              ImpUtil;

#if defined(BSLS_PLATFORM_OS_WINDOWS) && !defined(FP_NAN)

// MS does not provide standard floating-point classification so we do

// First, make sure the environment is sane

#if defined(FP_NORMAL) || defined(FP_INFINITE) || defined(FP_ZERO) || \
    defined(FP_SUBNORMAL)
#error Standard FP_ macros are not defined properly.
#endif

// Make it look like stiff MS has in ymath.h

#define FP_SUBNORMAL (-2)
#define FP_NORMAL    (-1)
#define FP_ZERO        0
#define FP_INFINITE    1
#define FP_NAN         2

#endif

//=============================================================================
//                          PERFORMANCE TEST HELPERS
//-----------------------------------------------------------------------------

// Accumulate data per symbol.
struct SymbolDataDecimal64 {
    BDEC::Decimal64    d_low;
    BDEC::Decimal64    d_high;
    BDEC::Decimal64    d_valueTraded;
    double             d_vwap;
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
const int numMantissas = sizeof(mantissas) / sizeof(*mantissas);

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
const int numExps = sizeof(exps) / sizeof(*exps);
namespace {

// Testing apparatus

BDEC::DecimalImpUtil::ValueType64 makeDecimalRaw64Zero(long long mantissa,
                                                       int       exponent)
    // Return a 64-bit decimal floating point value with the specified
    // 'mantissa' and 'exponent', including for cases in which
    // 'exponent == 0'.  The behavior is undefined unless
    // 'abs(mantissa) <= 9,999,999,999,999,999' and '-398 <= exponent <= 369'.
{
#if defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    // The intel library does not return a canonical 0 (i.e., a 0 exponent)
    // when creating 0 values.

    if (0 == mantissa) {
        DpdUtil::StorageType64 value = DpdUtil::makeDecimalRaw64(mantissa,
                                                                 exponent);
        return bdldfp::DecimalImpUtil::convertFromDPD(value);         // RETURN
    }

#endif
    return BDEC::DecimalImpUtil::makeDecimalRaw64(mantissa, exponent);

}


BDEC::DecimalImpUtil::ValueType128 makeDecimalRaw128Zero(long long mantissa,
                                                         int       exponent)
    // Return a 128-bit decimal floating point value with the specified
    // 'mantissa' and 'exponent', including for cases in which
    // 'exponent == 0'.  The behavior is undefined unless
    // '-6176 <= exponent <= 6111'.
{
#if defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    if (0 == mantissa) {
        DpdUtil::StorageType128 value =
            DpdUtil::makeDecimalRaw128(mantissa, exponent);
        return bdldfp::DecimalImpUtil::convertFromDPD(value);         // RETURN
    }
#endif
    return BDEC::DecimalImpUtil::makeDecimalRaw128(mantissa, exponent);
}


}  // close unnamed namespace

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

template <class TYPE>
struct NumberMaker;

template <>
struct NumberMaker<BDEC::Decimal64>
{
    BDEC::Decimal64 operator()(long long mantissa, int exponent) const
    {
        return makeDecimalRaw64Zero(mantissa, exponent);
    }
};

template <>
struct NumberMaker<BDEC::Decimal128>
{
    BDEC::Decimal128 operator()(long long mantissa, int exponent) const
    {
        return makeDecimalRaw128Zero(mantissa, exponent);
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

    using bsls::AssertFailureHandlerGuard;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    bslma::TestAllocator *pa = &ta;

    typedef BDEC::DecimalUtil Util;

    cout.precision(35);


    switch (test) { case 0:
    case 11: {
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
            for (int mi = 0; mi < numMantissas; ++mi) {
                for (int ei = 0; ei < numExps; ++ei) {
                    for (int ei2 = 0; ei2 < numExps; ++ei2) {

                        const long long MANTISSA = mantissas[mi];
                        const int EXP            = exps[ei];
                        const int POW            = exps[ei2];

                        if (veryVerbose) bsl::cout << "multiplyByPowerOf10 "
                            "tests on 'int' powers..." << bsl::endl;
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

                        if (veryVerbose) bsl::cout << "multiplyByPowerOf10 "
                            "tests on 'Decimal64' powers..." << bsl::endl;
                        {
                            const BDEC::Decimal64 VALUE = Util::makeDecimal64(
                                MANTISSA, EXP);
                            const BDEC::Decimal64 ACTUAL_RESULT =
                                Util::multiplyByPowerOf10(VALUE,
                                                         BDEC::Decimal64(POW));
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
    }
    case 10: {
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
    case 9: {
        // --------------------------------------------------------------------
        // TESTING quantize
        // Concerns: Forwarding to the right routines
        // Plan: Try with several variations and combinations of
        //       decimal floats (different mantissas and exponents, both
        //       positive and negative.)
        // Testing: quantize
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "quantize Decimal64 tests..." << bsl::endl;
        {
            typedef BDEC::Decimal64 TYPE;
            typedef bsl::numeric_limits<TYPE> NumLim;
            NumberMaker<TYPE> makeNumber;

            // Test for quantize, which depends upon the strict, narrow
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
            // Concern: quantize with NaN in either parameter must return a
            // NaN.
            // Concern: quantize of NaN and Inf must set the invalid exception
            // bit in the flags.  The Invalid bit must not be set for NaN/NaN
            // cases.  This concern isn't presently tested.
            ASSERT(Util::isNan(Util::quantize(sNaN, sNaN)));
            ASSERT(Util::isNan(Util::quantize(sNaN, qNaN)));
            ASSERT(Util::isNan(Util::quantize(sNaN, pInf)));
            ASSERT(Util::isNan(Util::quantize(sNaN, nInf)));

            // o qNaN
            // Concern: quantize with NaN in either parameter must return a
            // NaN.
            // Concern: quantize of NaN and Inf must set the invalid exception
            // bit in the flags.  The Invalid bit must not be set for NaN/NaN
            // cases.  This concern isn't presently tested.
            ASSERT(Util::isNan(Util::quantize(qNaN, sNaN)));
            ASSERT(Util::isNan(Util::quantize(qNaN, qNaN)));
            ASSERT(Util::isNan(Util::quantize(qNaN, pInf)));
            ASSERT(Util::isNan(Util::quantize(qNaN, nInf)));

            // o +Inf
            // Concern: quantize with NaN in either parameter must return a
            // NaN.
            // Concern: quantize of NaN and Inf must set the invalid exception
            // bit in the flags.  The Invalid bit must not be set for NaN/NaN
            // cases.  This concern isn't presently tested.
            // Concern: Infinity by infinity must return an infinity with
            // the sign of the first argument
            ASSERT(Util::isNan(Util::quantize(pInf, sNaN)));
            ASSERT(Util::isNan(Util::quantize(pInf, qNaN)));
            ASSERT(pInf ==     Util::quantize(pInf, pInf));
            ASSERT(pInf ==     Util::quantize(pInf, nInf));

            // o -Inf
            // Concern: quantize with NaN in either parameter must return a
            // NaN.
            // Concern: quantize of NaN and Inf must set the invalid exception
            // bit in the flags.  The Invalid bit must not be set for NaN/NaN
            // cases.  This concern isn't presently tested.
            // Concern: Infinity by infinity must return an infinity with
            // the sign of the first argument
            ASSERT(Util::isNan(Util::quantize(nInf, sNaN)));
            ASSERT(Util::isNan(Util::quantize(nInf, qNaN)));
            ASSERT(nInf ==     Util::quantize(nInf, pInf));
            ASSERT(nInf ==     Util::quantize(nInf, nInf));


            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
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
                    //
                    // Concern: quantize with NaN in either parameter must
                    // return a NaN.
                    //
                    // Concern: quantize of NaN and Inf must set the invalid
                    // exception bit in the flags.  The Invalid bit must not be
                    // set for NaN/NaN cases.  This concern isn't presently
                    // tested.
                    //
                    // Concern: Infinity by infinity must return an infinity
                    // with the sign of the first argument

                    ASSERT(Util::isNan(Util::quantize(value, sNaN)));
                    ASSERT(Util::isNan(Util::quantize(value, qNaN)));
                    ASSERT(Util::isNan(Util::quantize(value, pInf)));
                    ASSERT(Util::isNan(Util::quantize(value, nInf)));

                    ASSERT(Util::isNan(Util::quantize(value, sNaN)));
                    ASSERT(Util::isNan(Util::quantize(value, qNaN)));
                    ASSERT(Util::isNan(Util::quantize(value, pInf)));
                    ASSERT(Util::isNan(Util::quantize(value, nInf)));
                }
            }

            // Iterate through all possible pairings of mantissa and
            // exponent, and build Decimal64 values for each of them.
            // These will be compared with other values created from
            // the same table, in the same way.  Quantize will
            // produce things which have the same quantum.
            for (long long tiM = 0; tiM < numMantissas; ++tiM) {
                for (  int tiE = 0; tiE < numExps;      ++tiE) {
                    const TYPE lhs =
                        makeNumber(mantissas[tiM], exps[tiE]);

                    for (long long tjM = 0; tjM < numMantissas; ++tjM) {
                        for (  int tjE = 0; tjE < numExps;      ++tjE) {
                            const TYPE rhs =
                                makeNumber(mantissas[tjM], exps[tjE]);

                            (void) rhs;
                            (void) lhs;

                            #if 0
                            LOOP4_ASSERT(mantissas[tiM], exps[tiE],
                                         mantissas[tjM], exps[tjE],
                                         Util::sameQuantum(
                                             Util::quantize(lhs, rhs),
                                             rhs));
                            #endif
                        }
                    }
                }
            }
        }

        // TODO: Make the Decimal128 variant.
    } break;
    case 8: {
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
                AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);
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
                AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

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
    case 7: {
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
    case 6: {
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
    case 5: {
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
    case 4: {
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
    case 3: {
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
    case 2: {
        // --------------------------------------------------------------------
        // TESTING Test apparatus
        // Concerns: Branching and forwarding to the right routines
        // Plan: Try with zero and non-zero mantissas, comparing against
        // preset bit-fields.
        // Testing: makeDecimalRaw64Zero and makeDecimalRaw128Zero
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTest apparatus"
                                << "\n==============" << bsl::endl;

        if (veryVerbose) bsl::cout << "makeDecimalRaw64Zero functions"
                                   << bsl::endl;

        {
            {
                BDEC::Decimal64 ACTUAL   = makeDecimalRaw64Zero  (42, 5);
                BDEC::Decimal64 EXPECTED = Util::makeDecimalRaw64(42, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }
            {
                BDEC::Decimal64 ACTUAL   = makeDecimalRaw64Zero  (42u, 5);
                BDEC::Decimal64 EXPECTED = Util::makeDecimalRaw64(42u, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }
            {
                BDEC::Decimal64 ACTUAL   = makeDecimalRaw64Zero  (42ll, 5);
                BDEC::Decimal64 EXPECTED = Util::makeDecimalRaw64(42ll, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }
            {
                BDEC::Decimal64 ACTUAL   = makeDecimalRaw64Zero  (42ull, 5);
                BDEC::Decimal64 EXPECTED = Util::makeDecimalRaw64(42ull, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }


            // Test against hard coded DPD bit values.

            {
                DpdUtil::StorageType64 ACTUAL = DpdUtil::makeDecimalRaw64(0,0);
                unsigned long long EXPECTED = 0x2238000000000000ull;
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));

            }
            {
                BDEC::Decimal64 value = makeDecimalRaw64Zero(0, 0);

                DpdUtil::StorageType64 ACTUAL =
                                ImpUtil::convertToDPD(*value.data());
                unsigned long long EXPECTED = 0x2238000000000000ull;
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }

            {
                BDEC::Decimal64 value = makeDecimalRaw64Zero(0, 5);

                DpdUtil::StorageType64 ACTUAL =
                                ImpUtil::convertToDPD(*value.data());
                unsigned long long EXPECTED = 0x224C000000000000ull;
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }

            {
                BDEC::Decimal64 value = makeDecimalRaw64Zero(0, -5);

                DpdUtil::StorageType64 ACTUAL =
                                ImpUtil::convertToDPD(*value.data());
                unsigned long long EXPECTED = 0x2224000000000000ull;
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 8));
            }

        }
        if (veryVerbose) bsl::cout << "makeDecimalRaw128Zero functions"
                                   << bsl::endl;

        {

            {
                BDEC::Decimal128 ACTUAL   = makeDecimalRaw128Zero  (42, 5);
                BDEC::Decimal128 EXPECTED = Util::makeDecimalRaw128(42, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }
            {
                BDEC::Decimal128 ACTUAL   = makeDecimalRaw128Zero  (42u, 5);
                BDEC::Decimal128 EXPECTED = Util::makeDecimalRaw128(42u, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }
            {
                BDEC::Decimal128 ACTUAL   = makeDecimalRaw128Zero  (42ll, 5);
                BDEC::Decimal128 EXPECTED = Util::makeDecimalRaw128(42ll, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }
            {
                BDEC::Decimal128 ACTUAL   = makeDecimalRaw128Zero  (42ull, 5);
                BDEC::Decimal128 EXPECTED = Util::makeDecimalRaw128(42ull, 5);
                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }

            // Test against hard coded DPD bit values.

            {
                BDEC::Decimal128 value = makeDecimalRaw128Zero(0, 0);

                DpdUtil::StorageType128 ACTUAL =
                                ImpUtil::convertToDPD(*value.data());
                BloombergLP::bdldfp::Uint128 EXPECTED(
                                 0x2208000000000000ull, 0x0000000000000000ull);

                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }


            {
                BDEC::Decimal128 value = makeDecimalRaw128Zero(0, 5);

                DpdUtil::StorageType128 ACTUAL =
                                ImpUtil::convertToDPD(*value.data());
                BloombergLP::bdldfp::Uint128 EXPECTED(
                                 0x2209400000000000ull, 0x0000000000000000ull);

                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }

            {
                BDEC::Decimal128 value = makeDecimalRaw128Zero(0, -5);

                DpdUtil::StorageType128 ACTUAL =
                                ImpUtil::convertToDPD(*value.data());
                BloombergLP::bdldfp::Uint128 EXPECTED(
                                 0x2206C00000000000ull, 0x0000000000000000ull);

                ASSERT(!bsl::memcmp(&ACTUAL, &EXPECTED, 16));
            }

        }
    }
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
               BDLDFP_DECIMAL_DD(-1.234567890123456e-382));
        ASSERT(Util::makeDecimal64(1234567890123456ull, -382-16+1) ==
               BDLDFP_DECIMAL_DD(1.234567890123456e-382));

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

        ASSERT(Util::fabs(BDLDFP_DECIMAL_DD(-1.234567890123456e-382))
               == BDLDFP_DECIMAL_DD(1.234567890123456e-382));
        ASSERT(Util::fabs(BDLDFP_DECIMAL_DL(-1.234567890123456e-382))
               == BDLDFP_DECIMAL_DL(1.234567890123456e-382));

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
                symbol2Index[symbol] = ++numSymbols;
            }
        }

        // Initialize data.
        bsl::vector<SymbolDataDecimal64> symbolData;
        for (int i = 0; i < numSymbols; ++i) {
            SymbolDataDecimal64 d;
            Util::parseDecimal64(&d.d_low , "+inf");
            Util::parseDecimal64(&d.d_high, "-inf");
            d.d_valueTraded = BDEC::Decimal64(0);
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
                BloombergLP::bdldfp::DecimalConvertUtil::decimalToDouble
                (symbolData[index].d_valueTraded /
                 symbolData[index].d_volume);
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
                symbol2Index[symbol] = ++numSymbols;
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
                symbol2Index[symbol] = ++numSymbols;
            }
        }

        // Initialize data.
        bsl::vector<SymbolDataDecimal64> symbolData;
        for (int i = 0; i < numSymbols; ++i) {
            SymbolDataDecimal64 d;
            Util::parseDecimal64(&d.d_low , "+inf");
            Util::parseDecimal64(&d.d_high, "-inf");
            d.d_valueTraded = BDEC::Decimal64(0);
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
                    BloombergLP::bdldfp::DecimalConvertUtil::decimalToDouble
                    (symbolData[index].d_valueTraded /
                     symbolData[index].d_volume);
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
                symbol2Index[symbol] = ++numSymbols;
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
