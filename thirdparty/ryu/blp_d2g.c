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
// -DRYU_ONLY_64_BIT_OPS Avoid using uint128_t or 64-bit intrinsics. Slower,
//     depending on your compiler.
//
// -DRYU_OPTIMIZE_SIZE Use smaller lookup tables. Instead of storing every
//     required power of 5, only store every 26th entry, and compute
//     intermediate values with a multiplication. This reduces the lookup table
//     size by about 10x (only one case, and only double) at the cost of some
//     performance. Currently requires MSVC intrinsics.
//
// Bloomberg LP CHANGELOG
//
// 2021.07.16 Initial version built from d2s.c of Ulf's Ryu code
//            See README.BDE.md for further explanations.

#include "ryu/blp_ryu.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef RYU_DEBUG
#include <inttypes.h>
#include <stdio.h>
#endif

#include "ryu/blp_common.h"
#include "ryu/common.h"
#include "ryu/digit_table.h"
#include "ryu/d2s_intrinsics.h"

// Include either the small or the full lookup tables depending on the mode.
#if defined(RYU_OPTIMIZE_SIZE)
#include "ryu/d2s_small_table.h"
#else
#include "ryu/d2s_full_table.h"
#endif

#define DOUBLE_MANTISSA_BITS 52
#define DOUBLE_EXPONENT_BITS 11
#define DOUBLE_BIAS 1023

static inline uint32_t decimalLength17(const uint64_t v) {
  // This is slightly faster than a loop.
  // The average output length is 16.38 digits, so we check high-to-low.
  // Function precondition: v is not an 18, 19, or 20-digit number.
  // (17 digits are sufficient for round-tripping.)
  assert(v < 100000000000000000L);
  if (v >= 10000000000000000L) { return 17; }
  if (v >= 1000000000000000L) { return 16; }
  if (v >= 100000000000000L) { return 15; }
  if (v >= 10000000000000L) { return 14; }
  if (v >= 1000000000000L) { return 13; }
  if (v >= 100000000000L) { return 12; }
  if (v >= 10000000000L) { return 11; }
  if (v >= 1000000000L) { return 10; }
  if (v >= 100000000L) { return 9; }
  if (v >= 10000000L) { return 8; }
  if (v >= 1000000L) { return 7; }
  if (v >= 100000L) { return 6; }
  if (v >= 10000L) { return 5; }
  if (v >= 1000L) { return 4; }
  if (v >= 100L) { return 3; }
  if (v >= 10L) { return 2; }
  return 1;
}

// A floating decimal representing m * 10^e.
typedef struct floating_decimal_64 {
  uint64_t mantissa;
  // Decimal exponent's range is -324 to 308
  // inclusive, and can fit in a short if needed.
  int32_t exponent;
} floating_decimal_64;

