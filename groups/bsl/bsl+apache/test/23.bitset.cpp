/***************************************************************************
 *
 * 23.bitset.cpp - test exercising [lib.bitset]
 *
 * $Id: 23.bitset.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <bitset>
#include <istream>
#include <ostream>

#include <cstdio>     // for sprintf()
#include <climits>

#include <rw_char.h>  // for UserChar
#include <rw_rand.h>  // for rw_rand()
#include <driver.h>   // for rw_test(), ...


#define NLOOPS 128

/**************************************************************************/

// sets the `n' least significant bits
std::size_t
bitmax (std::size_t n)
{
    std::size_t result = 0;
    while (n--)
        result = result << 1 | 1U;
    return result;
}

/**************************************************************************/

// fake bitset class, bit pattern represented as a character string
template <std::size_t N>
struct test_set
{
    char bits_ [N + 1];

    test_set () {
        reset ();
        bits_ [N] = '\0';   // null-terminate
    }

    _EXPLICIT test_set (unsigned long val) {
        for (std::size_t i = 0; i != N; ++i)
            set (i, !!(val & (1UL << i)));
        bits_ [N] = '\0';   // NUL-terminate
    }

    _EXPLICIT test_set (const std::bitset<N> &rhs) {
        for (std::size_t i = 0; i != N; ++i)
            set (i, rhs.test (i));
        bits_ [N] = '\0';   // NUL-terminate
    }

    // accessor function provided to work around
    // a MIPSpro 7.3.1.1 bug (PR #25682)
    const char* bits () const {
        return bits_;
    }

    test_set& random () {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = char ('0' + rw_rand (2U));
        return *this;
    }

    std::bitset<N> to_bitset () const {
        std::bitset<N> tmp;
        for (std::size_t i = 0; i != N; ++i)
            tmp.set (i, test (i));
        return tmp;
    }

    test_set& operator&= (const test_set &rhs) {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = '0' + ('1' == bits_ [i] && '1' == rhs.bits_ [i]);
        return *this;
    }

    test_set& operator|= (const test_set &rhs) {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = '0' + ('1' == bits_ [i] || '1' == rhs.bits_ [i]);
        return *this;
    }

    test_set& operator^= (const test_set &rhs) {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = '0' + !!(bits_ [i] - rhs.bits_ [i]);
        return *this;
    }

    test_set& operator<<= (std::size_t n) {
        for (std::size_t i = 0; i < N - n && i + n + 1 < N + 1; ++i)
            bits_ [i] = bits_ [i + n];
        for (std::size_t j = N - n; j != N; ++j)
            bits_ [j] = '0';
        return *this;
    }

    test_set& operator>>= (std::size_t n) {
        for (std::size_t i = N - 1; i != n - 1; --i)
            bits_ [i] = bits_ [i - n];
        for (std::size_t j = n; j != 0; )
            bits_ [--j] = '0';
        return *this;
    }

    test_set& set () {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = '1';
        return *this;
    }

    test_set& set (std::size_t pos, bool val) {
        bits_ [N - pos - 1] = '0' + val;
        return *this;
    }

    test_set& reset () {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = '0';
        return *this;
    }

    test_set& reset (std::size_t pos) {
        return bits_ [N - pos - 1] = '0', *this;
    }

    test_set operator~ () const {
        return test_set (*this).flip ();
    }

    test_set& flip () {
        for (std::size_t i = 0; i != N; ++i)
            bits_ [i] = '0' + ('0' == bits_ [i]);
        return *this;
    }

    test_set& flip (std::size_t pos) {
        bits_ [N - pos - 1] = '0' + ('0' == bits_ [N - pos - 1]);
        return *this;
    }

    std::size_t count () const {
        std::size_t n = 0;
        for (std::size_t i = 0; i != N; ++i)
            n += bits_ [i] != '0';
        return n;
    }

    bool operator== (const test_set &rhs) const {
        for (std::size_t i = 0; i != N; ++i)
            if (bits_ [i] != rhs.bits_ [i])
                return false;
        return true;
    }

    bool test (std::size_t pos) const {
        return '0' != bits_ [N - pos - 1];
    }
};

/**************************************************************************/

#ifndef _RWSTD_NO_EXPLICIT

// helper to verify that bitset ctor is explicit
// not defined since it must not be referenced if test is successful
// static commented out to prevent gcc warning: function declared
// 'static' but never defined
/* static */ void
#if TEST_RW_EXTENSIONS
is_explicit (const std::bitset<0>&);
#else
is_explicit (const std::bitset<1>&);
#endif

struct has_implicit_ctor
{
    // NOT explicit
    has_implicit_ctor (const std::string&) { }
};

// calls to the overloaded is_explicit() resolve to the function below
static void
is_explicit (const has_implicit_ctor&) { }

#endif   // _RWSTD_NO_EXPLICIT


static void
#if TEST_RW_EXTENSIONS
test_synopsis (std::bitset<0>*)
#else
test_synopsis (std::bitset<1>*)
#endif
{
    // prevent warnings about unreachable code
    volatile bool dummy = false;

    if (!dummy)
        return;

#if TEST_RW_EXTENSIONS
    typedef std::bitset<0>    Bitset;
#else
    typedef std::bitset<1>    Bitset;
#endif
    typedef Bitset::reference Reference;

    Bitset::reference *pr = (Bitset::reference*)0;
    _RWSTD_UNUSED (pr);

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#define MEMFUN(result, name, arg_list) do {                                   \
        result (Bitset::reference::*pf) arg_list = &Bitset::reference::name;  \
        _RWSTD_UNUSED (pf);                                                   \
    } while (0)

    // exercise bitset::reference members
    MEMFUN (Reference&, operator=, (bool));
    MEMFUN (Reference&, operator=, (const Reference&));
    MEMFUN (bool, operator~, () const);
    MEMFUN (bool, operator bool, () const);
    MEMFUN (Reference&, flip, ());

    // 23.3.5.1 - verify bitset ctors
