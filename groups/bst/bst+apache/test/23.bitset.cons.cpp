/***************************************************************************
 *
 * 23.bitset.cons.cpp - test exercising [lib.bitset.cons]
 *
 * $Id: 23.bitset.cons.cpp 429065 2006-08-05 21:57:17Z sebor $
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
 * Copyright 2004-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <bitset>      // for bitset
#include <stdexcept>   // for invalid_argument, out_of_range
#include <string>      // for basic_string

#include <cstddef>     // for size_t

#include <rw_char.h>   // for UserChar, UserTraits
#include <driver.h>    // for rw_test(), ...

/**************************************************************************/

int opt_char;            // for --no-char
int opt_wchar;           // for --no-wchar_t
int opt_userchar;        // for --no-UserChar
int opt_no_exceptions;   // for --no-exceptions

/**************************************************************************/

template <std::size_t N>
int bcmp (const std::bitset<N> &bset,
          const char           *str)
{
    RW_ASSERT (0 != str);

    for (std::size_t i = 0; i != N; ++i) {

        const char bit = bset [i] ? '1' : '0';

        if (bit != str [N - i - 1])
            return -1;
    }

    return 0;
}

/**************************************************************************/

// equivalent to the result of std::bitset<bits>(val).to_string<char>()
const char*
ulong_to_bitstring (unsigned long val, std::size_t bits)
{
    const std::size_t ulongbits = sizeof (unsigned long) * 8;

    static char bitstr [1024];

    for (std::size_t i = 0; i != bits; ++i) {
        if (i < ulongbits)
            bitstr [bits - i - 1] = val & (1UL << i) ? '1' : '0';
        else
            bitstr [bits - i - 1] = '0';
    }

    bitstr [bits] = '\0';
    return bitstr;
}

/**************************************************************************/

// exercises the effects of the default bitset ctor
template <std::size_t N>
void test_default_ctor (std::bitset<N>*)
{
    rw_info (0, 0, __LINE__, "std::bitset<%u>::bitset()", N);

    const std::bitset<N> bset;

    char all_zeros [N + 1];
    for (std::size_t i = 0; i != N; ++i) all_zeros [i] = '0';
    all_zeros [N] = '\0';

    rw_assert (!bcmp (bset, all_zeros), 0, __LINE__,
               "bitset<%u>::bitset() expected 0, got { %.*b }",
               N, int (N), &bset);
}

/**************************************************************************/

// exercises the effects of bitset<N>::bitset(unsigned long)
template <std::size_t N>
void test_long_ctor (std::bitset<N>*)
{
    rw_info (0, 0, __LINE__, "std::bitset<%u>::bitset(unsigned long)", N);

#undef DO_TEST
#define DO_TEST(T, val)                                                 \
    do {                                                                \
        const T v = val;                                                \
        const std::bitset<N> bset (v);                                  \
        const char* const bstr = ulong_to_bitstring (val, N);           \
        rw_assert (!bcmp (bset, (const char*)bstr), 0, __LINE__,        \
                   "bitset<%u>::bitset(%zu) == { %{.*b} }, "            \
                   "got { %{.*b} }", N, val, bstr, int (N), &bset);     \
    } while (0)

    DO_TEST (unsigned long,  0);
    DO_TEST (unsigned long,  1);
    DO_TEST (unsigned long,  2);
    DO_TEST (unsigned long,  3);
    DO_TEST (unsigned long,  4);
    DO_TEST (unsigned long,  5);
    DO_TEST (unsigned long,  6);
    DO_TEST (unsigned long,  7);
    DO_TEST (unsigned long,  8);
    DO_TEST (unsigned long,  9);
    DO_TEST (unsigned long, 10);
    DO_TEST (unsigned long, 11);
    DO_TEST (unsigned long, 12);
    DO_TEST (unsigned long, 13);
    DO_TEST (unsigned long, 14);
    DO_TEST (unsigned long, 15);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX / 16);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX /  8);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX /  4);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX /  2);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX -  2);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX -  1);
    DO_TEST (unsigned long, _RWSTD_LONG_MAX);

    // exercise calls to the ctor with arguments of other integer types
    DO_TEST (char,           16);
    DO_TEST (signed char,    17);
    DO_TEST (unsigned char,  18);
    DO_TEST (short,          19);
    DO_TEST (unsigned short, 20);
    DO_TEST (int,            21);
    DO_TEST (unsigned int,   22);
    DO_TEST (long,           23);
}

