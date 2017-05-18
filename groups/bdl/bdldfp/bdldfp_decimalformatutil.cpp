// bdldfp_decimalformatutil.cpp                                       -*-C++-*-
#include <bdldfp_decimalformatutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalformatutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalutil.h>
#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bsl_cstdio.h>

#include <bsl_iostream.h>

namespace BloombergLP {
namespace bdldfp {

namespace {

template <class DECIMAL>
struct DecimalTraits;

template <>
struct DecimalTraits<Decimal32>
{
    typedef unsigned int Significand;
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

int formatSign(char *buffer,
               int length,
               int sign,
               DecimalFormatUtil::Sign& signOutput)
    // Format the specified 'sign' according to the specified 'signOutput'
    // type and load the result into the specified 'buffer'.  Return the
    // length of the formatted sign.  The behavior is undefined if
    // 'length < 1'.
{
    BSLS_ASSERT(length >= 1);

    int len = 1;
    if (sign < 0) {
        *buffer = '-';
    }
    else {
      switch (signOutput) {
        case DecimalFormatUtil::e_ALWAYS: {
          *buffer = '+';
        } break;
        case DecimalFormatUtil::e_POSITIVE_SPACE: {
          *buffer = ' ';
        } break;
        case DecimalFormatUtil::e_NEGATIVE_ONLY: {
          len = 0;
        } break;
        default: {
          BSLS_ASSERT(!"Unexpected sign value");
        }
      }
    }
    return len;
}

template <class T>
T divmod10(T& v)
    // Load the resultant value of division the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    T r =  v % 10;
    v /= 10;
    return r;
}

bsls::Types::Uint64 divmod10(Uint128& v)
    // Load the resultant value of division the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    // Set a = v.high(), b = v.low().
    // Let Q = (2^64 * a + b) / 10 and R = (2^64 * a + b) % 10.
    // Set a to Q / 2^64, b to Q % 2^64, and return R.
    bsls::Types::Uint64 a = v.high();
    bsls::Types::Uint64 b = v.low();
    bsls::Types::Uint64 r = 0;
    if (a) {
        bsls::Types::Uint64 qa = a / 10;
        bsls::Types::Uint64 ra = a % 10;
        bsls::Types::Uint64 qb = b / 10;
        bsls::Types::Uint64 rb = b % 10;
        v.setHigh(qa);
        v.setLow(qb + 1844674407370955161ull * ra + (6 * ra + rb) / 10);
        return (6 * ra + rb) % 10;
    }
    else {
        r = divmod10(b);
        v.setLow(b);
        return r;
    }
}

template <class SIGNIFICAND>
int parseSignificand(int *remainders, SIGNIFICAND significand)
    // Divide the specified 'significand' value by 10 unless it equals 0 and
    // load the result of each division into the specified buffer 'reminders'.
    // Note that the 'reminders' will represent the 'significand' in
    // 'little-endian' format, i.e. the least significand digit will be stored
    // first.  Return the length of resultant buffer.
{
    int *begin = remainders;
    while (significand > 0) {
        *remainders++ = divmod10(significand);
    }
    if (begin == remainders) {
        *remainders = 0;
        return 1;
    }
    return remainders - begin;
}

int parseSignificand(int *remainders, Uint128 significand)
    // Divide the specified 'significand' value by 10 unless it equals 0 and
    // load the result of each division into the specified buffer 'reminders'.
    // Note that the 'reminders' will represent the 'significand' in
    // 'little-endian' format, i.e. the least significand digit will be stored
    // first.  Return the length of resultant buffer.
{
    int *begin = remainders;
    while (significand.high() > 0 || significand.low() > 0) {
        *remainders++ = divmod10(significand);
    }
    if (begin == remainders) {
        *remainders = 0;
        return 1;
    }
    return remainders - begin;
}

template <class SIGNIFICAND>
int pointPosition(SIGNIFICAND significand, int significandLength, int exponent)
    // Return the point position relatively the most significand digit of a
    // decimal value designated by the specified 'significand',
    // 'significandLength' and 'exponent' values.
{
    return significand ? significandLength + exponent : 0;
}

int pointPosition(Uint128 significand, int significandLength, int exponent)
    // Return the point position relatively the most significand digit of a
    // decimal value designated by the specified 'significand',
    // 'significandLength' and 'exponent' values.
{
    return (significand.high() || significand.low())
           ? significandLength + exponent
           : 0;
}

template <class SIGNIFICAND>
int printFixed(char        *buffer,
               int          length,
               SIGNIFICAND  significand,
               int          exponent,
               int          precision,
               char         point)
{
    // Format a decimal value designated by the specified 'significand' and
    // 'exponent' values using 'e_FIXED' style and load the output into the
    // specified 'buffer'.  If there is insufficient room in the buffer, its
    // contents will be left in an unspecified state, with the returned
    // value indicating the necessary size.  The behavior is undefined if
    // either 'significand' or 'length' or 'precision' is negative or
    // 'buffer == 0'.

    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(precision >= 0);
    // BSLS_ASSERT(significand >= 0);

    const int k_MAX_SIGNIFICAND_LENGTH = 34;
    int reminders[k_MAX_SIGNIFICAND_LENGTH];

    int significandLength = parseSignificand(&reminders[0], significand);
    int pointPos          = pointPosition(significand,
                                          significandLength,
                                          exponent);
    int outputLength      = (pointPos > 0 ? pointPos : sizeof('0'))
                            + (precision ? sizeof('.') : 0) + precision;

    if (outputLength <= length) {

        int i    = 0;
        int j    = significandLength - 1;
        int stop = bsl::min(j + 1, pointPos);

        for (; i < stop; ++i, --j) {
            buffer[i] = '0' + reminders[j];
        }

        for (; i < pointPos; ++i) {
            buffer[i] = '0';
        }

        if (0 >= pointPos) {
            buffer[i++] = '0';
        }

        if (precision) {
            buffer[i++] = '.';

            stop = bsl::min(i - pointPos, outputLength);
            for (; i < stop; ++i) {
                buffer[i] = '0';
            }

            stop = bsl::min(i + j + 1, outputLength);
            for (; i < stop; ++i, --j) {
                buffer[i] = '0' + reminders[j];
            }

            for (; i < outputLength; ++i) {
                buffer[i] = '0';
            }
        }
    }

    return outputLength;
}

template <class SIGNIFICAND>
int printScientific(char        *buffer,
                    int          length,
                    SIGNIFICAND  significand,
                    int          exponent,
                    int          precision,
                    char         point)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(precision >= 0);

