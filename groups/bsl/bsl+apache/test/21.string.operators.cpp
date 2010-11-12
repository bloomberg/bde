/***************************************************************************
 *
 * 21.string.operators.cpp - test exercising [lib.string.nonmembers]
 *      except lib.string::op+ and lib.string.io
 *      
 * $Id: 21.string.operators.cpp 590052 2007-10-30 12:44:14Z faridz $
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

#include <21.strings.h>   // for StringMembers
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define OpEqual(which)             StringIds::op_equal_ ## which
#define OpNotEqual(which)          StringIds::op_not_equal_ ## which
#define OpLess(which)              StringIds::op_less_ ## which
#define OpLessEqual(which)         StringIds::op_less_equal_ ## which
#define OpGreater(which)           StringIds::op_greater_ ## which
#define OpGreaterEqual(which)      StringIds::op_greater_equal_ ## which

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// operator == (const value_type*, const basic_string&)
// operator != (const value_type*, const basic_string&)
// operator <  (const value_type*, const basic_string&)
// operator <= (const value_type*, const basic_string&)
// operator >  (const value_type*, const basic_string&)
// operator >= (const value_type*, const basic_string&)
static const StringTestCase
cptr_cstr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                         \
    { __LINE__, -1, -1, -1, -1, -1,                 \
      str, sizeof str - 1, arg, sizeof arg - 1,     \
      0, (std::size_t) res, 0                       \
    }

    //    +----------------------------------------- first sequence
    //    |                 +----------------------- second sequence 
    //    |                 |                  +---- expected result 
    //    |                 |                  |         -1 - first is less
    //    |                 |                  |          0 - equal
    //    |                 |                  |          1 - first is greater
    //    |                 |                  |        
    //    |                 |                  |        
    //    V                 V                  V          
    TEST ("ab",             "c",              -1),

    TEST ("",               "",                0),
    TEST ("",               "a",              -1),
    TEST ("a",              "",                1),
    TEST ("a",              "a",               0),

    TEST ("<U0>",           "",                0),
    TEST ("",               "<U0>",           -1),
    TEST ("<U0>",           "<U0>",           -1),

    TEST ("abcdefgjih",     "abcdefgji",       1),
    TEST ("abcdefgjih",     "abcdefgjih",      0),
    TEST ("abcdefgji",      "abcdefgjih",     -1),
    TEST ("abcefgjih",      "abcdefgjih",      1),
    TEST ("abcdefgjih",     "abcefgjih",      -1),

    TEST ("<U0>ab<U0>@2c",  "<U0>ab<U0>@2c",  -1),
    TEST ("ab<U0>@2c<U0>",  "ab",              0),
    TEST ("e<U0>",          "ab<U0>@2c<U0>",   1),

    TEST ("x@2048",         "x@2048",          0),
    TEST ("x@2048",         "x@2049",         -1),
    TEST ("x@2048",         "x@2047",          1),
    TEST ("a@2048",         "b@2048",         -1),
    TEST ("b@2048",         "a@2048",          1),
    TEST ("a@2048<U0>",     "a@2048",          0),
    TEST ("a@2048",         "a@2048<U0>",     -1),

    TEST ("x@128",          "x@207",          -1),
    TEST ("x@334",          "x@334",           0),
    TEST ("x@873",          "x@540",           1),
    TEST ("x@1412",         "x@2284",         -1),
    TEST ("x@3695",         "x@3695",          0),
    TEST ("x@4096",         "x@3695",          1),

    TEST (0,                "abc",             0),
    TEST (0,                "<U0>ab<U0>",     -1),
    TEST (0,                "x@2048",          0),

    TEST ("last",           "last",            0)
};

/**************************************************************************/

