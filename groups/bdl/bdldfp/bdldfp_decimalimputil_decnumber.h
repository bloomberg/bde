// bdldfp_decimalimputil_inteldfp.h                                           -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_DECNUMBER
#define INCLUDED_BDLDFP_DECIMALIMPUTIL_DECNUMBER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utility implementing decimal FP  with decNumber library.
//
//@CLASSES:
//  bdldfp::DecimalImpUtil_IntelDFP: Namespace for Intel decimal FP functions
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION:
// This component is for internal use only by the 'bdldfp_decimal*' components.
// Direct use of any names declared in this component by any other code invokes
// undefined behavior.  In other words: this code may change, disappear, break,
// move without notice, and no support whatsoever will ever be provided for it.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: None
///- - - - - - - -
//..
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BDLDFP_BUFFERBUF
#include <bdldfp_bufferbuf.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_LOCALE
#include <bsl_locale.h>
#endif

#ifndef INCLUDED_BDLDFP_DENSELYPACKEDECIMALIMPUTIL
#include <bdldfp_denselypackeddecimalimputil.h>
#endif

#if BDLDFP_DECIMALPLATFORM_DECNUMBER 

#error DO NOT WANT DECNUMBER

# ifndef INCLUDED_DECSINGLE
 extern "C" {
#  include <decSingle.h>
}
#  define INCLUDED_DECSINGLE
# endif


namespace BloombergLP {
namespace bdldfp {

                          // ===============================
                          // class DecimalImplUtil_decNumber
                          // ===============================

struct DecimalImpUtil_decNumber {
    // This 'struct' provides a namespace for implementation functions that
    // work in terms of the underlying C-style decimal floating point
    // implementation, Intel's DFP library.

    // TYPES
    
    typedef decSingle ValueType32;
    typedef decDouble ValueType64;
    typedef decQuad   ValueType128;

    static decContext *getDecNumberContext();
        // Return a pointer providing modifiable access to the floating point
        // environment of the 'decNumber' library.  This function exists on
        // certain supported platforms only.


    struct DecimalTriple {
        bool               sign;  // 'true' if negative, 'false' if positive.
        unsigned long long mantissa;
        int                exponent;
    };


    // CLASS METHODS

                        // compose and decompose

    static ValueType32  composeDecimal32 (DecimalTriple triple);
    static ValueType64  composeDecimal64 (DecimalTriple triple);
    static ValueType128 composeDecimal128(DecimalTriple triple);
        // Return a 'ValueTypeXX' number having the value as specified by the
        // salient attributes of the specified 'triple'.  The behavior is
        // undefined if the 'mantissa' has too many decimal digits for
        // 'ValueType', or the 'exponent' is too large for 'ValueType'

    static DecimalTriple decomposeDecimal(ValueType32  value);
    static DecimalTriple decomposeDecimal(ValueType64  value);
    static DecimalTriple decomposeDecimal(ValueType128 value);
        // Return a 'DecimalTriple' object representing the salient attributes
        // of the specified 'value'.  The behavior is undefined, unless 'value'
        // is neither 'NaN' nor 'Inf'.

                        // Integer construction

    static ValueType32  int32ToDecimal32 (int value);
    static ValueType64  int32ToDecimal64 (int value);
    static ValueType128 int32ToDecimal128(int value);

    static ValueType32  uint32ToDecimal32 (unsigned int value);
    static ValueType64  uint32ToDecimal64 (unsigned int value);
    static ValueType128 uint32ToDecimal128(unsigned int value);

    static ValueType32  int64ToDecimal32 (long long int value);
    static ValueType64  int64ToDecimal64 (long long int value);
    static ValueType128 int64ToDecimal128(long long int value);

    static ValueType32  uint64ToDecimal32 (unsigned long long int value);
    static ValueType64  uint64ToDecimal64 (unsigned long long int value);
    static ValueType128 uint64ToDecimal128(unsigned long long int value);

                        // Arithmetic

