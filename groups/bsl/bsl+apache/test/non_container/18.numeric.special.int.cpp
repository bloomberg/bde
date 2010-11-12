/***************************************************************************
 *
 * 18.numeric.special.int.cpp - tests specializations of the numeric_limits
 *                              class template on integer types
 *
 * $Id: 18.numeric.special.int.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2004-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <limits>    // for numeric_limits
#include <climits>   // for {CHAR,SHRT,INT,LONG}_{MIN,MAX}, etc.
#include <cstdio>    // for sprintf()

#include <driver.h>


template <class T> 
struct Limits
{
    enum { is_specialized = false };

    static T (min) () { return 0; }
    static T (max) () { return 0; }

    enum { digits };
    enum { digits10 };
    enum { is_signed };
    enum { is_integer };
    enum { is_exact };
    enum { radix };
    enum { is_bounded = false };

    static bool is_modulo () { return false; }

    static int compute_digits10 () {
        return 0;
    }
};


// edg (DEC cxx and others) gives an error in strict ANSI mode
// for things like INT_MAX + 1; this works around that error...
template <class T>
inline bool is_modulo (T _max)
{
    // avoid MSVC warning C4800: 'int' :
    //     forcing value to bool 'true' or 'false' (performance warning)
    T max_plus_one = _max;

    return ++max_plus_one < _max;
}

_RWSTD_SPECIALIZED_FUNCTION
inline bool is_modulo (bool)
{
    return false;
}


_RWSTD_SPECIALIZED_CLASS
struct Limits<int>
{
    enum { is_specialized = true };
    static int (min) ()  { return INT_MIN; }
    static int (max) ()  { return INT_MAX; }

    enum { digits = CHAR_BIT * sizeof (int) - 1 };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = true };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix = 2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%d", INT_MAX) - 1;
    }
};


#ifndef _RWSTD_NO_BOOL
_RWSTD_SPECIALIZED_CLASS
struct Limits<bool>
{  
    enum { is_specialized = true };
    static bool (min) ()  { return false; }
    static bool (max) ()  { return true;  }

    enum { digits = 1 }; 
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = false };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        return 0;
    }
};

#endif //_RWSTD_NO_BOOL


_RWSTD_SPECIALIZED_CLASS
struct Limits<char>                              
{  
    enum { is_specialized = true };
    static char (min) ()  { return CHAR_MIN; }
    static char (max) ()  { return CHAR_MAX; }
        
    enum { is_signed =  CHAR_MAX == SCHAR_MAX ? true : false };
    enum {
        digits = is_signed ? CHAR_BIT * sizeof (char) -1
                           : CHAR_BIT * sizeof (char)
    };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }
        
    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%d", (unsigned char)CHAR_MAX) - 1;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Limits<signed char>
{

    enum { is_specialized = true };
    static signed char (min) ()  { return SCHAR_MIN; }
    static signed char (max) ()  { return SCHAR_MAX; }
  
    enum { digits = CHAR_BIT * sizeof (signed char) - 1 };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = true };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%d", (unsigned char)SCHAR_MAX) - 1;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Limits<unsigned char>
{
    enum { is_specialized = true };
    static unsigned char (min) ()  { return 0;         }
    static unsigned char (max) ()  { return UCHAR_MAX; }

    enum { digits = CHAR_BIT * sizeof (unsigned char) };
    enum { digits10 = (digits * 301) / 1000 };

    enum { is_signed = false };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%d", (unsigned char)UCHAR_MAX) - 1;
    }
};

#ifndef _RWSTD_NO_NATIVE_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
struct Limits<wchar_t>
{
    enum { is_specialized = true };

    static wchar_t (min) () {
        return  wchar_t (-1) > wchar_t (0) ? 0
            : sizeof (wchar_t) == sizeof (short) ? wchar_t (SHRT_MIN)
            : sizeof (wchar_t) == sizeof (int)   ? wchar_t (INT_MIN)
            : sizeof (wchar_t) == sizeof (long)  ? wchar_t (LONG_MIN)
            : wchar_t (SCHAR_MIN);
    }

    static wchar_t (max) () {
        return   wchar_t (-1) > wchar_t (0) ?
            (  sizeof (wchar_t) == sizeof (short)   ? wchar_t (USHRT_MAX)
               : sizeof (wchar_t) == sizeof (int)   ? wchar_t (UINT_MAX)
               : sizeof (wchar_t) == sizeof (long)  ? wchar_t (ULONG_MAX)
               : wchar_t (SCHAR_MAX))
            : (  sizeof (wchar_t) == sizeof (short)   ? wchar_t (SHRT_MAX)
                 : sizeof (wchar_t) == sizeof (int)   ? wchar_t (INT_MAX)
                 : sizeof (wchar_t) == sizeof (long)  ? wchar_t (LONG_MAX)
                 : wchar_t (UCHAR_MAX));
    }

    enum { is_signed = wchar_t (0) > wchar_t (~0) };

    enum {
        digits = is_signed ? CHAR_BIT * sizeof (wchar_t) - 1
                           : CHAR_BIT*sizeof(wchar_t)
    };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }
        
    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%u", wchar_t (~0)) - 1;
    }
};

#endif //_RWSTD_NO_NATIVE_WCHAR_T


_RWSTD_SPECIALIZED_CLASS
struct Limits<short>
{
    enum { is_specialized = true };
    static short (min) ()  { return SHRT_MIN; }
    static short (max) ()  { return SHRT_MAX; }

    enum { digits = CHAR_BIT * sizeof (short) - 1 };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = true };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%d", (int)SHRT_MAX) - 1;
    }
};



_RWSTD_SPECIALIZED_CLASS
struct Limits<long>
{
    enum { is_specialized = true };
    static long (min) ()  { return LONG_MIN; }
    static long (max) ()  { return LONG_MAX; }

    enum { digits = CHAR_BIT * sizeof(long)-1 };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = true };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 }; 
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%ld", LONG_MAX) - 1;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Limits <unsigned short>
{
    enum { is_specialized = true };
    static unsigned short (min) ()  { return 0;         }
    static unsigned short (max) ()  { return USHRT_MAX; }

    enum { digits = CHAR_BIT * sizeof(unsigned short) };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = false };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%u", unsigned (USHRT_MAX)) - 1;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Limits<unsigned int>
{
    enum { is_specialized = true };
    static unsigned int (min) ()  { return 0;        }
    static unsigned int (max) ()  { return UINT_MAX; }

    enum { digits = CHAR_BIT * sizeof(unsigned int) };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = false };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%u", UINT_MAX) - 1;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Limits<unsigned long>
{
    enum { is_specialized = true };

    static unsigned long (min) ()  { return 0;         }
    static unsigned long (max) ()  { return ULONG_MAX; }

    enum { digits = CHAR_BIT * sizeof(unsigned long) };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = false };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        return std::sprintf (buf, "%lu", ULONG_MAX) - 1;
    }
};
  

#ifdef _RWSTD_LONG_LONG

_RWSTD_SPECIALIZED_CLASS
struct Limits<_RWSTD_LONG_LONG>
{
    typedef _RWSTD_LONG_LONG LLong;

    enum { is_specialized = true };

    static LLong (min) () {
        typedef unsigned _RWSTD_LONG_LONG ULLong;
        ULLong zero = 0;   // prevent an EDG eccp warning #68-D
        return ~zero / ULLong (2) + ULLong (1);
    }

    static LLong (max) () {
        typedef unsigned _RWSTD_LONG_LONG ULLong;
        return ~ULLong (0) / ULLong (2);
    }

    enum { digits = CHAR_BIT * sizeof (LLong) - 1 };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = true };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        const char fmt[] = "%" _RWSTD_LLONG_PRINTF_PREFIX "u";
        return std::sprintf (buf, fmt, (max) ()) - 1;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Limits<unsigned _RWSTD_LONG_LONG>
{
    typedef unsigned _RWSTD_LONG_LONG ULLong;

    enum { is_specialized = true };

    static ULLong (min) ()  { return 0; }
    static ULLong (max) ()  { return ~ULLong (0); }

    enum { digits = CHAR_BIT * sizeof (ULLong) };
    enum { digits10 = (digits * 301) / 1000 };
    enum { is_signed = false };
    enum { is_integer = true };
    enum { is_exact = true };
    enum { radix =  2 };
    enum { is_bounded = true };

    static bool is_modulo () { return ::is_modulo ((max)()); }

    static int compute_digits10 () {
        char buf [80];
        const char fmt[] = "%" _RWSTD_LLONG_PRINTF_PREFIX "i";

        // work around libc bugs (e.g., glibc on Linux)
        const int n = std::sprintf (buf, fmt, (max) ());

        if (n < Limits<unsigned>::digits10)
            return digits10;

        return n - 1;
    }
};

#endif   // _RWSTD_LONG_LONG


template <class T>
void run_test (T*, const char *tname, const char *fmt)
{
    typedef std::numeric_limits<T> limT;
    typedef Limits<T>              Traits;

    RW_ASSERT (0 != tname);
    RW_ASSERT (0 != fmt);

#if !defined (__EDG__) || __EDG_VERSION__ > 245

#  define ASSERT(expr, fmt)                                             \
      /* verify that `expr' is a constant integral expression */        \
      { enum { is_const_integral_expression = limT::expr }; }           \
      rw_assert (limT::expr == int (Traits::expr), 0, __LINE__,         \
                 "std::numeric_limits<%s>::" #expr                      \
                 " == %{@}, got %{@}",                                  \
                 tname, fmt, Traits::expr, fmt, limT::expr)

