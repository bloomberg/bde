// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

// Runtime compiler options:
// -DRYU_DEBUG Generate verbose debugging output to stdout.
//
// Bloomberg LP CHANGELOG
//
// 2021.07.16 Initial version built from f2s.c of Ulf's Ryu code
//            See README.BDE.md for further explanations.

#include "ryu/blp_ryu.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef RYU_DEBUG
#include <stdio.h>
#endif

#include "ryu/blp_common.h"
#include "ryu/common.h"
#include "ryu/f2s_intrinsics.h"
#include "ryu/digit_table.h"

#define FLOAT_MANTISSA_BITS 23
#define FLOAT_EXPONENT_BITS 8
#define FLOAT_BIAS 127

// A floating decimal representing m * 10^e.
typedef struct floating_decimal_32 {
  uint32_t mantissa;
  // Decimal exponent's range is -45 to 38
  // inclusive, and can fit in a short if needed.
  int32_t exponent;
} floating_decimal_32;

static inline floating_decimal_32 f2d(const uint32_t ieeeMantissa, const uint32_t ieeeExponent) {
  int32_t e2;
  uint32_t m2;
  if (ieeeExponent == 0) {
    // We subtract 2 so that the bounds computation has 2 additional bits.
    e2 = 1 - FLOAT_BIAS - FLOAT_MANTISSA_BITS - 2;
    m2 = ieeeMantissa;
  } else {
    e2 = (int32_t) ieeeExponent - FLOAT_BIAS - FLOAT_MANTISSA_BITS - 2;
    m2 = (1u << FLOAT_MANTISSA_BITS) | ieeeMantissa;
  }
  const bool even = (m2 & 1) == 0;
  const bool acceptBounds = even;

#ifdef RYU_DEBUG
  printf("-> %u * 2^%d\n", m2, e2 + 2);
#endif

  // Step 2: Determine the interval of valid decimal representations.
  const uint32_t mv = 4 * m2;
  const uint32_t mp = 4 * m2 + 2;
  // Implicit bool -> int conversion. True is 1, false is 0.
  const uint32_t mmShift = ieeeMantissa != 0 || ieeeExponent <= 1;
  const uint32_t mm = 4 * m2 - 1 - mmShift;

  // Step 3: Convert to a decimal power base using 64-bit arithmetic.
  uint32_t vr, vp, vm;
  int32_t e10;
  bool vmIsTrailingZeros = false;
  bool vrIsTrailingZeros = false;
  uint8_t lastRemovedDigit = 0;
  if (e2 >= 0) {
    const uint32_t q = log10Pow2(e2);
    e10 = (int32_t) q;
    const int32_t k = FLOAT_POW5_INV_BITCOUNT + pow5bits((int32_t) q) - 1;
    const int32_t i = -e2 + (int32_t) q + k;
    vr = mulPow5InvDivPow2(mv, q, i);
    vp = mulPow5InvDivPow2(mp, q, i);
    vm = mulPow5InvDivPow2(mm, q, i);
#ifdef RYU_DEBUG
    printf("%u * 2^%d / 10^%u\n", mv, e2, q);
    printf("V+=%u\nV =%u\nV-=%u\n", vp, vr, vm);
#endif
    if (q != 0 && (vp - 1) / 10 <= vm / 10) {
      // We need to know one removed digit even if we are not going to loop below. We could use
      // q = X - 1 above, except that would require 33 bits for the result, and we've found that
      // 32-bit arithmetic is faster even on 64-bit machines.
      const int32_t l = FLOAT_POW5_INV_BITCOUNT + pow5bits((int32_t) (q - 1)) - 1;
      lastRemovedDigit = (uint8_t) (mulPow5InvDivPow2(mv, q - 1, -e2 + (int32_t) q - 1 + l) % 10);
    }
    if (q <= 9) {
      // The largest power of 5 that fits in 24 bits is 5^10, but q <= 9 seems to be safe as well.
      // Only one of mp, mv, and mm can be a multiple of 5, if any.
      if (mv % 5 == 0) {
        vrIsTrailingZeros = multipleOfPowerOf5_32(mv, q);
      } else if (acceptBounds) {
        vmIsTrailingZeros = multipleOfPowerOf5_32(mm, q);
      } else {
        vp -= multipleOfPowerOf5_32(mp, q);
      }
    }
  } else {
    const uint32_t q = log10Pow5(-e2);
    e10 = (int32_t) q + e2;
    const int32_t i = -e2 - (int32_t) q;
    const int32_t k = pow5bits(i) - FLOAT_POW5_BITCOUNT;
    int32_t j = (int32_t) q - k;
    vr = mulPow5divPow2(mv, (uint32_t) i, j);
    vp = mulPow5divPow2(mp, (uint32_t) i, j);
    vm = mulPow5divPow2(mm, (uint32_t) i, j);
#ifdef RYU_DEBUG
    printf("%u * 5^%d / 10^%u\n", mv, -e2, q);
    printf("%u %d %d %d\n", q, i, k, j);
    printf("V+=%u\nV =%u\nV-=%u\n", vp, vr, vm);
#endif
    if (q != 0 && (vp - 1) / 10 <= vm / 10) {
      j = (int32_t) q - 1 - (pow5bits(i + 1) - FLOAT_POW5_BITCOUNT);
      lastRemovedDigit = (uint8_t) (mulPow5divPow2(mv, (uint32_t) (i + 1), j) % 10);
    }
    if (q <= 1) {
      // {vr,vp,vm} is trailing zeros if {mv,mp,mm} has at least q trailing 0 bits.
      // mv = 4 * m2, so it always has at least two trailing 0 bits.
      vrIsTrailingZeros = true;
      if (acceptBounds) {
        // mm = mv - 1 - mmShift, so it has 1 trailing 0 bit iff mmShift == 1.
        vmIsTrailingZeros = mmShift == 1;
      } else {
        // mp = mv + 2, so it always has at least one trailing 0 bit.
        --vp;
      }
    } else if (q < 31) { // TODO(ulfjack): Use a tighter bound here.
      vrIsTrailingZeros = multipleOfPowerOf2_32(mv, q - 1);
#ifdef RYU_DEBUG
      printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
    }
  }
#ifdef RYU_DEBUG
  printf("e10=%d\n", e10);
  printf("V+=%u\nV =%u\nV-=%u\n", vp, vr, vm);
  printf("vm is trailing zeros=%s\n", vmIsTrailingZeros ? "true" : "false");
  printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif

  // Step 4: Find the shortest decimal representation in the interval of valid representations.
  int32_t removed = 0;
  uint32_t output;
  if (vmIsTrailingZeros || vrIsTrailingZeros) {
    // General case, which happens rarely (~4.0%).
    while (vp / 10 > vm / 10) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=23106
      // The compiler does not realize that vm % 10 can be computed from vm / 10
      // as vm - (vm / 10) * 10.
      vmIsTrailingZeros &= vm - (vm / 10) * 10 == 0;
#else
      vmIsTrailingZeros &= vm % 10 == 0;
#endif
      vrIsTrailingZeros &= lastRemovedDigit == 0;
      lastRemovedDigit = (uint8_t) (vr % 10);
      vr /= 10;
      vp /= 10;
      vm /= 10;
      ++removed;
    }
#ifdef RYU_DEBUG
    printf("V+=%u\nV =%u\nV-=%u\n", vp, vr, vm);
    printf("d-10=%s\n", vmIsTrailingZeros ? "true" : "false");
#endif
    if (vmIsTrailingZeros) {
      while (vm % 10 == 0) {
        vrIsTrailingZeros &= lastRemovedDigit == 0;
        lastRemovedDigit = (uint8_t) (vr % 10);
        vr /= 10;
        vp /= 10;
        vm /= 10;
        ++removed;
      }
    }
#ifdef RYU_DEBUG
    printf("%u %d\n", vr, lastRemovedDigit);
    printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
    if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
      // Round even if the exact number is .....50..0.
      lastRemovedDigit = 4;
    }
    // We need to take vr + 1 if vr is outside bounds or we need to round up.
    output = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
  } else {
    // Specialized for the common case (~96.0%). Percentages below are relative to this.
    // Loop iterations below (approximately):
    // 0: 13.6%, 1: 70.7%, 2: 14.1%, 3: 1.39%, 4: 0.14%, 5+: 0.01%
    while (vp / 10 > vm / 10) {
      lastRemovedDigit = (uint8_t) (vr % 10);
      vr /= 10;
      vp /= 10;
      vm /= 10;
      ++removed;
    }
#ifdef RYU_DEBUG
    printf("%u %d\n", vr, lastRemovedDigit);
    printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
    // We need to take vr + 1 if vr is outside bounds or we need to round up.
    output = vr + (vr == vm || lastRemovedDigit >= 5);
  }
  const int32_t exp = e10 + removed;