    static ValueType64  add(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 add(ValueType128 lhs,  ValueType128 rhs);

    static ValueType64  sub(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 sub(ValueType128 lhs,  ValueType128 rhs);

    static ValueType64  mul(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 mul(ValueType128 lhs,  ValueType128 rhs);

    static ValueType64  div(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 div(ValueType128 lhs,  ValueType128 rhs);

    static ValueType32  neg(ValueType32  value);
    static ValueType64  neg(ValueType64  value);
    static ValueType128 neg(ValueType128 value);

                        // Comparison

    static bool less(ValueType32  lhs, ValueType32  rhs);
    static bool less(ValueType64  lhs, ValueType64  rhs);
    static bool less(ValueType128 lhs, ValueType128 rhs);

    static bool greater(ValueType32  lhs, ValueType32  rhs);
    static bool greater(ValueType64  lhs, ValueType64  rhs);
    static bool greater(ValueType128 lhs, ValueType128 rhs);

    static bool lessEqual(ValueType32  lhs, ValueType32  rhs);
    static bool lessEqual(ValueType64  lhs, ValueType64  rhs);
    static bool lessEqual(ValueType128 lhs, ValueType128 rhs);

    static bool greaterEqual(ValueType32  lhs, ValueType32  rhs);
    static bool greaterEqual(ValueType64  lhs, ValueType64  rhs);
    static bool greaterEqual(ValueType128 lhs, ValueType128 rhs);

    static bool equal(ValueType32  lhs, ValueType32  rhs);
    static bool equal(ValueType64  lhs, ValueType64  rhs);
    static bool equal(ValueType128 lhs, ValueType128 rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' have the same value,
        // and 'false' otherwise.  Two decimal objects have the same value if
        // the 'compareQuietEqual' operation (IEEE-754 defined, non-total
        // ordering comparison) considers the underlying IEEE representations
        // equal.  In other words, two decimal objects have the same value if:
        //
        //: o both have a zero value (positive or negative), or
        //:
        //: o both have the same infinity value (both positive or negative), or
        //:
        //: o both have the value of a real number that are equal, even if they
        //:   are represented differently (cohorts have the same value)
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

    static bool notEqual(ValueType32  lhs, ValueType32  rhs);
    static bool notEqual(ValueType64  lhs, ValueType64  rhs);
    static bool notEqual(ValueType128 lhs, ValueType128 rhs);
        // Return 'false' if the specified 'lhs' and 'rhs' have the same value,
        // and 'true' otherwise.  Two decimal objects have the same value if
        // the 'compareQuietEqual' operation (IEEE-754 defined, non-total
        // ordering comparison) considers the underlying IEEE representations
        // equal.  In other words, two decimal objects have the same value if:
        //
        //: o both have a zero value (positive or negative), or
        //:
        //: o both have the same infinity value (both positive or negative), or
        //:
        //: o both have the value of a real number that are equal, even if they
        //:   are represented differently (cohorts have the same value)
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

    static ValueType32  convertToDecimal32 (const ValueType64&  input);
    static ValueType64  convertToDecimal64 (const ValueType32&  input);
    static ValueType64  convertToDecimal64 (const ValueType128& input);
    static ValueType128 convertToDecimal128(const ValueType32&  input);
    static ValueType128 convertToDecimal128(const ValueType64&  input);
        // Convert the specified 'input' to the indicated result type.  Note
        // that a conversion from 'ValueType128' to 'ValueType32' is not
        // provided (because such a conversion is not provided by the
        // 'decNumber' library).  A conversion from 128-bit to 32-bit
        // representations is *not* identical to the composing the conversions
        // from 128-bit to 64-bit, and 64-bit to 32-bit representations,
        // because rounding should only be performed once.

    static ValueType32  binaryToDecimal32(      float input);
    static ValueType32  binaryToDecimal32(     double input);
    static ValueType32  binaryToDecimal32(long double input);

    static ValueType64  binaryToDecimal64(      float input);
    static ValueType64  binaryToDecimal64(     double input);
    static ValueType64  binaryToDecimal64(long double input);

    static ValueType128 binaryToDecimal128(      float input);
    static ValueType128 binaryToDecimal128(     double input);
    static ValueType128 binaryToDecimal128(long double input);

    static ValueType32  makeDecimalRaw32(int mantissa, int exponent);
        // Create a 'ValueType32' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by 'mantissa'.  The behavior is undefined unless
        // 'abs(mantissa) <= 9,999,999' and '-101 <= exponent <= 90'.

    static ValueType64 makeDecimalRaw64(unsigned long long mantissa,
                                        int                exponent);
    static ValueType64 makeDecimalRaw64(long long          mantissa,
                                        int                exponent);
    static ValueType64 makeDecimalRaw64(unsigned int       mantissa,
                                        int                exponent);
    static ValueType64 makeDecimalRaw64(int                mantissa,
                                        int                exponent);
        // Create a 'ValueType64' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by 'mantissa'.  The behavior is undefined unless
        // 'abs(mantissa) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

    static ValueType128 makeDecimalRaw128(unsigned long long mantissa,
                                          int                exponent);
    static ValueType128 makeDecimalRaw128(long long          mantissa,
                                          int                exponent);
    static ValueType128 makeDecimalRaw128(unsigned int       mantissa,
                                          int                exponent);
    static ValueType128 makeDecimalRaw128(int                mantissa,
                                          int                exponent);
        // Create a 'ValueType128' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by 'mantissa'.  The behavior is undefined unless
        // '-6176 <= exponent <= 6111'.

    static ValueType32 scaleB(ValueType32 value, int power);
    static ValueType64 scaleB(ValueType64 value, int power);
    static ValueType128 scaleB(ValueType128 value, int power);


    static ValueType32  parse32 (const char *string);
    static ValueType64  parse64 (const char *string);
    static ValueType128 parse128(const char *string);
};

inline decContext *DecimalImpUtil_decNumber::getDecNumberContext()
{
    static decContext context = { 0, 0, 0, DEC_ROUND_HALF_EVEN, 0, 0, 0 };
    return &context;
}


typedef DecimalImpUtil_decNumber DecimalImpUtil_Platform;






    // Inline functions

                        // Integer construction

inline
DecimalImpUtil_Platform::ValueType32
DecimalImpUtil_Platform::int32ToDecimal32(int value)
{
    DecimalImpUtil_Platform::ValueType32 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decSingleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::int32ToDecimal64(int value)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decDoubleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::int32ToDecimal128(int value)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decQuadFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}


inline
DecimalImpUtil_Platform::ValueType32
DecimalImpUtil_Platform::uint32ToDecimal32(unsigned int value)
{
    DecimalImpUtil_Platform::ValueType32 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decSingleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::uint32ToDecimal64(unsigned int value)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decDoubleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::uint32ToDecimal128(unsigned int value)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decQuadFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}


inline
DecimalImpUtil_Platform::ValueType32 DecimalImpUtil_Platform::int64ToDecimal32(
                                                           long long int value)
{
    DecimalImpUtil_Platform::ValueType32 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decSingleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64 DecimalImpUtil_Platform::int64ToDecimal64(
                                                           long long int value)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decDoubleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType128 DecimalImpUtil_Platform::int64ToDecimal128(
                                                           long long int value)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decQuadFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}


inline
DecimalImpUtil_Platform::ValueType32 DecimalImpUtil_Platform::uint64ToDecimal32(
                                                  unsigned long long int value)
{
    DecimalImpUtil_Platform::ValueType32 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decSingleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64 DecimalImpUtil_Platform::uint64ToDecimal64(
                                                  unsigned long long int value)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decDoubleFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType128 DecimalImpUtil_Platform::uint64ToDecimal128(
                                                   unsigned long long int value)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    // TODO: TBD we should not convert through strings - it should be possible
    // to convert directly
    BufferBuf<24> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out << value;
    decQuadFromString(&retval, bb.str(), getDecNumberContext());
    return retval;
}

                        // Arithmetic

inline DecimalImpUtil_Platform::ValueType64  DecimalImpUtil_Platform::add(
                                               DecimalImpUtil_Platform::ValueType64 lhs,
                                               DecimalImpUtil_Platform::ValueType64 rhs)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decDoubleAdd(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}

inline DecimalImpUtil_Platform::ValueType128  DecimalImpUtil_Platform::add(
                                              DecimalImpUtil_Platform::ValueType128 lhs,
                                              DecimalImpUtil_Platform::ValueType128 rhs)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    decQuadAdd(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}



inline DecimalImpUtil_Platform::ValueType64  DecimalImpUtil_Platform::sub(
                                               DecimalImpUtil_Platform::ValueType64 lhs,
                                               DecimalImpUtil_Platform::ValueType64 rhs)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decDoubleSubtract(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}

inline DecimalImpUtil_Platform::ValueType128  DecimalImpUtil_Platform::sub(
                                              DecimalImpUtil_Platform::ValueType128 lhs,
                                              DecimalImpUtil_Platform::ValueType128 rhs)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    decQuadSubtract(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}



inline DecimalImpUtil_Platform::ValueType64  DecimalImpUtil_Platform::mul(
                                               DecimalImpUtil_Platform::ValueType64 lhs,
                                               DecimalImpUtil_Platform::ValueType64 rhs)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decDoubleMultiply(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}

inline DecimalImpUtil_Platform::ValueType128  DecimalImpUtil_Platform::mul(
                                              DecimalImpUtil_Platform::ValueType128 lhs,
                                              DecimalImpUtil_Platform::ValueType128 rhs)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    decQuadMultiply(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}



inline DecimalImpUtil_Platform::ValueType64  DecimalImpUtil_Platform::div(
                                               DecimalImpUtil_Platform::ValueType64 lhs,
                                               DecimalImpUtil_Platform::ValueType64 rhs)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decDoubleDivide(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}

inline DecimalImpUtil_Platform::ValueType128  DecimalImpUtil_Platform::div(
                                             DecimalImpUtil_Platform::ValueType128  lhs,
                                             DecimalImpUtil_Platform::ValueType128  rhs)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    decQuadDivide(&retval, &lhs, &rhs, getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType32 DecimalImpUtil_Platform::neg(
                                    DecimalImpUtil_Platform::ValueType32 value)
{
    DecimalImpUtil_Platform::ValueType32 retval;
    decSingleCopyNegate(&retval, &value);
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64 DecimalImpUtil_Platform::neg(
                                    DecimalImpUtil_Platform::ValueType64 value)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decDoubleCopyNegate(&retval, &value);
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType128 DecimalImpUtil_Platform::neg(
                                   DecimalImpUtil_Platform::ValueType128 value)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    decQuadCopyNegate(&retval, &value);
    return retval;
}

inline
bool DecimalImpUtil_Platform::less(DecimalImpUtil_Platform::ValueType32 lhs,
                                   DecimalImpUtil_Platform::ValueType32 rhs)
{
    return less(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool DecimalImpUtil_Platform::less(DecimalImpUtil_Platform::ValueType64 lhs,
                                   DecimalImpUtil_Platform::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsNegative(&comparison);
}

inline
bool DecimalImpUtil_Platform::less(DecimalImpUtil_Platform::ValueType128 lhs,
                                   DecimalImpUtil_Platform::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsNegative(&comparison);
}

inline
bool DecimalImpUtil_Platform::greater(DecimalImpUtil_Platform::ValueType32 lhs,
                                      DecimalImpUtil_Platform::ValueType32 rhs)
{
    return greater(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool DecimalImpUtil_Platform::greater(DecimalImpUtil_Platform::ValueType64 lhs,
                                      DecimalImpUtil_Platform::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsPositive(&comparison);
}

inline
bool DecimalImpUtil_Platform::greater(
                                     DecimalImpUtil_Platform::ValueType128 lhs,
                                     DecimalImpUtil_Platform::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsPositive(&comparison);
}

inline
bool DecimalImpUtil_Platform::lessEqual(
                                      DecimalImpUtil_Platform::ValueType32 lhs,
                                      DecimalImpUtil_Platform::ValueType32 rhs)
{
    return lessEqual(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool DecimalImpUtil_Platform::lessEqual(
                                      DecimalImpUtil_Platform::ValueType64 lhs,
                                      DecimalImpUtil_Platform::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsNegative(&comparison) || decDoubleIsZero(&comparison);
}

inline
bool DecimalImpUtil_Platform::lessEqual(
                                     DecimalImpUtil_Platform::ValueType128 lhs,
                                     DecimalImpUtil_Platform::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsNegative(&comparison) || decQuadIsZero(&comparison);
}

inline
bool DecimalImpUtil_Platform::greaterEqual(
                                      DecimalImpUtil_Platform::ValueType32 lhs,
                                      DecimalImpUtil_Platform::ValueType32 rhs)
{
    return greaterEqual(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool DecimalImpUtil_Platform::greaterEqual(
                                      DecimalImpUtil_Platform::ValueType64 lhs,
                                      DecimalImpUtil_Platform::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsPositive(&comparison) || decDoubleIsZero(&comparison);
}

inline
bool DecimalImpUtil_Platform::greaterEqual(
                                     DecimalImpUtil_Platform::ValueType128 lhs,
                                     DecimalImpUtil_Platform::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsPositive(&comparison) || decQuadIsZero(&comparison);
}

inline
bool DecimalImpUtil_Platform::equal(DecimalImpUtil_Platform::ValueType32 lhs,
                                    DecimalImpUtil_Platform::ValueType32 rhs)
{
    return equal(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool DecimalImpUtil_Platform::equal(DecimalImpUtil_Platform::ValueType64 lhs,
                                    DecimalImpUtil_Platform::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsZero(&comparison);
}

inline
bool DecimalImpUtil_Platform::equal(DecimalImpUtil_Platform::ValueType128 lhs,
                                    DecimalImpUtil_Platform::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsZero(&comparison);
}


inline
bool DecimalImpUtil_Platform::notEqual(
                                      DecimalImpUtil_Platform::ValueType32 lhs,
                                      DecimalImpUtil_Platform::ValueType32 rhs)
{
    return notEqual(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool DecimalImpUtil_Platform::notEqual(
                                      DecimalImpUtil_Platform::ValueType64 lhs,
                                      DecimalImpUtil_Platform::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsPositive(&comparison)
        || decDoubleIsNegative(&comparison);
}

inline
bool DecimalImpUtil_Platform::notEqual(
                                     DecimalImpUtil_Platform::ValueType128 lhs,
                                     DecimalImpUtil_Platform::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsPositive(&comparison) || decQuadIsNegative(&comparison);
}


inline
DecimalImpUtil_Platform::ValueType32 DecimalImpUtil_Platform::convertToDecimal32(
                                      const DecimalImpUtil_Platform::ValueType64& input)
{
    DecimalImpUtil_Platform::ValueType32 retval;
    decSingleFromWider(&retval, &input, getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::convertToDecimal64(
                             const DecimalImpUtil_Platform::ValueType32& input)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decSingleToWider(&input, &retval);
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::convertToDecimal64(
                            const DecimalImpUtil_Platform::ValueType128& input)
{
    DecimalImpUtil_Platform::ValueType64 retval;
    decDoubleFromWider(&retval, &input, getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::convertToDecimal128(
                             const DecimalImpUtil_Platform::ValueType32& input)
{
    return convertToDecimal128(convertToDecimal64(input));
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::convertToDecimal128(
                             const DecimalImpUtil_Platform::ValueType64& input)
{
    DecimalImpUtil_Platform::ValueType128 retval;
    decDoubleToWider(&input, &retval);
    return retval;
}

inline
DecimalImpUtil_Platform::ValueType32
DecimalImpUtil_Platform::binaryToDecimal32(float input)
{
    ValueType32 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);
    out << input;
    decSingleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType32
DecimalImpUtil_Platform::binaryToDecimal32(double input)
{
    ValueType32 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);
    out << input;
    decSingleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType32
DecimalImpUtil_Platform::binaryToDecimal32(long double input)
{
    ValueType32 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(7);
    out << input;
    decSingleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::binaryToDecimal64(float input)
{
    ValueType64 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << input;
    decDoubleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::binaryToDecimal64(double input)
{
    ValueType64 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << input;
    decDoubleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::binaryToDecimal64(long double input)
{
    ValueType64 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << input;
    decDoubleFromString(&result, bb.str(), getDecNumberContext());
    return result;
}


inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::binaryToDecimal128(float input)
{
    ValueType128 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(16);
    out << input;
    decQuadFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::binaryToDecimal128(double input)
{
    ValueType128 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << input;
    decQuadFromString(&result, bb.str(), getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::binaryToDecimal128(long double input)
{
    ValueType128 result;
    BufferBuf<48> bb;
    bsl::ostream out(&bb);
    out.imbue(bsl::locale::classic());
    out.precision(34);
    out << input;
    decQuadFromString(&result, bb.str(), getDecNumberContext());
    return result;
}


inline
DecimalImpUtil_Platform::ValueType32
DecimalImpUtil_Platform::makeDecimalRaw32(int mantissa,
                                          int exponent)
{
    DecimalImpUtil_Platform::ValueType32 result;
    result = DecimalImpUtil_Platform::int32ToDecimal32(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::makeDecimalRaw64(unsigned long long mantissa,
                                          int                exponent)
{
    DecimalImpUtil_Platform::ValueType64 result;
    result = DecimalImpUtil_Platform::uint64ToDecimal64(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::makeDecimalRaw64(long long mantissa,
                                          int       exponent)
{
    DecimalImpUtil_Platform::ValueType64 result;
    result = DecimalImpUtil_Platform::int64ToDecimal64(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64
DecimalImpUtil_Platform::makeDecimalRaw64(unsigned int mantissa,
                                          int          exponent)
{
    DecimalImpUtil_Platform::ValueType64 result;
    result = DecimalImpUtil_Platform::uint32ToDecimal64(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64 DecimalImpUtil_Platform::makeDecimalRaw64(
                                                                  int mantissa,
                                                                  int exponent)
{
    DecimalImpUtil_Platform::ValueType64 result;
    result = DecimalImpUtil_Platform::int32ToDecimal64(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::makeDecimalRaw128(unsigned long long mantissa,
                                           int                exponent)
{
    DecimalImpUtil_Platform::ValueType128 result;
    result = DecimalImpUtil_Platform::uint64ToDecimal128(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::makeDecimalRaw128(long long mantissa,
                                           int       exponent)
{
    DecimalImpUtil_Platform::ValueType128 result;
    result = DecimalImpUtil_Platform::int64ToDecimal128(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::makeDecimalRaw128(unsigned int mantissa,
                                           int          exponent)
{
    DecimalImpUtil_Platform::ValueType128 result;
    result = DecimalImpUtil_Platform::uint32ToDecimal128(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128
DecimalImpUtil_Platform::makeDecimalRaw128(int mantissa,
                                           int exponent)
{
    DecimalImpUtil_Platform::ValueType128 result;
    result = DecimalImpUtil_Platform::uint32ToDecimal128(mantissa);
    result = DecimalImpUtil_Platform::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_Platform::ValueType32 DecimalImpUtil_Platform::scaleB(
                                    DecimalImpUtil_Platform::ValueType32 value,
                                    int                                  power)
{
    return convertToDecimal32(scaleB(convertToDecimal64(value),power));
}

inline
DecimalImpUtil_Platform::ValueType64 DecimalImpUtil_Platform::scaleB(
                                    DecimalImpUtil_Platform::ValueType64 value,
                                    int                                  power)
{
    DecimalImpUtil_Platform::ValueType64 result;
    DecimalImpUtil_Platform::ValueType64 decPower;
    decDoubleFromInt32(&decPower, power);
    decDoubleScaleB(&result, &value, &decPower, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128 DecimalImpUtil_Platform::scaleB(
                                   DecimalImpUtil_Platform::ValueType128 value,
                                   int                                   power)
{
    DecimalImpUtil_Platform::ValueType128 result;
    DecimalImpUtil_Platform::ValueType128 decPower;
    decQuadFromInt32(&decPower, power);
    decQuadScaleB(&result, &value, &decPower, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType32 DecimalImpUtil_Platform::parse32(const char *str)
{
    DecimalImpUtil_Platform::ValueType32 result;
    decSingleFromString(&result, str, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType64 DecimalImpUtil_Platform::parse64(const char *str)
{
    DecimalImpUtil_Platform::ValueType64 result;
    decDoubleFromString(&result, str, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_Platform::ValueType128 DecimalImpUtil_Platform::parse128(const char *str)
{
    DecimalImpUtil_Platform::ValueType128 result;
    decQuadFromString(&result, str, getDecNumberContext());
    return result;
}


}  // close package namespace
}  // close enterprise namespace

#endif

#endif

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
