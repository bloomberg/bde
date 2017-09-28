// bdldfp_decimalimputil.cpp                                          -*-C++-*-
#include <bdldfp_decimalimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalimputil_cpp,"$Id$ $CSID$")

#include <bdldfp_uint128.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>

#include <bsls_performancehint.h>

namespace BloombergLP {
namespace bdldfp {

namespace {

template <class DECIMAL>
struct DecimalTraits;

template <>
struct DecimalTraits<DecimalImpUtil::ValueType32>
{
    // TYPES
    typedef unsigned int Significand;

    // CLASS DATA
    static const int k_MAX_DIGITS = 7;

    // CLASS METHODS
    static DecimalImpUtil::ValueType32 int32ToDecimal(int x)
    {
        return DecimalImpUtil::int32ToDecimal32(x);
    }

    static int classify(DecimalImpUtil::ValueType32 x)
    {
        return __bid32_class(x.d_raw);
    }
};

template <>
struct DecimalTraits<DecimalImpUtil::ValueType64>
{
    // TYPES
   typedef bsls::Types::Uint64 Significand;

    // CLASS DATA
    static const int k_MAX_DIGITS = 16;

    // CLASS METHODS
    static DecimalImpUtil::ValueType64 int32ToDecimal(int x)
    {
        return DecimalImpUtil::int32ToDecimal64(x);
    }

    static int classify(DecimalImpUtil::ValueType64 x)
    {
        return __bid64_class(x.d_raw);
    }
};

template <>
struct DecimalTraits<DecimalImpUtil::ValueType128>
{
    // TYPES
    typedef Uint128 Significand;

    // CLASS DATA
    static const int k_MAX_DIGITS = 34;

    // CLASS METHODS
    static DecimalImpUtil::ValueType128 int32ToDecimal(int x)
    {
        return DecimalImpUtil::int32ToDecimal128(x);
    }

    static int classify(DecimalImpUtil::ValueType128 x)
    {
        return __bid128_class(x.d_raw);
    }
};

template <class T>
static
T divmod10(T* v)
    // Load the resultant value of dividing the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    BSLS_ASSERT(v);
    T r =  *v % 10;
    *v /= 10;
    return r;
}

bsls::Types::Uint64 divmod10(Uint128* v)
    // Load the resultant value of dividing the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    // Set a = v.high(), b = v.low().
    // Let Q = (2^64 * a + b) / 10 and R = (2^64 * a + b) % 10.
    // Set a to Q / 2^64, b to Q % 2^64, and return R.
    BSLS_ASSERT(v);
    bsls::Types::Uint64 a  = v->high();
    bsls::Types::Uint64 b  = v->low();

    bsls::Types::Uint64 qa = a / 10;
    bsls::Types::Uint64 ra = a % 10;
    bsls::Types::Uint64 qb = b / 10;
    bsls::Types::Uint64 rb = b % 10;

    v->setHigh(qa);
    v->setLow(qb + 0x1999999999999999ull * ra + (6 * ra + rb) / 10);

