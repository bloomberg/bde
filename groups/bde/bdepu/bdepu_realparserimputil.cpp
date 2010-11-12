// bdepu_realparserimputil.cpp                                        -*-C++-*-
#include <bdepu_realparserimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_realparserimputil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bdes_bitutil.h>
#include <bsls_platformutil.h>

#include <bsl_c_math.h>

// To multiply high-precision values, the two values are first split into
// high-order and low-order halves and then these halves are multiplied and
// these multiplications summed.  For performance, the lowest order term is
// omitted from the summation.  This omitted term could be used to round the
// result and maintain a higher level of precision.

namespace BloombergLP {

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

enum {
    BDEPU_SUCCESS = 0,
    BDEPU_FAILURE = 1
};

void bdepu_RealParserImpUtil::
       convertBinaryExponentToDecimal(Uint64 *decFrac, int *decExp, int binExp)
{
    BSLS_ASSERT(decFrac);
    BSLS_ASSERT(decExp);

    if (0 == binExp) {  // simple case; 2^0 == 1
        //                 eighteen digit num
        *decFrac = (Uint64)100000000000000000uLL;
        *decExp = 1;
        return;                                                       // RETURN
    }

    // Initialize the result to 1.

    //                   eighteen digit num
    Uint64 res = (Uint64)100000000000000000uLL;
    int    exp = 1;

    Uint64 dF;   // decimal fraction of currently computed 2^bE
    int dE;      // decimal exponent of currently computed 2^bE
    int bE = 1;  // binary exponent currently converted to dF * 10^(dE - 18)

    // Assuming 'binExp' is positive, the desired conversion can be computed by
    // viewing 2^binExp as the product of the conversions of a set of values of
    // the form 2^bE where 'bE' is a power of two.  Given the conversion of
    // 2^bE into decimal form, the conversion of 2^(2 * bE) is trivially
    // computed as the squaring of the decimal representation of 2^bE.  By
    // repeatedly squaring the known conversion and maintaining the
    // appropriate product of intermediate results, the final conversion is
    // computed.  The process must be initialized with the value of 2^1.  Or,
    // if 'binExp' is negative, with 2^-1 and 'binExp' is replaced with
    // '-binExp'.

    // Initialize dF and dE to represent 2^1 or 2^-1 as per above.

    if (binExp > 0) {
        //           eighteen digit num
        dF = (Uint64)200000000000000000uLL;
        dE = 1;
    }
    else {
        binExp = -binExp;
        //           eighteen digit num
        dF = (Uint64)500000000000000000uLL;
        dE = 0;
    }

    // Repeatedly square the intermediate conversion represented by 'dF' and
    // 'dE'.  Maintain the desired product of intermediate conversions in 'res'
    // and 'exp'.

    while (bE <= binExp) {
        if (binExp & bE) {

            // Set result equal to result times intermediate conversion without
            // overflow.

            //               ten digits
            Uint64 a = res / 1000000000uLL;
            Uint64 b = res % 1000000000uLL;
            Uint64 c = dF  / 1000000000uLL;
            Uint64 d = dF  % 1000000000uLL;
            res = a * c + (a * d) / 1000000000uLL + (b * c) / 1000000000uLL;
            exp += dE;

            // Normalize representation.

            while (res < 100000000000000000uLL) {
                res *= 10;
                --exp;
            }
        }
        //              ten digits
        Uint64 a = dF / 1000000000uLL;
        Uint64 b = dF % 1000000000uLL;
        dF = a * a + ((a * b) / 500000000uLL);
        dE *= 2;
        bE = bE < 0x40000000 ? bE * 2 : binExp + 1;  // do not allow overflow

        // Normalize representation.

        while (dF < 100000000000000000uLL) {
            dF *= 10;
            --dE;
        }
    }

    // Store result.
    *decFrac = res;
    *decExp = exp;
}

void bdepu_RealParserImpUtil::
        convertBinaryFractionToDecimalFraction(Uint64 *decFrac, Uint64 binFrac)
{
    BSLS_ASSERT(decFrac);

    Uint64 res = 0;
    //                     eighteen digit num
    Uint64 digitLocation = 100000000000000000uLL;
    const Uint64 max = (Uint64)1 << 60;

    // Drop the lowest order four bits (to prevent overflow when binFrac is
    // multiplied by 10 below); they are 0 if 'binFrac' is from a double,
    // anyway.

    binFrac = binFrac >> 4;

    // Iteratively compute each decimal digit of the result.  Note that since
    // 10 * 2^60 fits in 64 bits, this will not overflow.

    while (binFrac && digitLocation) {
        binFrac *= 10;
        res += (binFrac / max) * digitLocation;
        binFrac %= max;
        digitLocation /= 10;
    }

    // Rounding.

    //                                eighteen digit num
    if (binFrac & (max >> 1) && res < 999999999999999999uLL) {
        ++res;
    }

    *decFrac = res;
}

void bdepu_RealParserImpUtil::convertBinaryToDecimal(Uint64 *decFrac,
                                                     int    *decExp,
                                                     Uint64  binFrac,
                                                     int     binExp)
{
    BSLS_ASSERT(decFrac);
    BSLS_ASSERT(decExp);

    if (0 == binFrac) {  // simple case; zero
        *decFrac = 0;
        *decExp = 0;
        return;                                                       // RETURN
    }

    Uint64 decFrac1;
    Uint64 decFrac2;
    Uint64 res;
    int    exp;

    // Convert the exponential and fraction parts and then multiply these
    // results.

    convertBinaryExponentToDecimal(&decFrac1, &exp, binExp);
    convertBinaryFractionToDecimalFraction(&decFrac2, binFrac);

    // Multiply the two fractional portions without overflowing.

    //                    ten digits
    Uint64 a = decFrac1 / 1000000000uLL;
    Uint64 b = decFrac1 % 1000000000uLL;
    Uint64 c = decFrac2 / 1000000000uLL;
    Uint64 d = decFrac2 % 1000000000uLL;

    res = a * c + (a * d) / 1000000000uLL + (b * c) / 1000000000uLL;

    // Normalize representation.

    //           eighteen digit num
    while (res < 100000000000000000uLL) {
        res *= 10;
        --exp;
    }

    *decFrac = res;
    *decExp = exp;
}

int bdepu_RealParserImpUtil::convertBinaryToDouble(double *value,
                                                   int     binSign,
                                                   Uint64  binFrac,
                                                   int     binExp)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(!!binSign == binSign);

