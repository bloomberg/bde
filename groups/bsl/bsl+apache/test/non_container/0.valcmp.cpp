/************************************************************************
 *
 * valcmp.cpp - test exercising the rw_valcmp() family
 *              of utility functions
 *
 * $Id: 0.valcmp.cpp 475989 2006-11-16 23:48:45Z sebor $
 *
 ************************************************************************
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <valcmp.h>

#include <stdio.h>   // for fprintf, size_t


// the exit status of the whole test
static int exit_status;


template <class T, class U>
void
test_case (const T* /* dummy */, const U* /* dummy */,
           const char *tname, const char *uname,
           int line, int expect,
           const char *str1, const char *str2, size_t nelems, int flags)
{
    T buf1 [256] = { T () };
    U buf2 [256] = { U () };

    typedef unsigned char UChar;

    for (size_t i = 0; i != nelems; ++i) {
        buf1 [i] = T (_RWSTD_STATIC_CAST (UChar, str1 [i])); 
        buf2 [i] = U (_RWSTD_STATIC_CAST (UChar, str2 [i]));
   }

    const int result = rw_valcmp (buf1, buf2, nelems, flags);

    if (result != expect) {
        fprintf (stderr,
                 "line %d: %d == rw_valcmp(const %s* = %p, const %s* = %p, "
                 "%u, %d), got %d\n",
                 line, expect, tname, (void*)buf1, uname, (void*)buf2,
                 unsigned (nelems), flags, result);
        exit_status = 1;
    }
}


struct Case {
    int         line;
    int         expect;
    const char *str1;
    const char *str2;
    size_t      nelems;
    int         flags;
};

