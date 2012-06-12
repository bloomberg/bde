/************************************************************************
 *
 * test_issue16717.cpp - regression test for Onyx issue #16717
 *
 * $Id: 17.names.cpp 594132 2007-11-12 13:49:37Z faridz $
 *
 ************************************************************************
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
 * Copyright 2001-2006 Rogue Wave Software.
 *
 **************************************************************************/

// ISSUE DESCRIPTION:
//   library headers must privatize all names other than those mandated
//   by the standard according to 17.4.3.1; this includes formal template
//   parameters and function arguments shown in the Standard
//   this test #defines the most common offenders and #includes all
//   library headers with the expectation that violations will be flagged
//   as compilation errors

#include <rw/_defs.h>
#include <driver.h>


#ifdef __SUNPRO_CC
   // included first to work around a SunPro 5.4 bug (PR #26255)
#  include <time.h>
#endif   // SunPro

// include ANSI C headers prior to attempting to induce errors in case
// the C library itself contains violations of this rule
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>

#ifndef _RWSTD_NO_ISO646_H
#  include <iso646.h>
#endif

#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif

// include system headers included by the library
#include <fcntl.h>

#if defined (_WIN32) || defined (_WIN64)
#  include <io.h>
#else
#  include <unistd.h>
#endif

#include _RWSTD_SYS_TYPES_H

#if defined (_RWSTD_SOLARIS_THREADS)
#  include <synch.h>
#  include <thread.h>
#elif defined (_RWSTD_POSIX_THREADS)
#  include <pthread.h>
#elif defined (_RWSTD_DCE_THREADS)
#  if defined (_RWSTD_NO_DCE_PTHREAD_H)
#    include <pthread.h>
#  else
#    include <dce/pthread.h>
#  endif
#elif defined (_WIN32)
#  include <windows.h>
#  if defined (_MSC_VER) && 1400 <= _MSC_VER
#    include <intrin.h>
#  endif   // 1400 <= _MSC_VER
#endif

#if TEST_RW_PEDANTIC

// symbols typically used by programmers for local names
#define a   !ERROR!
#define b   !ERROR!
#define c   !ERROR!

#if !defined (__sgi)
   // a member of __huge_val in /usr/include/math.h and elsewhere
#  define d !ERROR!
#endif

#define e   !ERROR!
#define f   !ERROR!
#define g   !ERROR!

// gets expanded in macros within #include directives
// during the second pass of the preprocessor...
// #define h   !ERROR!

#define i   !ERROR!
#define j   !ERROR!
#define k   !ERROR!
#define m   !ERROR!
#define n   !ERROR!
#define o   !ERROR!
#define p   !ERROR!
#define q   !ERROR!
#define r   !ERROR!
#define s   !ERROR!
#define t   !ERROR!
#define u   !ERROR!
#define v   !ERROR!
#define w   !ERROR!
#define x   !ERROR!
#define y   !ERROR!
#define z   !ERROR!

#define A   !ERROR!
#define B   !ERROR!
#define C   !ERROR!
#define D   !ERROR!
#define E   !ERROR!
#define F   !ERROR!
#define G   !ERROR!
#define H   !ERROR!
#define I   !ERROR!
#define J   !ERROR!
#define K   !ERROR!
#define L   !ERROR!
#define M   !ERROR!
#define N   !ERROR!
#define O   !ERROR!
#define P   !ERROR!
#define Q   !ERROR!
#define R   !ERROR!
#define S   !ERROR!
#define T   !ERROR!
#define U   !ERROR!
#define V   !ERROR!
#define W   !ERROR!
#define X   !ERROR!
#define Y   !ERROR!
#define Z   !ERROR!

// symbols typically used by programmers for names of class members
#define a_  !ERROR!
#define b_  !ERROR!
#define c_  !ERROR!
#define d_  !ERROR!
#define e_  !ERROR!
#define f_  !ERROR!
#define g_  !ERROR!
#define h_  !ERROR!
#define i_  !ERROR!
#define j_  !ERROR!
#define k_  !ERROR!
#define m_  !ERROR!
#define n_  !ERROR!
#define o_  !ERROR!
#define p_  !ERROR!
#define q_  !ERROR!
#define r_  !ERROR!
#define s_  !ERROR!
#define t_  !ERROR!
#define u_  !ERROR!
#define v_  !ERROR!
#define w_  !ERROR!
#define x_  !ERROR!
#define y_  !ERROR!
#define z_  !ERROR!

