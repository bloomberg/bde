// blp_ryu.h                                                            -*-C-*-
#ifndef BLP_RYU_H
#define BLP_RYU_H

#ifdef __cplusplus
extern "C" {
#endif

// The functions in this file convert IEEE-754 binary floating point numbers to
// their decimal, textual representation in so-called general format that
// uses the shorter of decimal and scientific formats. If both formats yield
// the same length, the decimal format is preferred.

int blp_d2g_buffered_n(double f, char* result);
int blp_f2g_buffered_n(float  f, char* result);
  // Write the general format of the specified 'f' to the specified 'result'
  // buffer and return the number of characters written.  This function does
  // not terminate 'result' with a null character.  The behavior is undefined
  // unless the 'result' buffer has at least 24 writable characters for when
  // `f` is `double` and 15 such characters for `float`.

void blp_d2g_buffered(double f, char* result);
void blp_f2g_buffered(float  f, char* result);
  // Write the general format of the specified 'f' to the specified 'result'
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