    // Assumes standard (IEEE-754) 'double' format.

    if (binExp > 1023) {  // overflow
        return BDEPU_FAILURE;                                         // RETURN
    }

    if (0 == binFrac || binExp < -1022) {  // simple case; zero or underflow
        *value = 0.0;
        return BDEPU_SUCCESS;                                         // RETURN
    }

    Uint64 tmp;

    int frac = (int)(binFrac & 0x7FF);
    binFrac = binFrac & ~0x7FFuLL;
    //                               sixteen digit num
    if (frac >= 0x400 && binFrac < 0xFFFFFFFFFFFFF800uLL) {
        binFrac += 0x800;
    }

    //                            sixteen digit num
    binFrac = (binFrac >> 11) & 0x000FFFFFFFFFFFFFuLL;
    --binExp;

    tmp = ((Uint64)binSign << 63) | ((Uint64)(binExp + 1023) << 52) | binFrac;
    *value = *((double *)&tmp);
    return BDEPU_SUCCESS;
}

int bdepu_RealParserImpUtil::
       convertDecimalExponentToBinary(Uint64 *binFrac, int *binExp, int decExp)
{
    BSLS_ASSERT(binFrac);
    BSLS_ASSERT(binExp);

    if (0 == decExp) {  // simple case; 10^0 == 1
        //                   sixteen digit num
        *binFrac = (Uint64)0x8000000000000000uLL;
        *binExp = 1;
        return BDEPU_SUCCESS;                                         // RETURN
    }
    if (decExp < -646456992) { // underflow
        *binFrac = 0;
        *binExp = 0;
        return BDEPU_SUCCESS;                                         // RETURN
    }
    if (decExp > 646456992) {  // overflow
        return BDEPU_FAILURE;                                         // RETURN
    }

    const Uint64 max = (Uint64)1 << 63;

    // Initialize the result to 1.

    //                     sixteen digit num
    Uint64 res = (Uint64)0x8000000000000000uLL;
    int    exp = 1;

    Uint64 bF;   // binary fraction of currently computed 10^dE
    int bE;      // binary exponent of currently computed 10^dE
    int dE = 1;  // decimal exponent currently converted to bF * 2^(bE - 64)

    // Assuming 'decExp' is positive, the desired conversion can be computed by
    // viewing 10^decExp as the product of the conversions of a set of values
    // of the form 2^dE where 'dE' is a power of two.  Given the conversion of
    // 2^dE into binary form, the conversion of 2^(2 * dE) is trivially
    // computed as the squaring of the binary representation of 2^dE.  By
    // repeatedly squaring the known conversion and maintaining the
    // appropriate product of intermediate results, the final conversion is
    // computed.  The process must be initialized with the value of 10^1.  Or,
    // if 'decExp' is negative, with 10^-1 and 'decExp' is replaced with
    // '-decExp'.

    // Initialize bF and bE to represent 10^1 or 10^-1 as per above.

    if (decExp > 0) {
        //             sixteen digit num
        bF = (Uint64)0xA000000000000000uLL;
        bE = 4;
    }
    else {
        decExp = -decExp;
        //             sixteen digit num
        bF = (Uint64)0xCCCCCCCCCCCCCCCCuLL;
        bE = -3;
    }

    // Repeatedly square the intermediate conversion represented by 'bF' and
    // 'bE'.  Maintain the desired product of intermediate conversions in 'res'
    // and 'exp'.  Note that there cannot be overflow or underflow due to
    // previous checks.

    while (dE <= decExp) {
        if (decExp & dE) {
            Uint64 a = res >> 32;
            Uint64 b = res & 0xFFFFFFFFuLL;
            Uint64 c = bF >> 32;
            Uint64 d = bF & 0xFFFFFFFFuLL;
            res = a * c + ((a * d) >> 32) + ((b * c) >> 32);
            exp += bE;

            // Normalize representation.

            while (0 == (res & max)) {
                res = res << 1;
                --exp;
            }
        }
        Uint64 a = bF >> 32;
        Uint64 b = bF & 0xFFFFFFFFuLL;
        bF = a * a + ((a * b) >> 31);
        bE *= 2;
        dE *= 2;

        // Normalize representation.

        while (0 == (bF & max)) {
            bF = bF << 1;
            --bE;
        }
    }

    // Store result.
    *binFrac = res;
    *binExp = exp;
    return BDEPU_SUCCESS;
}

