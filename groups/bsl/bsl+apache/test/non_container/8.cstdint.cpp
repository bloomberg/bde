// -*- C++ -*-
/***************************************************************************
 *
 * 8.cstdint.cpp - test exercising tr.c99.cstdint
 *
 * $Id: 8.cstdint.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2006 Rogue Wave Software.
 * 
 **************************************************************************/

#if TEST_RW_EXTENSIONS // TR1 cstdint

enum {
    // these are expected to be hidden by macros #defined in <cstdint>
    // #included below; references to macros that are not #defined in
    // the header will silently use these enumerators and prevent
    // compilation errors that would result otherwise
    PTRDIFF_MIN,      PTRDIFF_MAX,
    SIZE_MAX,
    WCHAR_MIN,        WCHAR_MAX,
    WINT_MIN,         WINT_MAX,
    SIG_ATOMIC_MIN,   SIG_ATOMIC_MAX,
    INT8_MIN,         INT8_MAX,
    INT16_MIN,        INT16_MAX,
    INT32_MIN,        INT32_MAX,
    INT64_MIN,        INT64_MAX,
    INT_LEAST8_MIN,   INT_LEAST8_MAX,
    INT_LEAST16_MIN,  INT_LEAST16_MAX,
    INT_LEAST32_MIN,  INT_LEAST32_MAX,
    INT_LEAST64_MIN,  INT_LEAST64_MAX,
    INT_FAST8_MIN,    INT_FAST8_MAX,
    INT_FAST16_MIN,   INT_FAST16_MAX,
    INT_FAST32_MIN,   INT_FAST32_MAX,
    INT_FAST64_MIN,   INT_FAST64_MAX,
    INTPTR_MAX,

    UINT8_MIN,        UINT8_MAX,
    UINT16_MIN,       UINT16_MAX,
    UINT32_MIN,       UINT32_MAX,
    UINT64_MIN,       UINT64_MAX,
    UINT_LEAST8_MIN,  UINT_LEAST8_MAX,
    UINT_LEAST16_MIN, UINT_LEAST16_MAX,
    UINT_LEAST32_MIN, UINT_LEAST32_MAX,
    UINT_LEAST64_MIN, UINT_LEAST64_MAX,
    UINT_FAST8_MIN,   UINT_FAST8_MAX,
    UINT_FAST16_MIN,  UINT_FAST16_MAX,
    UINT_FAST32_MIN,  UINT_FAST32_MAX,
    UINT_FAST64_MIN,  UINT_FAST64_MAX,
    UINTPTR_MAX
};


#include <tr1/cstdint>

#include <driver.h>

/**************************************************************************/

static int rw_opt_no_macros;   // for --no-macros
static int rw_opt_no_types;    // for --no-types
static int rw_opt_no_64_bit;   // for --no-64-bit

/**************************************************************************/