    return (6 * ra + rb) % 10;
}

bool operator !=(const Uint128& lhr, const bsls::Types::Uint64& rhr)
{
    return lhr.high() || lhr.low() != rhr;
}

template <class T>
int getMostSignificandPlace(T value)
    // Return the most significant decimal place of the specified 'value'.
{
    int place = 0;
    do {
        divmod10(&value);
        ++place;
    } while (value != 0);

    return place;
}

template <class T>
int print(char *b, char *e, T value)
    // Read a data from the specified 'value', convert them to character string
    // equivalent and load the result into a buffer confined by the specified
    // 'b' and 'e' iterators.  Return the total number of written characters.
{
    char *i = e;
    do {
        *--i = static_cast<char>('0' + divmod10(&value));
    } while (value != 0);

    bsl::copy(i, e, b);

    return static_cast<int>(e - i);
}

template <class DECIMAL>
int formatFixed(char                      *buffer,
                int                        length,
                DECIMAL                    value,
                const DecimalFormatConfig& cfg)
    // Convert the specified decimal 'value' to character string using
    // 'e_FIXED' style and the specified 'cfg' formatting options.  Load the
    // result into the specified 'buffer'.  If the length of resultant string
    // exceeds the specified 'length', then the 'buffer' will be left in an
    // unspecified state, with the returned value indicating the necessary
    // size.  The behavior is undefined if the 'length' is negative and
    // 'buffer' is not null.  The 'buffer' is permitted to be null if the
    // 'length' is not  positive.  This can be used to determine the necessary
    // buffer size.
{
    BSLS_ASSERT(buffer || length < 0);

    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    const DECIMAL k_ZERO = DecimalTraits<DECIMAL>::int32ToDecimal(0);

    if (DecimalImpUtil::notEqual(value, k_ZERO)) {
        value = DecimalImpUtil::round(value, cfg.precision());
    }

    int          sign;
    SIGNIFICAND  s;
    int          exponent;
    int          cls = DecimalImpUtil::decompose(&sign, &s, &exponent, value);

    BSLS_ASSERT(cls == FP_ZERO   ||
                cls == FP_NORMAL ||
                cls == FP_SUBNORMAL);

    (void)cls;

    const int k_MAX_DIGITS = DecimalTraits<DECIMAL>::k_MAX_DIGITS;

    char significand[k_MAX_DIGITS] = { 0 };
    int  len = print(&significand[0],
                     &significand[k_MAX_DIGITS],
                     s);

    int pointPos     = (s != 0) ? len + exponent : 0;
    int outputLength = static_cast<int>((pointPos > 0 ? pointPos : sizeof('0'))
                                    + (cfg.precision() > 0) + cfg.precision());

    if (outputLength <= length) {

        char       *oi = buffer;
        char       *oe = buffer + outputLength;
        const char *ii = significand;
        const char *ie = significand + len;

        if (0 >= pointPos) {   // value < 1
            *oi++ = '0';

        } else {               // value >= 1
            const char *end = bsl::min(ii + pointPos, ie);
            if (ii < end) {
                oi = bsl::copy(ii, end, oi);
                ii = end;
            }

            while (oi < buffer + pointPos) {
                *oi++ = '0';
            }
        }

        if (cfg.precision()) {
            *oi++ = cfg.decimalPoint();

            const char *end = bsl::min(oi - pointPos, oe);
            while (oi < end) {
                *oi++ = '0';
            }

            end = bsl::min(ii + pointPos + cfg.precision(), ie);
            if (ii < end) {
                oi = bsl::copy(ii, end, oi);
            }

            if (oe - oi > 0) {
                bsl::fill_n(oi, oe - oi, '0');
            }
        }
    }

    return outputLength;
}

template <class DECIMAL>
int formatScientific(char                      *buffer,
                     int                        length,
                     DECIMAL                    value,
                     const DecimalFormatConfig& cfg)
    // Convert the specified decimal 'value' to character string using
    // 'e_SCIENTIFIC' style and the specified 'cfg' formatting options.  Load
    // the result into the specified 'buffer'.  If the length of resultant
    // string exceeds the specified 'length', then the 'buffer' will be left in
    // an unspecified state, with the returned value indicating the necessary
    // size.  The behavior is undefined if the 'length' is negative and
    // 'buffer' is not null.  The 'buffer' is permitted to be null if the
    // 'length' is not  positive.  This can be used to determine the necessary
    // buffer size.
{
    BSLS_ASSERT(buffer || length < 0);

    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    int           sign;
    SIGNIFICAND   s;
    int           exponent;
    int           cls = DecimalImpUtil::decompose(&sign, &s, &exponent, value);

    BSLS_ASSERT(cls == FP_ZERO   ||
                cls == FP_NORMAL ||
                cls == FP_SUBNORMAL);

    (void)cls;

    if (s != 0) {
        int maxDigit = getMostSignificandPlace(s);
        if (maxDigit - 1 > cfg.precision()) {
            // round to the specified precision after point in scientific
            // format
            int roundExp = -exponent - maxDigit + 1 + cfg.precision();

            value = DecimalImpUtil::scaleB(value, roundExp);
            value = DecimalImpUtil::round(value);
            value = DecimalImpUtil::scaleB(value, -roundExp);

            cls = DecimalImpUtil::decompose(&sign, &s, &exponent, value);
            BSLS_ASSERT(cls == FP_ZERO   ||
                        cls == FP_NORMAL ||
                        cls == FP_SUBNORMAL);
        }
    }

    const int k_MAX_DIGITS = DecimalTraits<DECIMAL>::k_MAX_DIGITS;

    char significand[k_MAX_DIGITS] = { 0 };
    int  len = print(&significand[0],
                     &significand[k_MAX_DIGITS],
                     s);

    exponent += len - 1;

    const int k_MAX_EXPONENT_LENGTH = 6;
    char      exp[k_MAX_EXPONENT_LENGTH];
    int       exponentLength = bsl::sprintf(&exp[0], "%+d", exponent);
    int       outputLength   = 1
                               + (cfg.precision() > 0) + cfg.precision()
                               + static_cast<int>(sizeof 'E')
                               + exponentLength;
    char     *i              = buffer;

    if (outputLength <= length) {

        const char *j = significand;
        const char *e = significand + len;

        *i++ = *j++;

        if (cfg.precision()) {
            *i++ = cfg.decimalPoint();

            const char *end = bsl::min(j + cfg.precision(), e);
            if (j <= end) {
                i = bsl::copy(j, end, i);
                if (end == e) {
                    long int n = bsl::distance(e, j + cfg.precision());
                    bsl::fill_n(i, n, '0');
                    i += n;
                }
            }
        }
        *i++ = cfg.exponent();
        i    = bsl::copy(&exp[0], &exp[0] + exponentLength, i);
    }

    return outputLength;
}

template <class DECIMAL>
int formatNatural(char                      *buffer,
                  int                        length,
                  DECIMAL                    value,
                  const DecimalFormatConfig& cfg)
    // Convert the specified decimal 'value' to character string using
    // 'e_NATURAL' style and the specified 'cfg' formatting options.  Load the
    // result into the specified 'buffer'.  If the length of resultant string
    // exceeds the specified 'length', then the 'buffer' will be left in an
    // unspecified state, with the returned value indicating the necessary
    // size.  The behavior is undefined if the 'length' is negative and
    // 'buffer' is not null.  The 'buffer' is permitted to be null if the
    // 'length' is not  positive.  This can be used to determine the necessary
    // buffer size.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);

    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    int         sign;
    int         e;
    SIGNIFICAND s;
    int         cls = DecimalImpUtil::decompose(&sign, &s, &e, value);