#ifdef RYU_DEBUG
  printf("V+=%u\nV =%u\nV-=%u\n", vp, vr, vm);
  printf("O=%u\n", output);
  printf("EXP=%d\n", exp);
#endif

  floating_decimal_32 fd;
  fd.exponent = exp;
  fd.mantissa = output;
  return fd;
}

static inline int write_digits(uint32_t olength, uint32_t output, WriteDigitsMode mode, char* const result) {
  // Write the decimal digits in the specified `output`, according to the
  // specified `mode`, to the specified `result`.  The specified `olength` must
  // contain the number of decimal digits of `output`.  Return the number of
  // characters written.  In `DigitsOnly` mode, write the digits of `output`
  // to `result` (going backwards) and return `olength`.  In `Mantissa` mode,
  // if `1 == olength` (or in other words `1 <= output <= 9`) write that single
  // digit of `output` and return 1.  In `Mantissa` mode, if `1 < olength`,
  // write a decimal radix dot between the most significant decimal digit of
  // `output` and the rest of its digits to `result` (writing going backwards)
  // and return `olength + 1`.  The behavior is undefined unless
  // `decimalLength9(output) == olength` and `result` has at least
  // `olength + 1` characters when `Mantissa == mode` or `olength` characters
  // when `DigitsOnly == mode`.

  // The following code is equivalent to:
  // const bool dot = (WriteDigitsMode == mode && olength > 1);
  // for (uint32_t i = 0; i < olength - 1; ++i) {
  //   const uint32_t c = output % 10; output /= 10;
  //   result[olength - i + (dot ? 1 : 0)] = (char) ('0' + c);
  // }
  // if (dot) result[1] = '.';
  // result[0] = '0' + output % 10;

  if (Mantissa == mode && olength > 1) {
    ++olength;  // Shift up to make space for the '.'
  }

  // Code up to the `if (output2 >= 10)` is not very interesting other than it
  // uses the optimizers' ability to turn several `memcpy` calls into writing
  // of a (potentially 64 bit long) register to memory.  This is one of the
  // things that makes Ryu faster.

  uint32_t i = 0;
  while (output >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
    const uint32_t c = output - 10000 * (output / 10000);
#else
    const uint32_t c = output % 10000;
#endif
    output /= 10000;
    const uint32_t c0 = (c % 100) << 1;
    const uint32_t c1 = (c / 100) << 1;
    memcpy(result + olength - i - 2, DIGIT_TABLE + c0, 2);
    memcpy(result + olength - i - 4, DIGIT_TABLE + c1, 2);
    i += 4;
  }
  if (output >= 100) {
    const uint32_t c = (output % 100) << 1;
    output /= 100;
    memcpy(result + olength - i - 2, DIGIT_TABLE + c, 2);
    i += 2;
  }
  if (output >= 10) {
    // We are writing the top two digits
    const uint32_t c = output << 1;
    if (Mantissa == mode) {
      // In a mantissa there is a dot after the first digit
      result[2] = DIGIT_TABLE[c + 1];
      result[1] = '.';                     // Write the radix point
      result[0] = DIGIT_TABLE[c];
      ++i;                                 // Count the radix point
    } else {
      memcpy(result, DIGIT_TABLE + c, 2);  // Just the usual fast writing
    }
    i += 2;  // Written 2 digits in either `mode`
  } else {
      // We get here when writing a single top digit.  It means an `output` with
      // odd number of decimal digits (`olength % 2 == 1`).  If that odd number
      // is not 1, we need to write a decimal radix dot into the position that
      // follows it.  In other words we only write a dot if there are decimal
      // digits in the scientific significand.
    if (Mantissa == mode && 1 != olength) {
      result[1] = '.';                       // Write the radix point
      ++i;                                   // Count the radix point
    }
    result[0] = (char) ('0' + output);       // Write a single digit
    ++i;                                     // then count it
  }

  // Return the number of characters written
  return i;
}