#if    !defined (_RWSTD_NO_EXPLICIT) \
    && (!defined (__SUNPRO_CC) || __SUNPRO_CC > 0x530) \
    && (!defined (__GNUG__) || __GNUG__ >= 3)
    // working around a SunPro 5.2 bug (see PR #25959)

    // verify that bitset ctor is declared explicit
    is_explicit (std::string ());

#endif   // _RWSTD_NO_EXPLICIT && SunPro > 5.3

    // verify default arguments
    (void)Bitset (std::string ());
    (void)Bitset (std::string (), std::string::size_type ());

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list) do {              \
        result (Bitset::*pf) arg_list = &Bitset::name;   \
        _RWSTD_UNUSED (pf);                              \
    } while (0)

    MEMFUN (Bitset&, operator&=, (const Bitset&));
    MEMFUN (Bitset&, operator|=, (const Bitset&));
    MEMFUN (Bitset&, operator^=, (const Bitset&));
    MEMFUN (Bitset&, operator<<=, (std::size_t));
    MEMFUN (Bitset&, operator>>=, (std::size_t));

    MEMFUN (Bitset&, set, ());
#if TEST_RW_EXTENSIONS_LWG
    MEMFUN (Bitset&, set, (std::size_t, bool));   // lwg issue 186
#else
    MEMFUN (Bitset&, set, (std::size_t, int));   // lwg issue 186
#endif
    Bitset ().set (0);   // verify default argument

    MEMFUN (Bitset&, reset, ());
    MEMFUN (Bitset&, reset, (std::size_t));
    
    MEMFUN (Bitset, operator~, () const);
    MEMFUN (Bitset&, flip, ());
    MEMFUN (Bitset&, flip, (std::size_t));

    MEMFUN (Bitset::reference, operator[], (std::size_t));
    MEMFUN (bool, operator[], (std::size_t) const);

    MEMFUN (unsigned long, to_ulong, () const);

#ifndef _RWSTD_NO_MEMBER_TEMPLATES
#  if !defined (__HP_aCC) || __HP_aCC >= 60000

     // working around HP aCC bugs PR #23312 and bug #503

#    define PARAMLIST_3(T)   T, std::char_traits<T>, std::allocator<T>
#    define PARAMLIST_2(T)   T, std::char_traits<T>

    // exercise the overloaded member template function and ordinary
    // member function to_string()
#if TEST_RW_EXTENSIONS
    MEMFUN (std::basic_string<PARAMLIST_3 (char) >,
            to_string<PARAMLIST_3 (char) >, (char, char) const);
    MEMFUN (std::basic_string<PARAMLIST_3 (char) >,
            to_string, (char, char) const);

#    ifndef _RWSTD_NO_WCHAR_T

    MEMFUN (std::basic_string<PARAMLIST_3 (wchar_t) >,
            to_string<PARAMLIST_3 (wchar_t) >, (wchar_t, wchar_t) const);

#    endif   // _RWSTD_NO_WCHAR_T

    MEMFUN (std::basic_string<PARAMLIST_3 (int) >,
            to_string<PARAMLIST_3 (int) >, (int, int) const);
#endif // TEST_RW_EXTENSIONS

#    undef PARAMLIST_3
#    undef PARAMLIST_2

#  endif   // !__HP_aCC || __HP_aCC >= 60000
#endif   // _RWSTD_NO_MEMBER_TEMPLATES

    MEMFUN (std::size_t, size, () const);
    MEMFUN (std::size_t, count, () const);

    // The function signiture of these two operators takes
    // 'native_std::bitset' (which is ok since we 'bslstl::bitset'
    // automatically converts to 'native_std::bitset'.
    MEMFUN (bool, operator==, (const Bitset&) const);
    MEMFUN (bool, operator!=, (const Bitset&) const);

    MEMFUN (bool, test, (std::size_t) const);
    MEMFUN (bool, any, () const);
    MEMFUN (bool, none, () const);
    
    MEMFUN (Bitset, operator>>, (std::size_t) const);
    MEMFUN (Bitset, operator<<, (std::size_t) const);

#define FUN(result, name, arg_list) do {  \
        result (*pf) arg_list = &name;    \
        _RWSTD_UNUSED (pf);               \
    } while (0)

#if !defined (__IBMCPP__) || __IBMCPP__ > 502
    FUN (Bitset, std::operator&, (const Bitset&, const Bitset&));
    FUN (Bitset, std::operator|, (const Bitset&, const Bitset&));
    FUN (Bitset, std::operator^, (const Bitset&, const Bitset&));

#else
    // working around xlC 5.0.2.0 bug: PR #26561

    FUN (Bitset, std::operator&,(const Bitset&,const Bitset&) _PTR_THROWS(()));
    FUN (Bitset, std::operator|,(const Bitset&,const Bitset&) _PTR_THROWS(()));
    FUN (Bitset, std::operator^,(const Bitset&,const Bitset&) _PTR_THROWS(()));
#endif    


#define PARAMLIST(T)   T, std::char_traits<T>

    FUN (std::basic_istream< PARAMLIST (char) >&, std::operator>>,
         (std::basic_istream< PARAMLIST (char) >&, Bitset&));
    FUN (std::basic_ostream< PARAMLIST (char) >&, std::operator<<,
         (std::basic_ostream< PARAMLIST (char) >&, const Bitset&));

#ifndef _RWSTD_NO_MEMBER_TEMPLATES
#  ifndef _RWSTD_NO_WCHAR_T

    FUN (std::basic_istream< PARAMLIST (wchar_t) >&, std::operator>>,
         (std::basic_istream< PARAMLIST (wchar_t) >&, Bitset&));
    FUN (std::basic_ostream< PARAMLIST (wchar_t) >&, std::operator<<,
         (std::basic_ostream< PARAMLIST (wchar_t) >&, const Bitset&));

