/***************************************************************************
 *
 * 21.string.swap.cpp - string test exercising [lib.string::swap]
 *
 * $Id: 21.string.swap.cpp 590064 2007-10-30 13:16:44Z faridz $
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
#include <cstddef>          // size_t
#include <exception>        // for exception

#include <21.strings.h>     // for StringIds
#include <driver.h>         // for rw_assert()
#include <rw_allocator.h>   // foir UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Swap(sig)   StringIds::swap_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// swap (basic_string&)
static const StringTestCase
str_test_cases [] = {

#undef TEST
#define TEST(str, arg)                                  \
    { __LINE__, -1, -1, -1, -1, -1,                     \
      str, sizeof str - 1, arg, sizeof arg - 1,         \
      0, 0, 0                                           \
    }

    //    +------------------------- controlled "destination" (str) sequence
    //    |                +-------- controlled "source" (arg) sequence
    //    |                |
    //    V                V
    TEST ("",              ""),
    TEST ("",              "a"),
    TEST ("a",             ""),
    TEST ("",              "<U0>"),
    TEST ("<U0>",          ""),

    TEST ("a",             "b"),
    TEST ("a",             "bc"),
    TEST ("ab",            "c"),

    TEST ("a<U0>b<U0>@2c", "<U0>b<U0>@2c"),
    TEST ("<U0>b<U0>@2c",  "a<U0>b<U0>@2c"),

    TEST ("a<U0>b<U0>@2c", "<U0>@2"),
    TEST ("<U0>@2",        "a<U0>b<U0>@2c"),

    TEST ("x@4096",        ""),
    TEST ("",              "x@4096"),
    TEST ("x@4096",        "<U0>@3"),
    TEST ("<U0>@3",        "x@4096"),
    TEST ("x@4096",        "x@4096"),

    TEST ("",              "x@128"),
    TEST ("x@207",         "x@128"),
    TEST ("x@128",         "x@334"),
    TEST ("x@873",         "x@334"),
    TEST ("x@1412",        "x@540"),
    TEST ("x@540",         "x@2284"),
    TEST ("x@3695",        "x@2284"),
    TEST ("x@3695",        "x@128"),

    TEST ("",              0),
    TEST ("<U0>",          0),
    TEST ("abc",           0),
    TEST ("a<U0>b<U0>@2c", 0),
    TEST ("x@4096",        0),

    TEST (0,               ""),
    TEST (0,               "<U0>"),
    TEST (0,               "abc@1024"),
    TEST (0,               "a<U0>b<U0>@2c"),
    TEST (0,               "x@4096"),

    TEST ("last",          "test")
};


/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_swap (charT*, Traits*,
                Allocator                       &a1,
                Allocator                       &a2,
                const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef UserTraits<UserChar>::MemFun                 UTMemFun;

    const StringTestCase &tcase = tdata.tcase_;

    // construct the string object to be modified
    // and the argument string
    String s_str (tdata.str_, tdata.strlen_, a1);
    String s_arg (tdata.arg_, tdata.arglen_, a2);

    const charT* const p1 = s_str.data ();
    const charT* const p2 = tcase.arg ? s_arg.data () : s_str.data ();

    const char* const src     = tcase.arg ? tcase.arg : tcase.str;
    const std::size_t src_len = tcase.arg ? tcase.arg_len : tcase.str_len;
    const std::size_t srclen_ = tcase.arg ? tdata.arglen_ : tdata.strlen_;
    String& arg_str           = tcase.arg ? s_arg : s_str;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));
    const StringState arg_str_state (rw_get_string_state (arg_str));

    std::size_t n_total_op_assign  = 0;
    std::size_t n_total_op_assign2 = 0;
    std::size_t n_total_op_copy    = 0;
    std::size_t n_total_op_move    = 0;

    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);
    if (rg_calls) {
        n_total_op_assign  = rg_calls[UTMemFun::assign];
        n_total_op_assign2 = rg_calls[UTMemFun::assign2];
        n_total_op_copy    = rg_calls[UTMemFun::copy];
        n_total_op_move    = rg_calls[UTMemFun::move];
    }

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // iterate for`throw_count' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {

        const char* expected = 0;
        const char* caught = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        // no exceptions expected
        if (0 == tcase.bthrow) {
            // by default excercise the exception safety of the function
            // by iteratively inducing an exception at each call to operator
            // new or Allocator::allocate() until the call succeeds
            expected = exceptions [3];      // bad_alloc
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_count + 1;
            pal->throw_at_calls_ [pal->m_allocate] =
                pal->throw_at_calls_ [pal->m_allocate] + throw_count + 1;
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        try {

            // start checking for memory leaks
            rw_check_leaks (s_str.get_allocator ());
            rw_check_leaks (arg_str.get_allocator ());

            if (0 == tcase.str)
                String ().swap (arg_str);
            else
                s_str.swap (arg_str);

            if (rg_calls) {

                std::size_t n_op_assign  =
                    rg_calls[UTMemFun::assign]  - n_total_op_assign;
                std::size_t n_op_assign2 =
                    rg_calls[UTMemFun::assign2] - n_total_op_assign2;
                std::size_t n_op_copy    =
                    rg_calls[UTMemFun::copy]    - n_total_op_copy;
                std::size_t n_op_move    =
                    rg_calls[UTMemFun::move]    - n_total_op_move;

                bool success =
                    0 == (n_op_assign | n_op_assign2 | n_op_copy | n_op_move);
#if TEST_RW_EXTENSIONS  // no such operations only if allocators are the same
                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL}: complexity: %zu assigns, "
                           "%zu assign2s, %zu copies, %zu moves", __LINE__,
                           n_op_assign, n_op_assign2, n_op_copy, n_op_move);
#else
                if (a1 == a2 && 0 != tcase.str) {
                    rw_assert (success, 0, tcase.line,
                               "line %d. %{$FUNCALL}: complexity: %zu assigns, "
                               "%zu assign2s, %zu copies, %zu moves", __LINE__,
                               n_op_assign, n_op_assign2, n_op_copy, n_op_move);
                }
#endif
            }

            if (0 == tcase.str) {

                rw_assert (0 == arg_str.capacity (), 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected 0 capacity, "
                           "got %zu",  __LINE__, arg_str.capacity ());
            }
            else {

                const charT* const res_p1 = s_str.data ();
                const charT* const res_p2 =
                    tcase.arg ? s_arg.data () : s_str.data ();

                const std::size_t res1_len = s_str.size ();
                const std::size_t res2_len =
                    tcase.arg ? s_arg.size () : s_str.size ();

#if TEST_RW_EXTENSIONS  // the address of the string will be the same only if
                        // both allocators are the same

                rw_assert (res_p1 == p2 && res_p2 == p1, 0, tcase.line,
                           "line %d. %{$FUNCALL}: got offset %td from "
                           "expected value, arg.data (): got offset %td "
                           "from expected value",
                           __LINE__, p2 - res_p1, p1 - res_p2);

#else
                if (a1 == a2) {
                rw_assert (res_p1 == p2 && res_p2 == p1, 0, tcase.line,
                           "line %d. %{$FUNCALL}: got offset %td from "
                           "expected value, arg.data (): got offset %td "
                           "from expected value",
                           __LINE__, p2 - res_p1, p1 - res_p2);
                }

#endif
                std::size_t match =
                    rw_match (tcase.str, res_p2, res2_len);

                rw_assert (match == tdata.strlen_, 0, tcase.line,
                           "line %d. %{$FUNCALL}: this == %{#*s}, got this = "
                           "%{/*.*Gs}, differs at pos %zu",
                           __LINE__, int (src_len), src, int (sizeof (charT)),
                           int (res1_len), res_p1, match);

                match = rw_match (src, res_p1, res1_len);

                rw_assert (match == srclen_, 0, tcase.line,
                           "line %d. %{$FUNCALL}: str == %{#*s}, got str = "
                           "%{/*.*Gs}, differs at pos %zu",
                           __LINE__, int (tcase.str_len), tcase.str,
                           int (sizeof (charT)), int (res2_len),
                           res_p2, match);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::exception &ex) {
            caught = exceptions [4];
#if TEST_RW_EXTENSIONS  // only no allocations if allocators are the same.
                rw_assert (0, 0, tcase.line,
                           "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                           "unexpectedly%{;} caught std::%s(%#s)",
                           __LINE__, 0 != expected, expected, caught, ex.what ());
#else
            if (a1 == a2 && 0 != tcase.str) {
                rw_assert (0, 0, tcase.line,
                           "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                           "unexpectedly%{;} caught std::%s(%#s)",
                           __LINE__, 0 != expected, expected, caught, ex.what ());
            }
#endif
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
        rw_check_leaks (s_str.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        rw_check_leaks (arg_str.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object
            str_state.assert_equal (rw_get_string_state (s_str),
                                    __LINE__, tcase.line, caught);

            arg_str_state.assert_equal (rw_get_string_state (arg_str),
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

    // no exception expected
#if TEST_RW_EXTENSIONS  // There will be exceptions
    const std::size_t expect_throws = 0;

    rw_assert (expect_throws == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected exactly 0 %s exception "
               "while the swap, got %zu",
               __LINE__, exceptions [3], throw_count);
#else
    const std::size_t expect_throws = 2;  // no more than 2 expected

    rw_assert (expect_throws >= throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected exactly 0 %s exception "
               "while the swap, got %zu",
               __LINE__, exceptions [3], throw_count);
#endif


    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

template <class charT>
std::allocator<charT>
make_alloc (SharedAlloc&, std::allocator<charT>*) {
    return std::allocator<charT>();
}

template <class charT, class Types>
UserAlloc<charT, Types>
make_alloc (SharedAlloc &shal, UserAlloc<charT, Types>*) {
    return UserAlloc<charT, Types>(&shal);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_swap (charT*, Traits*, Allocator*,
                const StringTestCaseData<charT> &tdata)
{
    SharedAlloc sa1;
    Allocator a1 = make_alloc(sa1, (Allocator*)0);

    // test swap using the same allocator objects
    test_swap ((charT*)0, (Traits*)0, a1, a1, tdata);

    SharedAlloc sa2;
    Allocator a2 = make_alloc(sa2, (Allocator*)0);

    if (a1 != a2) {
        // test swap using different allocator objects
        test_swap ((charT*)0, (Traits*)0, a1, a2, tdata);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_swap);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Swap (sig), sig ## _test_cases,                         \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases, \
    }

        TEST (str)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_string_test (argc, argv, __FILE__,
                            "lib.string.swap",
                            test_swap_func_array, tests, test_count);

    return status;
}
