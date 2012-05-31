/***************************************************************************
 *
 * 21.string.substr.cpp - string test exercising [lib.string::substr]
 *
 * $Id: 21.string.substr.cpp 590052 2007-10-30 12:44:14Z faridz $
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
#include <rw_allocator.h>   // for UserAlloc

#include <string>           // for string
#include <cstddef>          // for size_t
#include <stdexcept>        // for out_of_range

#include <21.strings.h>     // for StringIds
#include <driver.h>         // for rw_test()
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define Substr(sig)               StringIds::substr_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// substr ()
static const StringTestCase
void_test_cases [] = {

#undef TEST
#define TEST(str, res)                                                   \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, 0, 0,           \
      res, sizeof res - 1, 0 }

    //    +------------------------------------- controlled sequence
    //    |                   +----------------- expected result sequence
    //    |                   |
    //    V                   V
    TEST ("abc",              "abc"),

    TEST ("",                 ""),
    TEST ("<U0>",             "<U0>"),
    TEST ("a<U0>",            "a<U0>"),
    TEST ("<U0>a",            "<U0>a"),

    TEST ("a<U0>@2bcd<U0>",   "a<U0>@2bcd<U0>"),
    TEST ("<U0>@2abcd<U0>",   "<U0>@2abcd<U0>"),
    TEST ("<U0>@2ab<U0>@2",   "<U0>@2ab<U0>@2"),

    TEST ("x@4096",           "x@4096"),

    TEST ("last",             "last")
};


/**************************************************************************/

// used to exercise
// substr (size_type)
static const StringTestCase
size_test_cases [] = {

#undef TEST
#define TEST(str, off, res, bthrow)                                         \
    { __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1, 0, 0,             \
      res, sizeof res - 1, bthrow }

    //    +--------------------------------------- controlled sequence
    //    |                     +----------------- substr() off argument
    //    |                     |   +------------- expected result sequence
    //    |                     |   |                 +--- exception info:
    //    |                     |   |                 |      0 - no exception
    //    |                     |   |                 |      1 - out_of_range
    //    V                     V   V                 V
    TEST ("abc",                0,  "abc",            0),

    TEST ("",                   0,  "",               0),
    TEST ("<U0>",               0,  "<U0>",           0),
    TEST ("<U0>",               1,  "",               0),
    TEST ("a<U0>",              0,  "a<U0>",          0),
    TEST ("a<U0>",              1,  "<U0>",           0),
    TEST ("<U0>a",              0,  "<U0>a",          0),
    TEST ("<U0>a",              1,  "a",              0),

    TEST ("a<U0>@2bcd<U0>",     0,  "a<U0>@2bcd<U0>", 0),
    TEST ("a<U0>@2bcd<U0>",     1,  "<U0>@2bcd<U0>",  0),
    TEST ("a<U0>@2bcd<U0>",     6,  "<U0>",           0),
    TEST ("a<U0>@2bcd<U0>",     7,  "",               0),
    TEST ("<U0>@2abcd<U0>",     0,  "<U0>@2abcd<U0>", 0),
    TEST ("<U0>@2abcd<U0>",     1,  "<U0>abcd<U0>",   0),
    TEST ("<U0>@2abcd<U0>",     2,  "abcd<U0>",       0),
    TEST ("<U0>@2ab<U0>@2",     0,  "<U0>@2ab<U0>@2", 0),
    TEST ("<U0>@2ab<U0>@2",     1,  "<U0>ab<U0>@2",   0),
    TEST ("<U0>@2ab<U0>@2",     4,  "<U0>@2",         0),
    TEST ("<U0>@2ab<U0>@2",     6,  "",               0),

    TEST ("x@4096",             0,  "x@4096",         0),
    TEST ("x@4096",          4094,  "xx",             0),
    TEST ("x@4096",          4088,  "xxxxxxxx",       0),
    TEST ("x@4096",          4096,  "",               0),
    TEST ("abx@4096",           2,  "x@4096",         0),
    TEST ("x@4096ab",        2048,  "x@2048ab",       0),

    TEST ("<U0>",               2,  "",               1),
    TEST ("a",                 10,  "",               1),
    TEST ("x@4096",          4106,  "",               1),

    TEST ("last",               0,  "last",           0)
};

/**************************************************************************/

