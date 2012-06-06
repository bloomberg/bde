/***************************************************************************
 *
 * 21.string.plus.equal.cpp - test exercising [lib.string.op+=]
 *
 * $Id: 21.string.op.plus.equal.cpp 590052 2007-10-30 12:44:14Z faridz $
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

#include <string>         // for string
#include <stdexcept>      // for out_of_range, length_error

#include <cstddef>        // for size_t

#include <21.strings.h>   // for StringIds
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define OpPlusEq(sig) StringIds::op_plus_eq_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// operator += (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, src, res, bthrow)                            \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, src,  \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |                 +----------------------- sequence to be appended
    //    |                 |             +--------- expected result sequence
    //    |                 |             |        +---- exception info
    //    |                 |             |        |         0 - no exception
    //    |                 |             |        |         1 - length_error
    //    |                 |             |        |
    //    |                 |             |        +--------------+
    //    V                 V             V                       V
    TEST ("ab",             "c",          "abc",                  0),

    TEST ("",               "",           "",                     0),
    TEST ("",               "<U0>",       "",                     0),
    TEST ("",               "abc",        "abc",                  0),

    TEST ("<U0>",           "",           "<U0>",                 0),
    TEST ("<U0>",           "a",          "<U0>a",                0),
    TEST ("<U0>",           "<U0>@2",     "<U0>",                 0),

    TEST ("ab",             "cd",         "abcd",                 0),
    TEST ("bcd",            "a",          "bcda",                 0),
    TEST ("cde",            "ab",         "cdeab",                0),
    TEST ("abc",            "",           "abc",                  0),
    TEST ("ab",             "c<U0>e",     "abc",                  0),

    TEST ("<U0>@2ab",       "cdefghij",   "<U0>@2abcdefghij",     0),
    TEST ("a<U0>@2b",       "cdefghij",   "a<U0>@2bcdefghij",     0),
    TEST ("ab<U0>@2",       "cdefghij",   "ab<U0>@2cdefghij",     0),
    TEST ("a<U0>b<U0>@2c",  "e<U0>",      "a<U0>b<U0>@2ce",       0),
    TEST ("<U0>ab<U0>@2c",  "e<U0>",      "<U0>ab<U0>@2ce",       0),
    TEST ("abcdefghij",     "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",               "x@4096",     "x@4096",               0),
    TEST ("x@4096",         "",           "x@4096",               0),
    TEST ("x@2048",         "y@2048",     "x@2048y@2048",         0),

    TEST ("x@10",           "x@118",      "x@128",                0),
    TEST ("x@128",          "x@79",       "x@207",                0),
    TEST ("x@207",          "x@127",      "x@334",                0),
    TEST ("x@334",          "x@206",      "x@540",                0),
    TEST ("x@540",          "x@333",      "x@873",                0),
    TEST ("x@539",          "x@873",      "x@1412",               0),
    TEST ("x@872",          "x@1412",     "x@2284",               0),
    TEST ("x@1411",         "x@2284",     "x@3695",               0),
    TEST ("x@1412",         "x@2284",     "x@3696",               0),

    TEST ("",               0,            "",                     0),
    TEST ("abc",            0,            "abcabc",               0),
    TEST ("a<U0>@2bc",      0,            "a<U0>@2bca",           0),
    TEST ("<U0>@2abc",      0,            "<U0>@2abc",            0),
    TEST ("abc<U0>@2",      0,            "abc<U0>@2abc",         0),
    TEST ("x@2048",         0,            "x@4096",               0),

    TEST ("last",           "test",       "lasttest",             0)
};

/**************************************************************************/

