// bdldfp_decimalutil.cpp                                             -*-C++-*-
#include <bdldfp_decimalutil.h>
#include <bdldfp_decimalimputil_decnumber.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalplatform.h>
#include <bdldfp_decimalimputil.h>

#include <bsls_assert.h>
#include <bslmf_assert.h>

#include <bsl_c_errno.h>
#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <errno.h>
#include <math.h>  // For the  FP_* macros

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

namespace BloombergLP {
namespace bdldfp {


namespace {

template <class DECIMAL>
struct DecimalTraits;

template <>
struct DecimalTraits<Decimal32>
{
    typedef unsigned int Significand;

    // Decimal32 makeDecimal(int significand, int significand 
};

template <>
struct DecimalTraits<Decimal64>
{
    typedef bsls::Types::Uint64 Significand;
};

template <>
struct DecimalTraits<Decimal128>
{
    typedef Uint128 Significand;
};

int formatSign(char                            *symbol,
               int                              sign,
               const DecimalFormatConfig::Sign& signOutput)
    // Format the specified 'sign' according to the specified 'signOutput'
    // type and load the result into the specified 'buffer'.  Return the
    // length of the formatted sign.
{
    int len = 1;
    if (sign < 0) {
        *symbol = '-';
    }
    else {
        if (DecimalFormatConfig::e_NEGATIVE_ONLY == signOutput) {
          len = 0;
        }
        else {
          *symbol = '+';
        }
    }
    return len;
}

template <class T>
static
T divmod10(T& v)
    // Load the resultant value of dividing the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    T r =  v % 10;
    v /= 10;
    return r;
}

bsls::Types::Uint64 divmod10(Uint128& v)
    // Load the resultant value of dividing the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    // Set a = v.high(), b = v.low().
    // Let Q = (2^64 * a + b) / 10 and R = (2^64 * a + b) % 10.
    // Set a to Q / 2^64, b to Q % 2^64, and return R.
    bsls::Types::Uint64 a  = v.high();
    bsls::Types::Uint64 b  = v.low();

    bsls::Types::Uint64 qa = a / 10;
    bsls::Types::Uint64 ra = a % 10;
    bsls::Types::Uint64 qb = b / 10;
    bsls::Types::Uint64 rb = b % 10;

    v.setHigh(qa);
    v.setLow(qb + 0x1999999999999999ull * ra + (6 * ra + rb) / 10);

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
        divmod10(value);
        ++place;
    } while (value != 0);

    return place;
}

template <class T>
bool isZero(const T& v)
    // Return true if the specified 'v' equals 0, and false otherwise.
{
    return 0 == v;
}

bool isZero(const Uint128& v)
    // Return true if the specified 'v' equals 0, and false otherwise.
{
    return 0 == v.high() && 0 == v.low();
}

template <class T>
int toString(char *b, char *e, T value)
    // Compose a string from the specified 'value' and load the output into a
    // buffer confined by the specified 'b' and 'e' iterators.  Return the
    // total number of written characters.
{
    char *i = e;
    do {
        *--i = static_cast<char>('0' + divmod10(value));
    } while (!isZero(value));

    bsl::copy(i, e, b);

    return static_cast<int>(e - i);
}

template <class SIGNIFICAND>
int pointPosition(SIGNIFICAND significand, int significandLength, int exponent)
    // Return the point position relative to the most significant digit of a
    // decimal value designated by the specified 'significand',
    // 'significandLength' and 'exponent' values.
{
    return isZero(significand) ? 0 : significandLength + exponent;
}

int formatFixed(char       *buffer,
                int         length,
                const char *significand,
                int         significandLength,
                int         exponent,
                int         precision,
                char        point)
    // Format a decimal value designated by the specified 'significand' and
    // 'exponent' values using 'e_FIXED' style and load the output into the
    // specified 'buffer'.  If there is insufficient room in the buffer, its
    // contents will be left in an unspecified state, with the returned
    // value indicating the necessary size.  The behavior is undefined if
    // either 'significandLength' or 'precision' are negative or pointers are
    // null.  The 'buffer' is permitted to be null if the 'length' is not
    // positive.  This can be used to determine the necessary buffer size.
{

    BSLS_ASSERT(buffer || length < 0);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(significandLength >= 0);
    BSLS_ASSERT(precision >= 0);

    int pointPos     = significand[0] != '0' ? significandLength + exponent
                                             : 0;
    int outputLength = static_cast<int>((pointPos > 0 ? pointPos : sizeof('0'))
                                        + (precision ? sizeof(point) : 0)
                                        + precision);

    if (outputLength <= length) {

        char       *oi = buffer;
        char       *oe = buffer + outputLength;
        const char *ii = significand;
        const char *ie = significand + significandLength;

        const char *end = bsl::min(ii + pointPos, ie);
        if (ii < end) {
            oi = bsl::copy(ii, end, oi);
            ii = end;
        }

        while (oi < buffer + pointPos) {
            *oi++ = '0';
        }

        if (0 >= pointPos) {
            *oi++ = '0';
        }

        if (precision) {
            *oi++ = point;

            end = bsl::min(oi - pointPos, oe);
            while (oi < end) {
                *oi++ = '0';
            }

            end = bsl::min(ii + pointPos + precision, ie);
            if (ii < end) {
                oi = bsl::copy(ii, end, oi);
            }
            bsl::fill_n(oi, oe - oi, '0');
        }
    }

    return outputLength;
}

