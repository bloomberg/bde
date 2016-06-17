// bdldfp_decimalconvertutil.cpp                                      -*-C++-*-
#include <bdldfp_decimalconvertutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalconvertutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalplatform.h>

#include <bsls_assert.h>

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     char die[-42];     // if '#error' unsupported
#  endif
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#   define snprintf _snprintf
#   define copysign _copysign
    #if BSLS_PLATFORM_CMP_VERSION >= 1800
        #define copysignf _copysignf
    #else
        #define copysignf _copysign
    #endif
#endif

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <ctype.h>
#include <bsl_cmath.h>
#include <bsl_cfloat.h>
#include <bdlb_float.h>

#include <math.h>

namespace BloombergLP {
namespace bdldfp {

namespace {
                        // Reverse Memory

inline
void memrev(void *buffer, size_t count)
    // Reverse the order of the first specified 'count' bytes, at the beginning
    // of the specified 'buffer'.  'count % 2' must be zero.
{
    unsigned char *b = static_cast<unsigned char *>(buffer);
    bsl::reverse(b, b + count);
}

                        // Mem copy with reversal functions

inline
unsigned char *memReverseIfNeeded(void *buffer, size_t count)
    // Reverse the first specified 'count' bytes from the specified 'buffer`,
    // if the host endian is different from network endian, and return the
    // address computed from 'static_cast<unsigned char *>(buffer) + count'.
{
#ifdef BDLDFP_DECIMALPLATFORM_LITTLE_ENDIAN
    // little endian, needs to do some byte juggling
    memrev(buffer, count);
#endif
    return static_cast<unsigned char*>(buffer) + count;
}

                        // Decimal-network conversion functions

template <class DECIMAL_TYPE>
inline
const unsigned char *decimalFromNetworkT(DECIMAL_TYPE        *decimal,
                                         const unsigned char *buffer)
    // Construct into the specified 'decimal', the base-10 value represented by
    // the network-ordered bytes in the specified 'buffer', and return a raw
    // memory pointer, providing modifiable access, to one byte past the last
    // byte read from 'buffer'.
{
    bsl::memcpy(decimal, buffer, sizeof(DECIMAL_TYPE));
    memReverseIfNeeded(decimal, sizeof(DECIMAL_TYPE));

    DecimalConvertUtil::decimalFromDPD(
                                   decimal,
                                   reinterpret_cast<unsigned char *>(decimal));

    return buffer + sizeof(DECIMAL_TYPE);
}


template <class DECIMAL_TYPE>
inline
unsigned char *decimalToNetworkT(unsigned char *buffer, DECIMAL_TYPE decimal)
    // Construct into the specified 'buffer', the network-ordered byte
    // representation of the base-10 value of the specified 'decimal', and,
    // return a raw memory pointer, providing modifiable access, to one byte
    // past the last written byte of the 'buffer'.
{
    DecimalConvertUtil::decimalToDPD(buffer, decimal);
    return memReverseIfNeeded(buffer, sizeof(DECIMAL_TYPE));
}

                        // =================
                        // class StdioFormat
                        // =================

template <class FORMATTED_TYPE> struct StdioFormat;
    // This 'struct' template provides a method, 'format', that returns a
    // 'printf'-style format string to format values of the template parameter
    // type 'FORMATTED_TYPE' that can be used to restore a decimal value that
    // was previously converted to this type.

template <>
struct StdioFormat<float> {
    // This template specialization of 'StdioFormat' provides a function that
    // returns a 'printf'-style format string for 'float' values.

    static const char* format(float);
        // Return a 'printf'-style format string that can be used to restore a
        // decimal value that was previously converted to a 'float' value.
        // Refer to the documentation of 'decimalFromFloat' for the conversion
        // rules.
};

template <>
struct StdioFormat<double> {
    // This template specialization of 'StdioFormat' provides a function that
    // returns a 'printf'-style format string for 'double' values.

    static char const* format(double);
        // Return a 'printf'-style format string that can be used to restore a
        // decimal value that was previously converted to a 'double' value.
        // Refer to the documentation of 'decimalFromDouble' for the conversion
        // rules.
};