static inline int write_scientific(uint32_t olength, const floating_decimal_32 v, char* const result) {
  // Write the positive number represented by the specified `v` in scientific
  // notation into the specified `result`.  The specified `olength` is the
  // pre-calculated number of decimal digits in `v.mantissa`.  The behavior is
  // undefined unless `decimalLength9(v.mantissa) == olength` and `result` has
  // at least 14 characters.

  // Write the mantissa.
  int32_t index = write_digits(olength, v.mantissa, Mantissa, result);

  // Write the exponent.
  result[index++] = 'E';
  int32_t exp = v.exponent + (int32_t)olength - 1;
  if (exp < 0) {
    result[index++] = '-';
    exp = -exp;  // Notice that we flip `exp` here to be positive
  } else {
    result[index++] = '+';  // ISO C++ requires sign for the exponent.
  }

  // ISO C++ requires at least two exponent digits.
  memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
  index += 2;

  // Return the number of characters written
  return index;
}

static inline int write_decimal(uint32_t olength, const floating_decimal_32 v, char* const result) {
  // Write the positive number represented by the specified `v` in decimal
  // notation into the specified `result`.  The specified `olength` is the
  // pre-calculated number of decimal digits in `v.mantissa`.  The behavior is
  // undefined unless `decimalLength9(v.mantissa) == olength` and `result` has
  // at least 14 characters.

  // We may change `output`, but `exponent` is constant
  uint32_t      output   = v.mantissa;
  const int32_t exponent = v.exponent;

  // The number of characters written
  int32_t index = 0;

  // The condition `-exponent >= (int32_t)olength` really means the more
  // involved `exponent < 0 && -exponent >= (int32_t)olength` because we know
  // `1 <= olength <= 9`.  Positive `exponent` will make `-exponent` negative,
  // so it can never be larger than `olength` that is positive.  If the
  // `exponent` is `0`, it will still be smaller.
  //
  // It is safe to cast the unsigned `olength` to signed `int32_t` as its range
  // representable even on a byte.  However it would not be safe to compare a
  // signed and unsigned value as negative `-exponent` values would get
  // converted to large positive numbers.
  if (-exponent >= (int32_t)olength) {  // abs(value) < 1: no integer portion
    // Write the zero integer portion and the decimal radix.
    memcpy(result, "0.", 2);
    index += 2;

    // Calculate how much of the `exponent` is "used up" by moving all digits
    // of `output` behind the radix point.  `olength` has he number of digits
    // in `output`.  If any of the negative exponent is left we need to write
    // that many zeros after the decimal dot.  The cast is necessary to silence
    // warnings.
    const uint32_t rest = (uint32_t)(-exponent) - olength;
    if (rest > 0) {
      memset(result + index, '0', rest);
      index += rest;
    }
  } else if (exponent < 0) { // Has integer and fractional portion
    // When we get to this branch we don't just know that `exponent` is
    // negative, but also know that its value is *less* than the number of
    // digits in `output`, because this is an `else` branch as well.

    // Obtain the 10^-exponent value so we can "split" the number to integer
    // and fractional parts.
    const uint64_t divider = ten_to_exp_for_decimal_notation(-exponent);

    // Determine the integer part that we write first
    const uint32_t outInt = output / divider;

    // Notice that we put the fractional part back into `output` for later use
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
    output = output - outInt * divider;
#else
    output = output % divider;
#endif

    // Write the integer part
    index += write_digits(olength + exponent,  // `exponent` is negative
                          outInt,              // these digits
                          DigitsOnly,
                          result + index);     // to this position

    result[index++] = '.';  // Insert the radix point

    // The decimal portion may start with zeros.  `write_digits` does not write
    // leading zeros, so we do it here.  `exponent` is negative, and its
    // absolute value is the number of decimal digits we need to write.  So
    // `-exponent` is minus the actual number of digits we have (in `output`)
    // is the number of leading decimal zeros to write.  For example `1.0025`
    // will have `25 == output` and `-4 == exponent`:
    olength = decimalLength9(output);
    const uint32_t numLeadZeros = -exponent - olength;
    if (numLeadZeros > 0) {
        memset(result + index, '0', numLeadZeros);
        index += numLeadZeros;
    }
  }

  // Writes the digits of `output` to `result + index`.  Notice that in
  // `output` we may have fractional digits, or if `0 <= exponent` we will
  // write the digits of the integer part here.
  index += write_digits(olength, output, DigitsOnly, result + index);

  // If the `exponent` is positive, it means we need zeros after our integer
  // digits we just written.
  if (exponent > 0) {
    memset(result + index, '0', exponent);
    index += exponent;
  }

  // Return the number of characters written
  return index;
}

