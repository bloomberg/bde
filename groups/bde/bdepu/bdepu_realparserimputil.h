// bdepu_realparserimputil.h       -*-C++-*-
#ifndef INCLUDED_BDEPU_REALPARSERIMPUTIL
#define INCLUDED_BDEPU_REALPARSERIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Supply high-precision floating point number conversions.
//
//@CLASSES:
//   bdepu_RealParserImpUtil: high-precision floating-point number conversions
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides conversion operations between a
// high-precision decimal representation of a floating-point number and a
// high-precision binary representation.  Also provided is conversion between
// the binary representation and the standard (IEEE-754) double format.  Note
// that all conversions except standard double format to binary are potentially
// lossy.
//
// The decimal representation is stored as a sign ('decSign'), a mantissa
// ('decFrac'), and an exponent ('decExp').  The value of the real number is
// (-1)^decSign * decFrac / 10^18 * 10^decExp.  Furthermore, 'decFrac' is
// required to be greater than or equal to 10^17 and less than 10^18.
//
// The binary representation is stored as a sign ('binSign' which is either 0
// or 1), a mantissa ('binFrac'), and an exponent ('binExp').  The value of the
// real number is (-1)^binSign * binFrac / 2^64 * 2^decExp.  Furthermore,
// 'binFrac' is required to be greater than or equal to 2^63 and less than
// 2^64.
//
///USAGE
///-----
// First, create a double to be converted into the binary representation:
//..
//      double initial = 3.1415;
//..
// Second, do the conversion:
//..
//      int                       binSign;
//      bsls_PlatformUtil::Uint64 binFrac;
//      int                       binExp;
//      bdepu_RealParserImpUtil::
//                 convertDoubleToBinary(&binSign, &binFrac, &binExp, initial);
//..
// Third, convert from binary representation to decimal representation:
//..
//      bsls_PlatformUtil::Uint64 decFrac;
//      int                       decExp;
//      bdepu_RealParserImpUtil::
//                  convertBinaryToDecimal(&decFrac, &decExp, binFrac, binExp);
//..
// Fourth, convert back to binary from the decimal representation:
//..
//      bdepu_RealParserImpUtil::
//                  convertDecimalToBinary(&binFrac, &binExp, decFrac, decExp);
//..
// Finally, convert back to a double and compare to the initial value:
//..
//      double result;
//      bdepu_RealParserImpUtil::
//                 convertBinaryToDouble(&result, binSign, binFrac, binExp);
//      assert(initial == result);
//..
// Note that obtaining the exact initial value is in general not guaranteed.



#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif




namespace BloombergLP {



                        // =======================
                        // bdepu_RealParserImpUtil
                        // =======================

struct bdepu_RealParserImpUtil {
    // This 'struct' provides a namespace for conversion methods between a
    // high-precision decimal representation of floating-point numbers and a
    // high-precision binary representation.  Also provided are conversion
    // methods between the binary representation and the standard (IEEE-754)
    // 'double' format.

    // PUBLIC METHODS
    static
    void convertBinaryExponentToDecimal(bsls_PlatformUtil::Uint64 *decFrac,
                                        int                       *decExp,
                                        int                        binExp);
        // Load into the specified 'decFrac' and 'decExp' the conversion from
        // the specified binary representation 'binExp' to decimal
        // representation.

    static
    void convertBinaryFractionToDecimalFraction(
                                           bsls_PlatformUtil::Uint64 *decFrac,
                                           bsls_PlatformUtil::Uint64  binFrac);
        // Load into the specified 'decFrac' the conversion from the specified
        // binary representation 'binFrac' to decimal representation.

    static
    void convertBinaryToDecimal(bsls_PlatformUtil::Uint64 *decFrac,
                                int                       *decExp,
                                bsls_PlatformUtil::Uint64  binFrac,
                                int                        binExp);
        // Load into the specified 'decFrac' and 'decExp' the conversion from
        // the specified binary representation 'binFrac' and 'binExp' to
        // decimal representation.

    static
    int convertBinaryToDouble(double                    *value,
                              int                        binSign,
                              bsls_PlatformUtil::Uint64  binFrac,
                              int                        binExp);
        // Load into the specified 'value' the conversion from the specified
        // binary representation 'binSign', 'binFrac', and 'binExp' to the
        // standard (IEEE-754) 'double' representation.  Return 0 on success,
        // and a non-zero value (with no effect on 'value') if the conversion
        // would overflow the 'double'.  The behavior is undefined unless
        // 0 <= binSign <= 1.

    static
    int convertDecimalExponentToBinary(bsls_PlatformUtil::Uint64 *binFrac,
                                       int                       *binExp,
                                       int                        decExp);
        // Load into the specified 'binFrac' and 'binExp' the conversion from
        // the specified decimal representation 'decExp' to binary
        // representation.  Return 0 on success and a non-zero value (with no
        // effect on 'binFrac' or 'binExp') if the conversion is not possible
        // (i.e., if 'binExp' would not be a valid integer).

    static
    void convertDecimalFractionToBinaryFraction(
                                           bsls_PlatformUtil::Uint64 *binFrac,
                                           bsls_PlatformUtil::Uint64  decFrac);
        // Load into the specified 'binFrac' the conversion from the specified
        // decimal representation 'decFrac' to binary representation.

    static
    int convertDecimalToBinary(bsls_PlatformUtil::Uint64 *binFrac,
                               int                       *binExp,
                               bsls_PlatformUtil::Uint64  decFrac,
                               int                        decExp);
        // Load into the specified 'binFrac' and 'binExp' the conversion from
        // the specified decimal representation 'decFrac' and 'decExp' to
        // binary representation.  Return 0 on success and a non-zero value
        // (with no effect on 'binFrac' or 'binExp') if the conversion is not
        // possible (i.e., if 'binExp' would not be a valid integer).

    static
    void convertDoubleToBinary(int                       *binSign,
                               bsls_PlatformUtil::Uint64 *binFrac,
                               int                       *binExp,
                               double                     value);
        // Load into the specified 'binSign', 'binFrac', and 'binExp' the
        // conversion from the specified standard (IEEE-754) 'double' 'value'
        // to binary representation.
};



// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================



}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