static inline floating_decimal_64 d2d(const uint64_t ieeeMantissa, const uint32_t ieeeExponent) {
  int32_t e2;
  uint64_t m2;
  if (ieeeExponent == 0) {
    // We subtract 2 so that the bounds computation has 2 additional bits.
    e2 = 1 - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS - 2;
    m2 = ieeeMantissa;
  } else {
    e2 = (int32_t) ieeeExponent - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS - 2;
    m2 = (1ull << DOUBLE_MANTISSA_BITS) | ieeeMantissa;
  }
  const bool even = (m2 & 1) == 0;
  const bool acceptBounds = even;

#ifdef RYU_DEBUG
  printf("-> %" PRIu64 " * 2^%d\n", m2, e2 + 2);
#endif

  // Step 2: Determine the interval of valid decimal representations.
  const uint64_t mv = 4 * m2;
  // Implicit bool -> int conversion. True is 1, false is 0.
  const uint32_t mmShift = ieeeMantissa != 0 || ieeeExponent <= 1;
  // We would compute mp and mm like this:
  // uint64_t mp = 4 * m2 + 2;
  // uint64_t mm = mv - 1 - mmShift;

  // Step 3: Convert to a decimal power base using 128-bit arithmetic.
  uint64_t vr, vp, vm;
  int32_t e10;
  bool vmIsTrailingZeros = false;
  bool vrIsTrailingZeros = false;
  if (e2 >= 0) {
    // I tried special-casing q == 0, but there was no effect on performance.
    // This expression is slightly faster than max(0, log10Pow2(e2) - 1).
    const uint32_t q = log10Pow2(e2) - (e2 > 3);
    e10 = (int32_t) q;
    const int32_t k = DOUBLE_POW5_INV_BITCOUNT + pow5bits((int32_t) q) - 1;
    const int32_t i = -e2 + (int32_t) q + k;
#if defined(RYU_OPTIMIZE_SIZE)
    uint64_t pow5[2];
    double_computeInvPow5(q, pow5);
    vr = mulShiftAll64(m2, pow5, i, &vp, &vm, mmShift);
#else
    vr = mulShiftAll64(m2, DOUBLE_POW5_INV_SPLIT[q], i, &vp, &vm, mmShift);
#endif
#ifdef RYU_DEBUG
    printf("%" PRIu64 " * 2^%d / 10^%u\n", mv, e2, q);
    printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
#endif
    if (q <= 21) {
      // This should use q <= 22, but I think 21 is also safe. Smaller values
      // may still be safe, but it's more difficult to reason about them.
      // Only one of mp, mv, and mm can be a multiple of 5, if any.
      const uint32_t mvMod5 = ((uint32_t) mv) - 5 * ((uint32_t) div5(mv));
      if (mvMod5 == 0) {
        vrIsTrailingZeros = multipleOfPowerOf5(mv, q);
      } else if (acceptBounds) {
        // Same as min(e2 + (~mm & 1), pow5Factor(mm)) >= q
        // <=> e2 + (~mm & 1) >= q && pow5Factor(mm) >= q
        // <=> true && pow5Factor(mm) >= q, since e2 >= q.
        vmIsTrailingZeros = multipleOfPowerOf5(mv - 1 - mmShift, q);
      } else {
        // Same as min(e2 + 1, pow5Factor(mp)) >= q.
        vp -= multipleOfPowerOf5(mv + 2, q);
      }
    }
  } else {
    // This expression is slightly faster than max(0, log10Pow5(-e2) - 1).
    const uint32_t q = log10Pow5(-e2) - (-e2 > 1);
    e10 = (int32_t) q + e2;
    const int32_t i = -e2 - (int32_t) q;
    const int32_t k = pow5bits(i) - DOUBLE_POW5_BITCOUNT;
    const int32_t j = (int32_t) q - k;
#if defined(RYU_OPTIMIZE_SIZE)
    uint64_t pow5[2];
    double_computePow5(i, pow5);
    vr = mulShiftAll64(m2, pow5, j, &vp, &vm, mmShift);
#else
    vr = mulShiftAll64(m2, DOUBLE_POW5_SPLIT[i], j, &vp, &vm, mmShift);
#endif
#ifdef RYU_DEBUG
    printf("%" PRIu64 " * 5^%d / 10^%u\n", mv, -e2, q);
    printf("%u %d %d %d\n", q, i, k, j);
    printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
#endif
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
    } else if (q < 63) { // TODO(ulfjack): Use a tighter bound here.
      // We want to know if the full product has at least q trailing zeros.
      // We need to compute min(p2(mv), p5(mv) - e2) >= q
      // <=> p2(mv) >= q && p5(mv) - e2 >= q
      // <=> p2(mv) >= q (because -e2 >= q)
      vrIsTrailingZeros = multipleOfPowerOf2(mv, q);
#ifdef RYU_DEBUG
      printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
    }
  }