static inline int to_chars(const floating_decimal_32 v, const bool sign, char* result) {
  // Write the decimal representation of the floating point value represented
  // by the specified `v`, having the specified `sign` into the specified
  // `result`.  Write using the shorter notation from decimal and scientific,
  // preferring decimal notation over scientific in case of a tie.  Return the
  // number of characters written.  The behavior is undefined unless `result`
  // has at least 16 characters space.  Note that we do not null terminate
  // `result`.

  // Determine the number of significant digits of the output
  const uint32_t olength = decimalLength9(v.mantissa);

#ifdef RYU_DEBUG
  printf("DIGITS=%u\n", v.mantissa);
  printf("OLEN=%u\n", olength);
  printf("EXP=%u\n", v.exponent + olength);
#endif

  // Negative values are handled here by writing the sign and adjusting the
  // `result` pointer for the rest of the functions to write after the sign.
  if (sign) {
    *result++ = '-';
  }

  // The ternary operator chooses between the two notation functions, then we
  // immediately call the chosen one.  The parentheses around the ternary
  // operator are necessary because it has much lower precedence the function
  // call operator.  The last line adjusts the number of characters written in
  // case we wrote a negative sign earlier.
  return (needs_decimal_notation(olength, v.exponent)
      ? write_decimal : write_scientific)(olength, v, result)
      + (sign ? 1 : 0);
}

