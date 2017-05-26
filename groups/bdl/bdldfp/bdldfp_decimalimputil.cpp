// bdldfp_decimalimputil.cpp                                          -*-C++-*-
#include <bdldfp_decimalimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalimputil_cpp,"$Id$ $CSID$")

#include <bdldfp_uint128.h>

#include <bsl_cstring.h>

#include <bsls_performancehint.h>

namespace BloombergLP {
namespace bdldfp {

namespace {

struct Properties64
    // Properties64, contains constants and member functions identifying key
    // properties of the 64-bit decimal type.
{
    static const int         digits          = 16;
    static const int         bias            = 398;
    static const int         maxExponent     = 369;
    static const long long   smallLimit      = 1000000000000000ll;
    static const long long   mediumLimit     = 10000000000000000ll;

};

                        // classification functions

#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
static int canonicalizeDecimalValueClassification(int classification)
    // Return a standard mandated constant indicating the kind of floating
    // point value specified by 'classification'.  The behavior is undefined
    // unless 'classification' is a valid classification code for the
    // underlying implementation.  Note that 'classification' is of an
    // implementation defined type, and corresponds to specific underlying
    // library constants.
{
    enum decClass cl = static_cast<decClass>(classification);
    switch (cl) {
    case DEC_CLASS_SNAN:
    case DEC_CLASS_QNAN:          return FP_NAN;                      // RETURN
    case DEC_CLASS_NEG_INF:
    case DEC_CLASS_POS_INF:       return FP_INFINITE;                 // RETURN
    case DEC_CLASS_NEG_ZERO:
    case DEC_CLASS_POS_ZERO:      return FP_ZERO;                     // RETURN
    case DEC_CLASS_NEG_NORMAL:
    case DEC_CLASS_POS_NORMAL:    return FP_NORMAL;                   // RETURN
    case DEC_CLASS_NEG_SUBNORMAL:
    case DEC_CLASS_POS_SUBNORMAL: return FP_SUBNORMAL;                // RETURN
    }
    BSLS_ASSERT(!"Unknown decClass");
    return -1;
}
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
static int canonicalizeDecimalValueClassification(int classification)
    // Return a standard mandated constant indicating the kind of floating
    // point value specified by 'classification'.  The behavior is undefined
    // unless 'classification' is a valid classification code for the
    // underlying implementation.  Note that 'classification' is of an
    // implementation defined type, and corresponds to specific underlying
    // library constants.
{
    enum class_types cl = static_cast<class_types>(classification);
    switch (cl) {
    case signalingNaN:
    case quietNaN:          return FP_NAN;                      // RETURN
    case negativeInfinity:
    case positiveInfinity:  return FP_INFINITE;                 // RETURN
    case negativeZero:
    case positiveZero:      return FP_ZERO;                     // RETURN
    case negativeNormal:
    case positiveNormal:    return FP_NORMAL;                   // RETURN
    case negativeSubnormal:
    case positiveSubnormal: return FP_SUBNORMAL;                // RETURN
    }
    BSLS_ASSERT(!"Unknown decClass");
    return -1;
}
#endif

}  // close unnamed namespace

                        // --------------------
                        // class DecimalImpUtil
                        // --------------------

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(
                                                   unsigned long long mantissa,
                                                   int                exponent)
{
    if ((-Properties64::bias <= exponent) &&
        (exponent <= Properties64::maxExponent) &&
        (mantissa <
               static_cast<unsigned long long>(Properties64::mediumLimit))) {

        // 'mantissa' and 'exponent' are in range of 64-bit decimal floating
        // point.

        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    if (exponent >= Properties64::maxExponent + Properties64::digits) {

        // 'exponent' too high.

        if (mantissa != 0) {
            return makeInfinity64();                                  // RETURN
        }

        // Make a '0' with the highest exponent possible.

        return makeDecimalRaw64(0, Properties64::maxExponent);        // RETURN
    }

    // Note that static_cast<int> is needed to prevent the RHS of the '<='
    // comparison from promoting to a signed int.  '3 * sizeof(long long)' is
    // at least the number of digits in the longest representable 'long long'.

    if (exponent <= -Properties64::bias -
                                     3 * static_cast<int>(sizeof(long long))) {

        // 'exponent' too low.

        return makeDecimalRaw64(0, -Properties64::bias);              // RETURN
    }

            // Precision too high.

    return convertToDecimal64(makeDecimalRaw128(mantissa, exponent)); // RETURN
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(long long mantissa,
                                                          int       exponent)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
            (-Properties64::bias <= exponent) &&
            (exponent <= Properties64::maxExponent) &&
            (-Properties64::mediumLimit < mantissa) &&
            (mantissa < Properties64::mediumLimit))) {

        // 'mantissa' and 'exponent' are in range of 64-bit decimal floating
        // point.

        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }

    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