extern const Case cases [] = {

#define CASE(expect, str1, str2, nelems, flags) \
        { __LINE__, expect, str1, str2, nelems, flags }

    CASE ( 0, 0,  0,   0, 0),

    CASE ( 0, "", "",  0, 0),
    CASE ( 0, "", "",  1, 0),


    CASE ( 0, "a", "\0", 0, 0),
    CASE (+1, "a", "\0", 1, 0),
    CASE (+1, "a", "\0", 2, 0),

    CASE ( 0, "\0", "a", 0, 0),
    CASE (-1, "\0", "a", 1, 0),
    CASE (-1, "\0", "a", 2, 0),


    CASE ( 0, "ab", "\0\0", 0, 0),
    CASE (+1, "ab", "\0\0", 1, 0),
    CASE (+1, "ab", "\0\0", 2, 0),
    CASE (+1, "ab", "\0\0", 3, 0),

    CASE ( 0, "\0\0", "ab", 0, 0),
    CASE (-1, "\0\0", "ab", 1, 0),
    CASE (-1, "\0\0", "ab", 2, 0),
    CASE (-1, "\0\0", "ab", 3, 0),


    CASE ( 0, "ab",  "a\0", 0, 0),
    CASE ( 0, "ab",  "a\0", 1, 0),
    CASE (+1, "ab",  "a\0", 2, 0),
    CASE (+1, "ab",  "a\0", 3, 0),

    CASE ( 0, "a\0", "ab",  0, 0),
    CASE ( 0, "a\0", "ab",  1, 0),
    CASE (-1, "a\0", "ab",  2, 0),
    CASE (-1, "a\0", "ab",  3, 0),


    CASE ( 0, "\0ab", "\0a\0", 0, 0),
    CASE ( 0, "\0ab", "\0a\0", 1, 0),
    CASE ( 0, "\0ab", "\0a\0", 2, 0),
    CASE (+1, "\0ab", "\0a\0", 3, 0),
    CASE (+1, "\0ab", "\0a\0", 4, 0),

    CASE ( 0, "\0a\0", "\0ab", 0, 0),
    CASE ( 0, "\0a\0", "\0ab", 1, 0),
    CASE ( 0, "\0a\0", "\0ab", 2, 0),
    CASE (-1, "\0a\0", "\0ab", 3, 0),
    CASE (-1, "\0a\0", "\0ab", 4, 0),


    CASE ( 0, "abc",  "aB\0", 0, 0),
    CASE ( 0, "abc",  "aB\0", 1, 0),
    CASE (+1, "abc",  "aB\0", 2, 0),
    CASE (+1, "abc",  "aB\0", 3, 0),
    CASE (+1, "abc",  "aB\0", 4, 0),

    CASE ( 0, "aB\0", "abc",  0, 0),
    CASE ( 0, "aB\0", "abc",  1, 0),
    CASE (-1, "aB\0", "abc",  2, 0),
    CASE (-1, "aB\0", "abc",  3, 0),
    CASE (-1, "aB\0", "abc",  4, 0),


    CASE ( 0, "a\0b\0c\0d", "a\0b\0c\0R", 0, 0),
    CASE ( 0, "a\0b\0c\0e", "a\0b\0c\0S", 1, 0),
    CASE ( 0, "a\0b\0c\0f", "a\0b\0c\0T", 2, 0),
    CASE ( 0, "a\0b\0c\0g", "a\0b\0c\0U", 3, 0),
    CASE ( 0, "a\0b\0c\0h", "a\0b\0c\0V", 4, 0),
    CASE ( 0, "a\0b\0c\0i", "a\0b\0c\0W", 5, 0),
    CASE ( 0, "a\0b\0c\0j", "a\0b\0c\0X", 6, 0),
    CASE (+1, "a\0b\0c\0k", "a\0b\0c\0Y", 7, 0),
    CASE (+1, "a\0b\0c\0l", "a\0b\0c\0Z", 8, 0),

    CASE ( 0, "a\0b\0c\0R", "a\0b\0c\0d", 0, 0),
    CASE ( 0, "a\0b\0c\0S", "a\0b\0c\0d", 1, 0),
    CASE ( 0, "a\0b\0c\0T", "a\0b\0c\0d", 2, 0),
    CASE ( 0, "a\0b\0c\0U", "a\0b\0c\0d", 3, 0),
    CASE ( 0, "a\0b\0c\0V", "a\0b\0c\0d", 4, 0),
    CASE ( 0, "a\0b\0c\0W", "a\0b\0c\0d", 5, 0),
    CASE ( 0, "a\0b\0c\0X", "a\0b\0c\0d", 6, 0),
    CASE (-1, "a\0b\0c\0Y", "a\0b\0c\0d", 7, 0),
    CASE (-1, "a\0b\0c\0Z", "a\0b\0c\0d", 8, 0),


    CASE ( 0, "a\0b\0c\0R", "a\0b\0c\0d", 0, CMP_NOCASE),
    CASE ( 0, "a\0b\0c\0S", "a\0b\0c\0e", 1, CMP_NOCASE),
    CASE ( 0, "a\0b\0c\0T", "a\0b\0c\0f", 2, CMP_NOCASE),
    CASE ( 0, "a\0b\0c\0U", "a\0b\0c\0g", 3, CMP_NOCASE),
    CASE ( 0, "a\0b\0c\0V", "a\0b\0c\0h", 4, CMP_NOCASE),
    CASE ( 0, "a\0b\0c\0W", "a\0b\0c\0i", 5, CMP_NOCASE),
    CASE ( 0, "a\0b\0c\0X", "a\0b\0c\0j", 6, CMP_NOCASE),
    CASE (+1, "a\0b\0c\0Y", "a\0b\0c\0k", 7, CMP_NOCASE),
    CASE (+1, "a\0b\0c\0Z", "a\0b\0c\0l", 8, CMP_NOCASE),

    CASE ( 0, "A\0b\0c\0d", "a\0B\0c\0R", 0, CMP_NOCASE),
    CASE ( 0, "A\0b\0c\0e", "a\0B\0c\0S", 1, CMP_NOCASE),
    CASE ( 0, "A\0b\0c\0f", "a\0B\0c\0T", 2, CMP_NOCASE),
    CASE ( 0, "A\0b\0c\0g", "a\0B\0c\0U", 3, CMP_NOCASE),
    CASE ( 0, "A\0b\0c\0h", "a\0B\0c\0V", 4, CMP_NOCASE),
    CASE ( 0, "A\0b\0c\0i", "a\0B\0c\0W", 5, CMP_NOCASE),
    CASE ( 0, "A\0b\0c\0j", "a\0B\0c\0X", 6, CMP_NOCASE),
    CASE (-1, "A\0b\0c\0k", "a\0B\0c\0Y", 7, CMP_NOCASE),
    CASE (-1, "A\0b\0c\0l", "a\0B\0c\0Z", 8, CMP_NOCASE),


    CASE ( 0, "Abc", "aB\0", 0, CMP_NOCASE),
    CASE ( 0, "Abc", "aB\0", 1, CMP_NOCASE),
    CASE ( 0, "Abc", "aB\0", 2, CMP_NOCASE),
    CASE (+1, "Abc", "aB\0", 3, CMP_NOCASE),
    CASE (+1, "Abc", "aB\0", 4, CMP_NOCASE),

    CASE ( 0, "aB\0", "Abc", 0, CMP_NOCASE),
    CASE ( 0, "aB\0", "Abc", 1, CMP_NOCASE),
    CASE ( 0, "aB\0", "Abc", 2, CMP_NOCASE),
    CASE (-1, "aB\0", "Abc", 3, CMP_NOCASE),
    CASE (-1, "aB\0", "Abc", 4, CMP_NOCASE),


    CASE ( 0, "abcd", "abc\0", 0, CMP_NULTERM),
    CASE ( 0, "abcd", "abc\0", 1, CMP_NULTERM),
    CASE ( 0, "abcd", "abc\0", 2, CMP_NULTERM),
    CASE ( 0, "abcd", "abc\0", 3, CMP_NULTERM),
    CASE (+1, "abcd", "abc\0", 4, CMP_NULTERM),
    CASE (+1, "abcd", "abc\0", 5, CMP_NULTERM),

    CASE ( 0, "abc\0", "abcd", 0, CMP_NULTERM),
    CASE ( 0, "abc\0", "abcd", 1, CMP_NULTERM),
    CASE ( 0, "abc\0", "abcd", 2, CMP_NULTERM),
    CASE ( 0, "abc\0", "abcd", 3, CMP_NULTERM),
    CASE (-1, "abc\0", "abcd", 4, CMP_NULTERM),
    CASE (-1, "abc\0", "abcd", 5, CMP_NULTERM)
};