int formatScientific(char       *buffer,
                     int         length,
                     const char *significand,
                     int         significandLength,
                     int         exponent,
                     int         precision,
                     char        point)
    // Format a decimal value designated by the specified 'significand' and
    // 'exponent' values using 'e_SCIENTIFIC' style and load the output into
    // the specified 'buffer'.  If there is insufficient room in the buffer,
    // its contents will be left in an unspecified state, with the returned
    // value indicating the necessary size.  The behavior is undefined if
    // either 'significandLength' or 'precision' are negative or pointers are
    // null.  The 'buffer' is permitted to be null if the 'length' is not
    // positive.  This can be used to determine the necessary buffer size.
{
    BSLS_ASSERT(buffer || length < 0);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(significandLength >= 0);
    BSLS_ASSERT(precision >= 0);

    exponent += significandLength - 1;

    const int k_MAX_EXPONENT_LENGTH = 6;
    char      exp[k_MAX_EXPONENT_LENGTH];
    int       exponentLength = bsl::sprintf(&exp[0], "%+d", exponent);
    int       outputLength   = 1
                               + (precision ? static_cast<int>(sizeof(point))
                                            : 0) + precision
                               + static_cast<int>(sizeof 'E')
                               + exponentLength;
    char     *i              = buffer;

    if (outputLength <= length) {

        const char *j = significand;
        const char *e = significand + significandLength;

        *i++ = *j++;

        if (precision) {
            *i++ = point;

            const char *end = bsl::min(j + precision, e);
            if (j <= end) {
                i = bsl::copy(j, end, i);
                if (end == e) {
                    long int n = bsl::distance(e, j + precision);
                    bsl::fill_n(i, n, '0');
                    i += n;
                }
            }
        }
        *i++ = 'E';
        i    = bsl::copy(&exp[0], &exp[0] + exponentLength, i);
    }

    return outputLength;
}

int formatNatural(char       *buffer,
                  int         length,
                  const char *significand,
                  int         significandLength,
                  int         exponent,
                  int         precision,
                  char        point)
    // Format a decimal value designated by the specified 'significand' and
    // 'exponent' values using 'e_NATURAL' style and load the output into
    // the specified 'buffer'.  If there is insufficient room in the buffer,
    // its contents will be left in an unspecified state, with the returned
    // value indicating the necessary size.  The behavior is undefined if
    // either 'significandLength' or 'precision' are negative or pointers are
    // null.  The 'buffer' is permitted to be null if the 'length' is not
    // positive.  This can be used to determine the necessary buffer size.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(significandLength >= 0);
    BSLS_ASSERT(precision >= 0);

    int len;
    int adjustedExponent = exponent + significandLength - 1;
    if (exponent <= 0 && adjustedExponent >= -6) {

        len = formatFixed(buffer,
                          length,
                          significand,
                          significandLength,
                          exponent,
                          precision,
                          point);
    }
    else {
        len = formatScientific(buffer,
                               length,
                               significand,
                               significandLength,
                               exponent,
                               precision,
                               point);
    }
    return len;
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

Decimal32 _round(Decimal32 value, int exponent)
    // Multiply the specified 'value' by ten raised to the specified
    // 'exponent'.  Round the result to the nearest integral value and return
    // the result of dividing the value by ten raised to the 'exponent'.
{
    Decimal64 v = value;
    v = DecimalUtil::multiplyByPowerOf10(v, exponent);
    v = DecimalUtil::round(v);
    v = DecimalUtil::multiplyByPowerOf10(v, -exponent);
    return Decimal32(v);
}