                        // -----------------
                        // class StdioFormat
                        // -----------------

inline
const char* StdioFormat<float>::format(float v)
{
    // 9.999993e-4 and 9.999994e-4 convert to the same float.
    // 8.589973e+9 and 8.589974e+9 convert to the same float.
    // All decimals with seven significant digits that lie in the range
    // '[ 9.999995e-4 .. 8.589972e+9 ]' convert to a unique float, so we can
    // do a seven-digit conversion to eke out an extra decimal places for this
    // important range in which many business numbers lie.
    v = fabsf(v);
    return v >= 9.999995e-4f && v <= 8.589972e+9f ? "%.7g" : "%.6g";
}

inline
const char* StdioFormat<double>::format(double)
{
    return "%.15g";
}

                        // ===================
                        // class DecimalTraits
                        // ===================

template <class DECIMAL_TYPE> struct DecimalTraits;
    // This 'struct' template provides a way to create an object of the
    // template parameter type 'DECIMAL_TYPE' though a consistent interface.


template <>
struct DecimalTraits<Decimal32> {
    // This template specialization of 'DecimalTraits' provides functions to
    // create 'Decimal32' values.

    typedef int SignificandType;
        // This 'typedef' defines a type that is large enough to hold the
        // significant of 'Decimal32'.

    static Decimal32 make(int significand, int exponent);
        // Return a 'Decimal32' value having the specified 'significand' and
        // the specified 'exponent'.
};

template <>
struct DecimalTraits<Decimal64> {
    // This template specialization of 'DecimalTraits' provides utilities to
    // create 'Decimal64' values.

    typedef long long SignificandType;
        // This 'typedef' defines a type that is large enough to hold the
        // significant of 'Decimal64'.

    static Decimal64 make(long long significand, int exponent);
        // Return a 'Decimal64' value having the specified 'significand' and
        // the specified 'exponent'.

};

template <>
struct DecimalTraits<bdldfp::Decimal128> {
    // This template specialization of 'DecimalTraits' provides utilities to
    // create 'Decimal128' values.

    typedef long long SignificandType;
        // This 'typedef' defines a type that is large enough to hold the
        // significant of 'Decimal128' if it's small enough to be convertible
        // to a double.

    static bdldfp::Decimal128 make(long long significand, int exponent);
        // Return a 'Decimal128' value having the specified 'significand' and
        // the specified 'exponent'.
};

                        // ===================
                        // class DecimalTraits
                        // ===================

inline
Decimal32 DecimalTraits<Decimal32>::make(int significand, int exponent)
{
    return bdldfp::DecimalUtil::makeDecimalRaw32(significand, exponent);
}

inline
Decimal64 DecimalTraits<Decimal64>::make(long long significand, int exponent)
{
    return bdldfp::DecimalUtil::makeDecimalRaw64(significand, exponent);
}

inline
Decimal128 DecimalTraits<Decimal128>::make(long long significand, int exponent)
{
    return bdldfp::DecimalUtil::makeDecimalRaw128(significand, exponent);
}