#else   // if EDG eccp < 3.0
  // working around an EDG eccp 2.4x ICE
#  define ASSERT(expr)                                                  \
      /* verify that `expr' is a constant integral expression */        \
      switch (limT::expr) { case limT::expr: break; };                  \
      rw_assert (limT::expr == int (Traits::expr), 0, __LINE__,         \
                 "std::numeric_limits<%s>::" #expr                      \
                 "== %{@}, got %{@}",                                   \
                 tname, fmt, Traits::expr, fmt, limT::expr)
#endif   // EDG eccp < 3.0

#define ASSERT_0(expr, fmt)                                             \
      rw_assert (!limT::expr, 0, __LINE__,                              \
                 "std::numeric_limits<%s>::" #expr " == 0, got %{@}",   \
                 tname, fmt, limT::expr)

    ASSERT (is_specialized, "%#b");

    // 18.2.1.2, p1
    rw_info (0, 0, __LINE__,
             "std::numeric_limits<%s>::min() == %{@}",
             tname, fmt, (Traits::min ()));

    rw_assert ((limT::min)() == (Traits::min)(), 0, __LINE__,
               "std::numeric_limits<%s>::min() == %{@}, got %{@}",
               tname, fmt, (Traits::min)(), fmt, (limT::min)());

    // 18.2.1.2, p4
    rw_info (0, 0, __LINE__,
             "std::numeric_limits<%s>::max() == %{@}",
             tname, fmt, (Traits::max ()));

    rw_assert ((limT::max)() == (Traits::max)(), 0, __LINE__,
               "std::numeric_limits<%s>::max() == %{@}, got %{@}",
               tname, fmt, (Traits::max)(), fmt, (limT::max)());

    // 18.2.1.2, p6
    ASSERT (digits, "%d");

    // 18.2.1.2, p9
    ASSERT (digits10, "%d");

    rw_assert (limT::digits10 == Traits::compute_digits10 (), 0, __LINE__,
               "std::numeric_limits<%s>::digits10 == %d (computed), got %d",
               tname, Traits::compute_digits10 (), limT::digits10);

    // 18.2.1.2, p11
    ASSERT (is_signed, "%b");

    // 18.2.1.2, p13
    ASSERT (is_integer, "%b");

    // 18.2.1.2, p15
    ASSERT (is_exact, "%b");

    // 18.2.1.2, p17
    ASSERT (radix, "%i");

    // 18.2.1.2, p20
    ASSERT_0 (epsilon (), fmt);

    // 18.2.1.2, p22
    ASSERT_0 (round_error (), fmt);
        
    // 18.2.1.2, p23
    ASSERT_0 (min_exponent, "%i");

    // 18.2.1.2, p25
    ASSERT_0 (min_exponent10, "%i");

    // 18.2.1.2, p27
    ASSERT_0 (max_exponent, "%i");

    // 18.2.1.2, p29
    ASSERT_0 (max_exponent10, "%i");

    // 18.2.1.2, p31
    ASSERT_0 (has_infinity, "%b");

    // 18.2.1.2, p34
    ASSERT_0 (has_quiet_NaN, "%b");

    // 18.2.1.2, p37
    ASSERT_0 (has_signaling_NaN, "%b");

    // 18.2.1.2, p40
    ASSERT_0 (has_denorm, "%b");

    // 18.2.1.2, p42
    ASSERT_0 (has_denorm_loss, "%b");

    // 18.2.1.2, p43
    ASSERT_0 (infinity (), fmt);

    // 18.2.1.2, p45
    ASSERT_0 (quiet_NaN (), fmt);

    // 18.2.1.2, p47
    ASSERT_0 (signaling_NaN (), fmt);

    // 18.2.1.2, p49
    ASSERT_0 (denorm_min (), fmt);

    // 18.2.1.2, p52
    ASSERT_0 (is_iec559, "%b");

    // 18.2.1.2, p54
    ASSERT (is_bounded, "%b");

    // 18.2.1.2, p56
    rw_assert (limT::is_modulo == Traits::is_modulo (), 0, __LINE__,
               "std::numeric_limits<%s>::is_modulo == %#b",
               tname, Traits::is_modulo ());

    // 18.2.1.2, p59
    ASSERT_0 (traps, "%b");

    // 18.2.1.2, p61
    ASSERT_0 (tinyness_before, "%b");
        
    // 18.2.1.2, p63
    rw_assert (limT::round_style == int (std::round_toward_zero), 0, __LINE__,
               "std::numeric_limits<%s>::round_style == %d",
               tname, std::round_toward_zero);
}


static int
run_test (int, char**)
{
#define TEST(T, fmt)   run_test ((T*)0, #T, fmt)


#ifndef _RWSTD_NO_BOOL

    TEST (bool, "%#b");

#endif //_RWSTD_NO_BOOL
  

    TEST (char, "%{#c}");
    TEST (signed char, "%{#c}");
    TEST (unsigned char, "%{#c}");

    TEST (short, "%hi");
    TEST (unsigned short, "%hu");

    TEST (int, "%i");
    TEST (unsigned int, "%u");

    TEST (long, "%li");
    TEST (unsigned long, "%lu");

#ifdef _RWSTD_LONG_LONG

    TEST (_RWSTD_LONG_LONG, "%lli");
    TEST (unsigned _RWSTD_LONG_LONG, "%llu");

#endif   // _RWSTD_LONG_LONG

#ifndef _RWSTD_NO_NATIVE_WCHAR_T

    TEST (wchar_t, "%{#lc}");

#endif   //_RWSTD_NO_NATIVE_WCHAR_T
 
    TEST (void*, "%#p");

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "numeric.special",
                    "integer specializations",
                    run_test,
                    "",
                    (void*)0);
}
