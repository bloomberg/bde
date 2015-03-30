// bdldfp_decimalutil.cpp                                             -*-C++-*-
#include <bdldfp_decimalutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalplatform.h>
#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bslmf_assert.h>

#include <bsl_cmath.h>
#include <errno.h>
#include <math.h>  // For the  FP_* macros

#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
extern "C" {
#include <decSingle.h>
}
#endif

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     BSLMF_ASSERT(false);
#  endif
#endif

#define BDLDFP_DISABLE_COMPILE BSLMF_ASSERT(false)

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
#include <bdldfp_intelimpwrapper.h>
#endif


#include <errno.h>


namespace BloombergLP {
namespace bdldfp {

                             // Creator functions


int DecimalUtil::parseDecimal32(Decimal32 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    *out = DecimalImpUtil::parse32(str);
    return 0;
}

int DecimalUtil::parseDecimal64(Decimal64 *out, const char *str)
{

    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    *out = DecimalImpUtil::parse64(str);
    return 0;
}

int DecimalUtil::parseDecimal128(Decimal128 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    *out = DecimalImpUtil::parse128(str);
    return 0;
}


int DecimalUtil::parseDecimal32(Decimal32 *out, const bsl::string& str)
{
    BSLS_ASSERT(out != 0);

    return parseDecimal32(out, str.c_str());
}
int DecimalUtil::parseDecimal64(Decimal64 *out, const bsl::string& str)
{
    BSLS_ASSERT(out != 0);

    return parseDecimal64(out, str.c_str());
}
int DecimalUtil::parseDecimal128(Decimal128 *out, const bsl::string& str)
{
    BSLS_ASSERT(out != 0);

    return parseDecimal128(out, str.c_str());
}