    return 0;

#if 0

    int significandLength = decimalLength(significand);
    exponent              = significandLength - 1 + exponent;
    int exponentLength    = decimalLength(exponent);
    int decimalLen        = 1
                            + (precision ? sizeof('.') : 0) + precision
                            + 2
                            + exponentLength;

    if (decimalLen <= length) {

        bsl::fill_n(buffer, decimalLen, '0');

        char *it = buffer;

        if (precision < significandLength - 1) {
            significand /= pow10(significandLength - 1
                                 - precision);
            significandLength = precision + 1;
        }

        it += decimalLen - 1;

        for (char *end = it - exponentLength; it != end; --it) {
            *it = '0' + exponent % 10;
            exponent /= 10;
        }
        *it-- = (exponent >= 0 ? '+' : '-');
        *it-- = 'E';

        if (precision) {
            for (char *end = buffer + 1; it > end; --it) {
                *it = '0' + significand % 10;
                significand /= 10;
            }
            *it-- = point;
        }
        *it = '0' + significand;
    }

    return decimalLen;
#endif
}

template <class DECIMAL>
int formatImpl(char                       *buffer,
               int                         length,
               DECIMAL                     value,
               int                         precision,
               DecimalFormatUtil::Style    style,
               DecimalFormatUtil::Sign     signFormat,
               DecimalFormatUtil::Letters  letters,
               char                        point)
{
    BSLS_ASSERT(buffer || length < 0);
    BSLS_ASSERT(precision >= 0);

    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    int         sign;
    SIGNIFICAND significand;
    int         exponent;

    int cls = DecimalUtil::decompose(&sign, &significand, &exponent, value);

    char  signSymbol;
    int   signLength    = formatSign(&signSymbol, 1, sign, signFormat);
    int   bufferLength  = length - signLength;
    int   decimalLength = 0;
    char *it            = buffer;

    it += signLength;

    if (FP_NORMAL == cls || FP_ZERO == cls) {

        switch (style) {
          case DecimalFormatUtil::e_SCIENTIFIC: {
              decimalLength = printScientific(it,
                                              bufferLength,
                                              significand,
                                              exponent,
                                              precision,
                                              point);
          } break;
          case DecimalFormatUtil::e_FIXED: {
              decimalLength = printFixed(it,
                                         bufferLength,
                                         significand,
                                         exponent,
                                         precision,
                                         point);
          } break;
          default: {
            BSLS_ASSERT(!"Unexpected format style value");
          } break;
        }
    }

    decimalLength += signLength;
    if (decimalLength <= length && signLength) {
        *buffer = signSymbol;
    }

    return decimalLength;
}

}  // close unnamed namespace

int DecimalFormatUtil::format(char       *buffer,
                              int         length,
                              Decimal32   value,
                              int         precision,
                              Style       style,
                              Sign        sign,
                              Letters     letters,
                              char        point)
{
    return formatImpl(buffer,
                      length,
                      value,
                      precision,
                      style,
                      sign,
                      letters,
                      point);
}

int DecimalFormatUtil::format(char       *buffer,
                              int         length,
                              Decimal64   value,
                              int         precision,
                              Style       style,
                              Sign        sign,
                              Letters     letters,
                              char        point)
{
    return 0;
}

int DecimalFormatUtil::format(char       *buffer,
                              int         length,
                              Decimal128  value,
                              int         precision,
                              Style       style,
                              Sign        sign,
                              Letters     letters,
                              char        point)
{
    return formatImpl(buffer,
                      length,
                      value,
                      precision,
                      style,
                      sign,
                      letters,
                      point);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
