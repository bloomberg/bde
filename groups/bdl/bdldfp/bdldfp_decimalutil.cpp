// bdldfp_decimalutil.cpp                                             -*-C++-*-
#include <bdldfp_decimalutil.h>

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

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
#include <bdldfp_intelimpwrapper.h>
#endif

namespace BloombergLP {
namespace bdldfp {


namespace {

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

int DecimalUtil::parseDecimal32Exact(Decimal32 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    unsigned int status = 0;

    Decimal32 d = DecimalImpUtil::parse32(str, &status);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return status;
}

int DecimalUtil::parseDecimal64Exact(Decimal64 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    unsigned int status = 0;

    Decimal64 d = DecimalImpUtil::parse64(str, &status);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return status;
}

int DecimalUtil::parseDecimal128Exact(Decimal128 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    unsigned int status = 0;

    Decimal128 d = DecimalImpUtil::parse128(str, &status);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return status;
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
    return DecimalImpUtil::format(buffer,
                                  length,
                                  *value.data(),
                                  config);
}

int DecimalUtil::format(char                      *buffer,
                        int                        length,
                        Decimal64                  value,
                        const DecimalFormatConfig& config)
{
    return DecimalImpUtil::format(buffer,
                                  length,
                                  *value.data(),
                                  config);
}

int DecimalUtil::format(char                      *buffer,
                        int                        length,
                        Decimal128                 value,
                        const DecimalFormatConfig& config)
{
    return DecimalImpUtil::format(buffer,
                                  length,
                                  *value.data(),
                                  config);
}

Decimal32  DecimalUtil::trunc(Decimal32  x, unsigned int precision)
{
    int          sign;
    unsigned int significand;
    int          exponent;
    int          cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(precision) < 0))
    {
        x = multiplyByPowerOf10(x, precision);
        x = trunc(x);
        x = multiplyByPowerOf10(x, -precision);
    }
    return x;
}

Decimal64 DecimalUtil::trunc(Decimal64  x, unsigned int precision)
{
    int                 sign;
    bsls::Types::Uint64 significand;
    int                 exponent;
    int                 cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(precision) < 0))
    {
        x = multiplyByPowerOf10(x, precision);
        x = trunc(x);
        x = multiplyByPowerOf10(x, -precision);
    }
    return x;
}

Decimal128 DecimalUtil::trunc(Decimal128 x, unsigned int precision)
{
    int     sign;
    Uint128 significand;
    int     exponent;
    int     cl = decompose(&sign, &significand, &exponent, x);

    if ((FP_NORMAL == cl || FP_SUBNORMAL == cl) &&
        (exponent + static_cast<int>(precision) < 0))
    {
        x = multiplyByPowerOf10(x, precision);
        x = trunc(x);
        x = multiplyByPowerOf10(x, -precision);
    }
    return x;
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