static const char* const
missing_macros[] = {

#ifndef INT_LEAST8_MIN
    "INT_LEAST8_MIN",
#else
    "",
#endif   // INT_LEAST8_MIN

#ifndef INT_FAST8_MIN
    "INT_FAST8_MIN",
#else
    "",
#endif   // INT_FAST8_MIN

#ifndef INT_LEAST16_MIN
    "INT_LEAST16_MIN",
#else
    "",
#endif   // INT_LEAST16_MIN

#ifndef INT_FAST16_MIN
    "INT_FAST16_MIN",
#else
    "",
#endif   // INT_FAST16_MIN


#ifndef INT_LEAST32_MIN
    "INT_LEAST32_MIN",
#else
    "",
#endif   // INT_LEAST32_MIN

#ifndef INT_FAST32_MIN
    "INT_FAST32_MIN",
#else
    "",
#endif   // INT_FAST32_MIN


#ifndef INT_LEAST8_MAX
    "INT_LEAST8_MAX",
#else
    "",
#endif   // INT_LEAST8_MAX

#ifndef UINT_LEAST8_MAX
    "UINT_LEAST8_MAX",
#else
    "",
#endif   // UINT_LEAST8_MAX

#ifndef INT_FAST8_MAX
    "INT_FAST8_MAX",
#else
    "",
#endif   // INT_FAST8_MAX

#ifndef UINT_FAST8_MAX
    "UINT_FAST8_MAX",
#else
    "",
#endif   // UINT_FAST8_MAX


#ifndef INT_LEAST16_MAX
    "INT_LEAST16_MAX",
#else
    "",
#endif   // INT_LEAST16_MAX

#ifndef UINT_LEAST16_MAX
    "UINT_LEAST16_MAX",
#else
    "",
#endif   // UINT_LEAST16_MAX

#ifndef INT_FAST16_MAX
    "INT_FAST16_MAX",
#else
    "",
#endif   // INT_FAST16_MAX

#ifndef UINT_FAST16_MAX
    "UINT_FAST16_MAX",
#else
    "",
#endif   // UINT_FAST16_MAX


#ifndef INT_LEAST32_MAX
    "INT_LEAST32_MAX",
#else
    "",
#endif   // INT_LEAST32_MAX

#ifndef UINT_LEAST32_MAX
    "UINT_LEAST32_MAX",
#else
    "",
#endif   // UINT_LEAST32_MAX

#ifndef INT_FAST32_MAX
    "INT_FAST32_MAX",
#else
    "",
#endif   // INT_FAST32_MAX

#ifndef UINT_FAST32_MAX
    "UINT_FAST32_MAX",
#else
    "",
#endif   // UINT_FAST32_MAX

#ifndef INTPTR_MAX
    "INTPTR_MAX",
#else
    "",
#endif   // INTPTR_MAX

#ifndef UINTPTR_MAX
    "UINTPTR_MAX",
#else
    "",
#endif   // UINTPTR_MAX

#ifndef PTRDIFF_MIN
    "PTRDIFF_MIN",
#else
    "",
#endif   // PTRDIFF_MIN

#ifndef PTRDIFF_MAX
    "PTRDIFF_MAX",
#else
    "",
#endif   // PTRDIFF_MAX

#ifndef SIZE_MAX
    "SIZE_MAX",
#else
    "",
#endif   // SIZE_MAX

#ifndef WCHAR_MIN
    "WCHAR_MIN",
#else
    "",
#endif   // WCHAR_MIN

#ifndef WCHAR_MAX
    "WCHAR_MAX",
#else
    "",
#endif   // WCHAR_MAX

#ifndef WINT_MIN
    "WINT_MIN",
#else
    "",
#endif   // WINT_MIN

#ifndef WINT_MAX
    "WINT_MAX",
#else
    "",
#endif   // WINT_MAX

#ifndef SIG_ATOMIC_MIN
    "SIG_ATOMIC_MIN",
#else
    "",
#endif   // SIG_ATOMIC_MIN

#ifndef SIG_ATOMIC_MAX
    "SIG_ATOMIC_MAX",
#else
    "",
#endif   // SIG_ATOMIC_MAX

    0
};


static const char* const
missing_optional_macros[] = {

#ifndef UINT8_MAX
    "UINT8_MAX",
#else
    "",
#endif   // UINT8_MAX

#ifndef UINT16_MAX
    "UINT16_MAX",
#else
    "",
#endif   // UINT16_MAX

#ifndef UINT32_MAX
    "UINT32_MAX",
#else
    "",
#endif   // UINT32_MAX

    0
};


static const char* const
missing_64_bit_macros[] = {

#ifndef INT_LEAST64_MIN
    "INT_LEAST64_MIN",
#else
    "",
#endif   // INT_LEAST64_MIN

#ifndef INT_FAST64_MIN
    "INT_FAST64_MIN",
#else
    "",
#endif   // INT_FAST64_MIN

#ifndef UINT64_MAX
    "UINT64_MAX",
#else
    "",
#endif   // UINT64_MAX

#ifndef INT_LEAST64_MAX
    "INT_LEAST64_MAX",
#else
    "",
#endif   // INT_LEAST64_MAX

#ifndef UINT_LEAST64_MAX
    "UINT_LEAST64_MAX",
#else
    "",
#endif   // UINT_LEAST64_MAX

#ifndef INT_FAST64_MAX
    "INT_FAST64_MAX",
#else
    "",
#endif   // INT_FAST64_MAX

#ifndef UINT_FAST64_MAX
    "UINT_FAST64_MAX",
#else
    "",
#endif   // UINT_FAST64_MAX

    0
};

/**************************************************************************/

#ifndef _RWSTD_NO_NAMESPACE

// check types

#define DEFINE_TYPE(name)                                       \
    struct name {                                               \
    int i_;                                                     \
    char dummy_ [256];                                          \
                                                                \
    name (int i = 0): i_ (i) { }                                \
    operator int () const volatile { return i_; }               \
    name operator- (int i) const volatile { return i_ - i; }    \
    name operator~ () const volatile { return ~0; }             \
    name operator& (int i) const volatile { return i_ & i; }    \
    bool operator< (int i) const volatile { return i_ < i; }    \
}


