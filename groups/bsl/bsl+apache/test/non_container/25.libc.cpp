/***************************************************************************
 *
 * 25.libc.cpp - test exercising 25.4 [lib.alg.c.library]
 *
 * $Id: 25.libc.cpp 510970 2007-02-23 14:57:45Z faridz $
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
 * Copyright 1994-2006 Rogue Wave Software.
 *
 ***************************************************************************/

#include <cstdlib>      // for bsearch, qsort
#include <csignal>      // for signal
#include <csetjmp>      // for setjmp, longjmp

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

// used as a special value in comp below
static int global[] = { 0, 0 };


extern "C" {

    static std::jmp_buf jmp_env;

    // SIGABRT handler
    static void handle_ABRT (int)
    {
        // jump outta here to prevent abort() from trying too hard...
        std::longjmp (jmp_env, 1);
    }


    static int c_comp (const void *x, const void *y)
    {
#ifndef _RWSTD_NO_EXCEPTIONS

        // verify that a thrown exception can be caught
        if (x >= global && x < global + sizeof global / sizeof *global)
            throw x;

#endif   // _RWSTD_NO_EXCEPTIONS

        return   (*_RWSTD_STATIC_CAST (const UserClass*, x)).data_.val_
               - (*_RWSTD_STATIC_CAST (const UserClass*, y)).data_.val_;
    }
}

extern "C++" {

    static int cxx_comp (const void *x, const void *y)
    {
#ifndef _RWSTD_NO_EXCEPTIONS

        // verify that a thrown exception can be caught
        if (x >= global && x < global + sizeof global / sizeof *global)
            throw x;

#endif   // _RWSTD_NO_EXCEPTIONS

        return   (*_RWSTD_STATIC_CAST (const UserClass*, x)).data_.val_
               - (*_RWSTD_STATIC_CAST (const UserClass*, y)).data_.val_;
    }
}

/**************************************************************************/

// exrcises std::qqsort (25.4.4)
static void
test_qsort (int          line,
            const char  *src,
            std::size_t  nsrc,
            bool         cxx)
{
    UserClass* const xsrc = UserClass::from_char (src, nsrc);
    UserClass* const xsrc_end = xsrc + nsrc;
    RW_ASSERT (0 == nsrc || 0 != xsrc);

    if (cxx)
        std::qsort (xsrc, nsrc, sizeof *xsrc, cxx_comp);
    else
        std::qsort (xsrc, nsrc, sizeof *xsrc, c_comp);

    bool success = is_sorted_lt (xsrc, xsrc_end);
    rw_assert (success, 0, line,
               "line %d: extern \"C%{?}++%{;}\" qsort (\"%s\", ...) ==> "
               "\"%{X=*.*}\" not sorted",
               __LINE__, cxx, src, int (nsrc), -1, xsrc);

    delete[] xsrc;
}

/**************************************************************************/