/**************************************************************************/

// invokes coverting bitset ctor from basic_string<char, ...>
template <std::size_t N, class Traits, class Allocator>
std::bitset<N>
bitset_ctor (std::bitset<N>*,
             const std::basic_string<char, Traits, Allocator> *bstr,
             const char                                       *str,
             std::size_t pos, std::size_t n, const char bin_digits [2])
{
    if (bstr) {
        if (std::size_t (-1) == pos)
            return std::bitset<N>(*bstr);

        if (std::size_t (-1) == n)
            return std::bitset<N>(*bstr, pos);

        if (0 == bin_digits)
            return std::bitset<N>(*bstr, pos, n);

#if TEST_RW_EXTENSIONS
        if (Traits::eq (bin_digits [0], bin_digits [1]))
            return std::bitset<N>(*bstr, pos, n, bin_digits [0]);

        return std::bitset<N>(*bstr, pos, n, bin_digits [0], bin_digits [1]);
#else
        RW_ASSERT (!"test logic error");
        return std::bitset<N>();
#endif
    }

#if    !defined (_RWSTD_NO_EXT_BITSET_CTOR_CHAR_ARRAY) \
    && !defined (_RWSTD_NO_EXT_BITSET_CTOR_STRING)

    if (std::size_t (-1) == pos)
        return std::bitset<N>(str);

    if (std::size_t (-1) == n)
        return std::bitset<N>(str, pos);

    if (0 == bin_digits)
        return std::bitset<N>(str, pos, n);

#  if TEST_RW_EXTENSIONS
    if (Traits::eq (bin_digits [0], bin_digits [1]))
        return std::bitset<N>(str, pos, n, bin_digits [0]);

    return std::bitset<N>(str, pos, n, bin_digits [0], bin_digits [1]);
#  else
    RW_ASSERT (!"test logic error");
    return std::bitset<N>();
#  endif // TEST_RW_EXTENSIONS

#else

    RW_ASSERT (!"test logic error");

    return std::bitset<N>();

#endif   // _RWSTD_NO_EXT_BITSET_CTOR_{CHAR_ARRAY,STRING)

}


#ifndef _RWSTD_NO_WCHAR_T

// invokes coverting bitset ctor from basic_string<wchar_t, ...>
template <std::size_t N, class Traits, class Allocator>
std::bitset<N>
bitset_ctor (std::bitset<N>*,
             const std::basic_string<wchar_t, Traits, Allocator> *bstr,
             const wchar_t                                       *wstr,
             std::size_t pos, std::size_t n, const wchar_t bin_digits [2])
{
    if (bstr) {
        if (std::size_t (-1) == pos)
            return std::bitset<N>(*bstr);

        if (std::size_t (-1) == n)
            return std::bitset<N>(*bstr, pos);

        if (0 == bin_digits)
            return std::bitset<N>(*bstr, pos, n);

#if TEST_RW_EXTENSIONS_LWG
        if (Traits::eq (bin_digits [0], bin_digits [1]))
            return std::bitset<N>(*bstr, pos, n, bin_digits [0]);

        return std::bitset<N>(*bstr, pos, n, bin_digits [0], bin_digits [1]);
#else
        RW_ASSERT (!"test logic error");
        return std::bitset<N>();
#endif // TEST_RW_EXTENSIONS

    }

#if    !defined (_RWSTD_NO_EXT_BITSET_CTOR_CHAR_ARRAY) \
    && !defined (_RWSTD_NO_EXT_BITSET_CTOR_STRING)

    if (std::size_t (-1) == pos)
        return std::bitset<N>(wstr);

    if (std::size_t (-1) == n)
        return std::bitset<N>(wstr, pos);

    if (0 == bin_digits)
        return std::bitset<N>(wstr, pos, n);

#  if TEST_RW_EXTENSIONS
    if (Traits::eq (bin_digits [0], bin_digits [1]))
        return std::bitset<N>(wstr, pos, n, bin_digits [0]);

    return std::bitset<N>(wstr, pos, n, bin_digits [0], bin_digits [1]);
#  else
    RW_ASSERT (!"test logic error");
    return std::bitset<N>();
#  endif // TEST_RW_EXTENSIONS

#else

    RW_ASSERT (!"test logic error");

    return std::bitset<N>();

#endif   // _RWSTD_NO_EXT_BITSET_CTOR_{CHAR_ARRAY,STRING)

}

#endif   // _RWSTD_NO_WCHAR_T