#  endif   // _RWSTD_NO_WCHAR_T


#  if !defined (_MSC_VER) || _MSC_VER > 1300

#if TEST_RW_PEDANTIC // charT other that char or wchar_t
    // MSVC is too dumb to handle bitset inserters and extractors
    // parametrized on multiple template paramenters
    FUN (std::basic_istream< PARAMLIST (int) >&, std::operator>>,
         (std::basic_istream< PARAMLIST (int) >&, Bitset&));
    FUN (std::basic_ostream< PARAMLIST (int) >&, std::operator<<,
         (std::basic_ostream< PARAMLIST (int) >&, const Bitset&));
#endif // TEST_RW_PEDANTIC

#  endif   // !defined (_MSC_VER) || _MSC_VER > 1300
#endif   // _RWSTD_NO_MEMBER_TEMPLATES

#undef PARAMLIST

}

/**************************************************************************/

template <std::size_t N>
void test_ctors (const std::bitset<N>*)
{
    typedef unsigned long ULong;
    const ULong bmask = ULong (::bitmax (N));

    {   // bitset::bitset()
        rw_info (0, 0, __LINE__, "std::bitset<%d>::bitset()", N);

        const std::bitset<N> b;
        rw_assert (0 == b.to_ulong (), 0, __LINE__,
                   "bitset<%d>::bitset ().to_ulong() == 0, got %#lx",
                   b.to_ulong ());
    }
    
    {   // bitset::bitset (unsigned long)
        rw_info (0, 0, __LINE__, "std::bitset<%d>::bitset (unsigned long)", N);

        const std::bitset<N> b (ULONG_MAX & bmask);
        rw_assert ((ULONG_MAX & bmask) == b.to_ulong (), 0, __LINE__,
                   "bitset<%d>::bitset (%#lx).to_ulong() == 0, got %#lx",
                   N, ULONG_MAX & bmask, b.to_ulong ());
    }

    {   // bitset (const string& str, size_t pos = 0, size_t n = (size_t)-1);
        rw_info (0, 0, __LINE__, "std::bitset<%d>::bitset (string)", N);

        test_set<N> ts;
        ts.set ();

        _TRY {
            const std::bitset<N> b = std::bitset<N>(std::string (ts.bits ()));

            if (N <= sizeof (unsigned long) * CHAR_BIT)
                rw_assert (b == bmask, 0, __LINE__,
                           "bitset<%d>::bitset(string(\"%s\").to_ulong()"
                           " == %#x, got %#x", N, ts.bits (), bmask,
                           b.to_ulong ());
            else
                rw_assert (test_set<N>(b) == ts, 0, __LINE__,
                           "bitset<%d>::bitset(string(\"111...111\")"
                           " == 111...111, got %s",
                           N, test_set<N>(b).bits ());

        }
        _CATCH (...) {
            rw_assert (false,  0, __LINE__, (""));
        }
    }

    {   // bitset (const bitset<N>& rhs)
        rw_info (0, 0, __LINE__, "std::bitset<%d>::bitset (const bitset&)", N);

        const std::bitset<N> b1 (12345);
        const std::bitset<N> b2 (b1);

        rw_assert (b1.to_ulong () == b2.to_ulong (), 0, __LINE__,
                   "bitset<%d>::bitset (bitset<%d>(%#lx)).to_ulong() == %#lx,"
                   " got %#lx", N, b1.to_ulong (), b2.to_ulong ());

        rw_info (0, 0, __LINE__,
                 "std::bitset<%d>::operator=(const bitset&)", N);

        std::bitset<N> b3;
        b3 = b1;

        rw_assert (b1.to_ulong () == b3.to_ulong (), 0, __LINE__,
                   "bitset<%d>::bitset (bitset<%d>(%#lx)).to_ulong() == %#lx,"
                   " got %#lx", N, b1.to_ulong (), b3.to_ulong ());
    }
}

/**************************************************************************/

template <std::size_t N>
void stress_ctors (const std::bitset<N>*)
{
    rw_info (0, 0, __LINE__, "std::bitset<%d>::bitset (string)", N);

    const std::size_t max_mask = bitmax (N);

    for (std::size_t i = 0; i != 1000U; i++) {

        typedef unsigned long ULong;

        // exercise 23.3.5.1, p2
        const ULong n = ULong (rw_rand (0) & max_mask);
        const std::bitset<N> b1 (n);

        rw_assert (n == b1.to_ulong (), 0, __LINE__,
                   "bitset<%d>::bitset(%#lx).to_ulong() == %#lx, got %#lx",
                   N, n, n, b1.to_ulong ());

        test_set<N> ts;
        ts.random ();

        // exercise 23.3.5.1, p3
        std::bitset<N> b2 = std::bitset<N>(std::string (ts.bits ()));
        rw_assert (test_set<N>(b2) == ts, 0, __LINE__,
                   "bitset<%d>::bitset (\"%s\") got %s",
#if TEST_RW_EXTENSIONS_LWG
                   N, ts.bits (), b2.to_string ().c_str ());
#else
                   N, ts.bits (), b2.template to_string<char, std::char_traits<char>, std::allocator<char> >().c_str ());
#endif
    }
}

/**************************************************************************/