template <class DECIMAL>
DECIMAL _round(DECIMAL value, int exponent)
    // Multiply the specified 'value' by ten raised to the specified
    // 'exponent'.  Round the result to the nearest integral value and return
    // the result of dividing the value by ten raised to the 'exponent'.
{
    value = DecimalUtil::multiplyByPowerOf10(value, exponent);
    value = DecimalUtil::round(value);
    value = DecimalUtil::multiplyByPowerOf10(value, -exponent);
    return value;
}


bsl::ostream& operator <<(bsl::ostream& s, const Uint128& x)
{
    return s << x.low();
}

template <class DECIMAL>
static
int decompose(char                *buffer,
              int                  length,
              int                 *cls,
              int                 *sign,
              int                 *exponent,
              DecimalFormatConfig  cfg,
              DECIMAL              value)
    // Decompose the specified decimal 'value' into the specified 'sign'
    // significand  and 'exponent' components.  Compose a string from the
    // significand part and load the result into the specified 'buffer'.  If
    // the length of the fractional part of the 'value' exceeds the specified
    // 'precision' then the 'value' is rounded to be as close as possible to
    // the initial value given the constraints on precision.
{
    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    SIGNIFICAND significand;

    *cls = DecimalUtil::decompose(sign,
                                  &significand,
                                  exponent,
                                  value);

    int len = toString(buffer, buffer + length, significand);

    int adjustedExponent  = *exponent + len - 1;

    bool fixedFormat = (DecimalFormatConfig::e_FIXED   == cfg.style()) ||
                       (DecimalFormatConfig::e_NATURAL == cfg.style() &&
                        *exponent <= 0 && adjustedExponent >= -6);

    int pointPos = fixedFormat ? pointPosition(significand,
                                               len,
                                               *exponent)
                               : 1;

    int roundingPos = pointPos + cfg.precision();

    if (0 < roundingPos && roundingPos < len) {

        // bsl::cout << "\tsignificand =" << significand << '\n';
        // bsl::cout << "\texponent =" << *exponent << '\n';
        // bsl::cout << "\tpointPos =" << pointPos << '\n';
        // bsl::cout << "\tlen =" << len << '\n';
        // bsl::cout << "\troundingPos = " << roundingPos << '\n';
        // // value = DecimalUtil::round(value, -(*exponent) - len + roundingPos);
        // value = DecimalUtil::round(value, roundingPos - 1);
        value = _round(value, -(*exponent) - len + roundingPos);
        *cls  = DecimalUtil::decompose(sign,
                                       &significand,
                                       exponent,
                                       value);
        // len = toString(buffer, buffer + length,  significand);
        // bsl::cout << "\tsignificand =" << significand << '\n';
        // bsl::cout << "\texponent =" << *exponent << '\n';
        // bsl::cout << "\tlen =" << len << '\n';
    }
    return len;
}

