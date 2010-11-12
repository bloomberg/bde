/***************************************************************************
 *
 * 21.string.capacity.cpp - test exercising [lib.string.capacity]
 *
 * $Id: 21.string.capacity.cpp 590052 2007-10-30 12:44:14Z faridz $
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
#define Size(sig)                 StringIds::size_ ## sig
#define Length(sig)               StringIds::length_ ## sig
#define MaxSize(sig)              StringIds::max_size_ ## sig
#define Resize(sig)               StringIds::resize_ ## sig
#define Capacity(sig)             StringIds::capacity_ ## sig
#define Reserve(sig)              StringIds::reserve_ ## sig
#define Clear(sig)                StringIds::clear_ ## sig
#define Empty(sig)                StringIds::empty_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// size ()
static const StringTestCase
size_void_test_cases [] = {

#define length_void_test_cases  size_void_test_cases 

#undef TEST
#define TEST(str, res) {                    \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        0, res, -1                          \
    }

    //    +--------------------------------------- controlled sequence
    //    |                    +------------------ expected result
    //    |                    |                   
    //    |                    |                   
    //    V                    V                   
    TEST (0,                   0),
    TEST ("",                  0),

    TEST ("<U0>",              1),
    TEST ("a",                 1),
    TEST (" ",                 1),
    TEST ("ab",                2),
    TEST ("bc",                2),

    TEST ("test string",      11),
    TEST ("Test String",      11),

    TEST ("t<U0> s",           4),
    TEST ("Test<U0>string",   11),

    TEST ("<U0>a<U0>b",        4),
    TEST ("a<U0>@2b",          4),
    TEST ("a<U0>@3b",          5),
    TEST ("a<U0>@2b<U0>",      5),
    TEST ("a<U0>b<U0>@2c",     6),
    TEST ("a<U0>b<U0>c<U0>@2", 7),

    TEST ("x@128",           128),
    TEST ("x@207",           207),
    TEST ("x@334",           334),
    TEST ("x@540",           540),
    TEST ("x@873",           873),
    TEST ("x@1412",         1412),
    TEST ("x@2284",         2284),
    TEST ("x@3695",         3695),
    TEST ("x@4096",         4096),

    TEST ("last",              4)

};

/**************************************************************************/

// used to exercise
// max_size ()
static const StringTestCase
max_size_void_test_cases [] = {

#undef TEST
#define TEST(str) {                         \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        0, 0, -1                            \
    }

    //    +------------------------------ controlled sequence
    //    |                
    //    |                               
    //    |                               
    //    V                             
    TEST (0),     
    TEST (""),   

    TEST ("<U0>"),    
    TEST ("a"),       

    TEST ("test string"), 
    TEST ("a<U0>b<U0>c<U0>@2"),  

    TEST ("x@128"),     
    TEST ("x@207"),    
    TEST ("x@2284"),   
    TEST ("x@3695"), 

    TEST ("last")        

};

/**************************************************************************/

