 /************************************************************************
 *
 * rw_sigdefs.h - definitions of helpers macros to define member and
 *                non-member functions overload id's
 *
 * $Id: rw_sigdefs.h 509962 2007-02-21 10:38:19Z faridz $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 **************************************************************************/

#ifndef RW_SIGDEFS_H_INCLUDED
#define RW_SIGDEFS_H_INCLUDED

// FCAT() concatenates prefix, underscrore, and suffix
#define _FCAT(a, b)             a ## b
#define FCAT(a, b)              _FCAT (a ## _, b)

// FID_N() constructs the name for an overload of a container function
#define FID_0(f)                FCAT (f, void)
#define FID_1(f, a)             FCAT (f, a)
#define FID_2(f, a, b)          FID_1 (FCAT (f, a), b)
#define FID_3(f, a, b, c)       FID_2 (FCAT (f, a), b, c)
#define FID_4(f, a, b, c, d)    FID_3 (FCAT (f, a), b, c, d)
#define FID_5(f, a, b, c, d, e) FID_4 (FCAT (f, a), b, c, d, e)

// ARG() creates a bitmap of an argument type at the given position
#define ARG(a, N)   ((arg_ ## a << (N * arg_bits)) << fid_bits)

// SIG_N() creates an argument bitmap for the given function signature
#define SIG_0(f)                   fid_ ## f
#define SIG_1(f, a)                SIG_0 (f) | ARG (a, 0)
#define SIG_2(f, a, b)             SIG_1 (f, a) | ARG (b, 1)
#define SIG_3(f, a, b, c)          SIG_2 (f, a, b) | ARG (c, 2)
#define SIG_4(f, a, b, c, d)       SIG_3 (f, a, b, c) | ARG (d, 3)
#define SIG_5(f, a, b, c, d, e)    SIG_4 (f, a, b, c, d) | ARG (e, 4)
#define SIG_6(f, a, b, c, d, e, g) SIG_5 (f, a, b, c, d, e) | ARG (g, 5)

// convenience macro to define member function overload id's
// where the first argument encodes the constness of the member
// function (or the lack thereof)
#define MEMBER_0(f, self) \
    FID_0 (f) = SIG_1 (f, self) | bit_member
#define MEMBER_1(f, self, a) \
    FID_1 (f, a) = SIG_2 (f, self, a) | bit_member
#define MEMBER_2(f, self, a, b) \
    FID_2 (f, a, b) = SIG_3 (f, self, a, b) | bit_member
#define MEMBER_3(f, self, a, b, c) \
    FID_3 (f, a, b, c) = SIG_4 (f, self, a, b, c) | bit_member
#define MEMBER_4(f, self, a, b, c, d) \
    FID_4 (f, a, b, c, d) = SIG_5 (f, self, a, b, c, d) | bit_member
#define MEMBER_5(f, self, a, b, c, d, e) \
    FID_5 (f, a, b, c, d, e) = SIG_6 (f, self, a, b, c, d, e) | bit_member

// convenience macro to define non-member function overload id's
#define NON_MEMBER_0(f) \
    FID_0 (f) = SIG_0 (f)
#define NON_MEMBER_1(f, a) \
    FID_1 (f, a) = SIG_1 (f, a)
#define NON_MEMBER_2(f, a, b) \
    FID_2 (f, a, b) = SIG_2 (f, a, b)
#define NON_MEMBER_3(f, a, b, c) \
    FID_3 (f, a, b, c) = SIG_3 (f, a, b, c)
#define NON_MEMBER_4(f, a, b, c, d) \
    FID_4 (f, a, b, c, d) = SIG_4 (f, a, b, c, d)
#define NON_MEMBER_5(f, a, b, c, d, e) \
    FID_5 (f, a, b, c, d, e) = SIG_5 (f, a, b, c, d, e)

#else   // #ifdef RW_SIGDEFS_H_INCLUDED

// clean up helper macros used above
#undef _FCAT
#undef FCAT
#undef FID_0
#undef FID_1
#undef FID_2
#undef FID_3
#undef FID_4
#undef FID_5

#undef ARG

#undef SIG_0
#undef SIG_1
#undef SIG_2
#undef SIG_3
#undef SIG_4
#undef SIG_5

#undef MEMBER_0
#undef MEMBER_1
#undef MEMBER_2
#undef MEMBER_3
#undef MEMBER_4
#undef MEMBER_5

#undef RW_SIGDEFS_H_INCLUDED

#endif  // RW_SIGDEFS_H_INCLUDED