#if 1
template <class DECIMAL>
static
int formatImpl(char                      *buffer,
               int                        length,
               DECIMAL                    value,
               const DecimalFormatConfig& cfg)
{
    BSLS_ASSERT(buffer || length < 0);
    BSLS_ASSERT(cfg.precision() >= 0);

    const int   k_MAX_SIGNIFICAND_LENGTH = 34;
    char        significand[k_MAX_SIGNIFICAND_LENGTH] = { 0 };
    int         cls;
    int         sign;
    int         exponent;

    int significandLength = decompose(&significand[0],
                                      k_MAX_SIGNIFICAND_LENGTH,
                                      &cls,
                                      &sign,
                                      &exponent,
                                      cfg,
                                      value);

    char  signSymbol('+');
    int   signLength    = formatSign(&signSymbol, sign, cfg.sign());
    int   bufferLength  = length - signLength;
    int   decimalLength = 0;
    char *it            = buffer + signLength;

    switch (cls) {
      case FP_NORMAL:
      case FP_ZERO: {
        switch (cfg.style()) {
          case DecimalFormatConfig::e_SCIENTIFIC: {

              decimalLength = formatScientific(it,
                                               bufferLength,
                                               &significand[0],
                                               significandLength,
                                               exponent,
                                               cfg.precision(),
                                               cfg.decimalPoint());
          } break;
          case DecimalFormatConfig::e_FIXED: {

              decimalLength = formatFixed(it,
                                          bufferLength,
                                          &significand[0],
                                          significandLength,
                                          exponent,
                                          cfg.precision(),
                                          cfg.decimalPoint());
          } break;
          case DecimalFormatConfig::e_NATURAL: {

              decimalLength = formatNatural(it,
                                            bufferLength,
                                            &significand[0],
                                            significandLength,
                                            exponent,
                                            cfg.precision(),
                                            cfg.decimalPoint());
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
          const char *nan = cfg.nan();
          decimalLength = formatSpecial(it,
                                        bufferLength,
                                        nan,
                                        nan + bsl::strlen(nan));
      } break;
      case FP_SUBNORMAL: {
          const char *sNan= cfg.sNan();
          decimalLength = formatSpecial(it,
                                        bufferLength,
                                        sNan,
                                        sNan + bsl::strlen(sNan));
      } break;
    }

    decimalLength += signLength;
    if (decimalLength <= length && signLength) {
       *buffer = signSymbol;
    }

    return decimalLength;
}
#else
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

    int         sign;
    int         exponent;
    SIGNIFICAND s;
    int         cls = DecimalUtil::decompose(&sign,
                                             &s,
                                             &exponent,
                                             value);

    const int k_MAX_SIGNIFICAND_LENGTH = 34;
    char      significand[k_MAX_SIGNIFICAND_LENGTH] = { 0 };
    int       significandLength = toString(significand,
                                           significand + k_MAX_SIGNIFICAND_LENGTH,
                                           s);

    int  adjustedExponent  = exponent + significandLength - 1;
    bool fixedFormat       = (DecimalFormatConfig::e_FIXED   == cfg.style()) ||
                             (DecimalFormatConfig::e_NATURAL == cfg.style() &&
                             exponent <= 0 && adjustedExponent >= -6);
    int pointPos           = fixedFormat ? pointPosition(significand,
                                                         significandLength,
                                                         exponent)
                                           : 1;
    int roundingPos        = pointPos + cfg.precision();


    if (0 < roundingPos && roundingPos < significandLength) {
        // DECIMAL v = DecimalUtil::quantize(value, DecialUtil::makeDecimal - roundingPos);

        // cls = DecimalUtil::decompose(&sign, &s, &exponent, v);

        // significandLength = toString(significand,
        //                              significand + k_MAX_SIGNIFICAND_LENGTH,
        //                              s);
    }

    char  signSymbol('+');
    int   signLength    = formatSign(&signSymbol, sign, cfg.sign());
    int   bufferLength  = length - signLength;
    int   decimalLength = 0;
    char *it            = buffer + signLength;

    switch (cls) {
      case FP_NORMAL:
      case FP_ZERO: {
        switch (cfg.style()) {
          case DecimalFormatConfig::e_SCIENTIFIC: {

              decimalLength = formatScientific(it,
                                               bufferLength,
                                               &significand[0],
                                               significandLength,
                                               exponent,
                                               cfg.precision(),
                                               cfg.decimalPoint());
          } break;
          case DecimalFormatConfig::e_FIXED: {

              decimalLength = formatFixed(it,
                                          bufferLength,
                                          &significand[0],
                                          significandLength,
                                          exponent,
                                          cfg.precision(),
                                          cfg.decimalPoint());
          } break;
          case DecimalFormatConfig::e_NATURAL: {

              decimalLength = formatNatural(it,
                                            bufferLength,
                                            &significand[0],
                                            significandLength,
                                            exponent,
                                            cfg.precision(),
                                            cfg.decimalPoint());
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
          const char *nan = cfg.nan();
          decimalLength = formatSpecial(it,
                                        bufferLength,
                                        nan,
                                        nan + bsl::strlen(nan));
      } break;
      case FP_SUBNORMAL: {
          const char *sNan= cfg.sNan();
          decimalLength = formatSpecial(it,
                                        bufferLength,
                                        sNan,
                                        sNan + bsl::strlen(sNan));
      } break;
    }

    decimalLength += signLength;
    if (decimalLength <= length && signLength) {
       *buffer = signSymbol;
    }

    return decimalLength;
}
#endif

bool isNanString(const char *str) {
    // Return 'true' if the specified 'str' represents a NaN value, and 'false'
    // otherwise.  Note that the IEEE 754 standard specifies sequence of
    // characters equivalent to "NaN" or "sNaN" except for case is a valid
    // representation of NaN.

    // For the sake of efficiency, we rely on the trick that any alphabetic
    // character [a-zA-Z] in ASCII encoding can be bit-wise 'or'ed with '_'
    // (0x20) to get the corresponding lower case character.

    bsl::size_t len = bsl::strlen(str);
    if (len < 3) {
        return false;
    }

    if ((str[0] | ' ') == 's') {
        ++str;
        if (len != 4) {
            return false;
        }
    }
    else if (len != 3) {
        return false;
    }

    return ((str[0] | ' ') == 'n' &&
            (str[1] | ' ') == 'a' &&
            (str[2] | ' ') == 'n');
}

}  // close unnamed namespace


                             // Creator functions


int DecimalUtil::parseDecimal32(Decimal32 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    Decimal32 d = DecimalImpUtil::parse32(str);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return 0;
}

int DecimalUtil::parseDecimal64(Decimal64 *out, const char *str)
{

    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    Decimal64 d = DecimalImpUtil::parse64(str);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return 0;
}

int DecimalUtil::parseDecimal128(Decimal128 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    Decimal128 d = DecimalImpUtil::parse128(str);

    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
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

                                // Formatting functions

void DecimalUtil::format(Decimal32 value, bsl::string *out)
{
    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType32 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType32 dpdValue;
    bsl::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);
    out->assign(buffer);
}

void DecimalUtil::format(Decimal64 value, bsl::string *out)
{
    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType64 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType64 dpdValue;
    bsl::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);
    out->assign(buffer);
}

void DecimalUtil::format(Decimal128 value, bsl::string *out)
{
    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
    DenselyPackedDecimalImpUtil::StorageType128 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType128 dpdValue;
    bsl::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);
    out->assign(buffer);
}
                        // classification functions