#define A_  !ERROR!
#define B_  !ERROR!
#define C_  !ERROR!
#define D_  !ERROR!
#define E_  !ERROR!
#define F_  !ERROR!
#define G_  !ERROR!
#define H_  !ERROR!
#define I_  !ERROR!
#define J_  !ERROR!
#define K_  !ERROR!
#define L_  !ERROR!
#define M_  !ERROR!
#define N_  !ERROR!
#define O_  !ERROR!
#define P_  !ERROR!
#define Q_  !ERROR!
#define R_  !ERROR!
#define S_  !ERROR!
#define T_  !ERROR!
#define U_  !ERROR!
#define V_  !ERROR!
#define W_  !ERROR!
#define X_  !ERROR!
#define Y_  !ERROR!
#define Z_  !ERROR!

#define a1  !ERROR!
#define b1  !ERROR!
#define c1  !ERROR!
#define d1  !ERROR!
#define e1  !ERROR!
#define f1  !ERROR!
#define g1  !ERROR!
#define h1  !ERROR!
#define i1  !ERROR!
#define j1  !ERROR!
#define k1  !ERROR!
#define m1  !ERROR!
#define n1  !ERROR!
#define o1  !ERROR!
#define p1  !ERROR!
#define q1  !ERROR!
#define r1  !ERROR!
#define s1  !ERROR!
#define t1  !ERROR!
#define u1  !ERROR!
#define v1  !ERROR!
#define w1  !ERROR!
#define x1  !ERROR!
#define y1  !ERROR!
#define z1  !ERROR!

#define a2  !ERROR!
#define b2  !ERROR!
#define c2  !ERROR!
#define d2  !ERROR!
#define e2  !ERROR!
#define f2  !ERROR!
#define g2  !ERROR!
#define h2  !ERROR!
#define i2  !ERROR!
#define j2  !ERROR!
#define k2  !ERROR!
#define m2  !ERROR!
#define n2  !ERROR!
#define o2  !ERROR!
#define p2  !ERROR!
#define q2  !ERROR!
#define r2  !ERROR!
#define s2  !ERROR!
#define t2  !ERROR!
#define u2  !ERROR!
#define v2  !ERROR!
#define w2  !ERROR!
#define x2  !ERROR!
#define y2  !ERROR!
#define z2  !ERROR!

#define A1  !ERROR!
#define B1  !ERROR!
#define C1  !ERROR!
#define D1  !ERROR!
#define E1  !ERROR!
#define F1  !ERROR!
#define G1  !ERROR!
#define H1  !ERROR!
#define I1  !ERROR!
#define J1  !ERROR!
#define K1  !ERROR!
#define L1  !ERROR!
#define M1  !ERROR!
#define N1  !ERROR!
#define O1  !ERROR!
#define P1  !ERROR!
#define Q1  !ERROR!
#define R1  !ERROR!
#define S1  !ERROR!
#define T1  !ERROR!
#define U1  !ERROR!
#define V1  !ERROR!
#define W1  !ERROR!
#define X1  !ERROR!
#define Y1  !ERROR!
#define Z1  !ERROR!

#define A2  !ERROR!
#define B2  !ERROR!
#define C2  !ERROR!
#define D2  !ERROR!
#define E2  !ERROR!
#define F2  !ERROR!
#define G2  !ERROR!
#define H2  !ERROR!
#define I2  !ERROR!
#define J2  !ERROR!
#define K2  !ERROR!
#define L2  !ERROR!
#define M2  !ERROR!
#define N2  !ERROR!
#define O2  !ERROR!
#define P2  !ERROR!
#define Q2  !ERROR!
#define R2  !ERROR!
#define S2  !ERROR!
#define T2  !ERROR!
#define U2  !ERROR!
#define V2  !ERROR!
#define W2  !ERROR!
#define X2  !ERROR!
#define Y2  !ERROR!
#define Z2  !ERROR!

// symbols beginning with __<N> where is a digit are reserved
// by Sun dbx (see PR #30521)
#define __2logN                 !ERROR!

// symbols reserved by gcc (run `strings cc1plus' from the shell)
// see also http://gcc.gnu.org/libstdc++/17_intro/BADNAMES
#define __aa                    !ERROR!
#define __aad                   !ERROR!
#define __ad                    !ERROR!
#define __addr                  !ERROR!
#define __adv                   !ERROR!
#define __aer                   !ERROR!
#define __als                   !ERROR!
#define __alshift               !ERROR!
#define __amd                   !ERROR!
#define __ami                   !ERROR!
#define __aml                   !ERROR!
#define __amu                   !ERROR!
#define __aor                   !ERROR!
#define __apl                   !ERROR!
#define __array                 !ERROR!
#define __ars                   !ERROR!
#define __arshift               !ERROR!
#define __as                    !ERROR!
#define __bit_and               !ERROR!
#define __bit_ior               !ERROR!
#define __bit_not               !ERROR!
#define __bit_xor               !ERROR!
#define __call                  !ERROR!
#define __cl                    !ERROR!
#define __cm                    !ERROR!
#define __cn                    !ERROR!
#define __co                    !ERROR!
#define __component             !ERROR!
#define __compound              !ERROR!
#define __cond                  !ERROR!
#define __convert               !ERROR!
#define __delete                !ERROR!
#define __dl                    !ERROR!
#define __dv                    !ERROR!
#define __eq                    !ERROR!
#define __er                    !ERROR!
#define __ge                    !ERROR!
#define __gt                    !ERROR!
#define __indirect              !ERROR!
#define __le                    !ERROR!
#define __ls                    !ERROR!
#define __lt                    !ERROR!