void bdepu_RealParserImpUtil::
        convertDecimalFractionToBinaryFraction(Uint64 *binFrac, Uint64 decFrac)
{
    BSLS_ASSERT(binFrac);

    // Iteratively compute each binary digit of the result.  Note that since
    // 2 * 10^18 fits in 61 bits, this will not overflow.

    Uint64 bit = (Uint64)1 << 63;
    Uint64 res = 0;
    while (decFrac && bit) {
        decFrac = decFrac << 1;
        //             nineteen digit num
        if (decFrac >= 1000000000000000000uLL) {
            res |= bit;
            //         nineteen digit num
            decFrac -= 1000000000000000000uLL;
        }
        bit = bit >> 1;
    }

    // Rounding.
    //             eighteen digit num               sixteen digit num
    if (decFrac >= 500000000000000000uLL && res < 0xFFFFFFFFFFFFF800uLL) {
        ++res;
    }

    *binFrac = res;
}

int bdepu_RealParserImpUtil::convertDecimalToBinary(Uint64 *binFrac,
                                                    int    *binExp,
                                                    Uint64  decFrac,
                                                    int     decExp)
{
    BSLS_ASSERT(binFrac);
    BSLS_ASSERT(binExp);

    if (0 == decFrac) {  // simple case; zero
        *binFrac = 0;
        *binExp = 0;
        return BDEPU_SUCCESS;                                         // RETURN
    }

    // Constant powers of 10 used to check whether the normalized 'decFrac'
    // value represents an integer.

    static const Uint64 powersOfTen[] =
    {
        0, // decExp 0 => 10^0 => 1, handled elsewhere.
        100000000000000000uLL,
        10000000000000000uLL,
        1000000000000000uLL,
        100000000000000uLL,
        10000000000000uLL,
        1000000000000uLL,
        100000000000uLL,
        10000000000uLL,
        1000000000uLL,
        100000000uLL,
        10000000uLL,
        1000000uLL,
        100000uLL,
        10000uLL,
        1000uLL,
        100uLL,
        10uLL,
        1uLL,
    };
    const int powerCount = sizeof powersOfTen / sizeof *powersOfTen;

    if (0 < decExp && decExp < powerCount) {
        const Uint64 divisor = powersOfTen[decExp];
        const Uint64 intPart = decFrac / divisor;

        if (intPart * divisor == decFrac) {
            // we have an integer, naively convert to binary then shift.
            // This approach avoids round-trip errors for int->double->int
            // (DRQS 13577520).

            Uint64 bFrac =  intPart;

            int    bExp = 1 + bdes_BitUtil::find1AtLargestIndex64(intPart);

            // Normalize 'bFrac'.

            bFrac <<= 64 - bExp;

            *binFrac = bFrac;
            *binExp  = bExp;

            return BDEPU_SUCCESS;                                     // RETURN
        }
    }

    // We do not have an integer.

    Uint64 binFrac1;
    Uint64 binFrac2;
    Uint64 res;
    int    exp;

    // Convert the exponential and fraction parts and then multiply these
    // results.

    if (convertDecimalExponentToBinary(&binFrac1, &exp, decExp)) {
        return BDEPU_FAILURE;                                         // RETURN
    }
    convertDecimalFractionToBinaryFraction(&binFrac2, decFrac);

    // Multiply the two fractional portions without overflowing.

    Uint64 a = binFrac1 >> 32;
    Uint64 b = binFrac1 & 0xFFFFFFFFuLL;
    Uint64 c = binFrac2 >> 32;
    Uint64 d = binFrac2 & 0xFFFFFFFFuLL;

    res = a * c + ((a * d) >> 32) + ((b * c) >> 32);

    const Uint64 max = (Uint64)1 << 63;

    while (res && 0 == (res & max)) {
        res = res << 1;
        if (-2147483648LL == exp) {  // handle underflow at INT_MIN
            *binFrac = 0;
            *binExp = 0;

            return BDEPU_SUCCESS;                                     // RETURN
        }
        --exp;
    }

    *binFrac = res;
    *binExp = exp;

    return BDEPU_SUCCESS;
}

void bdepu_RealParserImpUtil::convertDoubleToBinary(int    *binSign,
                                                    Uint64 *binFrac,
                                                    int    *binExp,
                                                    double  value)
{
    BSLS_ASSERT(binSign);
    BSLS_ASSERT(binFrac);
    BSLS_ASSERT(binExp);

    if (0.0 == value) {  // simple case; zero
        *binSign = 0;
        *binFrac = 0;
        *binExp = 0;
        return;                                                       // RETURN
    }
    double q = frexp(value, binExp);

    if (q < 0.0) {
        *binSign = 1;
        q = -q;
    }
    else {
        *binSign = 0;
    }

    // Assumes standard (IEEE-754) 'double' format.

    *binFrac = *((Uint64 *)&q);
    //            sixteen digit num
    *binFrac &= 0x000FFFFFFFFFFFFFuLL;
    *binFrac |= 0x0010000000000000uLL;
    *binFrac = *binFrac << 11;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
