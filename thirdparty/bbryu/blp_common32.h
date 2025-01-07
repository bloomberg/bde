// blp_common32.h                                                       -*-C-*-
#ifndef BLP_RYU_COMMON32_H
#define BLP_RYU_COMMON32_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define FLOAT_MANTISSA_BITS 23
#define FLOAT_EXPONENT_BITS 8
#define FLOAT_BIAS 127

static inline uint32_t mantissa_countr_zero32(const uint32_t ieeeMantissa)
  // Return the bit position of the lowest set (value is 1) bit of the
  // specified `ieeeMantissa` of an IEEE-754 `binary32` floating point number.
  // The behavior is undefined unless `0 == ~0x7FFFFF & ieeeMantissa`, or in
  // other words no bit above bit 23 is set.
{
  // Implementation Note: `n & -n`, due to two's complement will leave only the
  // lowest originally set bit set.  Example on an imaginary 4 bit integer with
  // the decimal value 11.  Two's complement is inverting the digits then
  // adding 1, so for any number `n & -n` is equivalent to `n & (~n + 1)`
  //   `0b1011 & -0b1011`
  //   `0b1011 & (~0b1011 + 1)`
  //   `0b1011 & (0b0100 + 1)`
  //   `0b1011 & 0b0101`
  //   `0b0001`
  //
  // Note about zero mantissa (`case 0x0000:` below)
  // -----------------------------------------------
  // The mantissa of a 'float' can be zero in 3 cases:
  //
  //   * Positive or negative infinity
  //   * Positive or negative zero
  //   * A value that is a power of two
  //
  // Infinity and zero is handled early, so this function is not called for
  // such values, so we know we have a value that is a power of 2.  The value
  // 1 is stored as binary exponent -23 with zero mantissa, so in the full
  // binary mantissa the (imaginary) 24th bit is 1, with 23 zero bits after it,
  // divided by two to the 23nd power results in one.  Half is stored similarly
  // with a 0 mantissa, but with the binary exponent being -24.
    switch (ieeeMantissa & -ieeeMantissa) {
    case     0x0000: return  23;  // See note above
    case     0x0001: return  0;
    case     0x0002: return  1;
    case     0x0004: return  2;
    case     0x0008: return  3;
    case     0x0010: return  4;
    case     0x0020: return  5;
    case     0x0040: return  6;
    case     0x0080: return  7;
    case 0x00000100: return  8;
    case 0x00000200: return  9;
    case 0x00000400: return 10;
    case 0x00000800: return 11;
    case 0x00001000: return 12;
    case 0x00002000: return 13;
    case 0x00004000: return 14;
    case 0x00008000: return 15;
    case 0x00010000: return 16;
    case 0x00020000: return 17;
    case 0x00040000: return 18;
    case 0x00080000: return 19;
    case 0x00100000: return 20;
    case 0x00200000: return 21;
    case 0x00400000: return 22;
  }
  assert(false && "Invalid 'float' mantissa");
  return (uint32_t)-1;
}

static inline bool is_integer32(const uint32_t ieeeMantissa, const uint32_t ieeeExponent)
  // Return `true` if the specified `ieeeMantissa` and `ieeeExponent` values of
  // an IEEE-754 `binary32` floating point number represent an integer value.
  // Return `false` if they represent a value with a non-zero fractional part,
  // in other words not an integral value.  The behavior is undefined unless
  // `ieeeMantissa` and `ieeeExponent` are valid for an IEEE-754 `binary32`
  // non-zero numeric value, so that `0 == ~0x7FFFFF & ieeeMantissa`, and
  // `0 == ~0xFF & ieeeExponent`, and  `0x7F != ieeeExponent`, and
  // `0 != ieeeMantissa || 0x7F != ieeeExponent`; in other words:
  //    * no bit above bit 23 is set in the mantissa
  //    * no bit above bit 8 is set in the exponent
  //    * at least one exponent bit is unset (zero)
  //    * the mantissa and the exponent are not allowed to represent zero
{
  // Implementation notes:
  //
  // A value is integral if it has no fractional digits.  We can decide that by
  // looking at the exponent of a binary floating point number "normalized"
  // into a form where its mantissa is integer with its lowest digit set, or in
  // other words, and odd number (not dividable by 2), such as 1, 11, 101 etc.
  // In such form if the (binary) exponent is not negative, we have an integer.
  //
  // `binExponent` is the unbiased binary exponent.
  //
  // Simply said the value of the IEEE-754 `binary32` is
  // `1.{bits of ieeeMantissa} << binExponent`, or in other words, the bits
  // in `ieeeMantissa` are treated as fractional digits, or digits after the
  // radix point.  (It is a bit more difficult with denormals (also called
  // subnormals, where the invisible top/24th digit is considered 0) but we can
  // safely ignore those values here as they are never integers, and they will
  // be reported as such with this simple approach.)
  //
  // `intExponent` is the value of the exponent if we consider the mantissa
  // bits integer bits instead of fractional.  The value calculation with
  // `intExponent` simply becomes `0x800000 + ieeeMantissa << inExponent`.
  //
  // If `intExponent` is not negative, `intExponent >= 0`, we surely have an
  // integer value.  If not, we may still have an integer value if the mantissa
  // has trailing 0 bits (lowest value bits are 0).  In other words if the
  // mantissa is 100b and the binary exponent is -2, the value is 1.  If the
  // exponent is -1 for the same mantissa, the value would be 2.  If no
  // mantissa bit is set we considered the lowest bit set to be the invisible
  // 24th bit (bit 23, as we count from 0).

  const int binExponent = (int)ieeeExponent - FLOAT_BIAS;
  const int intExponent = binExponent - FLOAT_MANTISSA_BITS;
  return intExponent >= 0
             || (uint32_t)-intExponent <= mantissa_countr_zero32(ieeeMantissa);
}

#endif // BLP_RYU_COMMON32_H

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