int blp_f2g_buffered_n(float f, char* result) {
  // Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
  const uint32_t bits = float_to_bits(f);

#ifdef RYU_DEBUG
  printf("IN=");
  for (int32_t bit = 31; bit >= 0; --bit) {
    printf("%u", (bits >> bit) & 1);
  }
  printf("\n");
#endif

  // Shortcut for writing zero values.  Original `d2s.c` code uses
  // `copy_special_str`, but that writes in scientific format.
  switch (bits) {
  case 0:
    *result = '0';
    return 1;
  case 0x80000000:
    memcpy(result, "-0", 2);
    return 2;
  }

  // Decode bits into sign, mantissa, and exponent.
  const bool ieeeSign = ((bits >> (FLOAT_MANTISSA_BITS + FLOAT_EXPONENT_BITS)) & 1) != 0;
  const uint32_t ieeeMantissa = bits & ((1u << FLOAT_MANTISSA_BITS) - 1);
  const uint32_t ieeeExponent = (bits >> FLOAT_MANTISSA_BITS) & ((1u << FLOAT_EXPONENT_BITS) - 1);

  // Case distinction; exit early for NaN and Infinities.  Original Ryu code
  // also wrote zero values here.  We write them even earlier.
  if (ieeeExponent == ((1u << FLOAT_EXPONENT_BITS) - 1u)) {
    return copy_special_str(result, ieeeSign, ieeeExponent, ieeeMantissa);
  }

  const floating_decimal_32 v = f2d(ieeeMantissa, ieeeExponent);
  return to_chars(v, ieeeSign, result);
}

void blp_f2g_buffered(float f, char* result) {
  const int index = blp_f2g_buffered_n(f, result);

  // Terminate the string.
  result[index] = '\0';
}
