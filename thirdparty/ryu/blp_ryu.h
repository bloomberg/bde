// blp_ryu.h                                                            -*-C-*-
#ifndef BLP_RYU_H
#define BLP_RYU_H

#ifdef __cplusplus
extern "C" {
#endif

// Special values are written as follows:
// --------------------------------------
// * negative zero is written with a preceding negative sign
// * Infinity values are written as '"+INF"' and '"-INF"'
// * NaN values are written as '"NaN"'
//
// We follow the XSD 1.1 mapping specification for non-numeric values and write
// "+INF" with a sign as that is how our XML encoder traditionally worked.

// The 'm' functions in this file convert IEEE-754 binary floating point
// numbers to their decimal, textual representation in the ISO C++ defined
// shortest round-trip format that uses the shorter of decimal and scientific
// formats. If both formats yield the same length, the decimal format is
// preferred.  If decimal format is chosen integers will be written precisely,
// even if not strictly necessary for the round-trip property.  To
// differentiate this format from all others we call it minimal.

int blp_d2m_buffered_n(double f, char* result);
int blp_f2m_buffered_n(float  f, char* result);
  // Write the minimal format of the specified 'f' to the specified 'result'
  // buffer and return the number of characters written.  This function does
  // not terminate 'result' with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 24 writable characters for when
  // `f` is `double` and 15 such characters for `float`.

void blp_d2m_buffered(double f, char* result);
void blp_f2m_buffered(float  f, char* result);
  // Write the minimal format of the specified 'f' to the specified 'result'
  // buffer and terminate it with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 25 writable characters for when
  // `f` is `double` and 16 such characters for `float`.

// The 'd' functions in this file convert IEEE-754 binary floating point
// numbers to their decimal, textual representation in so-called decimal or
// fixed format that consist of an optional negative sign, followed by at least
// one integer digit, followed by an optional radix point and the fractional
// digits.  These functions also print the minimum number of digits necessary
// for round-tripping, as well as have an exemption for printing integers
// precisely, just like the 'm' format.

int blp_d2d_buffered_n(double f, char* result);
int blp_f2d_buffered_n(float  f, char* result);
  // Write the decimal format of the specified 'f' to the specified 'result'
  // buffer and return the number of characters written.  This function does
  // not terminate 'result' with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 327 writable characters for when
  // `f` is `double` and 48 such characters for `float`.

void blp_d2d_buffered(double f, char* result);
void blp_f2d_buffered(float  f, char* result);
  // Write the decimal format of the specified 'f' to the specified 'result'
  // buffer and terminate it with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 328 writable characters for when
  // `f` is `double` and 49 such characters for `float`.

// The 's' functions in this file convert IEEE-754 binary floating point
// numbers to their scientific, textual representation in the format defined by
// C++ that has at least 2 exponent digits and always writes the sign of the
// exponent.  These functions also print the minimum number of digits necessary
// for round-tripping.  The 's' function do not print integers differently from
// other numbers.

int blp_d2s_buffered_n(double f, char* result);
int blp_f2s_buffered_n(float  f, char* result);
  // Write the scientific format of the specified 'f' to the specified 'result'
  // buffer and return the number of characters written.  This function does
  // not terminate 'result' with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 24 writable characters for when
  // `f` is `double` and 15 such characters for `float`.

void blp_d2s_buffered(double f, char* result);
void blp_f2s_buffered(float  f, char* result);
  // Write the scientific format of the specified 'f' to the specified 'result'
  // buffer and terminate it with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 25 writable characters for when
  // `f` is `double` and 16 such characters for `float`.

#ifdef __cplusplus
}
#endif

#endif // BLP_RYU_H

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