    (void)cls;

    BSLS_ASSERT(cls == FP_ZERO   ||
                cls == FP_NORMAL ||
                cls == FP_SUBNORMAL);

    int maxDigit         = getMostSignificandPlace(s);
    int adjustedExponent = e + maxDigit - 1;

    if (e <= 0 && adjustedExponent >= -6) {
        return formatFixed(buffer, length, value, cfg);               // RETURN
    }

    return  formatScientific(buffer, length, value, cfg);
}

int formatSpecial(char        *buffer,
                  int          length,
                  const char  *begin,
                  const char  *end)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);

    const long int len = bsl::distance(begin, end);

    if (len <= length) {
        bsl::copy(begin, end, buffer);
    }

    return static_cast<int>(len);
}

template <class DECIMAL>
static
int formatImpl(char                      *buffer,
               int                        length,
               DECIMAL                    value,
               const DecimalFormatConfig& cfg)
{
    BSLS_ASSERT(buffer || length < 0);
    BSLS_ASSERT(cfg.precision() >= 0);

    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    SIGNIFICAND s;
    int         sign;
    int         exponent;
    int         cls = DecimalImpUtil::decompose(&sign, &s, &exponent, value);

    char  signSymbol('+');
    int   signLength(0);

    if (-1 == sign || cfg.sign() != DecimalFormatConfig::e_NEGATIVE_ONLY) {
        signLength = 1;
        if (-1 == sign) {
            signSymbol = '-';
        }
    }

    int   bufferLength  = length - signLength;
    char *it            = buffer + signLength;
    int   decimalLength = 0;

    switch (cls) {
      case FP_NORMAL:
      case FP_SUBNORMAL: {
          value = DecimalImpUtil::normalize(value);
      }                                                             // NO BREAK
      case FP_ZERO: {

        switch (cfg.style()) {
          case DecimalFormatConfig::e_SCIENTIFIC: {

              decimalLength = formatScientific(it, bufferLength, value, cfg);

          } break;
          case DecimalFormatConfig::e_FIXED: {

              decimalLength = formatFixed(it, bufferLength, value, cfg);

          } break;
          case DecimalFormatConfig::e_NATURAL: {

              decimalLength = formatNatural(it, bufferLength, value, cfg);

          } break;
          default: {
            BSLS_ASSERT(!"Unexpected format style value");
          } break;
        }
      } break;
      case FP_INFINITE: {
          const char *inf = cfg.infinity();
          decimalLength = formatSpecial(it,
                                        bufferLength,
                                        inf,
                                        inf + bsl::strlen(inf));
      } break;
      case FP_NAN: {
          enum class_types cl = static_cast<class_types>(
                                      DecimalTraits<DECIMAL>::classify(value));
          const char *nan = (quietNaN == cl) ? cfg.nan() : cfg.sNan();
          decimalLength = formatSpecial(it,
                                        bufferLength,
                                        nan,
                                        nan + bsl::strlen(nan));
      } break;
    }

    decimalLength += signLength;
    if (decimalLength <= length && signLength) {
       *buffer = signSymbol;
    }

    return decimalLength;
}

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

