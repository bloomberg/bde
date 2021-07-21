// blp_common.h                                                         -*-C-*-
#ifndef BLP_RYU_COMMON_H
#define BLP_RYU_COMMON_H

#include <stdint.h>

static inline bool needs_decimal_notation(uint32_t olength, int32_t exponent) {
  // Return 'false' if the ISO C++ scientifc notation for floating point
  // numbers having the specified 'olength' significant digits, and the
  // specified 'exponent' decimal exponet is shorter than the decimal notation.
  // Otherwise, if the decimal notation is not longer than the scientific
  // notation retrun 'true'.  The behavios is undefined unless
  // `0 < olength <= 17` and `-324 <= exponent <= 308`.

  // Inspired by xcharconv_ryu.h of Stephan T Lavavej and Microsoft, that has
  // Apache 2.0 License just like our code does
  int32_t low, high;
  if (olength == 1) { // One significant digit
    //      | value | Decimal | Scientific
    //------+-------+---------+-----------
    // from | 1e-3  | "0.001" | "1e-03"
    // to   | 1e4   | "10000" | "1e+04"
    low = -3;
    high = 4;
  }
  else {
    //      | value   | Decimal     | Scientific
    //------+---------+-------------+-----------
    // from | 1234e-7 | "0.0001234" | "1.234e-04"
    // to   | 1234e5  | "123400000" | "1.234e+08"
    low = -(int32_t)(olength + 3);
    high = 5;
  }

  return (low <= exponent && exponent <= high);
}

static inline uint64_t ten_to_exp_for_decimal_notation(uint32_t exponent) {
  // Return 10 to the specified 'exponent'.  The behavior is undefined unless
  // '0 <= exponent < 16'.

  static const uint64_t ten_to_the[17] = {
 // 0  1   2    3     4      5       6        7         8          9
    0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
 // 10           11            12             13              14
    10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000,
 // 15                  16
    1000000000000000ul, 10000000000000000ul
  };
  return ten_to_the[exponent];
}

typedef enum WriteDigitsMode {
  // An enumeration that determines the behavior of `write_digits` functions.
  DigitsOnly,  // Just write the digits in `output`
  Mantissa     // Write a '.' after the first digit if `output > 9`
} WriteDigitsMode;

#endif // BLP_RYU_COMMON_H

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