#ifdef RYU_DEBUG
  printf("e10=%d\n", e10);
  printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
  printf("vm is trailing zeros=%s\n", vmIsTrailingZeros ? "true" : "false");
  printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif

  // Step 4: Find the shortest decimal representation in the interval of valid representations.
  int32_t removed = 0;
  uint8_t lastRemovedDigit = 0;
  uint64_t output;
  // On average, we remove ~2 digits.
  if (vmIsTrailingZeros || vrIsTrailingZeros) {
    // General case, which happens rarely (~0.7%).
    for (;;) {
      const uint64_t vpDiv10 = div10(vp);
      const uint64_t vmDiv10 = div10(vm);
      if (vpDiv10 <= vmDiv10) {
        break;
      }
      const uint32_t vmMod10 = ((uint32_t) vm) - 10 * ((uint32_t) vmDiv10);
      const uint64_t vrDiv10 = div10(vr);
      const uint32_t vrMod10 = ((uint32_t) vr) - 10 * ((uint32_t) vrDiv10);
      vmIsTrailingZeros &= vmMod10 == 0;
      vrIsTrailingZeros &= lastRemovedDigit == 0;
      lastRemovedDigit = (uint8_t) vrMod10;
      vr = vrDiv10;
      vp = vpDiv10;
      vm = vmDiv10;
      ++removed;
    }
#ifdef RYU_DEBUG
    printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
    printf("d-10=%s\n", vmIsTrailingZeros ? "true" : "false");
#endif
    if (vmIsTrailingZeros) {
      for (;;) {
        const uint64_t vmDiv10 = div10(vm);
        const uint32_t vmMod10 = ((uint32_t) vm) - 10 * ((uint32_t) vmDiv10);
        if (vmMod10 != 0) {
          break;
        }
        const uint64_t vpDiv10 = div10(vp);
        const uint64_t vrDiv10 = div10(vr);
        const uint32_t vrMod10 = ((uint32_t) vr) - 10 * ((uint32_t) vrDiv10);
        vrIsTrailingZeros &= lastRemovedDigit == 0;
        lastRemovedDigit = (uint8_t) vrMod10;
        vr = vrDiv10;
        vp = vpDiv10;
        vm = vmDiv10;
        ++removed;
      }
    }
#ifdef RYU_DEBUG
    printf("%" PRIu64 " %d\n", vr, lastRemovedDigit);
    printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
    if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
      // Round even if the exact number is .....50..0.
      lastRemovedDigit = 4;
    }
    // We need to take vr + 1 if vr is outside bounds or we need to round up.
    output = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
  } else {
    // Specialized for the common case (~99.3%). Percentages below are relative to this.
    bool roundUp = false;
    const uint64_t vpDiv100 = div100(vp);
    const uint64_t vmDiv100 = div100(vm);
    if (vpDiv100 > vmDiv100) { // Optimization: remove two digits at a time (~86.2%).
      const uint64_t vrDiv100 = div100(vr);
      const uint32_t vrMod100 = ((uint32_t) vr) - 100 * ((uint32_t) vrDiv100);
      roundUp = vrMod100 >= 50;
      vr = vrDiv100;
      vp = vpDiv100;
      vm = vmDiv100;
      removed += 2;
    }
    // Loop iterations below (approximately), without optimization above:
    // 0: 0.03%, 1: 13.8%, 2: 70.6%, 3: 14.0%, 4: 1.40%, 5: 0.14%, 6+: 0.02%
    // Loop iterations below (approximately), with optimization above:
    // 0: 70.6%, 1: 27.8%, 2: 1.40%, 3: 0.14%, 4+: 0.02%
    for (;;) {
      const uint64_t vpDiv10 = div10(vp);
      const uint64_t vmDiv10 = div10(vm);
      if (vpDiv10 <= vmDiv10) {
        break;
      }
      const uint64_t vrDiv10 = div10(vr);
      const uint32_t vrMod10 = ((uint32_t) vr) - 10 * ((uint32_t) vrDiv10);
      roundUp = vrMod10 >= 5;
      vr = vrDiv10;
      vp = vpDiv10;
      vm = vmDiv10;
      ++removed;
    }
#ifdef RYU_DEBUG
    printf("%" PRIu64 " roundUp=%s\n", vr, roundUp ? "true" : "false");
    printf("vr is trailing zeros=%s\n", vrIsTrailingZeros ? "true" : "false");
#endif
    // We need to take vr + 1 if vr is outside bounds or we need to round up.
    output = vr + (vr == vm || roundUp);
  }
  const int32_t exp = e10 + removed;

#ifdef RYU_DEBUG
  printf("V+=%" PRIu64 "\nV =%" PRIu64 "\nV-=%" PRIu64 "\n", vp, vr, vm);
  printf("O=%" PRIu64 "\n", output);
  printf("EXP=%d\n", exp);
#endif

  floating_decimal_64 fd;
  fd.exponent = exp;
  fd.mantissa = output;
  return fd;
}