#if TEST_RW_EXTENSIONS
// invokes coverting bitset ctor from basic_string<UserChar, ...>
template <std::size_t N, class Traits, class Allocator>
std::bitset<N>
bitset_ctor (std::bitset<N>*,
             const std::basic_string<UserChar, Traits, Allocator> *bstr,
             const UserChar* /* intentionally unused */,
             std::size_t pos, std::size_t n, const UserChar bin_digits [2])
{
    if (std::size_t (-1) == pos)
        pos = 0;

#if ! TEST_RW_EXTENSIONS
    RW_ASSERT (0 == bin_digits && "test logic error");
#endif // TEST_RW_EXTENSIONS

    if (0 == bin_digits) {
        static const UserChar zero_one[] = { { 0.0, '0' }, { 0.0, '1' } };

        bin_digits = zero_one;
    }

    if (bstr) {
#if TEST_RW_EXTENSIONS
        return std::bitset<N>(*bstr, pos, n, bin_digits [0], bin_digits [1]);
#else
        return std::bitset<N>(*bstr, pos, n);
#endif // TEST_RW_EXTENSIONS
    }

    // cannot invoke std::bitset<N>(const UserChar*) without providing
    // an explicit specialization of std::char_traits<UserChar> that
    // implements eq() and length()
    RW_ASSERT (!"test logic error");

    return std::bitset<N>();
}
#endif // TEST_RW_EXTENSIONS

// constructs a bitset object by invoking one of the overloaded ctors
template <std::size_t N, class charT, class Traits, class Allocator>
std::bitset<N>
bitset_ctor (std::bitset<N>*,
             const std::basic_string<charT, Traits, Allocator> *p,
             const char *str, std::size_t pos, std::size_t n)
{
    charT wstr [1024];

    for (std::size_t i = 0; i != sizeof wstr / sizeof *wstr; ++i) {
        if (str [i])
            Traits::assign (wstr [i], make_char (str [i], (charT*)0));
        else {
            Traits::assign (wstr [i], charT ());
            break;
        }
    }

    typedef std::bitset<N>                              Bitset;
    typedef std::basic_string<charT, Traits, Allocator> String;

    if (p) {   // use one of the const basic_string& overloads

        const String bstr (wstr);

//#if TEST_RW_EXTENSIONS
        return bitset_ctor ((Bitset*)0, &bstr, 0, pos, n, (charT*)0);
//#else
//        return bitset_ctor ((Bitset*)0, &bstr, 0, pos, n);
//#endif // TEST_RW_EXTENSIONS
    }
    else {   // invoke one of the const charT* overloads

        return bitset_ctor ((Bitset*)0, (String*)0, wstr, pos, n, (charT*)0);
    }
}


template <class charT>
const char* type_prefix (charT) { return ""; }

#ifndef _RWSTD_NO_WCHAR_T

const char* type_prefix (wchar_t) { return "L"; }

#endif   // _RWSTD_NO_WCHAR_T

enum { InvalidArgument = 1, OutOfRange };

template <std::size_t N, class charT, class Traits, class Allocator>
void test_string_ctor (std::bitset<N>*,
                       std::basic_string<charT, Traits, Allocator> *p,
                       int line,
                       const char *str, std::size_t pos, std::size_t n,
                       const char *bitstr)
{
    static const char* const tpfx = type_prefix (charT ());

    typedef std::bitset<N>                              Bitset;
    typedef std::basic_string<charT, Traits, Allocator> String;

    static const char* const exstr[] = {
        "failed to throw",
        "caught std::invalid_argument",
        "caught std::out_of_range",
        "caught unknown exception"
    };

    // compute which exception, if any, the ctor is expected to throw
    const int except = bitstr - (char*)0 < 3 ? bitstr - (char*)0 : 0;

    if (except && opt_no_exceptions)
        return;

    int caught = 0;


    _TRY {
        // invoke one of the overloads of the constructor
        const Bitset bset = bitset_ctor ((Bitset*)0, p, str, pos, n);

        if (!except)
            rw_assert (!bcmp (bset, bitstr), 0, line,
                       "line %d: bitset<%u>::bitset(%s\"%s\""
                       "%{?}, %zu%{?}, %zu%{;}%{;}) "
                       "== { %s }, got { %{.*b} }",
                       __LINE__, N, tpfx, str,
                       pos != std::size_t (-1), pos,
                       n != std::size_t (-1), n,
                       bitstr, int (N), &bset);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::out_of_range&) {
        caught = OutOfRange;
    }
    catch (const std::invalid_argument&) {
        caught = InvalidArgument;
    }
    catch (...) {
        caught = 3;
    }
#endif   // _RWSTD_NO_EXCEPTIONS

    rw_assert (except == caught, 0, line,
               "line %d: bitset<%u>::bitset(%s\"%s\", %u, %u) "
               "unexpectedly %s", __LINE__, N, tpfx, str, pos, n,
               exstr [caught]);
}