template <std::size_t N>
void test_operators (const std::bitset<N>*)
{
#define TEST_OP(op) do {                                                   \
   rw_info (!i, 0, __LINE__, "std::bitset<%d>::operator" #op               \
                             "= (const bitset&)", N);                      \
                                                                           \
          test_set<N> lhs = test_set<N>().random ();                       \
    const test_set<N> rhs = test_set<N>().random ();                       \
          std::bitset<N> b_lhs = std::bitset<N>(std::string (lhs.bits ()));\
    const std::bitset<N> b_rhs = std::bitset<N>(std::string (rhs.bits ()));\
    const test_set<N> res = lhs op ## = rhs;                               \
    const std::bitset<N> b_res = b_lhs op ## = b_rhs;                      \
                                                                           \
    rw_assert (res == test_set<N>(b_res), 0, __LINE__,                     \
               "bitset<%lu>::operator" #op "= (const bitset<%lu>&):"       \
               " %s " #op " %s == %s, got %s",                             \
               N, N, lhs.bits (), rhs.bits (), res.bits (),                \
               test_set<N>(b_res).bits ());                                \
                                                                           \
    rw_info (!i, 0, __LINE__, "std::bitset<%d>::operator" #op              \
                              " (const bitset&)", N);                      \
    lhs.random ();                                                         \
    b_lhs = std::bitset<N>(std::string (lhs.bits ()));                     \
                                                                           \
    const test_set<N> res2 = lhs op ## = rhs;                              \
    const std::bitset<N> b_res2 = b_lhs op b_rhs;                          \
                                                                           \
    rw_assert (res2 == test_set<N>(b_res2), 0, __LINE__,                   \
               "bitset<%lu>::operator" #op " (const bitset<%lu>&):"        \
               " %s " #op " %s == %s, got %s",                             \
               N, N, lhs.bits (), rhs.bits (), res2.bits (),               \
               test_set<N>(b_res2).bits ());                               \
  } while (0)

    // prevent division by zero errors, also exercise shifting by
    // N bits (operation must zero out the first operand)
    // especially important is shifting bitset<N> by N bits where
    // N >= sizeof (unsigned long) * CHAR_BIT, since this operation
    // is undefined for unsigned longs but defined for bitset

    const std::size_t M = N + 1;

    for (int i = 0; i != NLOOPS; ++i) {

        // 23.3.5.2, p1 and 23.3.5.3, p1
        TEST_OP (&);
        // 23.3.5.2, p3 and 23.3.5.3, p2
        TEST_OP (|);
        // 23.3.5.2, p5 and 23.3.5.3, p3
        TEST_OP (^);

        rw_info (!i, 0, __LINE__, "std::bitset<%d>::operator<<=(size_t)", N);

        const test_set<N> ts1 = test_set<N>().random ();
        const test_set<N> ts2 = test_set<N>(ts1) <<= i % M;

        std::bitset<N> b1 = std::bitset<N> (std::string (ts1.bits ()));

        // 23.3.5.2, p7
        b1 <<= i % M;

        rw_assert (test_set<N>(b1) == ts2, 0, __LINE__,
                   "bitset<%d>::operator<<=(%lu): %s << %lu == %s, got %s",
                   N, i % M, ts1.bits (), i % M, ts2.bits (),
                   test_set<N>(b1).bits ());

        rw_info (!i, 0, __LINE__, "std::bitset<%d>::operator>>=(size_t)", N);

        const test_set<N> ts3 = test_set<N>(ts1) >>= i % M;
        std::bitset<N> b2 = std::bitset<N>(std::string (ts1.bits ()));

        // 23.3.5.2, p9
        b2 >>= i % M;

        rw_assert (test_set<N>(b2) == ts3, 0, __LINE__,
                   "bitset<%d>::operator>>=(%lu): %s >> %lu == %s, got %s",
                   N, i % M, ts1.bits (), i % M, ts3.bits (),
                   test_set<N>(b2).bits ());

        rw_info (!i, 0, __LINE__,
                 "std::bitset<%d>::operator<<=(size_t) (unused bits)", N);

        if (N) {
            b1.set (N - 1);
            std::size_t first  = b1.count ();
            std::size_t second = (b1 <<= 1).count ();
            rw_assert (!(first == second), 0, __LINE__,
                       "bitset<%lu>::operator<<=(1): "
                       "after <<= 1: expected %lu, got %lu",
                       N, first - 1, second);
        }

        rw_info (!i, 0, __LINE__,
                 "std::bitset<%d>::operator>>=(size_t) (unused bits)", N);

        if (N) {
            b2.set ();
            std::size_t first  = b2.count ();
            std::size_t second = (b2 >>= 1).count ();
            rw_assert (first - 1 == second, 0, __LINE__,
                       "bitset<%lu>::operator>>=(1): "
                       "after >>= 1: expected %lu, got %lu",
                       N, first - 1, second);
        }
    }
}

/**************************************************************************/

