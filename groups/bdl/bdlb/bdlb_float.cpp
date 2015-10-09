// bdlb_float.cpp                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_float.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_float_cpp,"$Id$ $CSID$")


#include <bsls_platform.h>  // for testing only

#include <bsl_cstring.h>          // for 'memcpy'

namespace BloombergLP {

namespace {

typedef unsigned int       FloatRep_t;   // integer representation of 'float'
typedef unsigned long long DoubleRep_t;  // integer representation of 'double'

template <bool COND> struct CompiletimeAssert;
template <> struct CompiletimeAssert<true> { };
// CompiletimeAssert<false> is deliberately not defined.

// For each 'Rep' type, above, assert that 'sizeof Rep' type is the same as
// the 'sizeof' corresponding floating-point type.
typedef CompiletimeAssert<sizeof(FloatRep_t)  == sizeof(float)>  FloatAssert;
typedef CompiletimeAssert<sizeof(DoubleRep_t) == sizeof(double)> DoubleAssert;

// Use the types 'FloatAssert' and 'DoubleAssert' so that their use of
// 'CompileTimeAssert<>' will be evaluated and we will detect if either of the
// conditions are wrong.  The compiler does not issue diagnostics on unused
// types or unused 'enum's.
enum { k_NOT_USED = sizeof(FloatAssert) + sizeof(DoubleAssert) };

const FloatRep_t  floatExpMask   = 0x7f800000U;
const FloatRep_t  floatManMask   = 0x007fffffU;
const FloatRep_t  floatSignMask  = 0x80000000U;
const FloatRep_t  floatHBitMask  = 0x00400000U;  // high bit of Mantissa
const FloatRep_t  floatInfRep    = floatExpMask;
//..
// const FloatRep_t  floatQNanRep   = floatExpMask | floatHBitMask;
// const FloatRep_t  floatSNanRep   = floatExpMask | floatHBitMask >> 1;
//..

const DoubleRep_t doubleExpMask  = 0x7ff0000000000000ULL;
const DoubleRep_t doubleManMask  = 0x000fffffffffffffULL;
const DoubleRep_t doubleSignMask = 0x8000000000000000ULL;
const DoubleRep_t doubleHBitMask = 0x0008000000000000ULL;
const DoubleRep_t doubleInfRep   = doubleExpMask;
//..
// const DoubleRep_t doubleQNanRep  = doubleExpMask | doubleHBitMask;
// const DoubleRep_t doubleSNanRep  = doubleExpMask | doubleHBitMask >> 1;
//..

}  // close unnamed namespace

//                      ** Functions that operate on 'float' **

namespace {

inline
FloatRep_t toRep(float number)
    // Convert the specified 'number' to an integral (bitwise) representation.
    // Using 'bsl::memcpy' is the only portable way to copy the contents of
    // 'number' into an integral type without aliasing and/or alignment
    // problems.
{
    FloatRep_t ret;
    bsl::memcpy(&ret, &number, sizeof(number));
    return ret;
}

inline
bdlb::Float::Classification classifyImp(FloatRep_t number)
    // Implementation of 'bdlb::Float::classify(float number)'.
{
    FloatRep_t numberExp = floatExpMask & number;

    // Efficiency note: Every category is resolved with exactly two conditional
    // statements and at most two bit-wise AND operations.  The intention was
    // not fairness, but rather to minimize the number of operations necessary
    // to detect a normal number.

    if (! numberExp) {
        // Zero exponent.  Number is either zero or subnormal.
        if (number & floatManMask) {
            // Zero exponent and non-zero mantissa.  Number is subnormal.
            return bdlb::Float::k_SUBNORMAL;                          // RETURN
        }
        else {
            // Zero exponent and zero mantissa.  Number is zero.
            return bdlb::Float::k_ZERO;                               // RETURN
        }
    }
    else if (floatExpMask == numberExp) {
        // Exponent is all ones: Number is either infinity or NaN
        if (number & floatManMask) {
            // Exponent is all ones and mantissa is non-zero.  Number is NaN.
            return bdlb::Float::k_NAN;                                // RETURN
        }
        else {
            // Exponent is all ones and mantissa is zero.  Number is infinite.
            return bdlb::Float::k_INFINITE;                           // RETURN
        }
    }

    return bdlb::Float::k_NORMAL;
}

}  // close unnamed namespace

namespace bdlb {