DecimalImpUtil::ValueType32 DecimalImpUtil::normalize(ValueType32 value)
{
    ValueType32 result;

    int          sign;
    unsigned int significand;
    int          exponent;
    int          objClass;

    objClass = decompose(&sign, &significand, &exponent, value);

    switch (objClass) {
      case FP_ZERO: {
        result = makeDecimalRaw32(0, 0);
      } break;

      case FP_INFINITE: {
        if (sign == 1) {
            result = infinity32();
        } else {
            result = negate(infinity32());
        }
      } break;

      case FP_NAN: {
        if (sign == 1) {
            result = quietNaN32();
        } else {
            result = negate(quietNaN32());
        }
      } break;

      case FP_NORMAL:
      case FP_SUBNORMAL: {
        while ((significand % 10 == 0) && (exponent < 90)) {
            significand /= 10;
            ++exponent;
        }

        result = makeDecimalRaw32(significand, exponent);
        result = (sign == 1) ? result : negate(result);
      } break;

      default:
        BSLS_ASSERT(false);
    }

    return result;
}

DecimalImpUtil::ValueType64 DecimalImpUtil::normalize(ValueType64 value)
{
    ValueType64 result;

    int                 sign;
    bsls::Types::Uint64 significand;
    int                 exponent;
    int                 objClass;

    objClass = decompose(&sign, &significand, &exponent, value);

    switch (objClass) {
      case FP_ZERO: {
        result = makeDecimalRaw64(0, 0);
      } break;

      case FP_INFINITE: {
        if (sign == 1) {
            result = infinity64();
        } else {
            result = negate(infinity64());
        }
      } break;

      case FP_NAN: {
        if (sign == 1) {
            result = quietNaN64();
        } else {
            result = negate(quietNaN64());
        }
      } break;

      case FP_NORMAL:
      case FP_SUBNORMAL: {
        while ((significand % 10 == 0) && (exponent < 369)) {
            significand /= 10;
            ++exponent;
        }

        result = makeDecimalRaw64(significand, exponent);
        result = (sign == 1) ? result : negate(result);
      } break;

      default:
        BSLS_ASSERT(false);
    }

    return result;
}