// exrcises std::qsort (25.4.4 Note)
static void
test_qsort_exception (int          line,
                      int         *src,
                      std::size_t  nsrc,
                      bool         cxx)
{
#ifndef _RWSTD_NO_EXCEPTIONS

    // install a SIGABRT handler in case libc can't throw
    // exceptions and aborts instead (e.g., glibc/gcc)
    std::signal (SIGABRT, handle_ABRT);

    bool success = false;
    try {
        if (0 == setjmp (jmp_env)) {
            if (cxx)
                std::qsort (src, nsrc, sizeof *src, cxx_comp);
            else
                std::qsort (src, nsrc, sizeof *src, c_comp);
        }
        else {
            // prevent double assertion
            success = true;
            rw_assert (false, 0, line,
                       "line %d: extern \"C%{?}++%{;}\" qsort() aborted "
                       "on exception", __LINE__, cxx);
        }
    }
    catch (const void* x) {
        success = x >= src && x < src + nsrc;
    }
    catch (...) {
    }

    rw_assert (success, 0, line,
               "line %d: extern \"C%{?}++%{;}\" qsort() failed to propagate "
               "exception", __LINE__, cxx);

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (line);
    _RWSTD_UNUSED (src);
    _RWSTD_UNUSED (nsrc);
    _RWSTD_UNUSED (key);
    _RWSTD_UNUSED (cxx);

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

// exrcises std::bsearch (25.4.3)
static void
test_bsearch (int          line,
              const char  *src,
              std::size_t  nsrc,
              const char   key_val,
              std::size_t  res,
              bool         cxx)
{
    UserClass* const xsrc = UserClass::from_char (src, nsrc,
                                                  true); // must be sorted
    RW_ASSERT (0 == nsrc || 0 != xsrc);

    UserClass key;
    key.data_.val_ = key_val;

    const void* result = cxx ?
        std::bsearch (&key, xsrc, nsrc, sizeof *xsrc, cxx_comp)
      : std::bsearch (&key, xsrc, nsrc, sizeof *xsrc, c_comp);

    const UserClass* exp_res = res == _RWSTD_SIZE_MAX ? 0 : xsrc + res;

    bool success = result == exp_res;
    rw_assert (success, 0, line,
               "line %d: extern \"C%{?}++%{;}\" bsearch (\"%s\", %#c, ...) "
               "== %p, got %p, difference is %td elements",
               __LINE__, cxx, src, key_val, result, exp_res,
               _RWSTD_STATIC_CAST (const UserClass*, result) - exp_res);

    delete[] xsrc;
}

/**************************************************************************/

// exrcises std::bsearch (25.4.4 Note)
static void
test_bsearch_exception (int         line,
                        const int  *src,
                        std::size_t nsrc,
                        const int  *key,
                        bool        cxx)
{
#ifndef _RWSTD_NO_EXCEPTIONS

    // install a SIGABRT handler in case libc can't throw
    // exceptions and aborts instead (e.g., glibc/gcc)
    std::signal (SIGABRT, handle_ABRT);

    bool success = false;
    try {
        if (0 == setjmp (jmp_env)) {
            if (cxx)
                std::bsearch (key, src, nsrc, sizeof *src, cxx_comp);
            else
                std::bsearch (key, src, nsrc, sizeof *src, c_comp);
        }
        else {
            // prevent double assertion
            success = true;
            rw_assert (false, 0, line,
                       "line %d: extern \"C%{?}++%{;}\" bsearch() aborted "
                       "on exception", __LINE__, cxx);
        }
    }
    catch (const void* x) {
        success = x >= src && x < src + nsrc;
    }
    catch (...) {
    }

    rw_assert (success, 0, line,
               "line %d: extern \"C%{?}++%{;}\" bsearch() failed to propagate "
               "exception", __LINE__, cxx);

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    _RWSTD_UNUSED (line);
    _RWSTD_UNUSED (src);
    _RWSTD_UNUSED (nsrc);
    _RWSTD_UNUSED (key);
    _RWSTD_UNUSED (cxx);

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

static int rw_opt_no_c;                   // --no-c
static int rw_opt_no_cxx;                 // --no-cpp
static int rw_opt_no_bsearch;             // --no-bsearch
static int rw_opt_no_qsort;               // --no-qsort
static int rw_opt_no_exceptions;          // --no-exceptions

/**************************************************************************/

static void
test_qsort (bool cxx)
{
    rw_info (0, 0, 0,
             "extern \"C%{?}++%{;}\" qsort (void*, size_t, size_t, "
             "int (*compar)(const void*, const void*))", cxx);

#define TEST(src)                                              \
    test_qsort (__LINE__, src, sizeof src - 1, cxx)

    TEST ("a");

    TEST ("ba");
    TEST ("cba");
    TEST ("dcba");
    TEST ("edcba");
    TEST ("fedcba");
    TEST ("gfedcba");
    TEST ("hgfedcba");
    TEST ("ihgfedcba");
    TEST ("jihgfedcba");

    TEST ("ab");
    TEST ("abc");
    TEST ("abcd");
    TEST ("abcde");
    TEST ("abcdef");
    TEST ("abcdefg");
    TEST ("abcdefgh");
    TEST ("abcdefghi");
    TEST ("abcdefghij");

    TEST ("aa");
    TEST ("aabb");
    TEST ("bbccaa");
    TEST ("ddbbccaa");
    TEST ("ddeebbccaa");

    TEST ("aaaaaaaaaa");
    TEST ("ababababab");
    TEST ("bababababa");

#undef TEST

    if (rw_opt_no_exceptions) {
        rw_note (0, 0, 0, "extern \"C%{?}++%{;}\" qsort() exceptions tests "
                 "disabled", cxx);
    }
    else {
        test_qsort_exception (__LINE__, global,
                              sizeof global / sizeof *global, cxx);
    }
}

/**************************************************************************/

static void
test_bsearch (bool cxx)
{
    rw_info (0, 0, 0,
             "extern \"C%{?}++%{;}\" bsearch (const void*, const void*, "
             "size_t, size_t, int (*compar)(const void*, const void*))",
             cxx);

#define TEST(src, key, res)                              \
     test_bsearch (__LINE__, src, sizeof src - 1, key,   \
                   std::size_t (res), cxx)

    TEST ("",  'a', -1);
    TEST ("a", 'a',  0);
    TEST ("a", 'b', -1);
    TEST ("b", 'a', -1);

    TEST ("ab", 'a',  0);
    TEST ("ac", 'a',  0);
    TEST ("ab", 'b',  1);
    TEST ("bc", 'a', -1);

    TEST ("acegi", 'i',  4);
    TEST ("acegi", 'b', -1);
    TEST ("acegi", 'g',  3);
    TEST ("acegi", 'f', -1);
    TEST ("acegi", 'e',  2);
    TEST ("acegi", 'j', -1);
    TEST ("acegi", 'c',  1);
    TEST ("bdfhj", 'a', -1);

    TEST ("abcdefghij", 'a',  0);
    TEST ("abcdefghij", 'c',  2);
    TEST ("abcdefghij", 'f',  5);
    TEST ("abcdefghij", 'h',  7);
    TEST ("abcdefghij", 'j',  9);

#undef TEST

    if (rw_opt_no_exceptions) {
        rw_note (0, 0, 0, "extern \"C%{?}++%{;}\" bsearch() exceptions "
                 "tests disabled", cxx);
    }
    else {
        test_bsearch_exception (__LINE__, global,
                                sizeof global / sizeof *global, global, cxx);
    }
}

/**************************************************************************/

static void
test_libc (bool cxx)
{
    if (rw_opt_no_qsort)
        rw_note (0, 0, 0, "qsort test disabled");
    else
        test_qsort (cxx);

    if (rw_opt_no_bsearch)
        rw_note (0, 0, 0, "bsearch test disabled");
    else
        test_bsearch (cxx);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if (rw_opt_no_c)
        rw_note (0, 0, 0, "tests of extern \"C\" cfunctions disabled");
    else
        test_libc (false);

    if (rw_opt_no_cxx)
        rw_note (0, 0, 0, "tests of extern \"C++\" functions disabled");
    else
        test_libc (true);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.c.library",
                    0 /* no comment */,
                    run_test,
                    "|-no-extern_c# "
                    "|-no-extern_cxx# "
                    "|-no-bsearch# "
                    "|-no-qsort# "
                    "|-no-exceptions",
                    &rw_opt_no_c,
                    &rw_opt_no_cxx,
                    &rw_opt_no_bsearch,
                    &rw_opt_no_qsort,
                    &rw_opt_no_exceptions);
}