static inline int write_digits(uint32_t olength, uint64_t output, WriteDigitsMode mode, char* const result) {
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
  // `decimalLength17(output) == olength` and `result` has at least
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
  // We prefer 32-bit operations, even on 64-bit platforms.
  // We have at most 17 digits, and uint32_t can store 9 digits.
  // If output doesn't fit into uint32_t, we cut off 8 digits,
  // so the rest will fit into uint32_t.
  if ((output >> 32) != 0) {
    // Expensive 64-bit division.
    const uint64_t q = div1e8(output);
    uint32_t output2 = ((uint32_t) output) - 100000000 * ((uint32_t) q);
    output = q;

    const uint32_t c = output2 % 10000;
    output2 /= 10000;
    const uint32_t d = output2 % 10000;
    const uint32_t c0 = (c % 100) << 1;
    const uint32_t c1 = (c / 100) << 1;
    const uint32_t d0 = (d % 100) << 1;
    const uint32_t d1 = (d / 100) << 1;
    memcpy(result + olength - i - 2, DIGIT_TABLE + c0, 2);
    memcpy(result + olength - i - 4, DIGIT_TABLE + c1, 2);
    memcpy(result + olength - i - 6, DIGIT_TABLE + d0, 2);
    memcpy(result + olength - i - 8, DIGIT_TABLE + d1, 2);
    i += 8;
  }
  uint32_t output2 = (uint32_t) output;
  while (output2 >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
    const uint32_t c = output2 - 10000 * (output2 / 10000);
#else
    const uint32_t c = output2 % 10000;
#endif
    output2 /= 10000;
    const uint32_t c0 = (c % 100) << 1;
    const uint32_t c1 = (c / 100) << 1;
    memcpy(result + olength - i - 2, DIGIT_TABLE + c0, 2);
    memcpy(result + olength - i - 4, DIGIT_TABLE + c1, 2);
    i += 4;
  }
  if (output2 >= 100) {
    const uint32_t c = (output2 % 100) << 1;
    output2 /= 100;
    memcpy(result + olength - i - 2, DIGIT_TABLE + c, 2);
    i += 2;
  }
  if (output2 >= 10) {
    // We are writing the top two digits
    const uint32_t c = output2 << 1;
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
    result[0] = (char) ('0' + output2);      // Write a single digit
    ++i;                                     // then count it
  }

  // Return the number of characters written
  return i;
}

static inline int write_scientific(uint32_t olength, const floating_decimal_64 v, char* const result) {
  // Write the positive number represented by the specified `v` in scientific
  // notation into the specified `result`.  The specified `olength` is the
  // pre-calculated number of decimal digits in `v.mantissa`.  The behavior is
  // undefined unless `decimalLength17(v.mantissa) == olength` and `result` has
  // at least 23 characters.

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

  if (exp >= 100) {
    const int32_t c = exp % 10;
    memcpy(result + index, DIGIT_TABLE + 2 * (exp / 10), 2);
    result[index + 2] = (char) ('0' + c);
    index += 3;
  } else {
    // ISO C++ requires at least two exponent digits.
    memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
    index += 2;
  }

  // Return the number of characters written.
  return index;
}