DecimalImpUtil::ValueType128 DecimalImpUtil::normalize(ValueType128 value)
{
    ValueType128 result;

    int     sign;
    Uint128 significand;
    int     exponent;
    int     objClass;

    objClass = decompose(&sign, &significand, &exponent, value);

    switch (objClass) {
      case FP_ZERO: {
        result = makeDecimalRaw128(0, 0);
      } break;

      case FP_INFINITE: {
        if (sign == 1) {
            result = infinity128();
        } else {
            result = negate(infinity128());
        }
      } break;

      case FP_NAN: {
        if (sign == 1) {
            result = quietNaN128();
        } else {
            result = negate(quietNaN128());
        }
      } break;

      case FP_NORMAL:
      case FP_SUBNORMAL: {
        bsls::Types::Uint64 r = 0;  // reminder
        while (exponent < 6144) {
            bsls::Types::Uint64 high = significand.high();
            bsls::Types::Uint64 low  = significand.low();

            bsls::Types::Uint64 qh = high / 10;
            bsls::Types::Uint64 rh = high % 10;
            bsls::Types::Uint64 ql = low / 10;
            bsls::Types::Uint64 rl = low % 10;

            r = (6 * rh + rl) % 10;
            if (r != 0) {
                break;
            }

            significand.setHigh(qh);
            significand.setLow(ql + 0x1999999999999999ull * rh +
                                                           (6 * rh + rl) / 10);
            ++exponent;
        }

        // TODO: we need constructor/util function for Decimal128 creation,
        //       accepting Uint128.
        const ValueType128 MOVE_LEFT_64 = add(
                                     uint64ToDecimal128(0xFFFFFFFFFFFFFFFFull),
                                     uint64ToDecimal128(1));
        const ValueType128 SIGNIFICAND = add(
                                     multiply(
                                        uint64ToDecimal128(significand.high()),
                                        MOVE_LEFT_64),
                                     uint64ToDecimal128(significand.low()));
        result = scaleB(SIGNIFICAND, exponent);
        result = (sign == 1) ? result : negate(result);
      } break;

      default:
        BSLS_ASSERT(false);
    }

    return result;
}

DecimalImpUtil::ValueType32
DecimalImpUtil::round(ValueType32  x, unsigned int decimalPlaces)
{
    int          sign;
    unsigned int significand;
    int          exponent;
    int          cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(decimalPlaces) < 0))
    {
        x = scaleB(x, decimalPlaces);
        x = round(x);
        x = scaleB(x, -decimalPlaces);
    }
    return x;
}

DecimalImpUtil::ValueType64
DecimalImpUtil::round(ValueType64  x, unsigned int decimalPlaces)
{
    int                 sign;
    bsls::Types::Uint64 significand;
    int                 exponent;
    int                 cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(decimalPlaces) < 0))
    {
        x = scaleB(x, decimalPlaces);
        x = round(x);
        x = scaleB(x, -decimalPlaces);
    }
    return x;
}

DecimalImpUtil::ValueType128
DecimalImpUtil::round(ValueType128 x, unsigned int decimalPlaces)
{
    int     sign;
    Uint128 significand;
    int     exponent;
    int     cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(decimalPlaces) < 0))
    {
        x = scaleB(x, decimalPlaces);
        x = round(x);
        x = scaleB(x, -decimalPlaces);
    }
    return x;
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
        k_DECIMAL_EXPONENT_BIAS = 101
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

int DecimalImpUtil::format(char                      *buffer,
                           int                        length,
                           ValueType32               value,
                           const DecimalFormatConfig& config)
{
    return formatImpl(buffer, length, value, config);
}

