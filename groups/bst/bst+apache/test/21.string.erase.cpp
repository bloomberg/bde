/***************************************************************************
 *
 * 21.string.erase.cpp - string test exercising [lib.string::erase]
 *
 * $Id: 21.string.erase.cpp 590052 2007-10-30 12:44:14Z faridz $
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

#include <string>           // for string
#include <cstddef>          // for ptrdiff_t, size_t
#include <stdexcept>        // for out_of_range

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Erase(sig)                StringIds::erase_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// erase ()
static const StringTestCase
void_test_cases [] = {

#undef TEST
#define TEST(str, res) {                                                \
        __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1,              \
        0, 0, res, sizeof res - 1, 0 }

    //    +--------------------------------- controlled sequence
    //    |                 +--------------- expected result sequence
    //    |                 |
    //    V                 V
    TEST ("a",              ""),

    TEST ("",               ""),
    TEST ("<U0>",           ""),
    TEST ("abc",            ""),
    TEST ("t<U0> s",        ""),
    TEST ("<U0>ab<U0>@2c",  ""),
    TEST ("a<U0>b<U0>@2c",  ""),
    TEST ("a<U0>bc<U0>@2",  ""),

    TEST ("x@4096",         ""),
    TEST ("last",           "")
};


/**************************************************************************/

// used to exercise
// erase (size_type)
static const StringTestCase
size_test_cases [] = {

#undef TEST
#define TEST(str, off, res, bthrow) {                                    \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,              \
        0, 0, res, sizeof res - 1, bthrow }

    //    +-------------------------------------- controlled sequence
    //    |                   +------------------ erase() pos argument
    //    |                   |    +------------- expected result sequence
    //    |                   |    |                +--- exception info
    //    |                   |    |                |      0 - no exception
    //    |                   |    |                |      1 - out_of_range
    //    |                   |    |                |
    //    V                   V    V                V
    TEST ("",                 0,   "",              0),
    TEST ("<U0>",             0,   "",              0),
    TEST ("<U0>",             1,   "<U0>",          0),
    TEST ("<U0>@2",           1,   "<U0>",          0),

    TEST ("a",                0,   "",              0),
    TEST ("a",                1,   "a",             0),

    TEST ("abc",              0,   "",              0),
    TEST ("abc",              1,   "a",             0),
    TEST ("abc",              2,   "ab",            0),
    TEST ("abc",              3,   "abc",           0),

    TEST ("t<U0> s",          0,   "",              0),
    TEST ("t<U0> s",          1,   "t",             0),
    TEST ("t<U0> s",          2,   "t<U0>",         0),
    TEST ("t<U0> s",          3,   "t<U0> ",        0),
    TEST ("t<U0> s",          4,   "t<U0> s",       0),

    TEST ("a<U0>@3b",         2,   "a<U0>",         0),
    TEST ("a<U0>@3b",         1,   "a",             0),
    TEST ("a<U0>@3b",         0,   "",              0),
    TEST ("a<U0>b<U0>@2c",    0,   "",              0),
    TEST ("a<U0>b<U0>@2c",    4,   "a<U0>b<U0>",    0),
    TEST ("<U0>ab<U0>@2c",    2,   "<U0>a",         0),
    TEST ("ab<U0>c<U0>@2",    5,   "ab<U0>c<U0>",   0),

    TEST ("a",                3,   "a",             1),
    TEST ("t<U0> s",          5,   "t<U0> s",       1),
    TEST ("ab<U0>c<U0>@2",   10,   "ab<U0>c<U0>@2", 1),
    TEST ("x@4096",        4106,   "x@4096",        1),

    TEST ("x@4096",           0,   "",              0),
    TEST ("x@4096",           1,   "x",             0),
    TEST ("x@4096",           4,   "xxxx",          0),
    TEST ("x@4096",        4096,   "x@4096",        0),
    TEST ("x@2048y@2048",  2048,   "x@2048",        0),

    TEST ("last test",        4,   "last",          0)
};


/**************************************************************************/