// used to exercise
// substr (size_type, size_type)
static const StringTestCase
size_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, res, bthrow)                                    \
    { __LINE__, off, size, -1, -1, -1, str, sizeof str - 1, 0, 0,            \
      res, sizeof res - 1, bthrow }

    //    +------------------------------------------ controlled sequence
    //    |                     +-------------------- substr() off argument
    //    |                     |   +---------------- substr() n argument
    //    |                     |   |  +------------- expected result sequence
    //    |                     |   |  |         +--- exception info:
    //    |                     |   |  |         |      0 - no exception
    //    |                     |   |  |         |      1 - out_of_range
    //    |                     |   |  |         |
    //    |                     |   |  |         +-------+
    //    V                     V   V  V                 V
    TEST ("abc",                0,  3, "abc",            0),

    TEST ("",                   0,  0, "",               0),
    TEST ("<U0>",               0,  1, "<U0>",           0),
    TEST ("<U0>",               0,  0, "",               0),
    TEST ("<U0>",               1,  1, "",               0),
    TEST ("a<U0>",              0,  2, "a<U0>",          0),
    TEST ("a<U0>",              1,  2, "<U0>",           0),
    TEST ("a<U0>",              0,  1, "a",              0),
    TEST ("a<U0>",              1,  1, "<U0>",           0),
    TEST ("a<U0>",              1,  0, "",               0),
    TEST ("<U0>a",              0,  2, "<U0>a",          0),
    TEST ("<U0>a",              0,  1, "<U0>",           0),
    TEST ("<U0>a",              1,  2, "a",              0),

    TEST ("a<U0>@2bcd<U0>",     0,  7, "a<U0>@2bcd<U0>", 0),
    TEST ("a<U0>@2bcd<U0>",     1,  7, "<U0>@2bcd<U0>",  0),
    TEST ("a<U0>@2bcd<U0>",     0,  3, "a<U0>@2",        0),
    TEST ("a<U0>@2bcd<U0>",     1,  2, "<U0>@2",         0),
    TEST ("a<U0>@2bcd<U0>",     6,  7, "<U0>",           0),
    TEST ("a<U0>@2bcd<U0>",     7,  7, "",               0),
    TEST ("<U0>@2abcd<U0>",     0,  7, "<U0>@2abcd<U0>", 0),
    TEST ("<U0>@2abcd<U0>",     1,  7, "<U0>abcd<U0>",   0),
    TEST ("<U0>@2abcd<U0>",     0,  3, "<U0>@2a",        0),
    TEST ("<U0>@2abcd<U0>",     1,  1, "<U0>",           0),
    TEST ("<U0>@2abcd<U0>",     6,  3, "<U0>",           0),
    TEST ("<U0>@2abcd<U0>",     1,  6, "<U0>abcd<U0>",   0),
    TEST ("<U0>@2abcd<U0>",     0,  0, "",               0),
    TEST ("<U0>@2abcd<U0>",     2,  7, "abcd<U0>",       0),
    TEST ("<U0>@2ab<U0>@2",     0,  6, "<U0>@2ab<U0>@2", 0),
    TEST ("<U0>@2ab<U0>@2",     1,  6, "<U0>ab<U0>@2",   0),
    TEST ("<U0>@2ab<U0>@2",     0,  2, "<U0>@2",         0),
    TEST ("<U0>@2ab<U0>@2",     4,  6, "<U0>@2",         0),
    TEST ("<U0>@2ab<U0>@2",     2,  3, "ab<U0>",         0),
    TEST ("<U0>@2ab<U0>@2",     6,  6, "",               0),

    TEST ("x@4096",           0, 4096, "x@4096",         0),
    TEST ("x@4096",        4095, 4096, "x",              0),
    TEST ("x@4096",          4088,  2, "xx",             0),
    TEST ("x@4096",          4093,  5, "xxx",            0),
    TEST ("x@4096",          4088,  8, "xxxxxxxx",       0),
    TEST ("x@4096",          4096,  1, "",               0),
    TEST ("ax@4096b",         1, 4094, "x@4094",         0),

    TEST ("<U0>",               2,  0, "",               1),
    TEST ("a",                 10,  0, "",               1),
    TEST ("x@4096",          4106,  0, "",               1),

    TEST ("last",               0,  4, "last",           0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_substr (charT, Traits*, Allocator*,
                  const StringFunc     &func,
                  const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    // construct the string object
    const String str (wstr, str_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    wstr = 0;

    String s_res;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected?
    const char* const expected = tcase.bthrow ? exceptions [1] : 0;
    const char* caught = 0;

#else

    if (tcase.bthrow) {
        if (wres != wres_buf)
            delete[] wres;

        return;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        switch (func.which_) {

        case Substr (void):
            s_res = str.substr ();
            break;

        case Substr (size):
            s_res = str.substr (tcase.off);
            break;

        case Substr (size_size):
            s_res = str.substr (tcase.off, tcase.size);
            break;

        default:
            RW_ASSERT (!"logic error: unknown substr overload");
            return;
        }

        // verfiy that strings length are equal
        rw_assert (res_len == s_res.size (), 0, tcase.line,
                   "line %d. %{$FUNCALL} expected %{#*s} with length "
                   "%zu, got %{/*.*Gs} with length %zu",
                   __LINE__, int (tcase.nres), tcase.res,
                   res_len, int (sizeof (charT)),
                   int (s_res.size ()), s_res.c_str (), s_res.size ());

        if (res_len == s_res.size ()) {
            // if the result length matches the expected length
            // (and only then), also verify that the resulted
            // string matches the expected result
            const std::size_t match =
                rw_match (tcase.res, s_res.c_str(), s_res.size ());

            rw_assert (match == res_len, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s}, "
                       "got %{/*.*Gs}, difference at offset %zu",
                       __LINE__, int (tcase.nres), tcase.res,
                       int (sizeof (charT)), int (s_res.size ()),
                       s_res.c_str (), match);
        }
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::out_of_range &ex) {
        caught = exceptions [1];
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught std::%s(%#s)",
                   __LINE__, 0 != expected, expected, caught, ex.what ());
    }
    catch (const std::exception &ex) {
        caught = exceptions [4];
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught std::%s(%#s)",
                   __LINE__, 0 != expected, expected, caught, ex.what ());
    }
    catch (...) {
        caught = exceptions [0];
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught %s",
                   __LINE__, 0 != expected, expected, caught);
    }

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (str),
                                __LINE__, tcase.line, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    if (wres != wres_buf)
        delete[] wres;
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_substr);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Substr (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (void),
        TEST (size),
        TEST (size_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.substr",
                               test_substr, tests, test_count);
}