int DecimalImpUtil::format(char                      *buffer,
                           int                        length,
                           ValueType64                value,
                           const DecimalFormatConfig& config)
{
    return formatImpl(buffer, length, value, config);
}

int DecimalImpUtil::format(char                      *buffer,
                           int                        length,
                           ValueType128               value,
                           const DecimalFormatConfig& config)
{
    return formatImpl(buffer, length, value, config);
}

DecimalImpUtil::ValueType32 DecimalImpUtil::min32() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-95df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "1e-95",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw32(1, -95);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::max32() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999e96df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                       "9.999999e96",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse32("9.999999e96");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::epsilon32() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-6df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "1e-6",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw32(1, -6);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::roundError32() BSLS_CPP11_NOEXCEPT
{ // TBD TODO - determine the real value from the round mode!
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "1.0",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw32(1, 0);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::denormMin32() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000001E-95df;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "0.000001E-95",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse32("0.000001E-95");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::infinity32() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D32;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "INF",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse32("INF");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::quietNaN32() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN32;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "NaN",
                         DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse32("NaN");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType32 DecimalImpUtil::signalingNaN32()
    BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_SNAN32;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decSingle (endianness!)
    decSingle rv;
    decSingleFromString(&rv,
                        "sNaN",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse32("sNaN");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::min64() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-383dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "1e-383",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw64(1, -383);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::max64() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999999999999e384dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "9.999999999999999e384",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse64("9.999999999999999e384");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::epsilon64() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-15dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "1e-15",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse64("1e-15");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::roundError64() BSLS_CPP11_NOEXCEPT
{  // TBD TODO - determine the real value from the round mode!
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "1.0",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw64(1, 0);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::denormMin64() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000000000000001e-383dd;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "0.000000000000001e-383",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse64("0.000000000000001e-383");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::infinity64() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D64;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "INF",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse64("INF");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::quietNaN64() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN64;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "qNaN",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    BSLS_ASSERT(reinterpret_cast<const unsigned long long &>(rv) != 0);
    decDouble rv2 = rv;
    BSLS_ASSERT(reinterpret_cast<const unsigned long long &>(rv2) != 0);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse64("NaN");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType64 DecimalImpUtil::signalingNaN64()
    BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_SNAN64;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decDouble (endianness!)
    decDouble rv;
    decDoubleFromString(&rv,
                        "sNaN",
                        DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse64("sNaN");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::min128() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-6143dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "1e-6143",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw128(1, -6143);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::max128() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 9.999999999999999999999999999999999e6144dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "9.999999999999999999999999999999999e6144",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse128("9.999999999999999999999999999999999e6144");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::epsilon128() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1e-33dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "1e-33",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw128(1, -33);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::roundError128()
    BSLS_CPP11_NOEXCEPT
{  // TBD TODO - determine the real value from the round mode setting!
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 1.0dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "1.0",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return makeDecimalRaw128(1, 0);
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::denormMin128() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return 0.000000000000000000000000000000001e-6143dl;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "0.000000000000000000000000000000001e-6143",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse128("0.000000000000000000000000000000001e-6143");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::infinity128() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return HUGE_VAL_D128;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "INF",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse128("INF");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::quietNaN128() BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN128;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "NaN",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse128("NaN");
#else
BSLMF_ASSERT(false);; // Unsupported platform
#endif
}

DecimalImpUtil::ValueType128 DecimalImpUtil::signalingNaN128()
    BSLS_CPP11_NOEXCEPT
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return BDLDFP_DECIMALPLATFORM_C99_QNAN128;
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    // TBD TODO - just return a statically initialized decQuad (endianness!)
    decQuad rv;
    decQuadFromString(&rv,
                      "sNaN",
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return parse128("sNaN");
#else
BSLMF_ASSERT(false);; // Unsupported platform
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
