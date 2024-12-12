// bslmf_floatingtypestructuraltraits.h                               -*-C++-*-
#ifndef INCLUDED_BSLMF_FLOATING_TYPE_STRUCTURAL_TRAITS
#define INCLUDED_BSLMF_FLOATING_TYPE_STRUCTURAL_TRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide traits to describe floating point type structure.
//
//@CLASSES:
//  bsl::FloatingTypeStructuralTraits: traits for float.pt. type structure info
//
//@DESCRIPTION: This component provides a traits class type that describes the
// structure of the IEEE-754 based `float`, and `double` types for code that
// intends to interpret or manipulate the representation directly.
//
// This code has been lifted from the open source Microsoft STL, from the file
// `type_traits` and modified to remove "support" for `long double`.  (The MSFT
// code does not really support `long double`, it assumes that it is the same
// type as `double`, which is not true on most compilers.)
//
///Why No `long double`?
///---------------------
// The extended precision `long double` type is not fully portable, because
// Microsoft made it only double precision (same as the `double` type).  This
// traits class also provides an unsigned integer equivalent of a given
// floating point type.  Unfortunately for an extended precision `long double`
// that would be a more-than-64-bits type, and there is no such standard type.
// Since in our own code we do not support `long double` (with the formatting
// and parsing that this traits may be used for) we've foregone the trouble of
// trying to find a portable solution and simply decided not to support the
// `long double` type.

#include <bslscm_version.h>

#include <bslmf_assert.h>

#include <stdint.h>
#include <string.h>

namespace BloombergLP {
namespace bslmf {


// The following section contains modified test data from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/type_traits
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md

// vvvvvvvvvv DERIVED FROM corecrt_internal_fltintrn.h vvvvvvvvvv

/// This is the primary template declaration for
/// `bslmf::FloatingTypeStructuralTraits`, which is never defined.
template <class t_FLOATING_TYPE>
struct FloatingTypeStructuralTraits;

/// This is the full specialization of `bslmf::FloatingTypeStructuralTraits`
/// for the `float` type.  See the individual members for documentation.
template <>
struct FloatingTypeStructuralTraits<float> {
    // PUBLIC TYPES
    typedef float    FloatType;  /// The floating point type
    typedef uint32_t UintType;   /// Size-equivalent unsigned integer type

    BSLMF_ASSERT(sizeof(UintType) == sizeof(FloatType));

    // CLASS METHODS

    /// Converts the bits of the specified floating point `value` into the
    /// size-equivalent unsigned integer type.
    static UintType toUintType(FloatType value)
    {
        UintType rv;
        memcpy(&rv, &value, sizeof rv);
        return rv;
    }

    /// Converts the bits of the specified size-equivalent unsigned integer
    /// `uIntValue` into the "original" floating point type.
    static FloatType toFloatType(UintType uIntValue)
    {
        FloatType rv;
        memcpy(&rv, &uIntValue, sizeof rv);
        return rv;
    }

    /// Adjust the specified `ieeeMantissa` bits for hexfloat conversion.  This
    /// method is needed because `float` needs adjustment and `double` doesn't.
    static UintType adjustMantissaForHexDigits(UintType ieeeMantissa)
    {
        // align to hex digit boundary (23 isn't divisible by 4)
        return ieeeMantissa << 1;
    }

    // PUBLIC CONSTANTS

    /// Maximum required buffer size for `std::to_chars_format::general` with
    /// the maximum precision.
    static const int k_GENERAL_MAX_OUTPUT_LENGTH = 117;
        // The value 0x1.fffffep-126f

    /// The maximum meaningful precision for `std::to_chars_format::fixed`.
    static const int k_MAX_FIXED_PRECISION = 37;
        // The value 0x1.fffffep-14f

    /// Maximum meaningful precision for `std::to_chars_format::scientific`.
    static const int k_MAX_SCIENTIFIC_PRECISION = 111;
        // The value 0x1.fffffep-126f

    /// The hex digits needed to represent the mantissa in a hexfloat.
    static const int32_t k_MANTISSA_HEX_DIGITS = 6;
        // `float` explicitly stores 23 fraction bits.  23 / 4 == 5.75, which
        // needs 6 digits to represent.

    static const int32_t k_MANTISSA_BITS = 24;                  // FLT_MANT_DIG
    static const int32_t k_EXPONENT_BITS =  8;
                                     // sizeof(float) * CHAR_BIT - FLT_MANT_DIG

    static const int32_t k_MAX_BINARY_EXPONENT =  127;       // FLT_MAX_EXP - 1
    static const int32_t k_MIN_BINARY_EXPONENT = -126;       // FLT_MIN_EXP - 1

    static const int32_t k_EXPONENT_BIAS = 127;

    static const int32_t k_SIGN_SHIFT     = 31;
                                         // s_ExponentBits + s_MantissaBits - 1
    static const int32_t k_EXPONENT_SHIFT = 23;           // s_MantissaBits - 1

    static const uint32_t k_EXPONENT_MASK = 0x000000FFu;
                                                  // (1u << s_ExponentBits) - 1
    static const uint32_t k_NORMAL_MANTISSA_MASK   = 0x00FFFFFFu;
                                                  // (1u << s_MantissaBits) - 1
    static const uint32_t k_DENORM_MANTISSA_MASK   = 0x007FFFFFu;
                                            // (1u << (a_MantissaBits - 1)) - 1
    static const uint32_t k_SPEC_NAN_MANTISSA_MASK = 0x00400000u;
                                                  // 1u << (s_MantissaBits - 2)