static inline int write_decimal(uint32_t olength, const floating_decimal_64 v, char* const result) {
  // Write the positive number represented by the specified `v` in decimal
  // notation into the specified `result`.  The specified `olength` is the
  // pre-calculated number of decimal digits in `v.mantissa`.  The behavior is
  // undefined unless `decimalLength17(v.mantissa) == olength` and `result` has
  // at least 23 characters.

  // We may change `output`, but `exponent` is constant
  uint64_t      output   = v.mantissa;
  const int32_t exponent = v.exponent;

  // The number of characters written
  int32_t index = 0;

  // The condition `-exponent >= (int32_t)olength` really means the more
  // involved `exponent < 0 && -exponent >= (int32_t)olength` because we know
  // `1 <= olength <= 17`.  Positive `exponent` will make `-exponent` negative,
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
    const uint64_t outInt = output / divider;

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
    olength = decimalLength17(output);
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

static inline int to_chars(const floating_decimal_64 v, const bool sign, char* result) {
  // Write the decimal representation of the floating point value represented
  // by the specified `v`, having the specified `sign` into the specified
  // `result`.  Write using the shorter notation from decimal and scientific,
  // preferring decimal notation over scientific in case of a tie.  Return the
  // number of characters written.  The behavior is undefined unless `result`
  // has at least 24 characters space.  Note that we do not null terminate
  // `result`.

  // Determine the number of significant digits of the output
  const uint32_t olength = decimalLength17(v.mantissa);

#ifdef RYU_DEBUG
  printf("DIGITS=%" PRIu64 "\n", v.mantissa);
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

static inline bool d2d_small_int(const uint64_t ieeeMantissa, const uint32_t ieeeExponent,
  floating_decimal_64* const v) {
  const uint64_t m2 = (1ull << DOUBLE_MANTISSA_BITS) | ieeeMantissa;
  const int32_t e2 = (int32_t) ieeeExponent - DOUBLE_BIAS - DOUBLE_MANTISSA_BITS;

  if (e2 > 0) {
    // f = m2 * 2^e2 >= 2^53 is an integer.
    // Ignore this case for now.
    return false;
  }

  if (e2 < -52) {
    // f < 1.
    return false;
  }

  // Since 2^52 <= m2 < 2^53 and 0 <= -e2 <= 52: 1 <= f = m2 / 2^-e2 < 2^53.
  // Test if the lower -e2 bits of the significand are 0, i.e. whether the fraction is 0.
  const uint64_t mask = (1ull << -e2) - 1;
  const uint64_t fraction = m2 & mask;
  if (fraction != 0) {
    return false;
  }

  // f is an integer in the range [1, 2^53).
  // Note: mantissa might contain trailing (decimal) 0's.
  // Note: since 2^53 < 10^16, there is no need to adjust decimalLength17().
  v->mantissa = m2 >> -e2;
  v->exponent = 0;
  return true;
}

int blp_d2g_buffered_n(double f, char* result) {
  // Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
  const uint64_t bits = double_to_bits(f);

#ifdef RYU_DEBUG
  printf("IN=");
  for (int32_t bit = 63; bit >= 0; --bit) {
    printf("%d", (int) ((bits >> bit) & 1));
  }
  printf("\n");
#endif

  // Shortcut for writing zero values.  Original `d2s.c` code uses
  // `copy_special_str`, but that writes in scientific format.
  switch (bits) {
  case 0:
    *result = '0';
    return 1;
  case 0x8000000000000000:
    memcpy(result, "-0", 2);
    return 2;
  }

  // Decode bits into sign, mantissa, and exponent.
  const bool ieeeSign = ((bits >> (DOUBLE_MANTISSA_BITS + DOUBLE_EXPONENT_BITS)) & 1) != 0;
  const uint64_t ieeeMantissa = bits & ((1ull << DOUBLE_MANTISSA_BITS) - 1);
  const uint32_t ieeeExponent = (uint32_t) ((bits >> DOUBLE_MANTISSA_BITS) & ((1u << DOUBLE_EXPONENT_BITS) - 1));

  // Case distinction; exit early for NaN and Infinities.  Original Ryu code
  // also wrote zero values here.  We write them even earlier.
  if (ieeeExponent == ((1u << DOUBLE_EXPONENT_BITS) - 1u)) {
    return copy_special_str(result, ieeeSign, ieeeExponent, ieeeMantissa);
  }

  floating_decimal_64 v;
  const bool isSmallInt = d2d_small_int(ieeeMantissa, ieeeExponent, &v);
  if (isSmallInt) {
    // For small integers in the range [1, 2^53), v.mantissa might contain trailing (decimal) zeros.
    // For scientific notation we need to move these zeros into the exponent.
    // (This is not needed for fixed-point notation, so it might be beneficial to trim
    // trailing zeros in to_chars only if needed - once fixed-point notation output is implemented.)
    for (;;) {
      const uint64_t q = div10(v.mantissa);
      const uint32_t r = ((uint32_t) v.mantissa) - 10 * ((uint32_t) q);
      if (r != 0) {
        break;
      }
      v.mantissa = q;
      ++v.exponent;
    }
  } else {
    v = d2d(ieeeMantissa, ieeeExponent);
  }

  return to_chars(v, ieeeSign, result);
}

void blp_d2g_buffered(double f, char* result) {
  const int index = blp_d2g_buffered_n(f, result);

  // Terminate the string.
  result[index] = '\0';
}
