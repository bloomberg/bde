// bdldfp_decimalformatutil.cpp                                       -*-C++-*-
#include <bdldfp_decimalformatutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalformatutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalutil.h>

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

template <class SIGNIFICAND>
int decimalLength(SIGNIFICAND significand)
    // Compute the length of decimal presentations of the specified
    // 'significand' value.
{
    int len = 1;
    significand /= 10;
    while (significand) {
        significand /= 10;
        ++len;
    }
    return len;
}

template <class SIGNIFICAND>
SIGNIFICAND powOf10(int exp)
    // Compute the value of 10 raised to the specified power 'exp'.  The
    // behavior is undefined unless 'exp >= 0'.
{
    BSLS_ASSERT(exp >= 0);

    SIGNIFICAND res = 1;
    while (exp) {
        res *= 10;
        --exp;
    }
    return res;
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
    BSLS_ASSERT(significand >= 0);

    int significandLength = decimalLength(significand);
    int pointPos          = significand ? significandLength + exponent : 0;
    int decimalLen        = (pointPos > 0 ? pointPos : sizeof('0'))
                            + (precision ? sizeof('.') : 0) + precision;

    if (decimalLen <= length) {

        bsl::fill_n(buffer, decimalLen, '0');

        char *it                 = buffer;
        int   lessSignificandPos = pointPos + precision;

        if (lessSignificandPos <= 0) {
            *++it = point;
            return decimalLen;
        }
        else if (lessSignificandPos < significandLength) {
            significand /= powOf10<SIGNIFICAND>(significandLength
                                                - lessSignificandPos);
            significandLength = lessSignificandPos;
        }


        if (pointPos <= 0) {
            if (precision) {
                *++it  = point;
                it    += (-pointPos + significandLength);

                for (char *end = it - significandLength; it != end; --it) {
                    *it = '0' + significand % 10;
                    significand /= 10;
                }
            }
        }
        else {
            it += significandLength;

            if (precision) {
                for (char *end = buffer + pointPos; it > end; --it) {
                    *it = '0' + significand % 10;
                    significand /= 10;
                }
                *it = point;
            }
            --it;

            for (; it != buffer; --it) {
                *it = '0' + significand % 10;
                significand /= 10;
            }
            *it = '0' + significand;
        }
    }

    return decimalLen;
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
            significand /= powOf10<SIGNIFICAND>(significandLength - 1
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
    return 0;
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
