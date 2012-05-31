/************************************************************************
 *
 * 0.strcmp.cpp - test exercising the rw_strcmp() utility functions
 *
 * $Id: 0.strncmp.cpp 430542 2006-08-10 21:33:38Z sebor $
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
 * Copyright 2004-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <valcmp.h>   // for rw_strncmp()

#include <stdio.h>    // for fprintf()

/***********************************************************************/

static int
run_test (int, char*[])
{
    int exit_status = 0;

    // rw_strncmp():
    //
    // compares up to a maximum number of characters from the two strings
    // posisbly including any embedded NULs (when the CMP_NULTERM bit is
    // clear) and returns -1, 0, or +1 if the first string compares less,
    // equal, or greater, respectively, than the second string, or the
    // offset of the first mismatched character (when the CMP_RETOFF bit
    // is set) or an out of bounds value such as -1 when no such character
    // exists
    //
    // rw_strncmp(s1, s2) is equivalent to a call to strcmp(s1, s2) when
    // the type of s1 and s2 is char*, wcscmp(s1, s2) when the type is
    // wchar_t*
    //
    // when (N != SIZE_MAX) is true,
    // rw_strncmp(s1, s2, N, CMP_NULTERM) is equivalent to a call to
    // strncmp(s1, s2, N) or wcsncmp(s1, s2, N), respectively
    //
    // rw_strncmp(s1, s2, N) is equivalent to a call to memcmp(s1, s2, N)
    // or wmemcmp(s1, s2, n), respectively

#undef TEST
#define TEST(expect, s1, s2, nc, fl)                                      \
    do {                                                                  \
        const int result = (fl) < 0 ?                                     \
            rw_strncmp (s1, s2, nc) : rw_strncmp (s1, s2, nc, fl);        \
        if (expect != result) {                                           \
           exit_status = 2;                                               \
           fprintf (stderr, "line %d: rw_strncmp(%p, %p, %u, %d) == %d, " \
                            "got %d\n", __LINE__, (const void*)s1,        \
                            (const void*)s2, unsigned (nc), fl, expect,   \
                            result);                                      \
        }                                                                 \
    } while (0)

    //////////////////////////////////////////////////////////////////
    printf ("rw_strncmp(const char*, const char*, size_t, int)\n");

    //     +------------------ expected result
    //     |  +--------------- first string
    //     |  |   +----------- second string
    //     |  |   |    +------ value of N
    //     |  |   |    |   +-- value of flags, or none when -1
    //     |  |   |    |   |
    //     v  v   v    v   v
    TEST ( 0, "", "",  _RWSTD_SIZE_MAX, -1);
    TEST (+1, "a", "", _RWSTD_SIZE_MAX, -1);
    TEST (-1, "", "a", _RWSTD_SIZE_MAX, -1);

    TEST ( 0, "a",    "b",    0, -1);
    TEST ( 0, "a\0b", "a\0b", 3, -1);
    TEST ( 0, "a\0c", "a\0b", 3, -1);
    TEST ( 0, "a\0b", "a\0c", 3, -1);

    TEST ( 0, "a\0b", "a\0b", 3, CMP_NULTERM);
    TEST (+1, "a\0c", "a\0b", 3, 0);
    TEST (-1, "a\0b", "a\0c", 3, 0);

    TEST (-1, "a\0b", "a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, "a\0c", "a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, "a\0b", "a\0c", 3, CMP_NULTERM | CMP_RETOFF);

    TEST (-1, "a\0b", "a\0b", 3, CMP_RETOFF);
    TEST ( 2, "a\0c", "a\0b", 3, CMP_RETOFF);
    TEST ( 2, "a\0b", "a\0c", 3, CMP_RETOFF);

#ifndef _RWSTD_NO_WCHAR_T

    //////////////////////////////////////////////////////////////////
    printf ("rw_strncmp(const wchar_t*, const wchar_t*, size_t, int)\n");

    TEST ( 0, L"",  L"",  ~0, -1);
    TEST (+1, L"a", L"",  ~0, -1);
    TEST (-1, L"",  L"a", ~0, -1);

    TEST ( 0, L"a",    L"b",    0, -1);
    TEST ( 0, L"a\0b", L"a\0b", 3, -1);
    TEST ( 0, L"a\0c", L"a\0b", 3, -1);
    TEST ( 0, L"a\0b", L"a\0c", 3, -1);

    TEST ( 0, L"a\0b", L"a\0b", 3, 0);
    TEST (+1, L"a\0c", L"a\0b", 3, 0);
    TEST (-1, L"a\0b", L"a\0c", 3, 0);

    TEST (-1, L"a\0b", L"a\0b", 3, CMP_RETOFF);
    TEST (+2, L"a\0c", L"a\0b", 3, CMP_RETOFF);
    TEST (+2, L"a\0b", L"a\0c", 3, CMP_RETOFF);

    TEST (-1, L"a\0b", L"a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, L"a\0c", L"a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, L"a\0b", L"a\0c", 3, CMP_NULTERM | CMP_RETOFF);

    //////////////////////////////////////////////////////////////////
    printf ("rw_strncmp(const wchar_t*, const char*, size_t, int)\n");

    TEST ( 0, L"", "",  ~0, -1);
    TEST (+1, L"a", "", ~0, -1);
    TEST (-1, L"", "a", ~0, -1);

    TEST ( 0, L"a",    "b",    0, -1);
    TEST ( 0, L"a\0b", "a\0b", 3, -1);
    TEST ( 0, L"a\0c", "a\0b", 3, -1);
    TEST ( 0, L"a\0b", "a\0c", 3, -1);

    TEST ( 0, L"a\0b", "a\0b", 3, CMP_NULTERM);
    TEST ( 0, L"a\0c", "a\0b", 3, CMP_NULTERM);
    TEST ( 0, L"a\0b", "a\0c", 3, CMP_NULTERM);

    TEST (-1, L"a\0b", "a\0b", 3, CMP_RETOFF);
    TEST ( 2, L"a\0c", "a\0b", 3, CMP_RETOFF);
    TEST ( 2, L"a\0b", "a\0c", 3, CMP_RETOFF);

    TEST (-1, L"a\0b", "a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, L"a\0c", "a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, L"a\0b", "a\0c", 3, CMP_NULTERM | CMP_RETOFF);

    //////////////////////////////////////////////////////////////////
    printf ("rw_strncmp(const char*, const wchar_t*, size_t, int)\n");

    TEST ( 0, "", L"",  ~0, -1);
    TEST (+1, "a", L"", ~0, -1);
    TEST (-1, "", L"a", ~0, -1);

    TEST ( 0, "a",    L"b",    0, -1);
    TEST ( 0, "a\0b", L"a\0b", 3, -1);
    TEST ( 0, "a\0c", L"a\0b", 3, -1);
    TEST ( 0, "a\0b", L"a\0c", 3, -1);

    TEST ( 0, "a\0b", L"a\0b", 3, CMP_NULTERM);
    TEST ( 0, "a\0c", L"a\0b", 3, CMP_NULTERM);
    TEST ( 0, "a\0b", L"a\0c", 3, CMP_NULTERM);

    TEST (-1, "a\0b", L"a\0b", 3, CMP_RETOFF);
    TEST ( 2, "a\0c", L"a\0b", 3, CMP_RETOFF);
    TEST ( 2, "a\0b", L"a\0c", 3, CMP_RETOFF);

    TEST (-1, "a\0b", L"a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, "a\0c", L"a\0b", 3, CMP_NULTERM | CMP_RETOFF);
    TEST (-1, "a\0b", L"a\0c", 3, CMP_NULTERM | CMP_RETOFF);

#endif   // _RWSTD_NO_WCHAR_T

    return exit_status;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    return run_test (argc, argv);
}