std::string
fillstr (const char *pfx, int bit, std::size_t count, const char *sfx)
{
    std::string str (pfx);
    str.append (count, char ('0' + bit));
    str.append (sfx);
    return str;
}


template <class charT, class Traits, class Allocator>
void test_string_ctor (std::basic_string<charT, Traits, Allocator> *p,
                       const char *cname)
{
#undef DO_TEST
#define DO_TEST(N, str, pos, n, except)                         \
    test_string_ctor ((std::bitset<N>*)0, p, __LINE__,          \
                      str, std::size_t (pos), std::size_t (n),  \
                      (const char*)except)

    //////////////////////////////////////////////////////////////////

#define CTOR_INFO(N)                                            \
    rw_info (0, 0, __LINE__, "std::bitset<%i>::bitset(const "   \
             "%{?}std::basic_string<%s>&%{:}%3$s%{;})",         \
             N, 0 != p, cname)

#if TEST_RW_EXTENSIONS
    CTOR_INFO (0);
#endif

#define TEST_1(N, str, except)   DO_TEST(N, str, -1, -1, except)

    //      +----------- bitset size
    //      |  +-------- string argument
    //      |  |     +-- result or exception
    //      v  v     v
#if TEST_RW_EXTENSIONS
    TEST_1 (0, "",   "");
    TEST_1 (0, "0",  "");
    TEST_1 (0, "1",  "");
    TEST_1 (0, "2",  InvalidArgument);
    TEST_1 (0, "00", "");
    TEST_1 (0, "01", "");
    TEST_1 (0, "02", InvalidArgument);
#endif
    CTOR_INFO (1);

    TEST_1 (1, "",   "0");
    TEST_1 (1, "0",  "0");
    TEST_1 (1, "1",  "1");
    TEST_1 (1, "2",  InvalidArgument);
    TEST_1 (1, "00", "0");
    TEST_1 (1, "01", "0");

#if TEST_RW_EXTENSIONS
    // Test is incorrect here.  According to standard, only the first 'N'
    // characters (where 'N' corresponds to the bitset template parameter)
    // is considered.  So shouldn't throw any exceptions.
    TEST_1 (1, "02", InvalidArgument);
#else
    TEST_1 (1, "02", "0");
#endif

    CTOR_INFO (2);

    TEST_1 (2, "",    "00");
    TEST_1 (2, "0",   "00");
    TEST_1 (2, "1",   "01");
    TEST_1 (2, "2",   InvalidArgument);
    TEST_1 (2, "00",  "00");
    TEST_1 (2, "01",  "01");
    TEST_1 (2, "02",  InvalidArgument);
    TEST_1 (2, "10",  "10");
    TEST_1 (2, "11",  "11");
    TEST_1 (2, "12",  InvalidArgument);
    TEST_1 (2, "20",  InvalidArgument);
    TEST_1 (2, "21",  InvalidArgument);
    TEST_1 (2, "22",  InvalidArgument);
    TEST_1 (2, "000", "00");
    TEST_1 (2, "001", "00");
#if TEST_RW_EXTENSIONS
    TEST_1 (2, "002", InvalidArgument);
#else
    TEST_1 (2, "002", "00");
#endif
    TEST_1 (2, "010", "01");
    TEST_1 (2, "011", "01");
#if TEST_RW_EXTENSIONS
    TEST_1 (2, "012", InvalidArgument);
#else
    TEST_1 (2, "012", "01");
#endif
    TEST_1 (2, "020", InvalidArgument);
    TEST_1 (2, "100", "10");
    TEST_1 (2, "101", "10");
    TEST_1 (2, "110", "11");
    TEST_1 (2, "111", "11");
#if TEST_RW_EXTENSIONS
    TEST_1 (2, "112", InvalidArgument);
#else
    TEST_1 (2, "112", "11");
#endif
    TEST_1 (2, "200", InvalidArgument);
    TEST_1 (2, "201", InvalidArgument);
    TEST_1 (2, "202", InvalidArgument);

    CTOR_INFO (3);

    TEST_1 (3, "",     "000");
    TEST_1 (3, "0",    "000");
    TEST_1 (3, "1",    "001");
    TEST_1 (3, "2",    InvalidArgument);
    TEST_1 (3, "00",   "000");
    TEST_1 (3, "01",   "001");
    TEST_1 (3, "02",   InvalidArgument);
    TEST_1 (3, "10",   "010");
    TEST_1 (3, "11",   "011");
    TEST_1 (3, "12",   InvalidArgument);
    TEST_1 (3, "000",  "000");
    TEST_1 (3, "001",  "001");
    TEST_1 (3, "002",  InvalidArgument);
    TEST_1 (3, "010",  "010");
    TEST_1 (3, "011",  "011");
    TEST_1 (3, "012",  InvalidArgument);
    TEST_1 (3, "100",  "100");
    TEST_1 (3, "101",  "101");
    TEST_1 (3, "102",  InvalidArgument);
    TEST_1 (3, "100",  "100");
    TEST_1 (3, "101",  "101");
    TEST_1 (3, "102",  InvalidArgument);

    TEST_1 (3, "0001", "000");
    TEST_1 (3, "0010", "001");
    TEST_1 (3, "0100", "010");
    TEST_1 (3, "1000", "100");
    TEST_1 (3, "1001", "100");

    TEST_1 (3, "0001", "000");
    TEST_1 (3, "0010", "001");
    TEST_1 (3, "0100", "010");
    TEST_1 (3, "1000", "100");
    TEST_1 (3, "1001", "100");
#if TEST_RW_EXTENSIONS
    TEST_1 (3, "1002", InvalidArgument);
#else
    TEST_1 (3, "1002", "100");
#endif
    TEST_1 (3, "2000", InvalidArgument);

    TEST_1 (3, "10010",  "100");
    TEST_1 (3, "10011",  "100");
#if TEST_RW_EXTENSIONS
    TEST_1 (3, "10012",  InvalidArgument);
    TEST_1 (3, "100102", InvalidArgument);
#else
    TEST_1 (3, "10012",  "100");
    TEST_1 (3, "100102", "100");
#endif

    // macro to insert N zeros or ones in between two string literals
#define FILL(pfx, bit, N, sfx) fillstr (pfx, bit, N, sfx).c_str ()

    CTOR_INFO (33);

    //                        +------------- preceded by these characters
    //                        |   +--------- binary digit (0 or 1)
    //                        |   |  +------ repeated this many times
    //                        |   |  |   +-- followed by these characters
    //                        |   |  |   |
    //                        v   v  v   v
    TEST_1 (33, "",     FILL ("", 0, 33, ""));
    TEST_1 (33, "0",    FILL ("", 0, 33, ""));
    TEST_1 (33, "1",    FILL ("", 0, 32, "1"));
    TEST_1 (33, "00",   FILL ("", 0, 31, "00"));
    TEST_1 (33, "01",   FILL ("", 0, 31, "01"));
    TEST_1 (33, "10",   FILL ("", 0, 31, "10"));
    TEST_1 (33, "11",   FILL ("", 0, 31, "11"));
    TEST_1 (33, "100",  FILL ("", 0, 30, "100"));
    TEST_1 (33, "101",  FILL ("", 0, 30, "101"));

    TEST_1 (33, FILL ("1", 0, 32, ""),  FILL ("10", 0, 30, "0"));
    TEST_1 (33, FILL ("1", 0, 31, "1"), FILL ("10", 0, 30, "1"));
    TEST_1 (33, FILL ("1", 0, 31, "2"), InvalidArgument);

    TEST_1 (33, FILL ("", 1, 33, ""),      FILL ("", 1, 33, ""));
    TEST_1 (33, FILL ("", 1, 32, "0"),     FILL ("", 1, 32, "0"));
    TEST_1 (33, FILL ("", 1, 31, "00"),    FILL ("", 1, 31, "00"));
    TEST_1 (33, FILL ("", 1, 30, "000"),   FILL ("", 1, 30, "000"));
    TEST_1 (33, FILL ("", 1, 29, "0000"),  FILL ("", 1, 29, "0000"));
    TEST_1 (33, FILL ("", 1, 28, "00000"), FILL ("", 1, 28, "00000"));
    TEST_1 (33, FILL ("", 1, 28, "00001"), FILL ("", 1, 28, "00001"));
    TEST_1 (33, FILL ("", 1, 28, "00010"), FILL ("", 1, 28, "00010"));
    TEST_1 (33, FILL ("", 1, 28, "00100"), FILL ("", 1, 28, "00100"));
    TEST_1 (33, FILL ("", 1, 28, "01000"), FILL ("", 1, 28, "01000"));
    TEST_1 (33, FILL ("", 1, 28, "02000"), InvalidArgument);

    TEST_1 (33, FILL ("",  1, 33, "0"),   FILL ("", 1, 33, ""));
    TEST_1 (33, FILL ("",  1, 33, "1"),   FILL ("", 1, 33, ""));
#if TEST_RW_EXTENSIONS
    TEST_1 (33, FILL ("",  1, 33, "2"),   InvalidArgument);
#else
    TEST_1 (33, FILL ("",  1, 33, "2"),   FILL ("", 1, 33, ""));
#endif
    TEST_1 (33, FILL ("2", 1, 33, ""),    InvalidArgument);

    CTOR_INFO (80);

    TEST_1 (80, FILL ("" ,  1, 80, ""),   FILL (""  , 1, 80, ""));
    TEST_1 (80, FILL ("1",  0, 78, "1"),  FILL ("1" , 0, 78, "1"));
    TEST_1 (80, FILL ("1",  0, 77, "10"), FILL ("1" , 0, 77, "10"));
    TEST_1 (80, FILL ("1",  0, 77, "20"), InvalidArgument);
    TEST_1 (80, FILL ("2",  0, 77, "00"), InvalidArgument);
    TEST_1 (80, FILL ("02", 0, 76, "00"), InvalidArgument);
    TEST_1 (80, FILL ("12", 0, 76, "00"), InvalidArgument);

    //////////////////////////////////////////////////////////////////
#undef CTOR_INFO
#define CTOR_INFO(N)                                            \
    rw_info (0, 0, __LINE__, "std::bitset<%i>::bitset(const "   \
             "%{?}std::basic_string<%s>&%{:}%3$s%{;}, size_t)", \
             N, 0 != p, cname)

#define TEST_2(N, str, pos, except)   DO_TEST(N, str, pos, -1, except)

#if TEST_RW_EXTENSIONS
    CTOR_INFO (0);

    TEST_2 (0, "",  0, "");
    TEST_2 (0, "",  1, OutOfRange);
    TEST_2 (0, "0", 0, "");
    TEST_2 (0, "1", 0, "");
    TEST_2 (0, "2", 0, InvalidArgument);
    TEST_2 (0, "0", 1, "");
    TEST_2 (0, "1", 1, "");
    TEST_2 (0, "2", 1, "");
#endif

    CTOR_INFO (1);

    TEST_2 (1, "",   0, "0");
    TEST_2 (1, "0",  0, "0");
    TEST_2 (1, "1",  0, "1");
    TEST_2 (1, "2",  0, InvalidArgument);

    TEST_2 (1, "00", 0, "0");
    TEST_2 (1, "01", 0, "0");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "02", 0, InvalidArgument);