                  // Helpers for Restoring Decimal from Binary

template <class DECIMAL_TYPE, class BINARY_TYPE>
static inline
bool isInRange(BINARY_TYPE)
    // Return true iff the non-singular argument is in the representable range
    // of 'DECIMAL_TYPE'.  Only 'isInRange<Decimal32, double>' actually needs
    // to test the value of the argument.
{
    return true;
}

template <>
inline
bool isInRange<Decimal32, double>(double value)
{
    static double max_decimal = DecimalConvertUtil::decimalToDouble(
                                        bsl::numeric_limits<Decimal32>::max());
    return bsl::abs(value) <= max_decimal;
}

template <class DECIMAL_TYPE, class BINARY_TYPE>
static inline
bool restoreSingularDecimalFromBinary(DECIMAL_TYPE *dfp, BINARY_TYPE bfp)
    // If the specified 'bfp' is a singular value ('Inf', 'Nan', or -0) or is
    // out of range of 'DECIMAL_TYPE', construct the equivalent decimal form or
    // the infinity of the appropriate sign in the specified 'dfp' and return
    // true, otherwise leave 'dfp' unchanged and return false.
{
    bool negative = bdlb::Float::signBit(bfp);

    if (bdlb::Float::isNan(bfp)) {
        *dfp = bsl::numeric_limits<DECIMAL_TYPE>::quiet_NaN();
        if (negative) {
            *dfp = -*dfp;
        }
        return true;                                                  // RETURN
    }

    if (bdlb::Float::isInfinite(bfp) || !isInRange<DECIMAL_TYPE>(bfp)) {
        *dfp = bsl::numeric_limits<DECIMAL_TYPE>::infinity();
        if (negative) {
            *dfp = -*dfp;
        }
        return true;                                                  // RETURN
    }

    if (bfp == 0 && negative) {
        *dfp = DECIMAL_TYPE(0);
        *dfp = -*dfp;
        return true;                                                  // RETURN
    }

    return false;
}

template <class DECIMAL_TYPE, class BINARY_TYPE>
void restoreDecimalFromBinary(DECIMAL_TYPE *dfp, BINARY_TYPE bfp)
    // Construct, in the specified 'dfp', a decimal floating point
    // representation of the value of the binary floating point value specified
    // by 'bfp'.
{

    if (restoreSingularDecimalFromBinary(dfp, bfp)) {
        return;                                                       // RETURN
    }

    char buffer[48];
    int  rc = snprintf(
           buffer, sizeof(buffer), StdioFormat<BINARY_TYPE>::format(bfp), bfp);
    (void)rc;
    BSLS_ASSERT(0 <= rc && rc < static_cast<int>(sizeof(buffer)));

    typename DecimalTraits<DECIMAL_TYPE>::SignificandType significand(0);
    int  exponent(0);
    bool negative(false);

    char const* it(buffer);
    if (*it == '-') {
        negative = true;
        ++it;
    }
    for (; isdigit(static_cast<unsigned char>(*it)); ++it) {
        significand = significand * 10 + (*it - '0');
    }
    if (*it == '.') {
        ++it;
        for (; isdigit(static_cast<unsigned char>(*it)); ++it) {
            significand = significand * 10 + (*it - '0');
            --exponent;
        }
    }
    if (*it == 'e' || *it == 'E') {
        ++it;
        exponent += bsl::atoi(it);
    }

    *dfp = DecimalTraits<DECIMAL_TYPE>::make(significand, exponent);

    if (negative) {
        *dfp = -*dfp;
    }
}

template <class INTEGER_TYPE>
inline void reduce(INTEGER_TYPE *psignificand, int *pscale)
    // Divide the value of the specified '*psignificand' by 10 and increment
    // '*pscale' as few times as needed to either make '*psignificand' not be
    // divisible by 10 or to make '*pscale' non-negative.  The purpose is to
    // avoid scaling artifacts when converting to decimal floating point; e.g.,
    // given .001, we want 1e-3 instead of 1000e-6.
{
    INTEGER_TYPE n = *psignificand;
    int scale = *pscale;

    // Do a quick check for trailing 0 bits before the more expensive % check
    // since each factor of 10 has a corresponding factor of 2.  Try to lop off
    // factors of 1000 first if they're present.

    while ((n & 7) == 0 && n % 1000 == 0 && scale <= -3) {
        n /= 1000;
        scale += 3;
    }

    while ((n & 1) == 0 && n % 10 == 0 && scale <= -1) {
        n /= 10;
        ++scale;
    }

    *psignificand = n;
    *pscale = scale;
}

inline
bool quickDecimal64FromDouble(Decimal64 *result,
                              double     binary,
                              double     threshold)
    // Return 'true' iff an attempt to set the specified 'result' to a "quick"
    // conversion from the specified 'binary' succeeds.  This occurs when
    // 'binary' is in an appropriate range and scaling and rounding it to an
    // integer results in a remainder whose ratio with the result is less than
    // the specified 'threshold'.
{
    BSLS_ASSERT(result);

    // Use the "Olkin-Farber-Rosen" method for speed.  Multiply the double by a
    // power of 10, round it to an integer, then use the faster scaled
    // conversion to decimal to produce a hoped-for result.  The conversion is
    // correct if the result converts back to the original binary, provided
    // that the intermediate integer has no more than 15 (DBL_DIG) significant
    // digits (because no two decimal numbers with 15 or fewer significant
    // digits can convert to the same double).  Since we want to optimize for
    // original numbers that may have as many as 9 decimal places, we scale by
    // 1e9, and therefore the magnitude of the original number must be less
    // than 1e6.
    if (binary != 0 && -1e6 < binary && binary < 1e6) {
        double d = binary * 1e9;
        long long n = static_cast<long long>(d + copysign(.5, d));  // round
        int scale = -9;

        // Divide powers of 10 out of n to avoid unpleasant scaling artifacts.
        // E.g., we want .001 to appear to have 3 digits, not 9.
        reduce(&n, &scale);

        *result = DecimalUtil::makeDecimalRaw64(n, scale);

        // Decimal64 values hold 16 significant digits, so given a Decimal64
        // value 'V = M * 10^E with 1 <= M < 10', its successor is
        // 'Vnext = (M + 10^-15) * 10^E' and the ratio of their difference to
        // the value is '10^(E-15) / (M * 10^E) = 10^-15 / M >= 10^-16'.
        //
        // We are concerned that the Decimal64 value we have computed for the
        // 'binary' value may be wrong, i.e., that its successor or predecessor
        // may be closer in value to 'binary'.  If so, that difference comes
        // from the fractional portion of the scaled 'binary'.  If we can
        // determine that the fractional portion is too small to push the value
        // up or down, then we do not need to verify by back conversion.
        //
        // Compute the ratio of the fractional part of the scaled binary number
        // to its integer part.  If it's small enough (using 1e-17, generously
        // below the 1e-16 computed above), skip the verification.
        double dn, r;
        r = bsl::modf(d, &dn);
        if ((dn != 0 && r / dn < threshold) || r == 0) {
            return true;                                              // RETURN
        }

        // Otherwise, see if the decimal converts back to the original value.
        double test = DecimalConvertUtil::decimalToDouble(*result);
        if (test == binary) {
            return true;                                              // RETURN
        }
    }
    return false;
}

inline
bool quickDecimal64FromFloat(Decimal64 *result, float binary, float threshold)
    // Return 'true' iff an attempt to set the specified 'result' to a "quick"
    // conversion from the specified 'binary' succeeds.  This occurs when
    // 'binary' is in an appropriate range and scaling and rounding it to an
    // integer results in a remainder whose ratio with the result is less than
    // the specified 'threshold'.
{
    // Try the "Olkin-Farber-Rosen" method for speed.  Multiply the float by a
    // power of 10, round it to an integer, then use the faster scaled
    // conversion to decimal to produce a hoped-for result.  The conversion is
    // correct if the result converts back to the original binary, provided
    // that the intermediate integer has no more than 6 (FLT_DIG) significant
    // digits (because no two decimal numbers with 6 or fewer significant
    // digits can convert to the same float).

    // Avoid going through this code for 0; it will not work correctly for -0,
    // and the scale reduction would need to move -6 up to 0.
    if (binary != 0 && -1e06 < binary && binary < 1e06) {
        int   scale;
        float d;
        float a = fabsf(binary);

        // Because we have only six digits of precision with which to work,
        // find in which order of magnitude the value is contained and scale
        // accordingly.  (This differs from the double case, where we can scale
        // by a single power of ten for the entire range of interest.)
        if (a < 1e3f) {
            if (a < 1e1f) {
                if (a < 1e0f) {
                    scale = -6;
                    d = binary * 1e6f;
                } else {
                    scale = -5;
                    d = binary * 1e5f;
                }
            } else if (a < 1e2f) {
                scale = -4;
                d = binary * 1e4f;
            } else {
                scale = -3;
                d = binary * 1e3f;
            }
        } else if (a < 1e5f) {
            if (a < 1e4f) {
                scale = -2;
                d = binary * 1e2f;
            } else {
                scale = -1;
                d = binary * 1e1f;
            }
        } else {
            scale = 0;
            d = binary;
        }

        int   n    = static_cast<int>(d + copysignf(.5, d));
        float diff = d - float(n);

        reduce(&n, &scale);
        *result = DecimalUtil::makeDecimalRaw64(n, scale);

        // We have generated a Decimal64 with six significant digits, and are
        // concerned that it may be wrong, i.e., that the next higher or lower
        // six-digit Decimal64 may be closer in value to 'binary'.
        //
        // Suppose our generated number is 'V = M * 10^E, 1 <= M < 10'.  Its
        // six-significant-digit successor is 'V+ = (M + 10^-5) * 10^E' and the
        // relative difference of the two is
        // '(V+ - V) / V = 10^(E-5) / (M * 10^E) > 10^-6'.  If we can determine
        // that the relative difference between the fractional portion of the
        // scaled 'binary' and its integer part is too small to push the
        // decimal value up or down in the sixth significant digit, then we do
        // not need to verify by back conversion.
        //
        // Compute the ratio of the fractional part of the scaled binary number
        // to its integer part.  If it's small enough, skip the verification.
        //
        // So what is "small enough"?  5e-7 would do.  However, we want to
        // allow 7-digit decimals in a restricted range (see StdioFormat above)
        // and using 5e-7 causes some of those to convert as accurate 6-digit
        // numbers.  1e-8 eliminates those at the cost of about 7% fewer quick
        // conversions.

        if (fabsf(diff / d) < threshold) {
            return true;                                              // RETURN
        }

        // Otherwise, see if the decimal converts back to the original
        // value.
        float test = DecimalConvertUtil::decimalToFloat(*result);
        if (test == binary) {
            return true;                                              // RETURN
        }
    }
    return false;
}

}  // close unnamed namespace