// used to exercise
// erase (size_type, size_type)
// erase (iterator, iterator)
static const StringTestCase
size_size_test_cases [] = {

// size_size_test_cases serves a double duty
#define iter_iter_test_cases size_size_test_cases

#undef TEST
#define TEST(str, off, size, res, bthrow) {                                \
        __LINE__, off, size, -1, -1, -1, str, sizeof str - 1,              \
        0, 0, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |                   +--------------------- erase() pos argument
    //    |                   |         +----------- erase() n argument
    //    |                   |         |   +------- expected result sequence
    //    |                   |         |   |        +--- exception info
    //    |                   |         |   |        |      0 - no exception
    //    |                   |         |   |        |      1 - out_of_range
    //    |                   |         |   |        |
    //    |                   |         |   |        +-------+
    //    V                   V         V   V                V
    TEST ("",                 0,        0,  "",              0),
    TEST ("<U0>",             0,        1,  "",              0),
    TEST ("<U0>",             0,        0,  "<U0>",          0),
    TEST ("<U0>",             1,        1,  "<U0>",          0),
    TEST ("<U0>@2",           1,        1,  "<U0>",          0),
    TEST ("<U0>@2",           0,        1,  "<U0>",          0),

    TEST ("a",                0,        1,  "",              0),
    TEST ("a",                0,        0,  "a",             0),
    TEST ("a",                1,        1,  "a",             0),

    TEST ("abc",              0,        3,  "",              0),
    TEST ("abc",              0,        2,  "c",             0),
    TEST ("abc",              1,        2,  "a",             0),
    TEST ("abc",              1,        1,  "ac",            0),
    TEST ("abc",              2,        1,  "ab",            0),
    TEST ("abc",              3,        0,  "abc",           0),

    TEST ("t<U0> s",          0,        3,  "s",             0),
    TEST ("t<U0> s",          0,        4,  "",              0),
    TEST ("t<U0> s",          0,        1,  "<U0> s",        0),
    TEST ("t<U0> s",          1,        3,  "t",             0),
    TEST ("t<U0> s",          1,        2,  "ts",            0),
    TEST ("t<U0> s",          2,        2,  "t<U0>",         0),
    TEST ("t<U0> s",          2,        1,  "t<U0>s",        0),
    TEST ("t<U0> s",          3,        2,  "t<U0> ",        0),
    TEST ("t<U0> s",          4,        0,  "t<U0> s",       0),

    TEST ("a<U0>@3b",         2,        0,  "a<U0>@3b",      0),
    TEST ("a<U0>@3b",         2,        3,  "a<U0>",         0),
    TEST ("a<U0>@3b",         2,        2,  "a<U0>b",        0),
    TEST ("a<U0>@3b",         1,        4,  "a",             0),
    TEST ("a<U0>@3b",         0,        5,  "",              0),
    TEST ("a<U0>@3b",         0,        2,  "<U0>@2b",       0),
    TEST ("a<U0>b<U0>@2c",    0,        6,  "",              0),
    TEST ("a<U0>b<U0>@2c",    4,        2,  "a<U0>b<U0>",    0),
    TEST ("a<U0>b<U0>@2c",    4,        1,  "a<U0>b<U0>c",   0),
    TEST ("<U0>ab<U0>@2c",    2,        5,  "<U0>a",         0),
    TEST ("<U0>ab<U0>@2c",    0,        4,  "<U0>c",         0),
    TEST ("ab<U0>c<U0>@2",    5,        1,  "ab<U0>c<U0>",   0),

    TEST ("a",                0,        3,  "",              0),
    TEST ("t<U0> s",          0,        9,  "",              0),
    TEST ("ab<U0>c<U0>@2",    0,       10,  "",              0),
    TEST ("x@4096",           0,     4105,  "",              0),

    TEST ("a",                3,        1,  "a",             1),
    TEST ("t<U0> s",          5,        1,  "t<U0> s",       1),
    TEST ("ab<U0>c<U0>@2",   10,        1,  "ab<U0>c<U0>@2", 1),
    TEST ("x@4096",        4106,        1,  "x@4096",        1),

    TEST ("x@4096",           0,     4096,  "",              0),
    TEST ("x@4096",           1,     4095,  "x",             0),
    TEST ("x@4096",           4,     4092,  "xxxx",          0),
    TEST ("x@4096",           4,     4090,  "xxxxxx",        0),
    TEST ("x@4096",        4096,     4096,  "x@4096",        0),
    TEST ("x@2048y@2048",     1,     4094,  "xy",            0),

    TEST ("last test",        4,        1,  "lasttest",      0)
};


/**************************************************************************/

// used to exercise
// erase (iterator)
static const StringTestCase
iter_test_cases [] = {

#undef TEST
#define TEST(str, off, res) {                                      \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,        \
        0, 0, res, sizeof res - 1, 0 }

    //    +-------------------------------------- controlled sequence
    //    |                   +------------------ iterator offset
    //    |                   |    +------------- expected result sequence
    //    |                   |    |
    //    V                   V    V
    TEST ("a",                0,   ""),

    TEST ("<U0>",             0,   ""),
    TEST ("<U0>@2",           0,   "<U0>"),
    TEST ("<U0>@2",           1,   "<U0>"),

    TEST ("abc",              0,   "bc"),
    TEST ("abc",              1,   "ac"),
    TEST ("abc",              2,   "ab"),

    TEST ("t<U0> s",          0,   "<U0> s"),
    TEST ("t<U0> s",          1,   "t s"),
    TEST ("t<U0> s",          2,   "t<U0>s"),
    TEST ("t<U0> s",          3,   "t<U0> "),

    TEST ("a<U0>@3b",         4,   "a<U0>@3"),
    TEST ("a<U0>@3b",         2,   "a<U0>@2b"),
    TEST ("a<U0>@3b",         1,   "a<U0>@2b"),
    TEST ("a<U0>@3b",         0,   "<U0>@3b"),
    TEST ("a<U0>b<U0>@2c",    4,   "a<U0>b<U0>c"),
    TEST ("<U0>ab<U0>@2c",    0,   "ab<U0>@2c"),
    TEST ("<U0>ab<U0>@2c",    2,   "<U0>a<U0>@2c"),
    TEST ("ab<U0>c<U0>@2",    5,   "ab<U0>c<U0>"),

    TEST ("x@4096y",       4096,   "x@4096"),
    TEST ("x@4096",        4088,   "x@4095"),
    TEST ("ax@4096",          0,   "x@4096"),
    TEST ("x@4096",           9,   "x@4095"),

    TEST ("last test",        4,   "lasttest")
};

