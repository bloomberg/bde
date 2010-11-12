/***************************************************************************
 *
 * 21.string.copy.cpp - string test exercising [lib.string::copy]
 *
 * $Id: 21.string.copy.cpp 590052 2007-10-30 12:44:14Z faridz $ 
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
#include <stdexcept>        // for out_of_range

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_assert()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define Copy(which)               StringIds::copy_ ## which

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// copy (value_type*, size_type)
static const StringTestCase
ptr_size_test_cases [] = {

#undef TEST
#define TEST(str, size, res) {                \
        __LINE__, -1, size, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,            \
        res, sizeof res - 1, 0                \
    }

    //    +--------------------------------------- controlled sequence
    //    |                       +--------------- copy() n argument
    //    |                       |   +----------- expected result sequence
    //    |                       |   |     
    //    V                       V   V             
    TEST ("ab",                   2,  "ab"),       

    TEST ("",                     0,  ""),         
    TEST ("",                    10,  ""),          

    TEST ("<U0>",                 1,  "<U0>"),       
    TEST ("<U0>@2",               2,  "<U0>@2"),    

    TEST ("abc",                  0,  ""), 
    TEST ("abc",                  1,  "a"),                 
    TEST ("abc",                  2,  "ab"),       
    TEST ("abc",                  3,  "abc"), 
    TEST ("abc",                  5,  "abc"),

    TEST ("a<U0>b<U0>@2c",       10,  "a<U0>b<U0>@2c"),  
    TEST ("<U0>ab<U0>@3c<U0>",    1,  "<U0>"),
    TEST ("<U0>ab<U0>@3c<U0>",    8,  "<U0>ab<U0>@3c<U0>"),
    TEST ("<U0>ab<U0>@3c<U0>",    5,  "<U0>ab<U0>@2"),   
    TEST ("<U0>@2ab<U0>@2c<U0>",  6,  "<U0>@2ab<U0>@2"), 

    TEST ("x@4096",            4096,  "x@4096"),

    TEST ("last",                 4,  "last")       
};

/**************************************************************************/

// exercises:
// copy (value_type*, size_type, size_type)
static const StringTestCase
ptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, size, off, res, bthrow)  {    \
        __LINE__, off, size, -1, -1, -1,        \
        str, sizeof str - 1, 0, 0,              \
        res, sizeof res - 1, bthrow             \
    }

    //    +------------------------------------------ controlled sequence
    //    |                       +------------------ copy() n argument
    //    |                       |   +-------------- copy() pos argument
    //    |                       |   |  +----------- expected result sequence
    //    |                       |   |  |       +--- exception info:
    //    |                       |   |  |       |      0 - no exception    
    //    |                       |   |  |       |      1 - out_of_range 
    //    |                       |   |  |       |   
    //    |                       |   |  |       +---------+   
    //    V                       V   V  V                 V  
    TEST ("ab",                   2,  0, "ab",             0),

    TEST ("",                     0,  0, "",               0),
    TEST ("",                    10,  0, "",               0),

    TEST ("<U0>",                 1,  0, "<U0>",           0),
    TEST ("<U0>@2",               1,  1, "<U0>",           0),
    TEST ("<U0>@2",               2,  0, "<U0>@2",         0),

    TEST ("abc",                  1,  0, "a",              0),
    TEST ("abc",                  1,  1, "b",              0),
    TEST ("abc",                  1,  2, "c",              0),

    TEST ("abc",                  0,  0, "",               0),
    TEST ("abc",                  2,  0, "ab",             0),
    TEST ("abc",                  2,  1, "bc",             0),
    TEST ("abc",                  3,  0, "abc",            0),
    TEST ("abc",                 10,  1, "bc",             0),
    TEST ("abc",                  3,  2, "c",              0),

    TEST ("a<U0>b<U0>@2c",       10,  1, "<U0>b<U0>@2c",   0),
    TEST ("a<U0>b<U0>@2c",       10,  0, "a<U0>b<U0>@2c",  0),
    TEST ("a<U0>b<U0>@2c",        1,  1, "<U0>",           0),

    TEST ("<U0>ab<U0>@3c<U0>",   10,  1, "ab<U0>@3c<U0>",  0),
    TEST ("<U0>ab<U0>@3c<U0>",    5,  0, "<U0>ab<U0>@2",   0),
    TEST ("<U0>ab<U0>@3c<U0>",    3,  3, "<U0>@3",         0),

    TEST ("<U0>@2ab<U0>@2c<U0>",  6,  0, "<U0>@2ab<U0>@2", 0),
    TEST ("<U0>@2ab<U0>@2c<U0>",  4,  1, "<U0>ab<U0>",     0),

    TEST ("x@4096",            4096,  0, "x@4096",         0),
    TEST ("x@4096",               2,  1, "xx",             0),
    TEST ("x@4096",            4096, 4095, "x",            0),

    TEST ("<U0>",                 0,  2, "",               1),
    TEST ("a",                    0, 10, "",               1),
    TEST ("x@4096",               0, 4106, "",             1),

    TEST ("last",                 4,  0, "last",           0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_copy (charT, Traits*, Allocator*,                
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
    const String  str (wstr, str_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    wstr = 0;

    const std::size_t min_len =
        str_len < std::size_t (tcase.size) ? str_len : tcase.size;

    std::size_t res = 0;

    // create destination array and initialize it with garbage
    charT* const s_res = new charT [min_len + 2];

    const char cgb [2] = "#";
    const charT wcgb = make_char (cgb [0], (charT*)0);
    Traits::assign (s_res, min_len + 1, wcgb);
    s_res [min_len + 1] = charT ();

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected?
    const char* const expected = tcase.bthrow ? exceptions [1] : 0;
    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow) {
        if (wres != wres_buf)
            delete[] wres;

        return;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        switch (func.which_) {
        case Copy (ptr_size): {
            res = str.copy (s_res, tcase.size);
            break;
        }

        case Copy (ptr_size_size): {
            res = str.copy (s_res, tcase.size, tcase.off);
            break;
        }
        default:
            RW_ASSERT (!"logic error: unknown copy overload");
            return;
        }

        // verify the returned value
        rw_assert (res == res_len, 0, tcase.line,
                   "line %d. %{$FUNCALL} == %zu, got %zu", 
                   __LINE__, res_len, res);

        if (res == res_len) 
        {
            const std::size_t match = rw_match (tcase.res, s_res, res_len);
            bool success = match == res_len;

            rw_assert (success, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{#*s}, "
                       "got %{/*.*Gs}, differ at pos %zu",
                       __LINE__, int (tcase.nres), tcase.res, 
                       int (sizeof (charT)), int (res), s_res, match);

            success = 1 == rw_match (cgb, s_res + min_len, 1);
            rw_assert (success, 0, tcase.line,
                       "line %d. %{$FUNCALL} detected writing past the end of "
                       "the provided buffer", __LINE__);
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
    else if (-1 != tcase.bthrow) {
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    if (wres != wres_buf)
        delete[] wres;

    delete[] s_res;
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_copy);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(which) {                                               \
        Copy (which), which ## _test_cases,                         \
        sizeof which ## _test_cases / sizeof *which ## _test_cases  \
    }

        TEST (ptr_size),
        TEST (ptr_size_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.copy",
                               test_copy, tests, test_count);
}
