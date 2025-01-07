// blp_common64.h                                                       -*-C-*-
#ifndef BLP_RYU_COMMON64_H
#define BLP_RYU_COMMON64_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define DOUBLE_MANTISSA_BITS 52
#define DOUBLE_EXPONENT_BITS 11
#define DOUBLE_BIAS 1023

static inline uint32_t mantissa_countr_zero64(const uint64_t ieeeMantissa)
  // Return the bit position of the lowest set (value is 1) bit of the
  // specified `ieeeMantissa` of an IEEE-754 `binary64` floating point number.
  // The behavior is undefined unless `ieeeMantissa` is valid for a `binary64`,
  // such as that `0 == ~0xFFFFFFFFFFFFFull & ieeeMantissa`, or in other words
  // no bit above bit 52 is set.
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
  // The mantissa of a 'double' can be zero in 3 cases:
  //
  //   * Positive or negative infinity
  //   * Positive or negative zero
  //   * A value that is a power of two
  //
  // Infinity and zero is handled early, so this function is not called for
  // such values, so we know we have a value that is a power of 2.  The value
  // 1 is stored as binary exponent -52 with zero mantissa, so in the full
  // binary mantissa the (imaginary) 53rd bit is 1, with 52 zero bits after it,
  // divided by two to the 52nd power results in one.  Half is stored similarly
  // with a 0 mantissa, but with the binary exponent being -53.

  switch (ieeeMantissa & -ieeeMantissa) {
    case             0x0000ull: return  52;  // See note above
    case             0x0001ull: return  0;
    case             0x0002ull: return  1;
    case             0x0004ull: return  2;
    case             0x0008ull: return  3;
    case             0x0010ull: return  4;
    case             0x0020ull: return  5;
    case             0x0040ull: return  6;
    case             0x0080ull: return  7;
    case             0x0100ull: return  8;
    case             0x0200ull: return  9;
    case             0x0400ull: return 10;
    case             0x0800ull: return 11;
    case             0x1000ull: return 12;
    case             0x2000ull: return 13;
    case             0x4000ull: return 14;
    case             0x8000ull: return 15;
    case         0x00010000ull: return 16;
    case         0x00020000ull: return 17;
    case         0x00040000ull: return 18;
    case         0x00080000ull: return 19;
    case         0x00100000ull: return 20;
    case         0x00200000ull: return 21;
    case         0x00400000ull: return 22;
    case         0x00800000ull: return 23;
    case         0x01000000ull: return 24;
    case         0x02000000ull: return 25;
    case         0x04000000ull: return 26;
    case         0x08000000ull: return 27;
    case         0x10000000ull: return 28;
    case         0x20000000ull: return 29;
    case         0x40000000ull: return 30;
    case         0x80000000ull: return 31;
    case     0x000100000000ull: return 32;
    case     0x000200000000ull: return 33;
    case     0x000400000000ull: return 34;
    case     0x000800000000ull: return 35;
    case     0x001000000000ull: return 36;
    case     0x002000000000ull: return 37;
    case     0x004000000000ull: return 38;
    case     0x008000000000ull: return 39;
    case     0x010000000000ull: return 40;
    case     0x020000000000ull: return 41;
    case     0x040000000000ull: return 42;
    case     0x080000000000ull: return 43;
    case     0x100000000000ull: return 44;
    case     0x200000000000ull: return 45;
    case     0x400000000000ull: return 46;
    case     0x800000000000ull: return 47;
    case 0x0001000000000000ull: return 48;
    case 0x0002000000000000ull: return 49;
    case 0x0004000000000000ull: return 50;
    case 0x0008000000000000ull: return 51;
  }
  assert(false && "Invalid 'double' mantissa");
  return (uint32_t)-1;
}

static inline bool is_integer64(const uint64_t ieeeMantissa,
                                const uint32_t ieeeExponent)
  // Return `true` if the specified `ieeeMantissa` and `ieeeExponent` values of
  // an IEEE-754 `binary32` floating point number represent an integer value.
  // Return `false` if they represent a value with a non-zero fractional part,
  // in other words not an integral value.  The behavior is undefined unless
  // `ieeeMantissa` and `ieeeExponent` are valid for an IEEE-754 `binary32`, so
  // that `0 == 0x7FFFFF & ieeeMantissa`, or in other words no bit above bit
  // 23 is set in the mantissa, and `0 == 0x7FF & ieeeExponent`, or in other
  // words no bit above bit 11 is set in the exponent.

  // Return `true` if the specified `ieeeMantissa` and `ieeeExponent` values of
  // an IEEE-754 `binary64` floating point number represent an integer value.
  // Return `false` if they represent a value with a non-zero fractional part,
  // in other words not an integral value.  The behavior is undefined unless
  // `ieeeMantissa` and `ieeeExponent` are valid for an IEEE-754 `binary32`
  // non-zero numeric value: `0 == ~0xFFFFFFFFFFFFFull & ieeeMantissa`, and
  // `0 == ~0x7FF & ieeeExponent`, and  `0x7FF != ieeeExponent`, and
  // `0 != ieeeMantissa || 0x3FF != ieeeExponent`; in other words:
  //    * no bit above bit 52 is set in the mantissa
  //    * no bit above bit 11 is set in the exponent
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
  // Simply said the value of the IEEE-754 `binary64` is
  // `1.{bits of ieeeMantissa} << binExponent`, or in other words, the bits
  // in `ieeeMantissa` are treated as fractional digits, or digits after the
  // radix point.  (It is a bit more difficult with denormals (also called
  // subnormals, where the invisible top/24th digit is considered 0) but we can
  // safely ignore those values here as they are never integers, and they will
  // be reported as such with this simple approach.)
  //
  // `intExponent` is the value of the exponent if we consider the mantissa
  // bits integer bits instead of fractional.  The value calculation with
  // simply becomes `0x20000000000000 + ieeeMantissa << inExponent`.
  //
  // If `intExponent` is not negative, `intExponent >= 0`, we surely have an
  // integer value.  If not, we may still have an integer value if the mantissa
  // has trailing 0 bits (lowest value bits are 0).  In other words if the
  // mantissa is 100b and the binary exponent is -2, the value is 1.  If the
  // exponent is -1 for the same mantissa, the value would be 2.  If no
  // mantissa bit is set we considered the lowest bit set to be the invisible
  // 53rd bit (bit 52, as we count from 0).

  const int binExponent = (int)ieeeExponent - DOUBLE_BIAS;
  const int intExponent = binExponent - DOUBLE_MANTISSA_BITS;
  return intExponent >= 0
             || (uint32_t)-intExponent <= mantissa_countr_zero64(ieeeMantissa);
}

#endif // BLP_RYU_COMMON64_H

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
