// bdes_float.cpp                  -*-C++-*-
#include <bdes_float.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdes_float_cpp,"$Id$ $CSID$")


#include <bsls_platform.h>  // for testing only

#include <bsl_cstring.h>          // for 'memcpy'

namespace BloombergLP {

namespace {

typedef unsigned int       FloatRep_t;   // integer representation of 'float'
typedef unsigned long long DoubleRep_t;  // integer representation of 'double'

template <bool COND> struct CompiletimeAssert;
template <> struct CompiletimeAssert<true> { };
// CompiletimeAssert<false> is deliberately not defined.

// For each 'Rep' type, above, assert that sizeof 'Rep' type is the same as
// sizeof corresponding floating-point type.
typedef CompiletimeAssert<sizeof(FloatRep_t)  == sizeof(float)>  FloatAssert;
typedef CompiletimeAssert<sizeof(DoubleRep_t) == sizeof(double)> DoubleAssert;

// Use the types 'FloatAssert' and 'DoubleAssert' so that their use of
// 'CompileTimeAssert<>' will be evaluated and we will detect if either of the
// conditions are wrong.  The compiler does not issue diagnostics on unused
// types or unused enums.
enum { NOT_USED = sizeof(FloatAssert) + sizeof(DoubleAssert) };

const FloatRep_t  floatExpMask   = 0x7f800000U;
const FloatRep_t  floatManMask   = 0x007fffffU;
const FloatRep_t  floatSignMask  = 0x80000000U;
const FloatRep_t  floatHBitMask  = 0x00400000U;  // high bit of Mantissa
const FloatRep_t  floatInfRep    = floatExpMask;
// const FloatRep_t  floatQNanRep   = floatExpMask | floatHBitMask;
// const FloatRep_t  floatSNanRep   = floatExpMask | floatHBitMask >> 1;

const DoubleRep_t doubleExpMask  = 0x7ff0000000000000ULL;
const DoubleRep_t doubleManMask  = 0x000fffffffffffffULL;
const DoubleRep_t doubleSignMask = 0x8000000000000000ULL;
const DoubleRep_t doubleHBitMask = 0x0008000000000000ULL;
const DoubleRep_t doubleInfRep   = doubleExpMask;
// const DoubleRep_t doubleQNanRep  = doubleExpMask | doubleHBitMask;
// const DoubleRep_t doubleSNanRep  = doubleExpMask | doubleHBitMask >> 1;

}  // close unnamed namespace

///////////////////////////////////////////////////////////////////////////////
// Functions that operate on 'float'
///////////////////////////////////////////////////////////////////////////////

namespace {

inline
FloatRep_t toRep(float number)
{
    // Using 'bsl::memcpy' is the only portable way to copy the contents of
    // 'number' into an integral type without aliasing and/or alignment
    // problems.

    FloatRep_t ret;
    bsl::memcpy(&ret, &number, sizeof(number));
    return ret;
}

inline
bdes_Float::Classification classifyImp(FloatRep_t number)
    // Implementation of 'bdes_Float::classify(float number)'.
{
    FloatRep_t numberExp = floatExpMask & number;

    // Efficiency note: Every category is resolved with exactly two
    // conditional statements and at most two bit-wise AND operations.  The
    // intention was not fairness, but rather to minimize the number of
    // operations necessary to detect a normal number.

    if (! numberExp) {
        // Zero exponent.  Number is either zero or subnormal.
        if (number & floatManMask) {
            // Zero exponent and non-zero mantissa.  Number is subnormal.
            return bdes_Float::BDES_SUBNORMAL;
        }
        else {
            // Zero exponent and zero mantissa.  Number is zero.
            return bdes_Float::BDES_ZERO;
        }
    }
    else if (floatExpMask == numberExp) {
        // Exponent is all ones: Number is either infinity or NaN
        if (number & floatManMask) {
            // Exponent is all ones and mantissa is non-zero.  Number is NaN.
            return bdes_Float::BDES_NAN;
        }
        else {
            // Exponent is all ones and mantissa is zero.  Number is infinite.
            return bdes_Float::BDES_INFINITE;
        }
    }

    return bdes_Float::BDES_NORMAL;
}

}  // close unnamed namespace

bdes_Float::Classification bdes_Float::classify(float number)
{
    return classifyImp(toRep(number));
}

bdes_Float::FineClassification bdes_Float::classifyFine(float number)
{
    FloatRep_t numberRep = toRep(number);
    int ret = classifyImp(numberRep);
    if (BDES_NAN == ret) {
        if (numberRep & floatHBitMask) {
            ret = BDES_QNAN;
        }
        else {
            ret = BDES_SNAN;
        }
    }
    else if (numberRep & floatSignMask) {
        ret |= BDES_NEGATIVE;
    }

    return static_cast<FineClassification>(ret);
}

bool bdes_Float::isZero(float number)
{
    return 0 == (toRep(number) & ~floatSignMask);
}

bool bdes_Float::isNormal(float number)
{
    FloatRep_t numberExp = toRep(number) & floatExpMask;
    return numberExp != 0 && numberExp != floatExpMask;
}

bool bdes_Float::isSubnormal(float number)
{
    register FloatRep_t numberRep = toRep(number);
    FloatRep_t numberExp = numberRep & floatExpMask;
    FloatRep_t numberMan = numberRep & floatManMask;
    return 0 == numberExp && 0 != numberMan;
}

bool bdes_Float::isInfinite(float number)
{
    return (toRep(number) & ~floatSignMask) == floatInfRep;
}

bool bdes_Float::isNan(float number)
{
    return (toRep(number) & ~floatSignMask) > floatExpMask;
}

bool bdes_Float::signBit(float number)
{
    return (toRep(number) & floatSignMask) != 0;
}

bool bdes_Float::isFinite(float number)
{
    return (toRep(number) & floatExpMask) != floatExpMask;
}

bool bdes_Float::isQuietNan(float number)
{
    static const FloatRep_t qmask = floatExpMask | floatHBitMask;
    return (toRep(number) & qmask) == qmask;
}

bool bdes_Float::isSignalingNan(float number)
{
    static const FloatRep_t qmask = floatExpMask | floatHBitMask;

    register FloatRep_t numberRep = toRep(number);
    FloatRep_t numberMan = numberRep & floatManMask;
    return (numberRep & qmask) == floatExpMask && 0 != numberMan;
}

///////////////////////////////////////////////////////////////////////////////
// Functions that operate on 'double'
///////////////////////////////////////////////////////////////////////////////

namespace {

inline
DoubleRep_t toRep(double number)
{
    // Using 'bsl::memcpy' is the only portable way to copy the contents of
    // 'number' into an integral type without aliasing and/or alignment
    // problems.

    DoubleRep_t ret;
    bsl::memcpy(&ret, &number, sizeof(number));
    return ret;
}

inline
bdes_Float::Classification classifyImp(DoubleRep_t number)
    // Implementation of 'bdes_Float::classify(double number)'.
{
    DoubleRep_t numberExp = doubleExpMask & number;

    // Efficiency note: Every category is resolved with exactly two
    // conditional statements and at most two bit-wise AND operations.  The
    // intention was not fairness, but rather to minimize the number of
    // operations necessary to detect a normal number.

    if (! numberExp) {
        // Zero exponent.  Number is either zero or subnormal.
        if (number & doubleManMask) {
            // Zero exponent and non-zero mantissa.  Number is subnormal.
            return bdes_Float::BDES_SUBNORMAL;
        }
        else {
            // Zero exponent and zero mantissa.  Number is zero.
            return bdes_Float::BDES_ZERO;
        }
    }
    else if (doubleExpMask == numberExp) {
        // Exponent is all ones: Number is either infinity or NaN
        if (number & doubleManMask) {
            // Exponent is all ones and mantissa is non-zero.  Number is NaN.
            return bdes_Float::BDES_NAN;
        }
        else {
            // Exponent is all ones and mantissa is zero.  Number is infinite.
            return bdes_Float::BDES_INFINITE;
        }
    }

    return bdes_Float::BDES_NORMAL;
}

}  // close unnamed namespace

bdes_Float::Classification bdes_Float::classify(double number)
{
    return classifyImp(toRep(number));
}

bdes_Float::FineClassification bdes_Float::classifyFine(double number)
{
    DoubleRep_t numberRep = toRep(number);
    int ret = classifyImp(numberRep);
    if (BDES_NAN == ret) {
        if (numberRep & doubleHBitMask) {
            ret = BDES_QNAN;
        }
        else {
            ret = BDES_SNAN;
        }
    }
    else if (numberRep & doubleSignMask) {
        ret |= BDES_NEGATIVE;
    }

    return static_cast<FineClassification>(ret);
}

bool bdes_Float::isZero(double number)
{
    return 0 == (toRep(number) & ~doubleSignMask);
}

bool bdes_Float::isNormal(double number)
{
    DoubleRep_t numberExp = toRep(number) & doubleExpMask;
    return numberExp != 0 && numberExp != doubleExpMask;
}

bool bdes_Float::isSubnormal(double number)
{
    register DoubleRep_t numberRep = toRep(number);
    DoubleRep_t numberExp = numberRep & doubleExpMask;
    DoubleRep_t numberMan = numberRep & doubleManMask;
    return 0 == numberExp && 0 != numberMan;
}

bool bdes_Float::isInfinite(double number)
{
    return (toRep(number) & ~doubleSignMask) == doubleInfRep;
}

bool bdes_Float::isNan(double number)
{
    return (toRep(number) & ~doubleSignMask) > doubleExpMask;
}

bool bdes_Float::signBit(double number)
{
    return (toRep(number) & doubleSignMask) != 0;
}

bool bdes_Float::isFinite(double number)
{
    return (toRep(number) & doubleExpMask) != doubleExpMask;
}

bool bdes_Float::isQuietNan(double number)
{
    static const DoubleRep_t qmask = doubleExpMask | doubleHBitMask;
    return (toRep(number) & qmask) == qmask;
}

bool bdes_Float::isSignalingNan(double number)
{
    static const DoubleRep_t qmask = doubleExpMask | doubleHBitMask;

    register DoubleRep_t numberRep = toRep(number);
    DoubleRep_t numberMan = numberRep & doubleManMask;
    return (numberRep & qmask) == doubleExpMask && 0 != numberMan;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