                        // Network format converters

// Note that we do not use platform or bslsl supported converters because they
// work in terms of integers, so they would probably bleed out on the
// strict-aliasing rules.  We may solve that later on using the "union trick"
// and delegating to 'bsls_byteorder', but for now let's take it slow.

                        // Conversion to Network functions

unsigned char *DecimalConvertUtil::decimal32ToNetwork(unsigned char *buffer,
                                                      Decimal32      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimal64ToNetwork(unsigned char *buffer,
                                                      Decimal64      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimal128ToNetwork(unsigned char *buffer,
                                                       Decimal128     decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimalToNetwork(unsigned char *buffer,
                                                    Decimal32      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimalToNetwork(unsigned char *buffer,
                                                    Decimal64      decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

unsigned char *DecimalConvertUtil::decimalToNetwork(unsigned char *buffer,
                                                    Decimal128     decimal)
{
    BSLS_ASSERT(buffer != 0);
    return decimalToNetworkT(buffer, decimal);
}

                        // Conversion to Network functions

const unsigned char *DecimalConvertUtil::decimal32FromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimal64FromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimal128FromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

const unsigned char *DecimalConvertUtil::decimalFromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal != 0);
    return decimalFromNetworkT(decimal, buffer);
}

            // Restore a Decimal Floating-Point from a Binary


                        // DecimalFromDouble functions

Decimal32 DecimalConvertUtil::decimal32FromDouble(double binary)
{
    Decimal32 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

Decimal64 DecimalConvertUtil::decimal64FromDouble(double binary)
{
    Decimal64 rv;

    if (!quickDecimal64FromDouble(&rv, binary, 1e-17)) {
        restoreDecimalFromBinary(&rv, binary);
    }
    return rv;
}

Decimal128 DecimalConvertUtil::decimal128FromDouble(double binary)
{
    Decimal128 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

                        // DecimalFromFloat functions

Decimal32 DecimalConvertUtil::decimal32FromFloat(float binary)
{
    Decimal32 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

Decimal64 DecimalConvertUtil::decimal64FromFloat(float binary)
{
    Decimal64 rv;
    if (!quickDecimal64FromFloat(&rv, binary, 1e-8f)) {
        restoreDecimalFromBinary(&rv, binary);
    }
    return rv;
}

Decimal128 DecimalConvertUtil::decimal128FromFloat(float binary)
{
    Decimal128 rv;
    restoreDecimalFromBinary(&rv, binary);
    return rv;
}

static inline
int bound(int value, int low, int high)
    // Return the specified 'low' if the specified 'value' is less than 1, the
    // specified 'high' if 'value' is greater than 'high', and 'value'
    // otherwise.
{
    return value < 1 ? low : value > high ? high : value;
}

static inline
void parseDecimal(Decimal32 *result, const char *buffer)
    // Set the specified 'result' to the result of calling
    // 'DecimalUtil::parseDecimal32' on the specified 'buffer'.
{
    DecimalUtil::parseDecimal32(result, buffer);
}

static inline
void parseDecimal(Decimal64 *result, const char *buffer)
    // Set the specified 'result' to the result of calling
    // 'DecimalUtil::parseDecimal64' on the specified 'buffer'.
{
    DecimalUtil::parseDecimal64(result, buffer);
}

static inline
void parseDecimal(Decimal128 *result, const char *buffer)
    // Set the specified 'result' to the result of calling
    // 'DecimalUtil::parseDecimal128' on the specified 'buffer'.
{
    DecimalUtil::parseDecimal128(result, buffer);
}

template <class DECIMAL_TYPE, int LIMIT, class BINARY_TYPE>
static DECIMAL_TYPE restoreDecimalDigits(BINARY_TYPE binary, int digits)
    // Return the closest decimal value with the specfied 'digits' significant
    // digits to the specified 'binary'.  Singular (infinity, NaN, and -0) and
    // out-of-range 'binary' values are converted to appropriate decimal
    // singular values.  If 'digits' is less than 1, use 'LIMIT' instead, and
    // if 'digits' is greater than the number of digits that 'DECIMAL_TYPE' can
    // hold, use that number instead.
{
    DECIMAL_TYPE result;
    char         buffer[32];
    if (!restoreSingularDecimalFromBinary(&result, binary)) {
        int rc = snprintf(
                        buffer,
                        sizeof(buffer),
                        "%1.*e",
                        bound(digits,
                              LIMIT,
                              bsl::numeric_limits<DECIMAL_TYPE>::digits10) - 1,
                        binary);
        (void)rc;
        BSLS_ASSERT(0 <= rc && rc < static_cast<int>(sizeof(buffer)));
        parseDecimal(&result, buffer);
    }
    return result;
}

Decimal32 DecimalConvertUtil::restoreDecimal32Digits(float binary, int digits)
{
    return restoreDecimalDigits<Decimal32, 7>(binary, digits);
}

Decimal64 DecimalConvertUtil::restoreDecimal64Digits(float binary, int digits)
{
    Decimal64 rv;
    if (digits <= 6 && quickDecimal64FromFloat(&rv, binary, 5e-7f) ||
        digits == 7 && quickDecimal64FromFloat(&rv, binary, 1e-8)) {
        return rv;                                                    // RETURN
    }
    return restoreDecimalDigits<Decimal64, 9>(binary, digits);
}

Decimal128 DecimalConvertUtil::restoreDecimal128Digits(float binary,
                                                       int   digits)
{
    return restoreDecimalDigits<Decimal128, 9>(binary, digits);
}

Decimal32 DecimalConvertUtil::restoreDecimal32Digits(double binary, int digits)
{
    return restoreDecimalDigits<Decimal32, 7>(binary, digits);
}

Decimal64 DecimalConvertUtil::restoreDecimal64Digits(double binary, int digits)
{
    Decimal64 rv;
    if (digits <= 9 && quickDecimal64FromDouble(&rv, binary, 1e-17)) {
        return rv;                                                    // RETURN
    }
    return restoreDecimalDigits<Decimal64, 16>(binary, digits);
}

Decimal128 DecimalConvertUtil::restoreDecimal128Digits(double binary,
                                                       int    digits)
{
    return restoreDecimalDigits<Decimal128, 17>(binary, digits);
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