#ifndef _MSC_VER
   // MSVC's libc defines __max in <stdlib.h>
#  define __max                 !ERROR!
#endif

#define __md                    !ERROR!
#define __method_call           !ERROR!
#define __mi                    !ERROR!

#ifndef _MSC_VER
   // MSVC's libc defines __min in <stdlib.h>
#  define __min                 !ERROR!
#endif

#define __minus                 !ERROR!
#define __ml                    !ERROR!
#define __mm                    !ERROR!
#define __mn                    !ERROR!
#define __mult                  !ERROR!
#define __mx                    !ERROR!
#define __ne                    !ERROR!
#define __negate                !ERROR!
#define __new                   !ERROR!
#define __nop                   !ERROR!
#define __nt                    !ERROR!
#define __nw                    !ERROR!
#define __oo                    !ERROR!
#define __op                    !ERROR!
#define __or                    !ERROR!
#define __pl                    !ERROR!
#define __plus                  !ERROR!
#define __postdecrement         !ERROR!
#define __postincrement         !ERROR!
#define __pp                    !ERROR!
#define __pt                    !ERROR!
#define __rf                    !ERROR!
#define __rm                    !ERROR!
#define __rs                    !ERROR!
#define __sz                    !ERROR!
#define __trunc_div             !ERROR!
#define __trunc_mod             !ERROR!
#define __truth_andif           !ERROR!
#define __truth_not             !ERROR!
#define __truth_orif            !ERROR!
// __value is reserved by MSVC 8.0
#define __value                 !ERROR!
#define __vc                    !ERROR!
#define __vd                    !ERROR!
#define __vn                    !ERROR!

// symbols reserved by the ARM compiler
#if !defined (sun) && !defined (__sun) && !defined (__sun__)
   // MB_CUR_MAX expands to __ctype on SunOS
#  define __ctype               !ERROR!
#endif

#define __ct                    !ERROR!

// symbols typically used by C++ library implementations
#define Alloc                   !ERROR!
#define Allocator               !ERROR!
#define alloc                   !ERROR!
#define Arg                     !ERROR!
#define Arg1                    !ERROR!
#define Arg2                    !ERROR!
#define adr                     !ERROR!
#define addr                    !ERROR!
#define array                   !ERROR!
#define BidirectionalIter       !ERROR!
#define BidirectionalIterator   !ERROR!
#define BinaryPred              !ERROR!
#define BinaryPredicate         !ERROR!
#define buf                     !ERROR!
#define buffer                  !ERROR!
#define CHAR                    !ERROR!
#define Char                    !ERROR!
#define charT                   !ERROR!
#define CharT                   !ERROR!
#define cmp                     !ERROR!
#define cnt                     !ERROR!
#define cpt                     !ERROR!
#define comp                    !ERROR!
#define Container               !ERROR!
#define Compare                 !ERROR!
#define dist                    !ERROR!
#define Dist                    !ERROR!
#define Distance                !ERROR!
#define finish                  !ERROR!
#define ForwardIter             !ERROR!
#define ForwardIterator         !ERROR!
#define found                   !ERROR!
#define fun                     !ERROR!
#define Function                !ERROR!
#define hint                    !ERROR!
#define gen                     !ERROR!
#define Gen                     !ERROR!
#define Generator               !ERROR!
#define get_array               !ERROR!
#define get_slice               !ERROR!
#define gs                      !ERROR!
#define InputIter               !ERROR!
#define InputIterator           !ERROR!
#define index                   !ERROR!
#define INT                     !ERROR!
#define inx                     !ERROR!
#define ipfx                    !ERROR!
#define istrm                   !ERROR!
#define last                    !ERROR!
#define len                     !ERROR!
#define lhs                     !ERROR!
#define line                    !ERROR!
#define lineno                  !ERROR!
#define loc                     !ERROR!
#define LONG                    !ERROR!
#define med                     !ERROR!
#define mesg                    !ERROR!
#define middle                  !ERROR!
#define mode                    !ERROR!
#define msg                     !ERROR!
#define msk                     !ERROR!
#define n                       !ERROR!
#define nchar                   !ERROR!
#define nchars                  !ERROR!
#define neg                     !ERROR!
#define negative                !ERROR!
#define num                     !ERROR!
#define numb                    !ERROR!
#define off                     !ERROR!
#define offset                  !ERROR!
#define old                     !ERROR!
#define opfx                    !ERROR!
#define os                      !ERROR!
#define ostrm                   !ERROR!
#define Operation               !ERROR!
#define OutputIter              !ERROR!
#define OutputIterator          !ERROR!
#define pf                      !ERROR!
#define Pointer                 !ERROR!
#define pos                     !ERROR!
#define position                !ERROR!
#define positive                !ERROR!
#define Pred                    !ERROR!
#define Predicate               !ERROR!
#define pend                    !ERROR!
#define pending                 !ERROR!
#define ptr                     !ERROR!
#define quant                   !ERROR!
#define RandomAccessIter        !ERROR!
#define RandomAccessIterator    !ERROR!
#define Reference               !ERROR!
#define Result                  !ERROR!
#define res                     !ERROR!