                                // Math functions

Decimal64 DecimalUtil::fma(Decimal64 x, Decimal64 y, Decimal64 z)
{
    Decimal64 rv;
#if defined(BDLDFP_DECIMALPLATFORM_C99_TR) \
 && defined(BDLDFP_DECIMALPLATFORM_C99_NO_FMAD64)
    // TODO TBD Is this OK?  Why don't we have fmad64 on IBM???
    // TODO: I believe that it is not okay -- fma exists not just for
    // performance, but for accuracy, by keeping "ideal" precision, until
    // the operation completes.  -- ADAM
    *rv.data() = (x.value() * y.value()) + z.value();
#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR)
    *rv.data() = fmad64(x.value(), y.value(), z.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decDoubleFMA(rv.data(),
                 x.data(),
                 y.data(),
                 z.data(),
                 DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    rv.data()->d_raw = __bid64_fma(x.data()->d_raw,
                                   y.data()->d_raw,
                                   z.data()->d_raw,
                                   &flags);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}

Decimal128 DecimalUtil::fma(Decimal128 x, Decimal128 y, Decimal128 z)
{
    Decimal128 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data()= fmad128(x.value(), y.value(), z.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decQuadFMA(rv.data(),
               x.data(),
               y.data(),
               z.data(),
               DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    rv.data()->d_raw = __bid128_fma(x.data()->d_raw,
                                    y.data()->d_raw,
                                    z.data()->d_raw,
                                    &flags);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
                       // Selecting, converting functions

Decimal32 DecimalUtil::fabs(Decimal32 value)
{
    Decimal32 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data() = fabsd32(value.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER) \
   || defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    // TODO TBD Just flip the sign bit, but beware of endianness
    rv = Decimal32(DecimalUtil::fabs(Decimal64(value)));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
Decimal64 DecimalUtil::fabs(Decimal64 value)
{
    Decimal64 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data() = fabsd64(value.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decDoubleAbs(rv.data(),
                 value.data(),
                 DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    rv.data()->d_raw = __bid64_abs(value.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
Decimal128 DecimalUtil::fabs(Decimal128 value)
{
    Decimal128 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data() = fabsd128(value.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decQuadAbs(rv.data(),
               value.data(),
               DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    rv.data()->d_raw = __bid128_abs(value.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
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

int DecimalUtil::classify(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decSingleClass(x.data());
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(
                                               __bid32_class(x.data()->d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}
int DecimalUtil::classify(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decDoubleClass(x.data());
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(
                                               __bid64_class(x.data()->d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}
int DecimalUtil::classify(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decQuadClass(x.data());
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(
                                              __bid128_class(x.data()->d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

bool DecimalUtil::isFinite(Decimal32 x)
{
    int cl = classify(x);
    return cl != FP_INFINITE && cl != FP_NAN;
}
bool DecimalUtil::isFinite(Decimal64 x)
{
    int cl = classify(x);
    return cl != FP_INFINITE && cl != FP_NAN;
}
bool DecimalUtil::isFinite(Decimal128 x)
{
    int cl = classify(x);
    return cl != FP_INFINITE && cl != FP_NAN;
}


bool DecimalUtil::isInf(Decimal32 x)
{
    return classify(x) == FP_INFINITE;
}
bool DecimalUtil::isInf(Decimal64 x)
{
    return classify(x) == FP_INFINITE;
}
bool DecimalUtil::isInf(Decimal128 x)
{
    return classify(x) == FP_INFINITE;
}

bool DecimalUtil::isNan(Decimal32 x)
{
    return classify(x) == FP_NAN;
}
bool DecimalUtil::isNan(Decimal64 x)
{
    return classify(x) == FP_NAN;
}
bool DecimalUtil::isNan(Decimal128 x)
{
    return classify(x) == FP_NAN;
}

bool DecimalUtil::isNormal(Decimal32 x)
{
    return classify(x) == FP_NORMAL;
}
bool DecimalUtil::isNormal(Decimal64 x)
{
    return classify(x) == FP_NORMAL;
}
bool DecimalUtil::isNormal(Decimal128 x)
{
    return classify(x) == FP_NORMAL;
}

                           // Comparison functions

bool DecimalUtil::isUnordered(Decimal32 x, Decimal32 y)
{
    return isNan(x) || isNan(y);
}
bool DecimalUtil::isUnordered(Decimal64 x, Decimal64 y)
{
    return isNan(x) || isNan(y);
}
bool DecimalUtil::isUnordered(Decimal128 x, Decimal128 y)
{
    return isNan(x) || isNan(y);
}
                             // Rounding functions

Decimal32 DecimalUtil::ceil(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return ceild32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_CEILING);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_positive(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::ceil(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return ceild64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_CEILING);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_positive(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::ceil(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return ceild128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_CEILING);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_positive(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal32 DecimalUtil::floor(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return floord32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_FLOOR);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_negative(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::floor(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return floord64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_FLOOR);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_negative(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::floor(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return floord128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_FLOOR);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_negative(x.data()->d_raw,
                                                       &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal32 DecimalUtil::round(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return roundd32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_HALF_UP);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_nearest_away(x.data()->d_raw,
                                                          &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::round(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return roundd64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_HALF_UP);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_nearest_away(x.data()->d_raw,
                                                          &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::round(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return roundd128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_HALF_UP);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_nearest_away(x.data()->d_raw,
                                                           &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal32 DecimalUtil::trunc(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return truncd32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_DOWN);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_zero(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::trunc(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return truncd64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_DOWN);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_zero(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::trunc(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return truncd128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_DOWN);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_zero(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}


                             // Quantum functions

Decimal64 DecimalUtil::multiplyByPowerOf10(Decimal64 value, Decimal64 exponent)
{
    BSLS_ASSERT_SAFE(
      makeDecimal64(-1999999997, 0) <= exponent);
    BSLS_ASSERT_SAFE(                  exponent <= makeDecimal64(99999999, 0));

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int intExponent = __d64_to_long_long(*exponent.data());
    return scalblnd64(*value.data(), intExponent);
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal64 result = value;
    decDoubleScaleB(result.data(),
                    value.data(),
                    exponent.data(),
                    DecimalImpUtil_DecNumber::getDecNumberContext());
    return result;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw =
        __bid64_scalbn(value.data()->d_raw,
                       __bid64_to_int32_int(exponent.data()->d_raw, &flags),
                       &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::multiplyByPowerOf10(Decimal128 value,
                                            Decimal128 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int intExponent = __d128_to_long_long(*exponent.data());
    return scalblnd128(*value.data(), intExponent);
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal128 result = value;
    decQuadScaleB(result.data(),
                  value.data(),
                  exponent.data(),
                  DecimalImpUtil_DecNumber::getDecNumberContext());
    return result;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw =
        __bid128_scalbn(value.data()->d_raw,
                        __bid128_to_int32_int(exponent.data()->d_raw, &flags),
                        &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::quantize(Decimal64 value, Decimal64 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return quantized64(*value.data(), *exponent.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal64 result = value;
    decDoubleQuantize(result.data(),
                      value.data(),
                      exponent.data(),
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return result;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw = __bid64_quantize(value.data()->d_raw,
                                           exponent.data()->d_raw,
                                           &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::quantize(Decimal128 value, Decimal128 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return quantized128(*value.data(), *exponent.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal128 rv = value;
    decQuadQuantize(rv.data(),
                    value.data(),
                    exponent.data(),
                    DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw = __bid128_quantize(value.data()->d_raw,
                                            exponent.data()->d_raw,
                                            &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalUtil::quantum(Decimal64 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int d64_bias = 398;
    return __d64_biased_exponent(*value.data()) - d64_bias;
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decDoubleGetExponent(value.data());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    BID_UINT64 sign;
    int exponent;
    BID_UINT64 coeff;
    unpack_BID64(&sign, &exponent, &coeff, value.data()->d_raw);
    return exponent - DECIMAL_EXPONENT_BIAS;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalUtil::quantum(Decimal128 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int d128_bias = 6176;
    return __d128_biased_exponent(*value.data()) - d128_bias;
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decQuadGetExponent(value.data());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    BID_UINT64 sign;
    int exponent;
    BID_UINT128 coeff;
    BID_UINT128 raw = value.data()->d_raw;
    BID_SWAP128(raw);
    unpack_BID128_value(&sign, &exponent, &coeff, raw);
    return exponent - DECIMAL_EXPONENT_BIAS_128;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

bool DecimalUtil::sameQuantum(Decimal64 x, Decimal64 y)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return samequantumd64(*x.data(), *y.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decDoubleSameQuantum(x.data(), y.data()) == 1;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return __bid64_sameQuantum(x.data()->d_raw, y.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

bool DecimalUtil::sameQuantum(Decimal128 x, Decimal128 y)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return samequantumd128(*x.data(), *y.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decQuadSameQuantum(x.data(), y.data()) == 1;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return __bid128_sameQuantum(x.data()->d_raw, y.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
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