template <class T, class U>
void
test_all_cases (const T* /* dummy */, const U* /* dummy */,
                const char *tname, const char *uname)
{
    // print out the name of the specialization being tested
    static const int one_time_per_specialization = 
        printf ("rw_valcmp(const %s*, const %s*, size_t, int)\n",
                tname, uname);

    _RWSTD_UNUSED (one_time_per_specialization);

    for (size_t i = 0; i != sizeof cases / sizeof cases; ++i) {
        test_case ((T*)0, (U*)0,
                   tname, uname,
                   cases [i].line,
                   cases [i].expect,
                   cases [i].str1,
                   cases [i].str2,
                   cases [i].nelems,
                   cases [i].flags);
    }
}


#define STR(x)   #x


template <class T>
void
run_test (const T* /* dummy */, const char *tname)
{
#define TEST_ALL_CASES(U)   test_all_cases ((T*)0, (U*)0, tname, STR (U))

    // run all test cases for all integral types except bool
    TEST_ALL_CASES (char);

    TEST_ALL_CASES (signed char);
    TEST_ALL_CASES (unsigned char);
    TEST_ALL_CASES (short);
    TEST_ALL_CASES (unsigned short);
    TEST_ALL_CASES (int);
    TEST_ALL_CASES (unsigned int);
    TEST_ALL_CASES (long);
    TEST_ALL_CASES (unsigned long);

#ifdef _RWSTD_LONG_LONG

    TEST_ALL_CASES (_RWSTD_LONG_LONG);
    TEST_ALL_CASES (unsigned _RWSTD_LONG_LONG);

#endif   // _RWSTD_LONG_LONG

#ifndef _RWSTD_NO_WCHAR_T

    TEST_ALL_CASES (wchar_t);

#endif   // _RWSTD_NO_WCHAR_T
}


int main ()
{
#define RUN_TEST(T) run_test ((T*)0, STR (T))

    // run the for all integral types except bool
    RUN_TEST (char);
    RUN_TEST (signed char);
    RUN_TEST (unsigned char);
    RUN_TEST (short);
    RUN_TEST (unsigned short);
    RUN_TEST (int);
    RUN_TEST (unsigned int);
    RUN_TEST (long);
    RUN_TEST (unsigned long);

#ifdef _RWSTD_LONG_LONG

    RUN_TEST (_RWSTD_LONG_LONG);
    RUN_TEST (unsigned _RWSTD_LONG_LONG);

#endif   // _RWSTD_LONG_LONG

#ifndef _RWSTD_NO_WCHAR_T

    RUN_TEST (wchar_t);

#endif   // _RWSTD_NO_WCHAR_T

    return exit_status;
}
