// bdldfp_decimalimputil.cpp                                          -*-C++-*-

#include <bdldfp_decimalimputil.h>

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

} // close anonymous namespace

DecimalImpUtil::ValueType64 DecimalImpUtil::makeInfinity64(bool isNegative)
{
    return DecimalImpUtil::parse64( isNegative ? "-inf" : "inf" );
}

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

    // Note that static_cast<int> is needed to prevent the RHS of the <=
    // comparison from promoting to a signed int. '3 * sizeof(long long)'
    // is at least the number of digits in the longest representable
    // 'long long'.

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

}
}

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