/**************************************************************************/
// exercises basic_string::erase, 21.3.5.5

template <class charT, class Traits, class Allocator>
void test_erase (charT, Traits*, Allocator*,
                 const StringFunc     &func,
                 const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;
    typedef typename String::const_iterator              ConstStringIter;

    const bool use_iters =
        StringIds::arg_iter == StringIds::arg_type (func.which_, 1);

    if (use_iters && tcase.bthrow)
        return;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    static char nres_buf [BUFSIZE];
    char* nres = 0;
    if (use_iters) {
        std::size_t nreslen = sizeof nres_buf / sizeof *nres_buf;
        nres = rw_expand (nres_buf, tcase.res, tcase.nres, &nreslen);
    }

    // construct the string object to be modified
    // and the (possibly unused) argument string
    String str (wstr, str_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    wstr = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    StringIter res_iter = str.begin ();

    // compute the offset and the extent (the number of elements)
    // of the first range into the string object being modified
    const std::size_t size1 = str_len;
    const std::size_t off1 =
        std::size_t (tcase.off) < size1 ? std::size_t (tcase.off) : size1;
    const std::size_t ext1 =
        off1 + tcase.size < size1 ? tcase.size : size1 - off1;

    // create a pair of iterators into the string object being
    // modified (used only by the iterator overloads)
    const StringIter it_first (str.begin () + off1);
    const StringIter it_last  (it_first + ext1);

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == tcase.bthrow)
        expected = exceptions [1];

    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow) {
        if (wres != wres_buf)
            delete[] wres;

        return;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // pointer to the returned reference
    const String* ret_ptr = 0;

    try {
        switch (func.which_) {

        case Erase (void):
            ret_ptr = &str.erase ();
            break;

        case Erase (size):
            ret_ptr = &str.erase (tcase.off);
            break;

        case Erase (size_size):
            ret_ptr = &str.erase (tcase.off, tcase.size);
            break;

        case Erase (iter):
            res_iter = str.erase (it_first);
            break;

        case Erase (iter_iter):
            res_iter = str.erase (it_first, it_last);
            break;

        default:
            RW_ASSERT (!"test logic error: unknown erase overload");
            return;
        }

        // verify the returned value
        if (use_iters) {
            const ConstStringIter begin = str.begin ();
            const ConstStringIter end   = str.end ();

            const bool success = begin <= res_iter && res_iter <= end;
            rw_assert (success, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid iterator, "
                       "difference with begin is %td",
                       __LINE__, res_iter - begin);

            if (std::size_t (tcase.off) >= res_len) {
                rw_assert (res_iter == end, 0, tcase.line,
                           "line %d. %{$FUNCALL} != end()", __LINE__);
            }
            else {
                const std::size_t match =
                    rw_match (nres + tcase.off, &(*res_iter), 1);

                rw_assert (1 == match, 0, tcase.line,
                           "line %d. %{$FUNCALL} == %{#c}, got %{#c}",
                           __LINE__, nres[tcase.off], *res_iter);
            }
        }
        else {
            // verify that the reference returned from the function
            // refers to the modified string object (i.e., *this
            // within the function)
            const std::ptrdiff_t ret_off = ret_ptr - &str;

            // verify the returned value
            rw_assert (0 == ret_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %td", __LINE__, ret_off);
        }

        // verfiy that strings length are equal
        rw_assert (res_len == str.size (), 0, tcase.line,
                   "line %d. %{$FUNCALL} expected %{#*s} with length "
                   "%zu, got %{/*.*Gs} with length %zu",
                   __LINE__, int (tcase.nres), tcase.res,
                   res_len, int (sizeof (charT)),
                   int (str.size ()), str.c_str (), str.size ());

        if (res_len == str.size ()) {
            // if the result length matches the expected length
            // (and only then), also verify that the modified
            // string matches the expected result
            const std::size_t match =
                rw_match (tcase.res, str.c_str(), str.size ());

            rw_assert (match == res_len, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s}, "
                       "got %{/*.*Gs}, difference at offset %zu",
                       __LINE__, int (tcase.nres), tcase.res,
                       int (sizeof (charT)), int (str.size ()),
                       str.c_str (), match);
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

#endif   // _RWSTD_NO_EXCEPTIONS

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (str),
                                __LINE__, tcase.line, caught);
    }

    if (nres != nres_buf)
        delete[] nres;

    if (wres != wres_buf)
        delete[] wres;
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_erase);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Erase (sig), sig ## _test_cases,                        \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (void),
        TEST (size),
        TEST (size_size),
        TEST (iter),
        TEST (iter_iter)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.erase",
                               test_erase, tests, test_count);
}