// used to exercise
// resize (size_type, value_type)
static const StringTestCase
resize_size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {    \
        __LINE__, -1, size, -1, -1, val,       \
        str, sizeof str - 1, 0, 0,             \
        res, sizeof res - 1, bthrow            \
    }

    //    +------------------------------------------ controlled sequence
    //    |                      +------------------- new size
    //    |                      |     +------------- value_type argument
    //    |                      |     |    +-------- expected result sequence
    //    |                      |     |    |      +- exception info
    //    |                      |     |    |      |     0 - no exception
    //    |                      |     |    |      |     1 - length_error
    //    |                      |     |    |      |    
    //    |                      |     |    |      +-----------+
    //    V                      V     V    V                  V
    TEST ("",                    0,   'a',  "",                0),
    TEST ("",                    5,  '\0',  "<U0>@3<U0>@2",    0),
    TEST ("",                  334,   'x',  "x@334",           0),

    TEST ("<U0>",                0,   'a',  "",                0),
    TEST ("<U0>",                2,   'a',  "<U0>a",           0),
    TEST ("<U0>",                1,   'a',  "<U0>",            0),
    TEST ("<U0>",              128,   'a',  "<U0>a@127",       0),

    TEST ("a",                   0,   'a',  "",                0),
    TEST ("a",                   2,  '\0',  "a<U0>",           0),
    TEST ("a",                 540,   'a',  "a@540",           0),

    TEST ("a<U0>@3b",           10,   'a',  "a<U0>@3baaaaa",   0),
    TEST ("ab<U0>@3",           10,   'a',  "ab<U0>@3aaaaa",   0),
    TEST ("<U0>@3ab",           10,   'a',  "<U0>@3abaaaaa",   0),
    TEST ("a<U0>@3b",            7,  '\0',  "a<U0>@3b<U0>@2",  0),
    TEST ("ab<U0>@3",            7,  '\0',  "ab<U0>@3<U0>@2",  0),
    TEST ("<U0>@3ba",            7,  '\0',  "<U0>@3ba<U0>@2",  0),

    TEST ("a<U0>b<U0>c<U0>@2",   6,   'a',  "a<U0>b<U0>c<U0>", 0),
    TEST ("a<U0>b<U0>c<U0>@2",   5,  '\0',  "a<U0>b<U0>c",     0),
    TEST ("<U0>ba<U0>c<U0>@2",   1,  '\0',  "<U0>",            0),
    TEST ("<U0>ba<U0>c<U0>@2",   0,  '\0',  "",                0),

    TEST ("x@540",             127,   'a',  "x@127",           0),
    TEST ("x@873",             127,   'a',  "x@127",           0),
    TEST ("x@1412",            127,   'a',  "x@127",           0),
    TEST ("x@2284",            127,   'a',  "x@127",           0),

    TEST ("x@127",             128,   'a',  "x@127a",          0),
    TEST ("x@128",             207,   'a',  "x@128a@79",       0),
    TEST ("x@207",             334,   'a',  "x@207a@127",      0),
    TEST ("x@334",             540,   'a',  "x@334a@206",      0),
    TEST ("x@540",             873,   'a',  "x@540a@333",      0),
    TEST ("x@873",            1412,   'a',  "x@873a@539",      0),
    TEST ("x@1412",           2284,   'a',  "x@1412a@872",     0),
    TEST ("x@2284",           3695,   'a',  "x@2284a@1411",    0),

    TEST ("",                 NPOS,   'a',  "",                1),
    TEST ("abc",              NPOS,   'a',  "abc",             1),
    TEST ("x@3695",           NPOS,   'a',  "x@3695",          1),

    TEST ("last",                4,   't',  "last",            0)

};

/**************************************************************************/