namespace Fallback {

DEFINE_TYPE (int8_t);
DEFINE_TYPE (uint8_t);
DEFINE_TYPE (int_least8_t);
DEFINE_TYPE (uint_least8_t);
DEFINE_TYPE (int_fast8_t);
DEFINE_TYPE (uint_fast8_t);

DEFINE_TYPE (int16_t);
DEFINE_TYPE (uint16_t);
DEFINE_TYPE (int_least16_t);
DEFINE_TYPE (uint_least16_t);
DEFINE_TYPE (int_fast16_t);
DEFINE_TYPE (uint_fast16_t);

DEFINE_TYPE (int32_t);
DEFINE_TYPE (uint32_t);
DEFINE_TYPE (int_least32_t);
DEFINE_TYPE (uint_least32_t);
DEFINE_TYPE (int_fast32_t);
DEFINE_TYPE (uint_fast32_t);

DEFINE_TYPE (int64_t);
DEFINE_TYPE (uint64_t);
DEFINE_TYPE (int_least64_t);
DEFINE_TYPE (uint_least64_t);
DEFINE_TYPE (int_fast64_t);
DEFINE_TYPE (uint_fast64_t);

DEFINE_TYPE (intmax_t);
DEFINE_TYPE (uintmax_t);

DEFINE_TYPE (intptr_t);
DEFINE_TYPE (uintptr_t);

}    // Namespace Fallback

namespace std {

// define test functions in namespace std to detect the presece
// or absence of the required types

namespace tr1 {

namespace Nested {

using namespace Fallback;

// each test_xxx_t typedef aliases std::tr1::xxx_t if the corresponding
// type is defined in namespace std::tr1, or Fallback::xxx_t otherwise

typedef int8_t         test_int8_t;
typedef uint8_t        test_uint8_t;
typedef int_least8_t   test_int_least8_t;
typedef uint_least8_t  test_uint_least8_t;
typedef int_fast8_t    test_int_fast8_t;
typedef uint_fast8_t   test_uint_fast8_t;

typedef int16_t        test_int16_t;
typedef uint16_t       test_uint16_t;
typedef int_least16_t  test_int_least16_t;
typedef uint_least16_t test_uint_least16_t;
typedef int_fast16_t   test_int_fast16_t;
typedef uint_fast16_t  test_uint_fast16_t;

typedef int32_t        test_int32_t;
typedef uint32_t       test_uint32_t;
typedef int_least32_t  test_int_least32_t;
typedef uint_least32_t test_uint_least32_t;
typedef int_fast32_t   test_int_fast32_t;
typedef uint_fast32_t  test_uint_fast32_t;

typedef int64_t        test_int64_t;
typedef uint64_t       test_uint64_t;
typedef int_least64_t  test_int_least64_t;
typedef uint_least64_t test_uint_least64_t;
typedef int_fast64_t   test_int_fast64_t;
typedef uint_fast64_t  test_uint_fast64_t;

typedef intmax_t       test_intmax_t;
typedef uintmax_t      test_uintmax_t;
typedef intptr_t       test_intptr_t;
typedef uintptr_t      test_uintptr_t;

}   // namespace Nested

}   // namespace tr1

}   // namespace std

typedef std::tr1::Nested::test_int8_t         test_int8_t;
typedef std::tr1::Nested::test_uint8_t        test_uint8_t;
typedef std::tr1::Nested::test_int_least8_t   test_int_least8_t;
typedef std::tr1::Nested::test_uint_least8_t  test_uint_least8_t;
typedef std::tr1::Nested::test_int_fast8_t    test_int_fast8_t;
typedef std::tr1::Nested::test_uint_fast8_t   test_uint_fast8_t;

typedef std::tr1::Nested::test_int16_t        test_int16_t;
typedef std::tr1::Nested::test_uint16_t       test_uint16_t;
typedef std::tr1::Nested::test_int_least16_t  test_int_least16_t;
typedef std::tr1::Nested::test_uint_least16_t test_uint_least16_t;
typedef std::tr1::Nested::test_int_fast16_t   test_int_fast16_t;
typedef std::tr1::Nested::test_uint_fast16_t  test_uint_fast16_t;

