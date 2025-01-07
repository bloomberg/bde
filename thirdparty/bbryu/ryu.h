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
#ifndef RYU_H
#define RYU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

int ryu_d2s_buffered_n(double f, char* result);
void ryu_d2s_buffered(double f, char* result);
char* ryu_d2s(double f);

int ryu_f2s_buffered_n(float f, char* result);
void ryu_f2s_buffered(float f, char* result);
char* ryu_f2s(float f);

int ryu_d2fixed_buffered_n(double d, uint32_t precision, char* result);
void ryu_d2fixed_buffered(double d, uint32_t precision, char* result);
char* ryu_d2fixed(double d, uint32_t precision);

int ryu_d2exp_buffered_n(double d, uint32_t precision, char* result);
void ryu_d2exp_buffered(double d, uint32_t precision, char* result);
char* ryu_d2exp(double d, uint32_t precision);

#ifdef __cplusplus
}
#endif

#endif // RYU_H