#else
    TEST_2 (1, "02", 0, "0");
#endif
    TEST_2 (1, "10", 0, "1");
    TEST_2 (1, "11", 0, "1");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "12", 0, InvalidArgument);
#else
    TEST_2 (1, "12", 0, "1");
#endif

    TEST_2 (1, "00", 1, "0");
    TEST_2 (1, "01", 1, "1");
    TEST_2 (1, "02", 1, InvalidArgument);
    TEST_2 (1, "10", 1, "0");
    TEST_2 (1, "11", 1, "1");
    TEST_2 (1, "12", 1, InvalidArgument);
    TEST_2 (1, "20", 1, "0");
    TEST_2 (1, "21", 1, "1");
    TEST_2 (1, "22", 1, InvalidArgument);

    TEST_2 (1, "000", 1, "0");
    TEST_2 (1, "001", 1, "0");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "002", 1, InvalidArgument);
#else
    TEST_2 (1, "002", 1, "0");
#endif
    TEST_2 (1, "010", 1, "1");
    TEST_2 (1, "011", 1, "1");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "012", 1, InvalidArgument);
#else
    TEST_2 (1, "012", 1, "1");
#endif
    TEST_2 (1, "020", 1, InvalidArgument);
    TEST_2 (1, "021", 1, InvalidArgument);
    TEST_2 (1, "022", 1, InvalidArgument);
    TEST_2 (1, "120", 1, InvalidArgument);
    TEST_2 (1, "121", 1, InvalidArgument);
    TEST_2 (1, "122", 1, InvalidArgument);
    TEST_2 (1, "200", 1, "0");
    TEST_2 (1, "201", 1, "0");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "202", 1, InvalidArgument);