template <std::size_t N>
void test_other (const std::bitset<N>*)
{
    for (std::size_t i = 0; i != NLOOPS; ++i) {

        // 23.3.5.2, p23
        test_set<N> ts1;
        ts1.random ();

        test_set<N> ts2 = ~ts1;

        std::bitset<N> b1 = std::bitset<N>(std::string (ts1.bits ()));
        std::bitset<N> b2 = ~b1;

        rw_assert (ts2 == test_set<N>(b2), 0, __LINE__,
                   "bitset<%d>::operator~(): ~%s == %s, got %s",
                   N, ts1.bits (), ts2.bits (), test_set<N>(b2).bits ());

        // 23.3.5.2, p25
        b2.flip ();
        rw_assert (ts1 == test_set<N>(b2), 0, __LINE__,
                   "bitset<%d>::flip (): ~%s == %s, got %s",
                   N, ts1.bits (), ts2.bits (), test_set<N>(b2).bits ());

        // 23.3.5.2, p27
        for (std::size_t _j = 0; _j != N; ++_j)
            b2.flip (_j);

        rw_assert (ts2 == test_set<N>(b2), 0, __LINE__,
            "bitset<%d>::flip () == %s, got %s",
            N, ts2.bits (), test_set<N>(b2).bits ());

        // 23.3.5.3, p35
        rw_assert (ts2.count () == b2.count (), 0, __LINE__,
                   "bitset<%d>::count () == %d, got %d [%s]",
                   N, ts2.count (), b2.count (), test_set<N>(b2).bits());
               
        // 23.3.5.3, p37
        rw_assert (b2 == b2 && (N && !(b1 == b2) || !N && b1 == b2),
                   0, __LINE__,
                   "bitset<%d>::operator==(const bitset<%ul>&) [%s]",
                   N, N, N ? test_set<N>(b2).bits () : "<empty>");
    
        // 23.3.5.3, p38
        rw_assert ((N && b1 != b2 || !N && !(b1 != b2)) && !(b2 != b2),
                   0, __LINE__,
                   "bitset<%d>::operator!=(const bitset<%ul>&) [%s]",
                   N, N, N ? test_set<N>(b2).bits () : "<empty>");
    
        // 23.3.5.3, p42
        rw_assert (b2.count() && b2.any() || !b2.count() && !b2.any(),
                   0, __LINE__,
                   "bitset<%d>::any () [%s]",
                   N, test_set<N>(b2).bits ());

        // 23.3.5.3, p43
        rw_assert (b2.count() && !b2.none() || !b2.count() && b2.none(),
                   0, __LINE__,
                   "bitset<%d>::none () [%s]",
                   N, test_set<N>(b2).bits ());

        for (std::size_t k = 0; k != N; ++k) {
            test_set<N>    ts3 = test_set<N>(ts1) <<= k;
            std::bitset<N> b3  = b1 << k;

            rw_assert (test_set<N>(b3) == ts3,  0, __LINE__,
                       "bitset<%lu>::operator<<(%lu)", N, k);

            ts3 = test_set<N>(ts1) >>= k;
            b3  = b1 >> k;

            rw_assert (test_set<N>(b3) == ts3, 0, __LINE__,
                       "bitset<%lu>::operator>>(%lu)", N, k);
        }
    }
}

/**************************************************************************/

template <std::size_t N>
void stress_count (const std::bitset<N>*)
{
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::count()", N);

    for (std::size_t i = 0; i != N; i++) {
        std::bitset<N> b;

        for (std::size_t j = 0; j != i; j++)
            b.set (j);

        rw_assert (b.count () == i,  0, __LINE__,
                   "%lu. std::bitset<%lu>::count()", i, N);
    }
}

/**************************************************************************/

template <std::size_t N>
void test_elem_access (const std::bitset<N>*)
{
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::test(size_t)", N);
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::operator[](size_t)", N);
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::operator[](size_t) const", N);

    for (std::size_t i = 0; i != NLOOPS; ++i) {

        const test_set<N> ts = test_set<N>().random ();
        std::bitset<N>    b  = std::bitset<N>(std::string (ts.bits ()));

        for (std::size_t _j = 0; _j != N; ++_j) {
            // 23.3.5.2, p39
            rw_assert (b.test (_j) == ts.test (_j), 0, __LINE__,
                       "bitset<%lu>::test (%lu): %s",
                       N, _j, test_set<N>(b).bits ());

            // 23.3.5.2, p??: see lwg issue 11
            rw_assert (b [_j] == ts.test (_j), 0, __LINE__,
                       "bitset<%lu>::operator[](%lu): %s",
                       N, _j, test_set<N>(b).bits ());

            // 23.3.5.2, p??: see lwg issue 11
            rw_assert (((const std::bitset<N>&)b) [_j] == ts.test (_j),
                       0, __LINE__,
                       "bitset<%lu>::operator[](%lu) const: %s",
                       N, _j, test_set<N>(b).bits ());

            // exercise std::bitset<N>::reference
            _TYPENAME std::bitset<N>::reference r = b [_j];

            // std::bitset<N>::reference::flip()
            r.flip ();
            rw_assert (r == !ts.test (_j), 0, __LINE__,
                       "bitset<%lu>::reference::flip()", N);

            // std::bitset<N>::reference::operator~()
            bool toggled = ~r;
            rw_assert (toggled == ts.test (_j), 0, __LINE__,
                       "bitset<%lu>::reference::operator~()", N);

            // std::bitset<N>::reference::operator=(bool)
            r = toggled;
            rw_assert (r == ts.test (_j) && b.test (_j) == ts.test (_j),
                       0, __LINE__,
                       "bitset<%lu>::reference::operator=(bool)", N);
        }
    }
}


/**************************************************************************/

//_RWSTD_NAMESPACE (std) {
#undef std
namespace std {

_RWSTD_SPECIALIZED_CLASS
struct char_traits<UserChar>: UserTraits<UserChar> { };

}   // namespace std
#define std bsl

const char* type_name (char) { return "char"; }

#ifndef _RWSTD_NO_WCHAR_T
const char* type_name (wchar_t) { return "wchar_t"; }
#endif   // _RWSTD_NO_WCHAR_T

const char* type_name (const UserChar&)
{
    return "UserChar";
}

template <class charT>
const char* traits_name (const std::char_traits<charT>*)
{
    static char name [64];
    std::sprintf (name, "std::char_traits<%s>", type_name (charT ()));
    return name;
}

const char* traits_name (const UserTraits<UserChar>*)
{
    return "UserTraits";
}

template <class charT>
struct MyAlloc: std::allocator<charT> { };

template <class charT>
const char* alloc_name (const std::allocator<charT>&)
{
    static char name [64];
    std::sprintf (name, "std::allocator<%s>", type_name (charT ()));
    return name;
}

template <class charT>
const char* alloc_name (const MyAlloc<charT>&)
{
    static char name [64];
    std::sprintf (name, "MyAlloc<%s>", type_name (charT ()));
    return name;
}

template <class charT>
charT& assign (charT &lhs, char rhs)
{
    return lhs = rhs;
}

UserChar& assign (UserChar &lhs, char rhs)
{
    lhs.c = _RWSTD_STATIC_CAST (unsigned char, rhs);

    return lhs;
}