// used to exercise
// resize (size_type)
static const StringTestCase
resize_size_test_cases [] = {

#undef TEST
#define TEST(str, size, res, bthrow) {      \
        __LINE__, -1, size, -1, -1, -1,     \
        str, sizeof str - 1, 0, 0,          \
        res, sizeof res - 1, bthrow         \
    }

    //    +------------------------------------------ controlled sequence
    //    |                      +------------------- new size
    //    |                      |    +-------------- expected result sequence
    //    |                      |    |              +-- exception info
    //    |                      |    |              |      0 - no exception
    //    |                      |    |              |      1 - length_error
    //    |                      |    |              |
    //    V                      V    V              V    
    TEST ("",                    0,   "",            0),
    TEST ("",                    5,   "",            0),
    TEST ("",                  334,   "",            0),

    TEST ("<U0>",                0,   "",            0),
    TEST ("<U0>",                2,   "<U0>",        0),
    TEST ("<U0>",                1,   "<U0>",        0),
    TEST ("<U0>",              127,   "<U0>",        0),

    TEST ("a",                   0,   "",            0),
    TEST ("a",                   2,   "a",           0),
    TEST ("a",                 539,   "a",           0),

    TEST ("a<U0>@3b",           10,   "a<U0>@3b",    0),
    TEST ("ab<U0>@3",           10,   "ab<U0>@3",    0),
    TEST ("<U0>@3ab",           10,   "<U0>@3ab",    0),
    TEST ("a<U0>@3b",            7,   "a<U0>@3b",    0),
    TEST ("ab<U0>@3",            7,   "ab<U0>@3",    0),
    TEST ("<U0>@3ba",            7,   "<U0>@3ba",    0),

    TEST ("a<U0>b<U0>c<U0>@2",   5,   "a<U0>b<U0>c", 0),
    TEST ("a<U0>b<U0>c<U0>@2",   4,   "a<U0>b<U0>",  0),
    TEST ("<U0>ba<U0>c<U0>@2",   1,   "<U0>",        0),
    TEST ("<U0>ba<U0>c<U0>@2",   0,   "",            0),

    TEST ("x@540",             127,   "x@127",       0),
    TEST ("x@873",             127,   "x@127",       0),
    TEST ("x@1412",            127,   "x@127",       0),
    TEST ("x@2284",            127,   "x@127",       0),

    TEST ("x@127",             128,   "x@127",       0),
    TEST ("x@128",             207,   "x@128",       0),
    TEST ("x@207",             334,   "x@207",       0),
    TEST ("x@334",             540,   "x@334",       0),
    TEST ("x@540",             873,   "x@540",       0),
    TEST ("x@873",            1412,   "x@873",       0),
    TEST ("x@1412",           2284,   "x@1412",      0),
    TEST ("x@2284",           3695,   "x@2284",      0),

    TEST ("",                 NPOS,   "",            1),
    TEST ("abc",              NPOS,   "abc",         1),
    TEST ("x@3695",           NPOS,   "x@3695",      1),

    TEST ("last",                4,   "last",        0)

};

/**************************************************************************/

// used to exercise
// capacity ()
static const StringTestCase
capacity_void_test_cases [] = {

#undef TEST
#define TEST(str) {                         \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        0, 0, -1                            \
    }

    //    +------------------------------ controlled sequence
    //    |                
    //    |                               
    //    |                               
    //    V  
    TEST (0), 
    TEST (""),   

    TEST ("<U0>"),    
    TEST ("a"),       

    TEST ("test string"), 
    TEST ("a<U0>b<U0>c<U0>@2"),  

    TEST ("x@128"),     
    TEST ("x@207"),    
    TEST ("x@334"),
    TEST ("x@540"),
    TEST ("x@873"),
    TEST ("x@1412"),
    TEST ("x@2284"),   
    TEST ("x@3695"), 
    TEST ("x@4096"), 

    TEST ("last") 
};

/**************************************************************************/

// used to exercise
// reserve (size_type)
static const StringTestCase
reserve_size_test_cases [] = {

#undef TEST
#define TEST(str, size, bthrow) {           \
        __LINE__, -1, size, -1, -1, -1,     \
        str, sizeof str - 1, 0, 0,          \
        0, 0, bthrow                        \
    }

    //    +--------------------------------------- controlled sequence
    //    |                      +---------------- requested capacity
    //    |                      |    +----------- exception info
    //    |                      |    |               0 - no exception
    //    |                      |    |               1 - length_error
    //    |                      |    |    
    //    V                      V    V    
    TEST ("",                    0,   0),
    TEST ("",                    5,   0),
    TEST ("",                  334,   0),

    TEST ("<U0>",                0,   0),
    TEST ("<U0>",                2,   0),
    TEST ("<U0>",                1,   0),
    TEST ("<U0>",              127,   0),

    TEST ("a",                   0,   0),
    TEST ("a",                   2,   0),
    TEST ("a",                 539,   0),

    TEST ("a<U0>@3b",           10,   0),
    TEST ("ab<U0>@3",           10,   0),
    TEST ("<U0>@3ab",           10,   0),
    TEST ("a<U0>b<U0>c<U0>@2",   5,   0),
    TEST ("a<U0>b<U0>c<U0>@2",   4,   0),
    TEST ("<U0>ba<U0>c<U0>@2",   1,   0),
    TEST ("<U0>ba<U0>c<U0>@2",   0,   0),

    TEST ("x@540",             127,   0),
    TEST ("x@873",             127,   0),
    TEST ("x@1412",            127,   0),
    TEST ("x@2284",            127,   0),

    TEST ("x@127",            1412,   0),
    TEST ("x@128",            1412,   0),
    TEST ("x@206",             207,   0),
    TEST ("x@333",             334,   0),
    TEST ("x@540",             540,   0),
    TEST ("x@873",             873,   0),
    TEST ("x@1412",           2284,   0),
    TEST ("x@2284",           3695,   0),

    TEST ("",                 NPOS,   1),
    TEST ("abc",              NPOS,   1),
    TEST ("x@3695",           NPOS,   1),

    TEST ("last",                4,   0)
};