#else
    TEST_2 (1, "202", 1, "0");
#endif

    TEST_2 (1, "320",  2, "0");
    TEST_2 (1, "321",  2, "1");
    TEST_2 (1, "322",  2, InvalidArgument);
    TEST_2 (1, "3200", 2, "0");
    TEST_2 (1, "3201", 2, "0");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "3202", 2, InvalidArgument);
#else
    TEST_2 (1, "3202", 2, "0");
#endif
    TEST_2 (1, "3210", 2, "1");
    TEST_2 (1, "3211", 2, "1");
#if TEST_RW_EXTENSIONS
    TEST_2 (1, "3212", 2, InvalidArgument);
#else
    TEST_2 (1, "3212", 2, "1");
#endif

    TEST_2 (1, "",   1, OutOfRange);

    //////////////////////////////////////////////////////////////////
#undef CTOR_INFO
#define CTOR_INFO(N)                                                    \
    rw_info (0, 0, __LINE__, "std::bitset<%i>::bitset(const "           \
             "%{?}std::basic_string<%s>&%{:}%3$s%{;}, size_t, size_t)", \
             N, 0 != p, cname)

#if TEST_RW_EXTENSIONS
    CTOR_INFO (0);
#endif

#define TEST_3(N, str, pos, n, except)   DO_TEST(N, str, pos, n, except)

    //      +-------------------- bitset size
    //      |  +----------------- string argument
    //      |  |      +---------- pos
    //      |  |      |   +------ n
    //      |  |      |   |  +--- result or exception
    //      |  |      |   |  |
    //      v  v      v   v  v