// compare two strings, return the offset of the first mismatch
// or -1 when the strings are equal
template <class charT>
int compare (const charT str[], const char s[], const char bits [2])
{
    const char* const beg = s;

    for ( ; *s; ++s, ++str) {

        if (*str != bits ['1' == *s])
            return int (s - beg);
    }

    return *str ? s - beg : -1;
}

// compare two strings, return the offset of the first mismatch
// or -1 when the strings are equal
int compare (const UserChar str[], const char s[], const char bits [2])
{
    const char* const beg = s;

    for (; *s; ++s, ++str) {

        if (char (str->c) != bits ['1' == *s])
            return int (s - beg);
    }

    return str->c ? s - beg : -1;
}


#ifndef _RWSTD_NO_MEMBER_TEMPLATES

// call the bitset<N>::to_string() member function template,
// explicitly specifying all three template arguments,
// and 2, 1, or 0 of the two default function arguments
template <std::size_t N, class charT, class Traits, class Alloc>
std::basic_string<charT, Traits, Alloc>
bitset_to_string_3 (const std::bitset<N> &bs, int nfargs,
                    charT zero, charT one,
                    std::basic_string<charT, Traits, Alloc>*)
{
#if TEST_RW_EXTENSIONS
    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.template to_string<charT, Traits, Alloc>(zero);
    case 0:
        return bs.template to_string<charT, Traits, Alloc>();
    }

    return bs.template to_string<charT, Traits, Alloc>(zero, one);
#else
    return bs.template to_string<charT, Traits, Alloc>();
#endif // TEST_RW_EXTENSIONS
}

#if TEST_RW_EXTENSIONS
template <std::size_t N, class Traits, class Alloc>
std::basic_string<UserChar, Traits, Alloc>
bitset_to_string_3 (const std::bitset<N> &bs, int nfargs,
                    UserChar zero, UserChar one,
                    std::basic_string<UserChar, Traits, Alloc>*)
{
    // UserChar digits zero and one
    static const UserChar dig[] = { { 0, '0' }, { 0, '1' } };

    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.template to_string<UserChar, Traits, Alloc>(zero, dig [1]);
    case 0:
        return bs.template to_string<UserChar, Traits, Alloc>(dig [0], dig [1]);
    }

    return bs.template to_string<UserChar, Traits, Alloc>(zero, one);
}
#endif // TEST_RW_EXTENSIONS

// call the bitset<N>::to_string() member function template,
// explicitly specifying two of the three template arguments,
// and 2, 1, or 0 of the two default function arguments
template <std::size_t N, class charT, class Traits>
std::basic_string<charT, Traits, std::allocator<charT> >
bitset_to_string_2 (const std::bitset<N> &bs, int nfargs,
                    charT zero, charT one,
                    std::basic_string<charT, Traits,
                                      std::allocator<charT> >*)
{
#if TEST_RW_EXTENSIONS
    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.template to_string<charT, Traits>(zero);
    case 0:
        return bs.template to_string<charT, Traits>();
    }

    return bs.template to_string<charT, Traits>(zero, one);
#else
    return bs.template to_string<charT, Traits, std::allocator<charT> >();
#endif // TEST_RW_EXTENSIONS
}

#if TEST_RW_EXTENSIONS
template <std::size_t N, class Traits>
std::basic_string<UserChar, Traits, std::allocator<UserChar> >
bitset_to_string_2 (const std::bitset<N> &bs, int nfargs,
                    UserChar zero, UserChar one,
                    std::basic_string<UserChar, Traits,
                                      std::allocator<UserChar> >*)
{
    static const UserChar dig[] = { { 0, '0' }, { 0, '1' } };

    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.template to_string<UserChar, Traits>(zero, dig [1]);
    case 0:
        return bs.template to_string<UserChar, Traits>(dig [0], dig [1]);
    }

    return bs.template to_string<UserChar, Traits>(zero, one);
}
#endif // TEST_RW_EXTENSIONS

// call the bitset<N>::to_string() member function template,
// explicitly specifying one of the three template arguments,
// and 2, 1, or 0 of the two default function arguments
template <std::size_t N, class charT>
std::basic_string<charT, std::char_traits<charT>, std::allocator<charT> >
bitset_to_string_1 (const std::bitset<N> &bs, int nfargs,
                    charT zero, charT one,
                    std::basic_string<charT,
                                      std::char_traits<charT>,
                                      std::allocator<charT> >*)
{
#if TEST_RW_EXTENSIONS
    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.template to_string<charT>(zero);
    case 0:
        return bs.template to_string<charT>();
    }

    return bs.template to_string<charT>(zero, one);
#else
    return bs.template to_string<charT, std::char_traits<charT>, std::allocator<charT> >();
#endif // TEST_RW_EXTENSIONS
}

#if TEST_RW_EXTENSIONS
template <std::size_t N>
std::basic_string<UserChar, std::char_traits<UserChar>,
                  std::allocator<UserChar> >
bitset_to_string_1 (const std::bitset<N> &bs, int nfargs,
                    UserChar zero, UserChar one,
                    std::basic_string<UserChar,
                                      std::char_traits<UserChar>,
                                      std::allocator<UserChar> >*)
{
    // UserChar digits zero and one
    static const UserChar dig[] = { { 0, '0' }, { 0, '1' } };

    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.template to_string<UserChar>(zero, dig [1]);
    case 0:
        return bs.template to_string<UserChar>(dig [0], dig [1]);
    }

    return bs.template to_string<UserChar>(zero, one);
}
#endif // TEST_RW_EXTENSIONS

#endif   // _RWSTD_NO_MEMBER_TEMPLATES


