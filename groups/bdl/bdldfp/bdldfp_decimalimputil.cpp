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