#if TEST_RW_EXTENSIONS
    TEST_3 (0, "",    0,  0, "");
    TEST_3 (0, "",    1,  0, OutOfRange);
    TEST_3 (0, "02",  0,  0, "");
#endif

    CTOR_INFO (1);

    TEST_3 (1, "012", 0,  1, "0");
    TEST_3 (1, "012", 1,  1, "1");
    TEST_3 (1, "012", 2,  1, InvalidArgument);

    TEST_3 (1, "",    0,  0, "0");
    TEST_3 (1, "",    1,  0, OutOfRange);

    CTOR_INFO (8);

    TEST_3 (8, "11111111",  0, 8, FILL ("", 1, 8, ""));
    TEST_3 (8, "11111111",  1, 7, FILL ("0", 1, 7, ""));
    TEST_3 (8, "11111111",  2, 6, FILL ("00", 1, 6, ""));
    TEST_3 (8, "11111111",  3, 5, FILL ("000", 1, 5, ""));
    TEST_3 (8, "11111111",  4, 4, FILL ("0000", 1, 4, ""));
    TEST_3 (8, "11111111",  5, 3, FILL ("00000", 1, 3, ""));
    TEST_3 (8, "11111111",  6, 2, FILL ("000000", 1, 2, ""));
    TEST_3 (8, "11111111",  7, 1, FILL ("0000000", 1, 1, ""));
    TEST_3 (8, "11111111",  8, 0, FILL ("00000000", 1, 0, ""));
    TEST_3 (8, "11111111",  9, 0, OutOfRange);
    TEST_3 (8, "11111111",  9, 1, OutOfRange);
    TEST_3 (8, "111111112", 7, 1, FILL ("0000000", 1, 1, ""));
    TEST_3 (8, "111111112", 7, 2, InvalidArgument);

    TEST_3 (8, "211111112", 1, 7, FILL ("0",   1, 7, ""));
    TEST_3 (8, "210111112", 1, 7, FILL ("010", 1, 5, ""));
    TEST_3 (8, "211111012", 1, 7, FILL ("0",   1, 5, "01"));

    CTOR_INFO (40);

    TEST_3 (40, FILL ("",  0, 40, ""),   0,  0, FILL ("",  0, 40, ""));
    TEST_3 (40, FILL ("",  0, 40, "1"),  0, 40, FILL ("",  0, 40, ""));
    TEST_3 (40, FILL ("",  0, 40, "1"),  0, 41, FILL ("",  0, 40, ""));
    TEST_3 (40, FILL ("",  0, 39, "1"),  0, 39, FILL ("",  0, 40, ""));
    TEST_3 (40, FILL ("",  0, 39, "1"),  0, 40, FILL ("",  0, 39, "1"));
    TEST_3 (40, FILL ("1", 0, 40, "1"),  0,  0, FILL ("",  0, 40, ""));
    TEST_3 (40, FILL ("1", 0, 38, "1"),  0, 40, FILL ("1", 0, 38, "1"));
    TEST_3 (40, FILL ("1", 0, 38, "1"),  0, 40, FILL ("1", 0, 38, "1"));
    TEST_3 (40, FILL ("2", 0, 40, "2"),  1, 40, FILL ("",  0, 40, ""));
}