    if (exponent >= Properties64::maxExponent + Properties64::digits) {

        // 'exponent' too high.

        if (mantissa != 0) {
            return makeInfinity64(mantissa < 0);                      // RETURN
        }

        // Make a '0' with the highest exponent possible.

        return makeDecimalRaw64(0, Properties64::maxExponent);        // RETURN
    }
    if (exponent <= -Properties64::bias -
                                     3 * static_cast<int>(sizeof(long long))) {

        // 'exponent' too low.

        if (mantissa >= 0) {
            return makeDecimalRaw64(0, -Properties64::bias);          // RETURN
        }

        // Create and return the decimal floating point value '-0'.

        return parse64("-0");                                         // RETURN
    }

    // Precision too high.

    return convertToDecimal64(makeDecimalRaw128(mantissa, exponent)); // RETURN
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(
                                                         unsigned int mantissa,
                                                         int          exponent)
{
    if ((-Properties64::bias <= exponent) &&
        (                       exponent <= Properties64::maxExponent)) {
        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    if (exponent >= Properties64::maxExponent + Properties64::digits) {
        if (mantissa != 0) {
            return makeInfinity64();                                  // RETURN
        }
        return makeDecimalRaw64(0, Properties64::maxExponent);        // RETURN
    }
    if (exponent <= -Properties64::bias - Properties64::digits) {
        return makeDecimalRaw64(0, -Properties64::bias);              // RETURN
    }
    return convertToDecimal64(makeDecimalRaw128(mantissa, exponent)); // RETURN
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(int mantissa,
                                                          int exponent)
{
    if ((-Properties64::bias <= exponent) &&
        (exponent <= Properties64::maxExponent)) {
        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    if (exponent >= Properties64::maxExponent + Properties64::digits) {
        if (mantissa != 0) {
            return makeInfinity64(mantissa < 0);                      // RETURN
        }
        return makeDecimalRaw64(0, Properties64::maxExponent);        // RETURN
    }
    if (exponent <= -Properties64::bias - Properties64::digits) {
        return makeDecimalRaw64(0, -Properties64::bias);              // RETURN
    }
    return convertToDecimal64(makeDecimalRaw128(mantissa, exponent)); // RETURN
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeInfinity64(bool isNegative)
{
    return DecimalImpUtil::parse64( isNegative ? "-inf" : "inf" );
}

int DecimalImpUtil::classify(ValueType32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x);
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decSingleClass(x);
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(__bid32_class(x.d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalImpUtil::classify(ValueType64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x);
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decDoubleClass(x);
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(__bid64_class(x.d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalImpUtil::classify(ValueType128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x);
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decQuadClass(x);
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(__bid128_class(x.d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalImpUtil::decompose(int          *sign,
                              unsigned int *significand,
                              int          *exponent,
                              ValueType32   value)
{
    BSLS_ASSERT(sign);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(exponent);

    enum {
        k_SIGN_MASK             = 0x80000000ul,
        k_SPECIAL_ENCODING_MASK = 0x60000000ul,
        k_INFINITY_MASK         = 0x78000000ul,
        k_SMALL_COEFF_MASK      = 0x001ffffful,
        k_LARGE_COEFF_MASK      = 0x007ffffful,
        k_LARGE_COEFF_HIGH_BIT  = 0x00800000ul,
        k_EXPONENT_MASK         = 0xff,
        k_EXPONENT_SHIFT_LARGE  = 21,
        k_EXPONENT_SHIFT_SMALL  = 23,
        k_DECIMAL_EXPONENT_BIAS = 101,
    };

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    unsigned int x = value.d_raw;
#else
    unsigned int x = *value.words;
#endif

    unsigned int tmp;
    int cl = classify(value);

    *sign = (x & k_SIGN_MASK) ? -1 : 1;

    if ((x & k_SPECIAL_ENCODING_MASK) == k_SPECIAL_ENCODING_MASK) {
        // special encodings
        if ((x & k_INFINITY_MASK) == k_INFINITY_MASK) {
            // NaN or Infinity
            *significand = (x & k_SMALL_COEFF_MASK) | k_LARGE_COEFF_HIGH_BIT;
            tmp = x >> k_EXPONENT_SHIFT_LARGE;
            *exponent = tmp & k_EXPONENT_MASK;
            return cl;
        }
        // get significand
        *significand = (x & k_SMALL_COEFF_MASK) | k_LARGE_COEFF_HIGH_BIT;

        // get exponent
        tmp = x >> k_EXPONENT_SHIFT_LARGE;
        *exponent = (tmp & k_EXPONENT_MASK) - k_DECIMAL_EXPONENT_BIAS;
        return cl;
    }
    // get exponent
    tmp = x >> k_EXPONENT_SHIFT_SMALL;
    *exponent = (tmp & k_EXPONENT_MASK) - k_DECIMAL_EXPONENT_BIAS;
    // get coefficient
    *significand = (x & k_LARGE_COEFF_MASK);

    return cl;
}

int DecimalImpUtil::decompose(int                 *sign,
                              bsls::Types::Uint64 *significand,
                              int                 *exponent,
                              ValueType64          value)
{
    BSLS_ASSERT(sign);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(exponent);

    const bsls::Types::Uint64 k_SIGN_MASK             = 0x8000000000000000ull;
    const bsls::Types::Uint64 k_SPECIAL_ENCODING_MASK = 0x6000000000000000ull;
    const bsls::Types::Uint64 k_INFINITY_MASK         = 0x7800000000000000ull;
    const bsls::Types::Uint64 k_SMALL_COEFF_MASK      = 0x0007ffffffffffffull;
    const bsls::Types::Uint64 k_LARGE_COEFF_MASK      = 0x001fffffffffffffull;
    const bsls::Types::Uint64 k_LARGE_COEFF_HIGH_BIT  = 0x0020000000000000ull;
    const bsls::Types::Uint64 k_EXPONENT_MASK         = 0x3ff;
    const int k_EXPONENT_SHIFT_LARGE                  = 51;
    const int k_EXPONENT_SHIFT_SMALL                  = 53;
    const int k_DECIMAL_EXPONENT_BIAS                 = 398;

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bsls::Types::Uint64 x = value.d_raw;
#else
    bsls::Types::Uint64 x = *value.longs;
#endif

    bsls::Types::Uint64 tmp;
    int cl = classify(value);

    *sign = (x & k_SIGN_MASK) ? -1 : 1;

    if ((x & k_SPECIAL_ENCODING_MASK) == k_SPECIAL_ENCODING_MASK) {
        // special encodings
        if ((x & k_INFINITY_MASK) == k_INFINITY_MASK) {
            // NaN or Infinity
            *significand = (x & k_SMALL_COEFF_MASK) | k_LARGE_COEFF_HIGH_BIT;
            tmp = x >> k_EXPONENT_SHIFT_LARGE;
            *exponent = static_cast<int>(tmp & k_EXPONENT_MASK);
            return cl;
        }
        *significand = (x & k_SMALL_COEFF_MASK) | k_LARGE_COEFF_HIGH_BIT;
        // get exponent
        tmp = x >> k_EXPONENT_SHIFT_LARGE;
        *exponent = static_cast<int>(tmp & k_EXPONENT_MASK)
                    - k_DECIMAL_EXPONENT_BIAS;
        return cl;
    }
    // exponent
    tmp = x >> k_EXPONENT_SHIFT_SMALL;
    *exponent = static_cast<int>(tmp & k_EXPONENT_MASK)
                - k_DECIMAL_EXPONENT_BIAS;
    // coefficient
    *significand = x & k_LARGE_COEFF_MASK;

    return cl;
}

int DecimalImpUtil::decompose(int          *sign,
                              Uint128      *significand,
                              int          *exponent,
                              ValueType128  value)
{
    const bsls::Types::Uint64 k_SIGN_MASK             = 0x8000000000000000ull;
    const bsls::Types::Uint64 k_SPECIAL_ENCODING_MASK = 0x6000000000000000ull;
    const bsls::Types::Uint64 k_SMALL_COEFF_MASK      = 0x00007fffffffffffull;
    const bsls::Types::Uint64 k_LARGE_COEFF_MASK      = 0x0001ffffffffffffull;
    const bsls::Types::Uint64 k_LARGE_COEFF_HIGH_BIT  = 0x0020000000000000ull;
    const bsls::Types::Uint64 k_EXPONENT_MASK         = 0x3fff;
    const int k_EXPONENT_SHIFT_LARGE                  = 47;
    const int k_EXPONENT_SHIFT_SMALL                  = 49;
    const int k_DECIMAL_EXPONENT_BIAS                 = 6176;

    BSLS_ASSERT(sign);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsls::Types::Uint64 xH = value.d_raw.w[0];
    bsls::Types::Uint64 xL = value.d_raw.w[1];
    #elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    bsls::Types::Uint64 xL = value.d_raw.w[0];
    bsls::Types::Uint64 xH = value.d_raw.w[1];
    #endif
#else
    #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsls::Types::Uint64 xH = *value.longs[0];
    bsls::Types::Uint64 xL = *value.longs[1];
    #elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    bsls::Types::Uint64 xL = *value.longs[0];
    bsls::Types::Uint64 xH = *value.longs[1];
    #endif
#endif

    bsls::Types::Uint64 tmp;
    int cl = classify(value);

   *sign = (xH & k_SIGN_MASK) ? -1 : 1;

    if ((xH & k_SPECIAL_ENCODING_MASK) == k_SPECIAL_ENCODING_MASK) {
        // special encodings
        significand->setHigh((xH & k_SMALL_COEFF_MASK)
                             | k_LARGE_COEFF_HIGH_BIT);
        significand->setLow(xL);
        tmp = xH >> k_EXPONENT_SHIFT_LARGE;
        *exponent = static_cast<int>(tmp & k_EXPONENT_MASK);
        return cl; // NaN or Infinity
    }

    tmp = xH >> k_EXPONENT_SHIFT_SMALL;
    *exponent = static_cast<int>(tmp & k_EXPONENT_MASK)
                - k_DECIMAL_EXPONENT_BIAS;
    // coefficient
    significand->setHigh(xH & k_LARGE_COEFF_MASK);
    significand->setLow(xL);

    return cl;
}

}  // close package namespace
}  // close enterprise namespace

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