typedef std::tr1::Nested::test_int32_t        test_int32_t;
typedef std::tr1::Nested::test_uint32_t       test_uint32_t;
typedef std::tr1::Nested::test_int_least32_t  test_int_least32_t;
typedef std::tr1::Nested::test_uint_least32_t test_uint_least32_t;
typedef std::tr1::Nested::test_int_fast32_t   test_int_fast32_t;
typedef std::tr1::Nested::test_uint_fast32_t  test_uint_fast32_t;

typedef std::tr1::Nested::test_int64_t        test_int64_t;
typedef std::tr1::Nested::test_uint64_t       test_uint64_t;
typedef std::tr1::Nested::test_int_least64_t  test_int_least64_t;
typedef std::tr1::Nested::test_uint_least64_t test_uint_least64_t;
typedef std::tr1::Nested::test_int_fast64_t   test_int_fast64_t;
typedef std::tr1::Nested::test_uint_fast64_t  test_uint_fast64_t;

typedef std::tr1::Nested::test_intmax_t       test_intmax_t;
typedef std::tr1::Nested::test_uintmax_t      test_uintmax_t;
typedef std::tr1::Nested::test_intptr_t       test_intptr_t;
typedef std::tr1::Nested::test_uintptr_t      test_uintptr_t;

/**************************************************************************/

template <class intT>
void test_bits (int         line,             // line number
                intT        testval,          // tested value (for macros)
                int         expect,           // expected number of bits
                bool        exact,            // exactly (or at least)
                const char *type_name,        // name of type
                const char *macro_name = 0,   // name of macro
                const char *valstr     = 0)   // value (for message)
{
    static volatile intT zero         = intT (0);
    static volatile intT one          = intT (1);
    static volatile intT minus_one    = zero - one;
    static volatile intT all_bits_set = ~zero;

    const bool is_signed = minus_one < zero;

    const unsigned maxbits = sizeof (intT) * _RWSTD_CHAR_BIT;

    int nbits = 0;

    intT maximum = one;
    intT minimum = is_signed ? minus_one : one;

    // count the number of bits
    for (unsigned i = 0; i != maxbits; ++i) {

        if (all_bits_set & (intT (1) << i))
            ++nbits;
        else
            break;

        maximum = maximum * intT (2);
        minimum = maximum * intT (2);
    }

    if (macro_name) {
        // verify macro value

        if (is_signed) {
            if (zero < testval) {
                rw_assert (maximum <= testval, 0, line,
                           "%s >= %s", macro_name, valstr);
            }
            else {
                rw_assert (testval <= minimum, 0, line,
                           "%s <= %s", macro_name, valstr);
            }
        }
        else {
            rw_assert (maximum <= testval, 0, line,
                       "%s >= %s", macro_name, valstr);
        }
    }
    else {
        if (expect < 0) {
            expect = -expect;

            // verify that the type is signed
            rw_assert (minus_one < zero, 0, line,
                       "%s must be a signed type", type_name);
        }
        else {
            // verify that the type is unsigned
            rw_assert (zero < minus_one, 0, line,
                       "%s must be an unsigned type", type_name);
        }

        // verify that the type is exactly/at least as wide as expected
        const bool success = exact ? expect == nbits : expect <= nbits;

        if (64 != expect || !rw_opt_no_64_bit)
            rw_assert (success, 0, 0,
                       "%s expected width %{?}exactly%{:}at least%{;} "
                       "%d bits, got %d", type_name, exact, expect, nbits);
    }
}

/**************************************************************************/