// exercises:
// operator == (const basic_string&, const basic_string&)
// operator != (const basic_string&, const basic_string&)
// operator <  (const basic_string&, const basic_string&)
// operator <= (const basic_string&, const basic_string&)
// operator >  (const basic_string&, const basic_string&)
// operator >= (const basic_string&, const basic_string&)
static const StringTestCase
cstr_cstr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                         \
    { __LINE__, -1, -1, -1, -1, -1,                 \
      str, sizeof str - 1, arg, sizeof arg - 1,     \
      0, (std::size_t) res, 0                       \
    }

    //    +----------------------------------------- first sequence
    //    |                 +----------------------- second sequence 
    //    |                 |                  +---- expected result 
    //    |                 |                  |        -1 - first is less
    //    |                 |                  |         0 - equal
    //    |                 |                  |         1 - first is greater
    //    |                 |                  |            
    //    |                 |                  |        
    //    V                 V                  V          
    TEST ("ab",             "c",              -1),

    TEST ("",               "",                0),
    TEST ("",               "a",              -1),
    TEST ("a",              "",                1),
    TEST ("a",              "a",               0),

    TEST ("<U0>",           "",                1),
    TEST ("",               "<U0>",           -1),
    TEST ("<U0>",           "<U0>",            0),

    TEST ("abcdefgjih",     "abcdefgji",       1),
    TEST ("abcdefgjih",     "abcdefgjih",      0),
    TEST ("abcdefgji",      "abcdefgjih",     -1),
    TEST ("abcefgjih",      "abcdefgjih",      1),
    TEST ("abcdefgjih",     "abcefgjih",      -1),

    TEST ("<U0>ab<U0>@2c",  "<U0>ab<U0>@2c",   0),
    TEST ("ab<U0>@2c<U0>",  "<U0>ab<U0>@2c",   1),
    TEST ("ab<U0>@2c<U0>",  "ab<U0>@2c<U0>",   0),
    TEST ("ab<U0>@2c<U0>",  "ab",              1),
    TEST ("ab",             "ab<U0>@2c<U0>",  -1),
    TEST ("ab<U0>@3",       "ab<U0>@2c<U0>",  -1),
    TEST ("ab<U0>@3",       "ab<U0>@3",        0),
    TEST ("e<U0>",          "ab<U0>@2c<U0>",   1),
    TEST ("ab<U0>@2c<U0>",  "e<U0>",          -1),

    TEST ("x@2048",         "x@2048",          0),
    TEST ("x@2048",         "x@2049",         -1),
    TEST ("x@2048",         "x@2047",          1),
    TEST ("a@2048",         "b@2048",         -1),
    TEST ("b@2048",         "a@2048",          1),
    TEST ("<U0>a@2048",     "a@2048<U0>",     -1),
    TEST ("a@2048<U0>",     "<U0>a@2048",      1),
    TEST ("a@2048<U0>",     "a@2048",          1),
    TEST ("a@2048",         "a@2048<U0>",     -1),
    TEST ("a@2048<U0>",     "a@2048<U0>",      0),

    TEST ("x@128",          "x@207",          -1),
    TEST ("x@334",          "x@334",           0),
    TEST ("x@873",          "x@540",           1),
    TEST ("x@1412",         "x@2284",         -1),
    TEST ("x@3695",         "x@3695",          0),
    TEST ("x@4096",         "x@3695",          1),

    TEST (0,                "abc",             0),
    TEST (0,                "<U0>ab<U0>",      0),
    TEST (0,                "x@2048",          0),
    TEST ("abc",            0,                 0),
    TEST ("<U0>ab<U0>",     0,                 0),
    TEST ("x@2048",         0,                 0),

    TEST ("last",           "last",            0)
};

/**************************************************************************/