int DecimalUtil::classify(Decimal32 x)
{
    return DecimalImpUtil::classify(x.value());
}

int DecimalUtil::classify(Decimal64 x)
{
    return DecimalImpUtil::classify(x.value());
}

int DecimalUtil::classify(Decimal128 x)
{
return DecimalImpUtil::classify(x.value());
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

Decimal32  DecimalUtil::round(Decimal32  x, unsigned int decimalPlaces)
{
    int          sign;
    unsigned int significand;
    int          exponent;
    int          cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(decimalPlaces) < 0))
    {
        x = multiplyByPowerOf10(x, decimalPlaces);
        x = round(x);
        x = multiplyByPowerOf10(x, -decimalPlaces);
    }
    return x;
}

Decimal64 DecimalUtil::round(Decimal64  x, unsigned int decimalPlaces)
{
    int                 sign;
    bsls::Types::Uint64 significand;
    int                 exponent;
    int                 cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(decimalPlaces) < 0))
    {
        x = multiplyByPowerOf10(x, decimalPlaces);
        x = round(x);
        x = multiplyByPowerOf10(x, -decimalPlaces);
    }
    return x;
}

Decimal128 DecimalUtil::round(Decimal128 x, unsigned int decimalPlaces)
{
    int     sign;
    Uint128 significand;
    int     exponent;
    int     cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(decimalPlaces) < 0))
    {
        x = multiplyByPowerOf10(x, decimalPlaces);
        x = round(x);
        x = multiplyByPowerOf10(x, -decimalPlaces);
    }
    return x;
}

                             // Quantum functions

int DecimalUtil::quantum(Decimal32 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));
    int          sign;
    unsigned int significand;
    int          exponent;
    decompose(&sign, &significand, &exponent, value);
    return exponent;
}

int DecimalUtil::quantum(Decimal64 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));
    int                 sign;
    bsls::Types::Uint64 significand;
    int                 exponent;
    decompose(&sign, &significand, &exponent, value);
    return exponent;
}

int DecimalUtil::quantum(Decimal128 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));
    int     sign;
    Uint128 significand;
    int     exponent;
    decompose(&sign, &significand, &exponent, value);
    return exponent;
}

int DecimalUtil::decompose(int          *sign,
                           unsigned int *significand,
                           int          *exponent,
                           Decimal32     value)
{
    return  DecimalImpUtil::decompose(sign,
                                      significand,
                                      exponent,
                                      value.value());
}

int DecimalUtil::decompose(int                 *sign,
                           bsls::Types::Uint64 *significand,
                           int                 *exponent,
                           Decimal64            value)
{
    return  DecimalImpUtil::decompose(sign,
                                      significand,
                                      exponent,
                                      value.value());
}

int DecimalUtil::decompose(int                 *sign,
                           Uint128             *significand,
                           int                 *exponent,
                           Decimal128           value)
{
    return  DecimalImpUtil::decompose(sign,
                                      significand,
                                      exponent,
                                      value.value());

}

int DecimalUtil::format(char                      *buffer,
                        int                        length,
                        Decimal32                  value,
                        const DecimalFormatConfig& config)
{
    return formatImpl(buffer,
                      length,
                      value,
                      config);
}

int DecimalUtil::format(char                      *buffer,
                        int                        length,
                        Decimal64                  value,
                        const DecimalFormatConfig& config)
{
    return formatImpl(buffer,
                      length,
                      value,
                      config);
}

int DecimalUtil::format(char                      *buffer,
                        int                        length,
                        Decimal128                 value,
                        const DecimalFormatConfig& config)
{
    return formatImpl(buffer,
                      length,
                      value,
                      config);
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