// exercises:
// operator += (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(str, src, res, bthrow)                            \
    { __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1, src,  \
      sizeof src - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |                 +----------------------- sequence to be appended
    //    |                 |             +--------- expected result sequence
    //    |                 |             |        +---- exception info
    //    |                 |             |        |         0 - no exception
    //    |                 |             |        |         1 - length_error
    //    |                 |             |        |
    //    |                 |             |        +--------------+
    //    V                 V             V                       V
    TEST ("ab",             "c",          "abc",                  0),

    TEST ("",               "",           "",                     0),
    TEST ("",               "<U0>",       "<U0>",                 0),
    TEST ("",               "abc",        "abc",                  0),

    TEST ("<U0>",           "",           "<U0>",                 0),
    TEST ("<U0>",           "a",          "<U0>a",                0),
    TEST ("<U0>",           "<U0>@2",     "<U0>@3",               0),

    TEST ("ab",             "cd",         "abcd",                 0),
    TEST ("bcd",            "a",          "bcda",                 0),
    TEST ("cde",            "ab",         "cdeab",                0),
    TEST ("abc",            "",           "abc",                  0),
    TEST ("ab",             "c<U0>e",     "abc<U0>e",             0),

    TEST ("<U0>@2ab",       "cdefghij",   "<U0>@2abcdefghij",     0),
    TEST ("a<U0>@2b",       "cdefghij",   "a<U0>@2bcdefghij",     0),
    TEST ("ab<U0>@2",       "cdefghij",   "ab<U0>@2cdefghij",     0),
    TEST ("a<U0>b<U0>@2c",  "e<U0>",      "a<U0>b<U0>@2ce<U0>",   0),
    TEST ("<U0>ab<U0>@2c",  "e<U0>",      "<U0>ab<U0>@2ce<U0>",   0),
    TEST ("ab<U0>@2c<U0>",  "<U0>e",      "ab<U0>@2c<U0>@2e",     0),
    TEST ("abcdefghij",     "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",               "x@4096",     "x@4096",               0),
    TEST ("x@4096",         "",           "x@4096",               0),
    TEST ("x@2048",         "y@2048",     "x@2048y@2048",         0),

    TEST ("x@10",           "x@118",      "x@128",                0),
    TEST ("x@128",          "x@79",       "x@207",                0),
    TEST ("x@207",          "x@127",      "x@334",                0),
    TEST ("x@334",          "x@206",      "x@540",                0),
    TEST ("x@540",          "x@333",      "x@873",                0),
    TEST ("x@539",          "x@873",      "x@1412",               0),
    TEST ("x@872",          "x@1412",     "x@2284",               0),
    TEST ("x@1411",         "x@2284",     "x@3695",               0),
    TEST ("x@1412",         "x@2284",     "x@3696",               0),

    TEST ("",               0,            "",                     0),
    TEST ("abc",            0,            "abcabc",               0),
    TEST ("a<U0>@2bc",      0,            "a<U0>@2bca<U0>@2bc",   0),
    TEST ("<U0>@2abc",      0,            "<U0>@2abc<U0>@2abc",   0),
    TEST ("abc<U0>@2",      0,            "abc<U0>@2abc<U0>@2",   0),
    TEST ("x@2048",         0,            "x@4096",               0),

    TEST ("last",           "test",       "lasttest",             0)
};

/**************************************************************************/