    static const uint32_t k_SHIFTED_SIGN_MASK = 0x80000000u;
                                                           // 1u << s_SignShift
    static const uint32_t k_SHIFTED_EXPONENT_MASK = 0x7F800000u;
                                           // s_ExponentMask << s_ExponentShift

    static const float k_MIN_VALUE /* = 0x1.000000p-126f */;         // FLT_MIN
    static const float k_MAX_VALUE /* = 0x1.FFFFFEp+127f */;         // FLT_MAX
};


/// This is the full specialization of `bslmf::FloatingTypeStructuralTraits`
/// for the `double` type.  See the individual members for documentation.
template <>
struct FloatingTypeStructuralTraits<double> {
    // PUBLIC TYPES
    typedef double   FloatType;  /// The floating point type
    typedef uint64_t UintType;   /// Size-equivalent unsigned integer type

    BSLMF_ASSERT(sizeof(UintType) == sizeof(FloatType));

    // CLASS METHODS

    /// Converts the bits of the specified floating point `value` into the
    /// size-equivalent unsigned integer type.
    static UintType toUintType(FloatType value)
    {
        UintType rv;
        memcpy(&rv, &value, sizeof rv);
        return rv;
    }

    /// Converts the bits of the specified size-equivalent unsigned integer
    /// `uIntValue` into the "original" floating point type.
    static FloatType toFloatType(UintType uIntValue)
    {
        FloatType rv;
        memcpy(&rv, &uIntValue, sizeof rv);
        return rv;
    }

    /// Adjust the specified `ieeeMantissa` bits for hexfloat conversion.  This
    /// method is a no-op for `double` type.
    static UintType adjustMantissaForHexDigits(UintType ieeeMantissa)
    {
        return ieeeMantissa; // already aligned (52 is divisible by 4)
    }

    // PUBLIC CONSTANTS

    /// Maximum required buffer size for `std::to_chars_format::general` with
    /// the maximum precision.
    static const int k_GENERAL_MAX_OUTPUT_LENGTH = 773;
        // The value 0x1.fffffffffffffp-1022

    /// The maximum meaningful precision for `std::to_chars_format::fixed`.
    static const int k_MAX_FIXED_PRECISION = 66;
        // The value 0x1.fffffffffffffp-14

    /// Maximum meaningful precision for `std::to_chars_format::scientific`.
    static const int k_MAX_SCIENTIFIC_PRECISION = 766;
        // The value 0x1.fffffffffffffp-1022

    /// The hex digits needed to represent the mantissa in a hexfloat.
    static const int32_t k_MANTISSA_HEX_DIGITS = 13;
        // `double` explicitly stores 52 fraction bits.   52 / 4 == 13, which
        // is 13 hex digits.

    static const int32_t k_MANTISSA_BITS = 53;                   // DBL_MANT_DIG
    static const int32_t k_EXPONENT_BITS = 11;
                                    // sizeof(double) * CHAR_BIT - DBL_MANT_DIG

    static const int32_t k_MAX_BINARY_EXPONENT =  1023;    // DBL_MAX_EXP - 1
    static const int32_t k_MIN_BINARY_EXPONENT = -1022;    // DBL_MIN_EXP - 1

    static const int32_t k_EXPONENT_BIAS =  1023;
    static const int32_t k_SIGN_SHIFT             =    63;
                                         // s_ExponentBits + s_MantissaBits - 1
    static const int32_t k_EXPONENT_SHIFT         =    52; // s_MantissaBits - 1

    static const uint64_t k_EXPONENT_MASK          = 0x00000000000007FFu;
                                                // (1ULL << s_ExponentBits) - 1
    static const uint64_t k_NORMAL_MANTISSA_MASK   = 0x001FFFFFFFFFFFFFu;
                                                // (1ULL << s_MantissaBits) - 1
    static const uint64_t k_DENORM_MANTISSA_MASK   = 0x000FFFFFFFFFFFFFu;
                                          // (1ULL << (s_MantissaBits - 1)) - 1
    static const uint64_t k_SPEC_NAN_MANTISSA_MASK = 0x0008000000000000u;
                                                // 1ULL << (s_MantissaBits - 2)

    static const uint64_t k_SHIFTED_SIGN_MASK     = 0x8000000000000000u;
                                                        // 1ULL << s_SignShift
    static const uint64_t k_SHIFTED_EXPONENT_MASK = 0x7FF0000000000000u;
                                           // s_ExponentMask << s_ExponentShift

    static const double k_MIN_VALUE /*= 0x1.0000000000000p-1022*/;   // DBL_MIN
    static const double k_MAX_VALUE /*= 0x1.FFFFFFFFFFFFFp+1023*/;   // DBL_MAX
};


/// This is the full specialization of `bslmf::FloatingTypeStructuralTraits`
/// for the `long double` type.  Notice that `long double` is deliberately not
/// supported at this time, it is defined only so compiler errors will guide
/// users to the comment stating so.
template <>
struct FloatingTypeStructuralTraits<long double> {
    // `long double` is not supported, see file-level documentation
};

// ^^^^^^^^^^ DERIVED FROM corecrt_internal_fltintrn.h ^^^^^^^^^^

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.
//
// Licensed under the Apache-2.0 License WITH LLVM-exception;
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.  You may not use
// this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