// call the bitset<N>::to_string() ordinary member function,
// explicitly specifying none of the three template arguments,
// and 2, 1, or 0 of the two default function arguments
template <std::size_t N>
std::basic_string<char, std::char_traits<char>, std::allocator<char> >
bitset_to_string_0 (const std::bitset<N> &bs, int nfargs,
                    char zero, char one,
                    std::basic_string<char, std::char_traits<char>,
                                      std::allocator<char> >*)
{
#if TEST_RW_EXTENSIONS
    // invoke to_string with the number of function arguments specified
    switch (nfargs) {
    case 1:
        return bs.to_string (zero);
    case 0:
        return bs.to_string ();
    }

    return bs.to_string (zero, one);
#else
    return bs.template to_string<char, std::char_traits<char>, std::allocator<char> >();
#endif // TEST_RW_EXTENSIONS
}


inline char to_char (char ch) { return ch; }
inline char to_char (UserChar ch) { return ch.c; }

#ifndef _RWSTD_NO_WCHAR_T
inline char to_char (wchar_t ch) { return char (ch); }
#endif   // _RWSTD_NO_WCHAR_T


// convert a basic_string object to a tempstr object for diagnostics
template <class charT, class Traits, class Alloc>
std::string narrow_string (const std::basic_string<charT, Traits, Alloc> &str)
{
    std::string res;

    for (std::size_t i = 0; i != str.size (); ++i)
        res += to_char (str [i]);

    return res;
}

#define TO_STR(s)   narrow_string (s).c_str ()


// convert an ordinary string to a tempstr object for diagnostics
// using the binary digits specified by `bits'
std::string to_string (const char *str, const char bits [2])
{
    std::string res;

    std::size_t i;

    for (i = 0; str [i]; ++i)
        res += bits ['1' == str [i]];

    return res;
}


template <std::size_t N, class charT, class Traits, class Alloc>
void test_to_string (std::bitset<N>*, charT*, Traits*, Alloc*,
                     bool nontemplate_done)
{
    static const char* const cname = type_name (charT ());
    static const char* const tname = traits_name ((Traits*)0);
    static const char* const aname = alloc_name (Alloc ());

    rw_info (0, 0, __LINE__,
             "std::bitset<%lu>::to_string<%s, %s, %s >()",
             N, cname, tname, aname);

    test_set<N> ts;

    ts.random ();

    const std::bitset<N> bs = ts.to_bitset ();

    charT zero;
    charT one;

    assign (zero, 'o');
    assign (one, 'x');

    int pos;

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    ////////////////////////////////////////////////////////////////////////
    // exercise the overload of the to_string() member function template
    // that takes all three template parameters different from char,
    // char_traits<char>, and allocator<char>

    typedef std::basic_string<charT, Traits, Alloc> String3;

    String3 str3;

    // specify none of the two function arguments (exercise defaults
    // or the respective overloads)
    str3 = bitset_to_string_3 (bs, 0, zero, one, (String3*)0);
    pos  = compare (str3.data (), ts.bits (), "01");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string () == \"%s\", got \"%s\": "
               "mismatch at bit %d",
               N, ts.bits (), TO_STR (str3), pos);

#if TEST_RW_EXTENSIONS
    // specify one of the two function arguments (exercise the default
    // or the respective overload)
    rw_info (0, 0, __LINE__,
             "std::bitset<%lu>::to_string<%s, %s, %s >(\"%s\")",
             N, cname, tname, aname, cname);

    str3 = bitset_to_string_3 (bs, 1, zero, one, (String3*)0);
    pos  = compare (str3.data (), ts.bits (), "o1");

    // No one or two arg 'to_string' function.
    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "o1").c_str (),
               TO_STR (str3), pos);


    // specify both of the two function arguments
    rw_info (0, 0, __LINE__,
             "std::bitset<%lu>::to_string<%s, %s, %s >(%s, %s)",
             N, cname, tname, aname, cname, cname);

    str3 = bitset_to_string_3 (bs, 2, zero, one, (String3*)0);
    pos  = compare (str3.data (), ts.bits (), "ox");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o', 'x') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "ox").c_str (),
               TO_STR (str3), pos);
#endif

    ////////////////////////////////////////////////////////////////////////
    // exercise the overload of the to_string() member function template
    // that takes the first two template parameters different from char,
    // and char_traits<char>
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::to_string<%s, %s >()",
             N, cname, tname);

    typedef std::allocator<charT>                        CharTAlloc;
    typedef std::basic_string<charT, Traits, CharTAlloc> String2;

    String2 str2;

    // specify none of the two function arguments (exercise defaults
    // or the respective overloads)
    str2 = bitset_to_string_2 (bs, 0, zero, one, (String2*)0);
    pos  = compare (str2.data (), ts.bits (), "01");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string () == %s, got %s: mismatch at bit %d",
               N, ts.bits (), TO_STR (str2), pos);


#if TEST_RW_EXTENSIONS
    // specify one of the two function arguments (exercise the default
    // or the respective overload)
    rw_info (0, 0, __LINE__,
             "std::bitset<%lu>::to_string<%s, %s >(%s)",
             N, cname, tname, cname);

    str2 = bitset_to_string_2 (bs, 1, zero, one, (String2*)0);
    pos  = compare (str2.data (), ts.bits (), "o1");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "o1").c_str (),
               TO_STR (str2), pos);

    // specify both of the two function arguments
    rw_info (0, 0, __LINE__,
             "std::bitset<%lu>::to_string<%s, %s >(%s, %s)",
             N, cname, tname, cname, cname);

    str2 = bitset_to_string_2 (bs, 2, zero, one, (String2*)0);
    pos  = compare (str2.data (), ts.bits (), "ox");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o', 'x') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "ox").c_str (),
               TO_STR (str2), pos);
#endif

    ////////////////////////////////////////////////////////////////////////
    // exercise the overload of the to_string() member function template
    // that takes the first template parameter different from char
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::to_string<%s>()",
             N, cname);

    typedef std::char_traits<charT>                          CharTraits;
    typedef std::basic_string<charT, CharTraits, CharTAlloc> String1;

    String1 str1;

    // specify none of the two function arguments (exercise defaults
    // or the respective overloads)
    str1 = bitset_to_string_1 (bs, 0, zero, one, (String1*)0);
    pos  = compare (str1.data (), ts.bits (), "01");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string () == %s, got %s: mismatch at bit %d",
               N, ts.bits (), TO_STR (str1), pos);