// exercises:
// operator+= (value_type)
static const StringTestCase
val_test_cases [] = {

#undef TEST
#define TEST(str, val, res, bthrow)                              \
    { __LINE__, -1, -1, -1, -1, val, str, sizeof str - 1, 0, 0,  \
      res, sizeof res - 1, bthrow }

    //    +---------------------------------- controlled sequence
    //    |                +----------------- character to be appended
    //    |                |   +------------- expected result sequence
    //    |                |   |           +- exception info
    //    |                |   |           |      0 - no exception
    //    |                |   |           |       1 - length_error
    //    |                |   |           |
    //    |                |   |           +----+
    //    V                V   V                V
    TEST ("ab",            'c', "abc",          0),

    TEST ("",              'a',  "a",           0),
    TEST ("",              '\0', "<U0>",        0),

    TEST ("<U0>",          'a',  "<U0>a",       0),
    TEST ("<U0>",          '\0', "<U0>@2",      0),

    TEST ("cde",           'a',  "cdea",        0),
    TEST ("abc",           '\0', "abc<U0>",     0),

    TEST ("a<U0>b<U0>@2c", '\0', "a<U0>b<U0>@2c<U0>", 0),
    TEST ("<U0>ab<U0>@2c", '\0', "<U0>ab<U0>@2c<U0>", 0),
    TEST ("a<U0>bc<U0>@2", 'a',  "a<U0>bc<U0>@2a",    0),

    TEST ("x@127",         'x',  "x@128",       0),
    TEST ("x@206",         'x',  "x@207",       0),
    TEST ("x@333",         'x',  "x@334",       0),
    TEST ("x@539",         'x',  "x@540",       0),
    TEST ("x@1411",        'x',  "x@1412",      0),
    TEST ("x@2283",        'x',  "x@2284",      0),
    TEST ("x@3694",        'x',  "x@3695",      0),
    TEST ("x@540",         'x',  "x@541",       0),

    TEST ("last",          't',  "lastt",       0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_op_plus_eq (charT, Traits*, Allocator*,
                      const StringFunc     &func,
                      const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;
    typedef typename UserTraits<charT>::MemFun           UTMemFun;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    // construct the string object to be modified
    // and the (possibly unused) argument string
    /* const */ String str (wstr, str_len);
    const       String arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    const charT* const arg_ptr = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      arg_str = tcase.arg ? arg : str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);

    if (rg_calls)
        total_length_calls = rg_calls [UTMemFun::length];

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // iterate for`throw_after' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (1 == tcase.bthrow)
            expected = exceptions [2];      // length_error
        else if (0 == tcase.bthrow) {
            // by default excercise the exception safety of the function
            // by iteratively inducing an exception at each call to operator
            // new or Allocator::allocate() until the call succeeds
            expected = exceptions [3];      // bad_alloc
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_count + 1;
            pal->throw_at_calls_ [pal->m_allocate] =
                pal->throw_at_calls_ [pal->m_allocate] + throw_count + 1;
        }
        else {
            // exceptions disabled for this test case
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow) {
            if (wres != wres_buf)
                delete[] wres;

            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        try {

            // pointer to the returned reference
            const String* ret_ptr = 0;

            switch (func.which_) {

            case OpPlusEq (cptr):
                ret_ptr = &str.operator+= (arg_ptr);
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;

            case OpPlusEq (cstr):
                ret_ptr = &str.operator+= (arg_str);
                break;

            case OpPlusEq (val):
                ret_ptr = &str.operator+= (arg_val);
                break;

            default:
                RW_ASSERT ("test logic error: unknown operator += overload");
                return;
            }

            // verify that the reference returned from the function
            // refers to the modified string object (i.e., *this
            // within the function)
            const std::ptrdiff_t ret_off = ret_ptr - &str;

            // verify the returned value
            rw_assert (0 == ret_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %td", __LINE__, ret_off);

            // verfiy that strings length are equal
            rw_assert (res_len == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s} "
                       "with length %zu, got %{/*.*Gs} with length %zu",
                       __LINE__, int (tcase.nres), tcase.res, res_len,
                       int (sizeof (charT)), int (str.size ()),
                       str.c_str (), str.size ());

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

            // verify that Traits::length was used
            if (OpPlusEq (cptr) == func.which_ && rg_calls) {
                rw_assert (n_length_calls - total_length_calls > 0,
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits::length()", __LINE__);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::length_error &ex) {
            caught = exceptions [2];
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (const std::bad_alloc &ex) {
            caught = exceptions [3];
            rw_assert (0 == tcase.bthrow, 0, tcase.line,
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

        // FIXME: verify the number of blocks the function call
        // is expected to allocate and detect any memory leaks
        rw_check_leaks (str.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object
            str_state.assert_equal (rw_get_string_state (str),
                                    __LINE__, tcase.line, caught);

            if (0 == tcase.bthrow) {
                // allow this call to operator new to succeed and try
                // to make the next one to fail during the next call
                // to the same function again
                continue;
            }
        }
        else if (0 < tcase.bthrow) {
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    const std::size_t expect_throws = str_state.capacity_ < str.capacity ();

#else   // if defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

    const std::size_t expect_throws =
        (StringIds::UserAlloc == func.alloc_id_)
      ? str_state.capacity_ < str.capacity (): 0;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    rw_assert (expect_throws == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected exactly 1 %s exception "
               "while changing capacity from %zu to %zu, got %zu",
               __LINE__, exceptions [3],
               str_state.capacity_, str.capacity (), throw_count);

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;

    if (wres != wres_buf)
        delete[] wres;
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_op_plus_eq);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        OpPlusEq (sig), sig ## _test_cases,                     \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (val)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.op+=",
                               test_op_plus_eq, tests, test_count);
}