static void
test_types ()
{
#define TEST_REQUIRED_TYPE(name, bits, exact)                           \
    do {                                                                \
        rw_info (0, 0, __LINE__, "std::tr1::%s (required)", # name);    \
        const bool type_defined =                                       \
            sizeof (test_ ## name) != sizeof (Fallback::name);          \
        rw_assert (type_defined, 0, __LINE__,                           \
                   "required type %s not defined in namespace "         \
                   "std::tr1", #name);                                  \
        if (type_defined)                                               \
            test_bits (__LINE__, test_ ## name (0),                     \
                       bits, exact, #name);                             \
    } while (0)

#define TEST_OPTIONAL_TYPE(name, bits, exact)                           \
    do {                                                                \
        rw_info (0, 0, __LINE__, "std::tr1::%s (optional)", # name);    \
        const bool type_defined =                                       \
            sizeof (test_ ## name) != sizeof (Fallback::name);          \
        rw_warn (type_defined, 0, __LINE__,                             \
                 "optional type %s not defined in namespace "           \
                 "std::tr1", #name);                                    \
        if (type_defined)                                               \
            test_bits (__LINE__, test_ ## name (0),                     \
                       bits, exact, #name);                             \
    } while (0)

    TEST_OPTIONAL_TYPE (int8_t,          -8, true);
    TEST_OPTIONAL_TYPE (uint8_t,          8, true);
    TEST_OPTIONAL_TYPE (int16_t,        -16, true);
    TEST_OPTIONAL_TYPE (uint16_t,        16, true);
    TEST_OPTIONAL_TYPE (int32_t,        -32, true);
    TEST_OPTIONAL_TYPE (uint32_t,        32, true);

    TEST_REQUIRED_TYPE (int_least8_t,    -8, false);
    TEST_REQUIRED_TYPE (uint_least8_t,    8, false);
    TEST_REQUIRED_TYPE (int_fast8_t,     -8, false);
    TEST_REQUIRED_TYPE (uint_fast8_t,     8, false);

    TEST_REQUIRED_TYPE (int_least16_t,  -16, false);
    TEST_REQUIRED_TYPE (uint_least16_t,  16, false);
    TEST_REQUIRED_TYPE (int_fast16_t,   -16, false);
    TEST_REQUIRED_TYPE (uint_fast16_t,   16, false);

    TEST_REQUIRED_TYPE (int_least32_t,  -32, false);
    TEST_REQUIRED_TYPE (uint_least32_t,  32, false);
    TEST_REQUIRED_TYPE (int_fast32_t,   -32, false);
    TEST_REQUIRED_TYPE (uint_fast32_t,   32, false);

    TEST_REQUIRED_TYPE (intmax_t,       -64, false);
    TEST_REQUIRED_TYPE (uintmax_t,       64, false);
    TEST_REQUIRED_TYPE (intptr_t,       -16, false);
    TEST_REQUIRED_TYPE (uintptr_t,       16, false);

    if (rw_opt_no_64_bit) {
        rw_note (0, 0, 0, "tests of 64-bit types disabled");
    }
    else {
        TEST_OPTIONAL_TYPE (int64_t,        -64, true);
        TEST_OPTIONAL_TYPE (uint64_t,        64, true);

        TEST_REQUIRED_TYPE (int_least64_t,  -64, false);
        TEST_REQUIRED_TYPE (uint_least64_t,  64, false);
        TEST_REQUIRED_TYPE (int_fast64_t,   -64, false);
        TEST_REQUIRED_TYPE (uint_fast64_t,   64, false);
    }
}

#else   // if defined (_RWSTD_NO_NAMESPACE)

static void
test_types ()
{
    rw_note (0, 0, 0, "_RWSTD_NO_NAMESPACE defined, cannot test");
}

#endif   // _RWSTD_NO_NAMESPACE

/**************************************************************************/

static void
test_macros ()
{
    for (unsigned i = 0; missing_macros [i]; ++i) {
        rw_assert ('\0' == missing_macros [i][0], 0, __LINE__,
                   "required macro %s not defined",
                   missing_macros [i]);
    }

#define TEST_MACRO(T, macro, bits, exact, valstr) \
    test_bits (__LINE__, test_ ## T (macro), bits, exact, #T, #macro, valstr)

    TEST_MACRO (int_least8_t,   INT_LEAST8_MIN,    -8, true, "-127");
    TEST_MACRO (int_least8_t,   INT_LEAST8_MAX,    -8, true, "+127");
    TEST_MACRO (uint_least8_t,  UINT_LEAST8_MAX,    8, true, "256");

    TEST_MACRO (int_fast8_t,    INT_FAST8_MIN,    -8, true, "-127");
    TEST_MACRO (int_fast8_t,    INT_FAST8_MAX,    -8, true, "+127");
    TEST_MACRO (uint_fast8_t,   UINT_FAST8_MAX,    8, true, "256");

    TEST_MACRO (int_least16_t,  INT_LEAST16_MIN,  -16, true, "-32767");
    TEST_MACRO (int_least16_t,  INT_LEAST16_MAX,  -16, true, "+32767");
    TEST_MACRO (uint_least16_t, UINT_LEAST16_MAX,  16, true, "65535");

    TEST_MACRO (int_fast16_t,   INT_FAST16_MIN,  -16, true, "-32767");
    TEST_MACRO (int_fast16_t,   INT_FAST16_MAX,  -16, true, "+32767");
    TEST_MACRO (uint_fast16_t,  UINT_FAST16_MAX,  16, true, "65535");

    TEST_MACRO (int_least32_t,  INT_LEAST32_MIN,  -32, true, "-2147483647");
    TEST_MACRO (int_least32_t,  INT_LEAST32_MAX,  -32, true, "+2147483647");
    TEST_MACRO (uint_least32_t, UINT_LEAST32_MAX,  32, true, "4294967295");

    TEST_MACRO (int_fast32_t,   INT_FAST32_MIN,  -32, true, "-2147483647");
    TEST_MACRO (int_fast32_t,   INT_FAST32_MAX,  -32, true, "+2147483647");
    TEST_MACRO (uint_fast32_t,  UINT_FAST32_MAX,  32, true, "4294967295");

#define TEST_MACRO_NO_TYPE(macro, bits, exact, valstr) \
    test_bits (__LINE__, macro, bits, exact, "", #macro, valstr)

    TEST_MACRO_NO_TYPE (PTRDIFF_MIN, 16, false, "-65535");
    TEST_MACRO_NO_TYPE (PTRDIFF_MAX, 16, false, "+65535");

    TEST_MACRO_NO_TYPE (SIZE_MAX, 16, false, "+65535");

    TEST_MACRO_NO_TYPE (SIG_ATOMIC_MIN, 8, false, "-127");
    TEST_MACRO_NO_TYPE (SIG_ATOMIC_MAX, 8, false, "+127");

    TEST_MACRO_NO_TYPE (WCHAR_MIN, 8, false, "-127");
    TEST_MACRO_NO_TYPE (WCHAR_MAX, 8, false, "+127");

    TEST_MACRO_NO_TYPE (WINT_MIN, 16, false, "-32767");
    TEST_MACRO_NO_TYPE (WINT_MAX, 16, false, "+32768");


    for (unsigned i = 0; missing_optional_macros [i]; ++i) {
        rw_warn ('\0' == missing_optional_macros [i][0], 0, __LINE__,
                 "optional macro %s not defined",
                 missing_optional_macros [i]);
    }

    TEST_MACRO (int8_t,   INT8_MIN,    -8, true, "-128");
    TEST_MACRO (int8_t,   INT8_MAX,    -8, true, "127");
    TEST_MACRO (uint8_t,  UINT8_MAX,    8, true, "256");

    TEST_MACRO (int16_t,  INT16_MIN,  -16, true, "-32768");
    TEST_MACRO (int16_t,  INT16_MAX,  -16, true, "32767");
    TEST_MACRO (uint16_t, UINT16_MAX,  16, true, "65535");

    TEST_MACRO (int32_t,  INT32_MIN,  -32, true, "-2147483648");
    TEST_MACRO (int32_t,  INT32_MAX,  -32, true, "2147483647");
    TEST_MACRO (uint32_t, UINT32_MAX,  32, true, "4294967295");

    if (rw_opt_no_64_bit) {
        rw_note (0, 0, 0, "tests of 64-bit macros disabled");
    }
    else {
        for (unsigned i = 0; missing_64_bit_macros [i]; ++i) {
            rw_assert ('\0' == missing_64_bit_macros [i][0], 0, __LINE__,
                       "required macro %s not defined",
                       missing_64_bit_macros [i]);
        }

        TEST_MACRO (int64_t,        INT64_MIN,        -64, true, "?");
        TEST_MACRO (int64_t,        INT64_MAX,        -64, true, "?");
        TEST_MACRO (uint64_t,       UINT64_MAX,        64, true, "?");

        TEST_MACRO (int_least64_t,  INT_LEAST64_MIN,  -64, true, "?");
        TEST_MACRO (int_least64_t,  INT_LEAST64_MAX,  -64, true, "?");
        TEST_MACRO (uint_least64_t, UINT_LEAST64_MAX,  64, true, "?");

        TEST_MACRO (int_fast64_t,   INT_FAST64_MIN,   -64, true, "?");
        TEST_MACRO (int_fast64_t,   INT_FAST64_MAX,   -64, true, "?");
        TEST_MACRO (uint_fast64_t,  UINT_FAST64_MAX,   64, true, "?");
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_types ();

    test_macros ();

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "tr.c99.cstdint",
                    0 /* no comment */,
                    run_test,
                    "|-no-macros# "
                    "|-no-types# "
                    "|-no-64-bit#",
                    &rw_opt_no_macros,
                    &rw_opt_no_types,
                    &rw_opt_no_64_bit);
}
#else // !TEST_RW_EXTENSIONS
int main()
{
    return 0;
}
#endif // !TEST_RW_EXTENSIONS