// exercises:
// operator == (const basic_string&, const value_type*)
// operator != (const basic_string&, const value_type*)
// operator <  (const basic_string&, const value_type*)
// operator <= (const basic_string&, const value_type*)
// operator >  (const basic_string&, const value_type*)
// operator >= (const basic_string&, const value_type*)
static const StringTestCase
cstr_cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                         \
    { __LINE__, -1, -1, -1, -1, -1,                 \
      str, sizeof str - 1, arg, sizeof arg - 1,     \
      0, (std::size_t) res, 0                       \
    }

    //    +----------------------------------------- first sequence
    //    |                 +----------------------- second sequence 
    //    |                 |                  +---- expected result 
    //    |                 |                  |        -1 - first is less
    //    |                 |                  |         0 - equal
    //    |                 |                  |         1 - first is greater
    //    |                 |                  |        
    //    |                 |                  |        
    //    V                 V                  V          
    TEST ("ab",             "c",              -1),

    TEST ("",               "",                0),
    TEST ("",               "a",              -1),
    TEST ("a",              "",                1),
    TEST ("a",              "a",               0),

    TEST ("<U0>",           "",                1),
    TEST ("",               "<U0>",            0),
    TEST ("<U0>",           "<U0>",            1),

    TEST ("abcdefgjih",     "abcdefgji",       1),
    TEST ("abcdefgjih",     "abcdefgjih",      0),
    TEST ("abcdefgji",      "abcdefgjih",     -1),
    TEST ("abcefgjih",      "abcdefgjih",      1),
    TEST ("abcdefgjih",     "abcefgjih",      -1),

    TEST ("<U0>ab<U0>@2c",  "<U0>ab<U0>@2c",   1),
    TEST ("ab",             "ab<U0>@2c<U0>",   0),
    TEST ("e<U0>",          "ab<U0>@2c<U0>",   1),
    TEST ("ab<U0>@2c<U0>",  "e<U0>",          -1),

    TEST ("x@2048",         "x@2048",          0),
    TEST ("x@2048",         "x@2049",         -1),
    TEST ("x@2048",         "x@2047",          1),
    TEST ("a@2048",         "b@2048",         -1),
    TEST ("b@2048",         "a@2048",          1),
    TEST ("a@2048",         "a@2048<U0>",      0),
    TEST ("a@2048<U0>",     "a@2048<U0>",      1),

    TEST ("x@128",          "x@207",          -1),
    TEST ("x@334",          "x@334",           0),
    TEST ("x@873",          "x@540",           1),
    TEST ("x@1412",         "x@2284",         -1),
    TEST ("x@3695",         "x@3695",          0),
    TEST ("x@4096",         "x@3695",          1),

    TEST ("abc",            0,                 0),
    TEST ("<U0>ab<U0>",     0,                 1),
    TEST ("x@2048",         0,                 0),

    TEST ("last",           "last",            0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_operators (charT, Traits*, Allocator*,
                     const StringFunc     &func,
                     const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename UserTraits<charT>::MemFun           UTMemFun;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    // construct the string object to be modified
    // and the (possibly unused) argument string
    /* const */ String  s_str (wstr, str_len);
    const       String  s_arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    const charT* const arg1_ptr = tcase.str ? s_str.c_str () : s_arg.c_str ();
    const String&      arg1_str = tcase.str ? s_str : s_arg;
    const charT* const arg2_ptr = tcase.arg ? s_arg.c_str () : s_str.c_str ();
    const String&      arg2_str = tcase.arg ? s_arg : s_str;

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

        if (0 == tcase.bthrow) {
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

        try {

            bool res = false;

            switch (func.which_) {
            // exercise cptr_cstr cases
            case OpEqual (cptr_cstr): 
                res = arg1_ptr == arg2_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpNotEqual (cptr_cstr): 
                res = arg1_ptr != arg2_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpLess (cptr_cstr): 
                res = arg1_ptr < arg2_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpLessEqual (cptr_cstr): 
                res = arg1_ptr <= arg2_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpGreater (cptr_cstr): 
                res = arg1_ptr > arg2_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpGreaterEqual (cptr_cstr): 
                res = arg1_ptr >= arg2_str;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            

            // exercise cstr_cstr cases
            case OpEqual (cstr_cstr): 
                res = arg1_str == arg2_str;
                break;
            
            case OpNotEqual (cstr_cstr): 
                res = arg1_str != arg2_str;
                break;
            
            case OpLess (cstr_cstr): 
                res = arg1_str < arg2_str;
                break;
            
            case OpLessEqual (cstr_cstr): 
                res = arg1_str <= arg2_str;
                break;
            
            case OpGreater (cstr_cstr): 
                res = arg1_str > arg2_str;
                break;
            
            case OpGreaterEqual (cstr_cstr): 
                res = arg1_str >= arg2_str;
                break;
            

            // exercise cstr_cptr cases
            case OpEqual (cstr_cptr): 
                res = arg1_str == arg2_ptr;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpNotEqual (cstr_cptr): 
                res = arg1_str != arg2_ptr;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpLess (cstr_cptr): 
                res = arg1_str < arg2_ptr;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpLessEqual (cstr_cptr): 
                res = arg1_str <= arg2_ptr;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpGreater (cstr_cptr): 
                res = arg1_str > arg2_ptr;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;
            
            case OpGreaterEqual (cstr_cptr): 
                res = arg1_str >= arg2_ptr;
                if (rg_calls)
                    n_length_calls = rg_calls [UTMemFun::length];
                break;

            default:
                RW_ASSERT ("test logic error: unknown operator overload");
                return;
            }

            // form the expected result
            bool exp_res = false;
            switch (StringIds::fid_mask & int (func.which_)) {
                case StringIds::fid_op_equal: 
                    exp_res = 0 == tcase.nres;
                    break;
                
                case StringIds::fid_op_not_equal: 
                    exp_res = 0 != tcase.nres;
                    break;
                
                case StringIds::fid_op_less: 
                    exp_res = NPOS == tcase.nres;
                    break;
                
                case StringIds::fid_op_less_equal: 
                    exp_res = 1 != tcase.nres;
                    break;
                
                case StringIds::fid_op_greater: 
                    exp_res = 1 == tcase.nres;
                    break;
                
                case StringIds::fid_op_greater_equal: 
                    exp_res = NPOS != tcase.nres;
                    break;
            }

            // verify the result
            rw_assert (res == exp_res, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %b, got %b",
                       __LINE__, exp_res, res);

            // verify that Traits::length was used
            std::size_t exp_len_used = 
                (StringIds::arg_cstr << int (StringIds::fid_bits))
              | (StringIds::arg_cstr << int (StringIds::arg_bits)
                                     << int (StringIds::fid_bits));

            std::size_t verlen = 
                func.which_ & ~StringIds::fid_mask & ~exp_len_used;

            if (verlen && rg_calls) {
                rw_assert (n_length_calls - total_length_calls > 0, 
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits::length()", __LINE__);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

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

        if (caught) {

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

    const std::size_t expect_throws = 0;

    rw_assert (expect_throws == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected no %s exceptions, got %zu",
               __LINE__, exceptions [3], throw_count);

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_operators);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(oper, tag) {                                           \
        StringIds::op_ ## oper ## _ ## tag, tag ## _test_cases,     \
        sizeof tag ## _test_cases / sizeof *tag ## _test_cases      \
    }

        TEST (equal, cptr_cstr),
        TEST (equal, cstr_cptr),
        TEST (equal, cstr_cstr),

        TEST (not_equal, cptr_cstr),
        TEST (not_equal, cstr_cptr),
        TEST (not_equal, cstr_cstr),

        TEST (less, cptr_cstr),
        TEST (less, cstr_cptr),
        TEST (less, cstr_cstr),

        TEST (less_equal, cptr_cstr),
        TEST (less_equal, cstr_cptr),
        TEST (less_equal, cstr_cstr),

        TEST (greater, cptr_cstr),
        TEST (greater, cstr_cptr),
        TEST (greater, cstr_cstr),

        TEST (greater_equal, cptr_cstr),
        TEST (greater_equal, cstr_cptr),
        TEST (greater_equal, cstr_cstr)

    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.nonmembers",
                               test_operators, tests, test_count);
}