// used by std::codecvt_base::result
// #define result                  !ERROR!

#define rhs                     !ERROR!
#define rlen                    !ERROR!
#define save                    !ERROR!
#define sec                     !ERROR!
#define sl                      !ERROR!
#define sla                     !ERROR!
#define sb                      !ERROR!
#define SCHAR                   !ERROR!
#define Size                    !ERROR!
#define slen                    !ERROR!
#define smanip                  !ERROR!
#define start                   !ERROR!
#define stream                  !ERROR!
#define strm                    !ERROR!
#define sz                      !ERROR!
#define tag                     !ERROR!
#define temp                    !ERROR!
#define tmp                     !ERROR!
#define Tp                      !ERROR!
#define traits                  !ERROR!
#define Traits                  !ERROR!
#define type                    !ERROR!
#define Type                    !ERROR!
#define UCHAR                   !ERROR!
#define UINT                    !ERROR!
#define ULONG                   !ERROR!
#define ULLONG                  !ERROR!
#define USHRT                   !ERROR!
#define UnaryFunction           !ERROR!
#define val                     !ERROR!
// value used in type traits
// #define value                   !ERROR!
#define var                     !ERROR!
#define way                     !ERROR!
#define which                   !ERROR!
#define xpos                    !ERROR!

// verify that the library gracefully handles
// the frequently #defined function macros
#undef max
#define max()                   !ERROR!
#undef min
#define min()                   !ERROR!

// symbols the DEC cxx compiler/linker/debugger use internally
#define __INTER__               !ERROR!
#define __bptr                  !ERROR!
#define __control               !ERROR!
#define __vptr                  !ERROR!
#define __result_pointer        !ERROR!
#define __result                !ERROR!
#define __vtbl                  !ERROR!
#define __btbl                  !ERROR!
#define evdw                    !ERROR!
#define __external_destructor_functor_list   !ERROR!
#define __fn                    !ERROR!
#define __fw                    !ERROR!
#define __init                  !ERROR!
#define __fini                  !ERROR!

#endif // PEDANTIC

#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>

#include <locale>

// assume <map> isn't #included by any other public header
#undef comp
#include <map>
#define comp                    !ERROR!

#include <memory>
#include <new>
#include <numeric>
#include <ostream>

// assume <queue> isn't #included by any other public header
// `c' and `comp' mandated by the standard
#undef c
#undef comp
#include <queue>
#define c                       !ERROR!
#define comp                    !ERROR!

// assume <set> isn't #included by any other public header
// `comp' mandated by the standard
#undef comp
#include <set>
#define comp                    !ERROR!
#include <sstream>

// assume <stack> isn't #included by any other public header
// `c' mandated by the standard
#undef c
#include <stack>
#define c                       !ERROR!

#include <stdexcept>
#include <streambuf>
#include <string>

#if defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x530
   // working around a SunPro 5.3 bug (see PR #26025)
#  undef rhs
#endif   // SunPro > 5.3

#include <typeinfo>

#define rhs                     !ERROR!

#include <utility>

// assume <valarray> isn't #included by any other public header
// `start' mandated by the standard
#undef start
#include <valarray>
#define start                   !ERROR!
#include <vector>

#ifndef _RWSTD_NO_NEW_HEADER

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>

#ifndef _RWSTD_NO_ISO646_H
#  include <ciso646>
#endif

#include <climits>
#include <clocale>
#include <cmath>
#ifdef DRQS_XX_FIXED // BUG
#include <csetjmp>
#endif // DRQS_XX_FIXED // BUG
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>


#ifndef _RWSTD_NO_WCHAR_H
#  include <cwchar>
#endif

#ifndef _RWSTD_NO_WCTYPE_H
#  include <cwctype>
#endif

#endif   // _RWSTD_NO_NEW_HEADER

/***********************************************************************/

static int
run_test (int, char**)
{
    // no-op (compile-only test)
    return 0;
}

/***********************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.intro",
                    "checking for namespace pollution",
                    run_test,
                    0, 0);
}