/**************************************************************************/

// used to exercise
// reserve ()
static const StringTestCase
reserve_void_test_cases [] = {

#undef TEST
#define TEST(str) {                      \
        __LINE__, -1, 0, -1, -1, -1,     \
        str, sizeof str - 1, 0, 0,       \
        0, 0, -1                         \
    }

    //    +----------------------------- controlled sequence
    //    |
    //    |                
    //    V               
    TEST (""),
    TEST ("<U0>"),
    TEST ("a"),

    TEST ("a<U0>@3b"),
    TEST ("a<U0>b<U0>c<U0>@2"),

    TEST ("x@127"),
    TEST ("x@128"),
    TEST ("x@206"),
    TEST ("x@333"),
    TEST ("x@540"),
    TEST ("x@873"),
    TEST ("x@1412"),
    TEST ("x@2284"),

    TEST ("last")
};

/**************************************************************************/

// used to exercise
// clear ()
static const StringTestCase
clear_void_test_cases [] = {

#undef TEST
#define TEST(str) {                         \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        "", 0, -1                           \
    }

    //    +------------------------------ controlled sequence
    //    |                
    //    |                               
    //    |                               
    //    V  
    TEST (0), 
    TEST (""),   

    TEST ("<U0>"),    
    TEST ("a"),       

    TEST ("test string"), 
    TEST ("a<U0>b<U0>c<U0>@2"),  

    TEST ("x@128"),     
    TEST ("x@207"),    
    TEST ("x@334"),
    TEST ("x@540"),
    TEST ("x@873"),
    TEST ("x@1412"),
    TEST ("x@2284"),   
    TEST ("x@3695"), 
    TEST ("x@4096"), 

    TEST ("last") 
};

/**************************************************************************/

// used to exercise
// empty ()
static const StringTestCase
empty_void_test_cases [] = {

#undef TEST
#define TEST(str, res) {                 \
        __LINE__, -1, -1, -1, -1, -1,    \
        str, sizeof str - 1, 0, 0,       \
        0, res, -1                       \
    }

    //    +------------------------------ controlled sequence
    //    |              +--------------- expected result              
    //    |              |                     
    //    |              |                     
    //    V              V
    TEST (0,             1), 
    TEST ("",            1),   

    TEST ("<U0>",        0),    
    TEST ("a",           0),       
    TEST ("<U0>ab<U0>c", 0),  

    TEST ("x@128",       0),     
    TEST ("x@3695",      0), 

    TEST ("last",        0) 
};