#if TEST_RW_EXTENSIONS
    // specify one of the two function arguments (exercise the default
    // or the respective overload)
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::to_string<%s>(%s)",
             N, cname, cname);

    str1 = bitset_to_string_1 (bs, 1, zero, one, (String1*)0);
    pos  = compare (str1.data (), ts.bits (), "o1");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "o1").c_str (),
               TO_STR (str1), pos);

    // specify both of the two function arguments
    rw_info (0, 0, __LINE__,
             "std::bitset<%lu>::to_string<%s>(%s, %s)",
             N, cname, cname, cname);

    str1 = bitset_to_string_1 (bs, 2, zero, one, (String1*)0);
    pos  = compare (str1.data (), ts.bits (), "ox");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o', 'x') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "ox").c_str (),
               TO_STR (str1), pos);
#endif

#endif   // _RWSTD_NO_MEMBER_TEMPLATES


    ////////////////////////////////////////////////////////////////////////
    // exercise the non-template overload of the to_string() member

    if (nontemplate_done)
        return;

    rw_info (0, 0, __LINE__, "std::bitset<%lu>::to_string ()", N);

    typedef std::string String0;

    String0 str0;

    // specify none of the two function arguments (exercise defaults
    // or the respective overloads)
    str0 = bitset_to_string_0 (bs, 0, 'o', 'x', (String0*)0);
    pos  = compare (str0.data (), ts.bits (), "01");

#if TEST_RW_EXTENSIONS
    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string () == %s, got %s: mismatch at bit %d",
               N, ts.bits (), str0.c_str (), pos);


    // specify one of the two function arguments (exercise the default
    // or the respective overload)
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::to_string (char)", N);

    str0 = bitset_to_string_0 (bs, 1, 'o', 'x', (String0*)0);
    pos  = compare (str0.data (), ts.bits (), "o1");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "o1").c_str (),
               str0.c_str (), pos);

    // specify both of the two function arguments
    rw_info (0, 0, __LINE__, "std::bitset<%lu>::to_string (char, char)", N);

    str0 = bitset_to_string_0 (bs, 2, 'o', 'x', (String0*)0);
    pos  = compare (str0.data (), ts.bits (), "ox");

    rw_assert (-1 == pos, 0, __LINE__,
               "bitset<%lu>::to_string ('o', 'x') == %s, got %s: "
               "mismatch at bit %d", N,
               to_string (ts.bits (), "ox").c_str (),
               str0.c_str (), pos);
#endif
}


template <std::size_t N>
void test_to_string (const std::bitset<N>*)
{
    test_to_string ((std::bitset<N>*)0,
                    (char*)0,
                    (std::char_traits<char>*)0,
                    (std::allocator<char>*)0,
                    false);

#ifndef _RWSTD_NO_WCHAR_T

    test_to_string ((std::bitset<N>*)0,
                    (wchar_t*)0,
                    (std::char_traits<wchar_t>*)0,
                    (std::allocator<wchar_t>*)0,
                    true);

#endif   // _RWSTD_NO_WCHAR_T

#if TEST_RW_EXTENSIONS
    test_to_string ((std::bitset<N>*)0,
                    (UserChar*)0,
                    (UserTraits<UserChar>*)0,
                    (std::allocator<UserChar>*)0,
                    true);
#endif // TEST_RW_EXTENSIONS

}

/**************************************************************************/

template <std::size_t N>
void run_test (const std::bitset<N>*)
{
    test_ctors ((std::bitset<N>*)0);
    stress_ctors ((std::bitset<N>*)0);
    test_elem_access ((std::bitset<N>*)0);
    test_operators ((std::bitset<N>*)0);
    test_other ((std::bitset<N>*)0);
    stress_count ((std::bitset<N>*)0);

    test_to_string ((std::bitset<N>*)0);
}

/**************************************************************************/

static int
run_test (int, char**)
{
#if TEST_RW_EXTENSIONS
    test_synopsis ((std::bitset<0>*)0);
#else
    test_synopsis ((std::bitset<1>*)0);
#endif

#define  DO_TEST(N)   run_test ((std::bitset<N>*)0)

#if TEST_RW_EXTENSIONS
    DO_TEST (   0);   // interesting case
#endif
    DO_TEST (   1);   // interesting case
    DO_TEST (   2);
    DO_TEST (  31);
#if DRQS  // problem with shifting by 32 bits since internal specialization
          // uses a single word to represent the data, and shifting a 32-bit
          // representation by 32 bits is undefined behavior.
    DO_TEST (  32);   // interesting case
#endif
    DO_TEST (  33);   // interesting case
    DO_TEST (  34);
    DO_TEST (  63);
#if DRQS  // problem with shifting when shifting whole words equal to its size
    DO_TEST (  64);   // interesting case
#endif
    DO_TEST (  65);   // interesting case
    DO_TEST (  66);

    DO_TEST ( 123);

    DO_TEST ( 127);   // interesting case
#if DRQS  // problem with shifting when shifting whole words equal to its size
    DO_TEST ( 128);   // interesting case
#endif
    DO_TEST ( 129);
    DO_TEST ( 130);
    DO_TEST ( 255);
    DO_TEST ( 256);   // interesting case

#if !defined(_MSC_VER) || _MSC_VER != 1300
    // FIXME: MSVC 514 can't compile bitset<257>!
    DO_TEST ( 257);   // interesting case
#endif

    DO_TEST ( 258);   // interesting case
    DO_TEST ( 333);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    // TODO: add command line options to control tested functionality
    return rw_test (argc, argv, __FILE__,
                    "lib.bitset",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