                        // ------------
                        // struct Float
                        // ------------

Float::Classification Float::classify(float number)
{
    return classifyImp(toRep(number));
}

Float::FineClassification Float::classifyFine(float number)
{
    FloatRep_t numberRep = toRep(number);
    int        ret       = classifyImp(numberRep);

    if (k_NAN == ret) {
        if (numberRep & floatHBitMask) {
            ret = k_QNAN;
        }
        else {
            ret = k_SNAN;
        }
    }
    else if (numberRep & floatSignMask) {
        ret |= k_NEGATIVE;
    }

    return static_cast<FineClassification>(ret);
}

bool Float::isZero(float number)
{
    return 0 == (toRep(number) & ~floatSignMask);
}

bool Float::isNormal(float number)
{
    FloatRep_t numberExp = toRep(number) & floatExpMask;

    return numberExp != 0 && numberExp != floatExpMask;
}

bool Float::isSubnormal(float number)
{
    register FloatRep_t numberRep = toRep(number);
    FloatRep_t          numberExp = numberRep & floatExpMask;
    FloatRep_t          numberMan = numberRep & floatManMask;

    return 0 == numberExp && 0 != numberMan;
}

bool Float::isInfinite(float number)
{
    return (toRep(number) & ~floatSignMask) == floatInfRep;
}

bool Float::isNan(float number)
{
    return (toRep(number) & ~floatSignMask) > floatExpMask;
}

bool Float::signBit(float number)
{
    return (toRep(number) & floatSignMask) != 0;
}

bool Float::isFinite(float number)
{
    return (toRep(number) & floatExpMask) != floatExpMask;
}

bool Float::isQuietNan(float number)
{
    static const FloatRep_t qmask = floatExpMask | floatHBitMask;

    return (toRep(number) & qmask) == qmask;
}

bool Float::isSignalingNan(float number)
{
    static const FloatRep_t qmask = floatExpMask | floatHBitMask;

    register FloatRep_t numberRep = toRep(number);
    FloatRep_t          numberMan = numberRep & floatManMask;

    return (numberRep & qmask) == floatExpMask && 0 != numberMan;
}
}  // close package namespace

//                      ** Functions that operate on 'double' **
namespace {

inline
DoubleRep_t toRep(double number)
    // Convert the specified 'number' to an integral (bitwise) representation.
    // Using 'bsl::memcpy' is the only portable way to copy the contents of
    // 'number' into an integral type without aliasing and/or alignment
    // problems.
{

    DoubleRep_t ret;
    bsl::memcpy(&ret, &number, sizeof(number));
    return ret;
}

inline
bdlb::Float::Classification classifyImp(DoubleRep_t number)
    // Implementation of 'bdlb::Float::classify(double number)'.
{
    DoubleRep_t numberExp = doubleExpMask & number;

    // Efficiency note: Every category is resolved with exactly two conditional
    // statements and at most two bit-wise AND operations.  The intention was
    // not fairness, but rather to minimize the number of operations necessary
    // to detect a normal number.

    if (! numberExp) {
        // Zero exponent.  Number is either zero or subnormal.
        if (number & doubleManMask) {
            // Zero exponent and non-zero mantissa.  Number is subnormal.
            return bdlb::Float::k_SUBNORMAL;                          // RETURN
        }
        else {
            // Zero exponent and zero mantissa.  Number is zero.
            return bdlb::Float::k_ZERO;                               // RETURN
        }
    }
    else if (doubleExpMask == numberExp) {
        // Exponent is all ones: Number is either infinity or NaN
        if (number & doubleManMask) {
            // Exponent is all ones and mantissa is non-zero.  Number is NaN.
            return bdlb::Float::k_NAN;                                // RETURN
        }
        else {
            // Exponent is all ones and mantissa is zero.  Number is infinite.
            return bdlb::Float::k_INFINITE;                           // RETURN
        }
    }

    return bdlb::Float::k_NORMAL;
}

}  // close unnamed namespace

namespace bdlb {
Float::Classification Float::classify(double number)
{
    return classifyImp(toRep(number));
}

Float::FineClassification Float::classifyFine(double number)
{
    DoubleRep_t numberRep = toRep(number);
    int         ret       = classifyImp(numberRep);

    if (k_NAN == ret) {
        if (numberRep & doubleHBitMask) {
            ret = k_QNAN;
        }
        else {
            ret = k_SNAN;
        }
    }
    else if (numberRep & doubleSignMask) {
        ret |= k_NEGATIVE;
    }

    return static_cast<FineClassification>(ret);
}

bool Float::isZero(double number)
{
    return 0 == (toRep(number) & ~doubleSignMask);
}

bool Float::isNormal(double number)
{
    DoubleRep_t numberExp = toRep(number) & doubleExpMask;
    return numberExp != 0 && numberExp != doubleExpMask;
}

bool Float::isSubnormal(double number)
{
    register DoubleRep_t numberRep = toRep(number);
    DoubleRep_t          numberExp = numberRep & doubleExpMask;
    DoubleRep_t          numberMan = numberRep & doubleManMask;

    return 0 == numberExp && 0 != numberMan;
}

bool Float::isInfinite(double number)
{
    return (toRep(number) & ~doubleSignMask) == doubleInfRep;
}

bool Float::isNan(double number)
{
    return (toRep(number) & ~doubleSignMask) > doubleExpMask;
}

bool Float::signBit(double number)
{
    return (toRep(number) & doubleSignMask) != 0;
}

bool Float::isFinite(double number)
{
    return (toRep(number) & doubleExpMask) != doubleExpMask;
}

bool Float::isQuietNan(double number)
{
    static const DoubleRep_t qmask = doubleExpMask | doubleHBitMask;
    return (toRep(number) & qmask) == qmask;
}

bool Float::isSignalingNan(double number)
{
    static const DoubleRep_t qmask = doubleExpMask | doubleHBitMask;

    register DoubleRep_t numberRep = toRep(number);
    DoubleRep_t          numberMan = numberRep & doubleManMask;
    return (numberRep & qmask) == doubleExpMask && 0 != numberMan;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