/**************************************************************************/
template <class charT, class Traits, class Allocator>
void test_capacity (charT, Traits*, Allocator*,
                    const StringFunc     &func,
                    const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename UserTraits<charT>::MemFun           UTMemFun;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);

    static charT wres_buf [BUFSIZE];
    std::size_t res_len = sizeof wres_buf / sizeof *wres_buf;
    charT* wres = rw_expand (wres_buf, tcase.res, tcase.nres, &res_len);

    // construct the string object to be modified and constant one
    String str (wstr, str_len);
    const String cstr (wstr, str_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    wstr = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    const charT arg_val = -1 != tcase.val ? 
        make_char (char (tcase.val), (charT*)0)
      : make_char (char (), (charT*)0);

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

        std::size_t res = 0;

        try {

            switch (func.which_) {

            case Size (void):
                res = cstr.size ();
                break;

            case Length (void):
                res = cstr.length ();
                break;

            case MaxSize (void):
                res = cstr.max_size ();
                break;

            case Resize (size_val):
                str.resize (tcase.size, arg_val);
                break;

            case Resize (size):
                str.resize (tcase.size);
                break;

            case Capacity (void):
                res = cstr.capacity ();
                break;

            case Reserve (size):
                str.reserve (tcase.size);
                break;

            case Reserve (void):
                str.reserve ();
                break;

            case Clear (void):
                str.clear ();
                break;

            case Empty (void):
                res = cstr.empty () ? 1 : 0;
                break;

            default:
                RW_ASSERT ("test logic error: unknown capacity overload");
                return;
            }

            // verify the returned value
            if (func.which_ == StringIds::size_void 
                || func.which_ == StringIds::length_void
                || func.which_ == StringIds::empty_void) {
                    rw_assert (res == tcase.nres, 0, tcase.line,
                               "line %d. %{$FUNCALL} expected %zu, got %zu",
                                __LINE__, tcase.nres, res);
            }

            if (func.which_ == StringIds::capacity_void) {
                std::size_t cur_sz = cstr.size ();
                std::size_t max_sz = cstr.max_size ();

                rw_assert (cur_sz <= res && res <= max_sz, 0, tcase.line,
                           "line %d. %{$FUNCALL} == %zu, "
                           "expected %zu < res < %zu", 
                           __LINE__, res, cur_sz, max_sz);
            }

            if (func.which_ == StringIds::max_size_void) {
                std::size_t cur_sz = cstr.size ();

                rw_assert (cur_sz <= res, 0, tcase.line,
                           "line %d. %{$FUNCALL} == %zu, expected res > %zu",
                           __LINE__, res, cur_sz);
            }

            if (func.which_ == StringIds::reserve_size
                || func.which_ == StringIds::reserve_void) {

                    res = str.capacity ();
                    rw_assert (std::size_t (tcase.size) <= res, 0, tcase.line,
                               "line %d. %{$FUNCALL} : capacity() >= "
                               "%zu, got %zu", __LINE__, tcase.size, res);
            }

            if (func.which_ == StringIds::resize_size_val 
                || func.which_ == StringIds::clear_void) {

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
            }

            if (func.which_ == StringIds::resize_size) {

                std::size_t match =
                    rw_match (tcase.res, str.data (), res_len);

                rw_assert (match == res_len, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{#*s}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, int (tcase.nres), tcase.res,
                           int (sizeof (charT)), int (str.size ()),
                           str.c_str (), match);

                for (std::size_t tmp = res_len; tmp < str.size (); tmp++) {

                    char c = char ();
                    match = rw_match (&c, str.data () + tmp, 1);
                    rw_assert (1 == match, 0, tcase.line,
                               "line %d. %{$FUNCALL} expected %c "
                               "at offset %zu, got %{#c}",
                               __LINE__, c, tmp, str.data () + tmp);
                }
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
      ? str_state.capacity_ < str.capacity () : 0;

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

DEFINE_STRING_TEST_DISPATCH (test_capacity);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(gsig, sig) {                                       \
        gsig, sig ## _test_cases,                               \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (StringIds::size_void, size_void),
        TEST (StringIds::length_void, length_void),
        TEST (StringIds::max_size_void, max_size_void),
        TEST (StringIds::resize_size_val, resize_size_val),
        TEST (StringIds::resize_size, resize_size),
        TEST (StringIds::capacity_void, capacity_void),
        TEST (StringIds::reserve_size, reserve_size),
        TEST (StringIds::reserve_void, reserve_void),
        TEST (StringIds::clear_void, clear_void),
        TEST (StringIds::empty_void, empty_void)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.capacity",
                               test_capacity, tests, test_count);
}