/**************************************************************************/

template <std::size_t N>
void test_ctors (std::bitset<N>* pb)
{
    test_default_ctor (pb);
    test_long_ctor (pb);
}

/**************************************************************************/

template <class Traits>
void test_ctors (const Traits*, const char *cname)
{
    typedef typename Traits::char_type                      char_type;
    typedef std::allocator<char_type>                       Allocator;
    typedef std::basic_string<char_type, Traits, Allocator> String;

    // exercise const std::string& overloads
    test_string_ctor ((String*)1, cname);

#if    !defined (_RWSTD_NO_EXT_BITSET_CTOR_CHAR_ARRAY) \
    && !defined (_RWSTD_NO_EXT_BITSET_CTOR_STRING)

    // exercise const char* and const wchar_t* extensions
    // but avoid calling the bitset(const UserChar*) overload in order
    if (sizeof (char_type) != sizeof (UserChar))
        test_string_ctor ((String*)0, cname);

#endif   // _RWSTD_NO_EXT_BITSET_CTOR_{CHAR_ARRAY,STRING}

}

/**************************************************************************/

static int
run_test (int, char**)
{
#ifdef _RWSTD_NO_EXCEPTIONS
    opt_no_exceptions = 1;
#endif   // _RWSTD_NO_EXCEPTIONS

    if (opt_no_exceptions)
        rw_note (0, 0, 0, "exception tests disabled");

#undef DO_TEST
#define DO_TEST(N)   test_ctors ((std::bitset<N>*)0)

#if TEST_RW_EXTENSIONS // 0 sized bitset should not be allowed
    DO_TEST (   0);   // interesting case
#endif
    DO_TEST (   1);   // interesting case
    DO_TEST (   2);
    DO_TEST (  31);
    DO_TEST (  32);   // interesting case
    DO_TEST (  33);   // interesting case
    DO_TEST (  34);
    DO_TEST (  63);
    DO_TEST (  64);   // interesting case
    DO_TEST (  65);   // interesting case
    DO_TEST (  66);

    DO_TEST ( 123);

    DO_TEST ( 127);   // interesting case
    DO_TEST ( 128);   // interesting case
    DO_TEST ( 129);
    DO_TEST ( 130);
    DO_TEST ( 255);
    DO_TEST ( 256);   // interesting case

#if !defined(_MSC_VER) || _MSC_VER != 1300
    // FIXME: MSVC can't compile bitset<257>!
    DO_TEST ( 257);   // interesting case
#endif

    DO_TEST ( 258);   // interesting case
    DO_TEST ( 333);

    if (0 <= opt_char) {
        typedef std::char_traits<char> NarrowTraits;

        test_ctors ((NarrowTraits*)0, "char");
    }
    else {
        rw_note (0, 0, __LINE__, "tests of char specialization disabled");
    }

#ifndef _RWSTD_NO_WCHAR_T

    if (0 <= opt_wchar) {
        typedef std::char_traits<wchar_t> WideTraits;

        test_ctors ((WideTraits*)0, "wchar_t");
    }
    else {
        rw_note (0, 0, __LINE__, "tests of wchar_t specialization disabled");
    }

#endif   // _RWSTD_NO_WCHAR_T

#if TEST_RW_EXTENSIONS
    if (0 <= opt_userchar) {
        test_ctors ((UserTraits<UserChar>*)0, "UserChar");
    }
    else {
        rw_note (0, 0, __LINE__, "tests of UserChar specialization disabled");
    }
#endif

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.bitset.cons",
                    0 /* no comment */,
                    run_test,
                    "|-char~ "
                    "|-wchar_t~ "
#if TEST_RW_EXTENSIONS
                    "|-UserChar~ "
#endif
                    "|-no-exceptions# ",
                    &opt_char,
                    &opt_wchar,
                    &opt_userchar,
                    &opt_no_exceptions,
                    (void*)0   /* sentinel */);
}
