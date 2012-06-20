#include <bsls_platform.h>
#if 1 == BSLS_PLATFORM__CMP_SUN && !defined(BSLS_PLATFORM__OS_WINDOWS)
int main() { return 0; }
#else
/***************************************************************************
 *
 * 21.string.find.cpp - string test exercising lib.string.find
 *
 * $Id: 21.string.find.cpp 590052 2007-10-30 12:44:14Z faridz $
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

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_assert()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define Find(sig)                 StringIds::find_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// find (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res) {                   \
        __LINE__, -1, -1, -1, -1, -1,           \
        str, sizeof str - 1,                    \
        arg,  sizeof arg - 1, 0, res, 0         \
    }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |                +---- expected result
    //    |             |                |
    //    |             |                |
    //    V             V                V
    TEST ("",           "",              0),
    TEST ("",           "a",          NPOS),
    TEST ("",           "\0",            0),

    TEST ("ab",         "a",             0),

    TEST ("\0",         "",              0),
    TEST ("\0",         "\0",            0),
    TEST ("\0",         "a",          NPOS),

    TEST ("aababcabcd", "",              0),
    TEST ("aababcabcd", "a",             0),
    TEST ("aababcabcd", "ab",            1),
    TEST ("aababcabcd", "abc",           3),
    TEST ("aababcabcd", "abcd",          6),
    TEST ("aababcabcd", "abcde",      NPOS),

    TEST ("bbcdefghij", "a",          NPOS),
    TEST ("abcdefghij", "a",             0),
    TEST ("abcdefghij", "f",             5),
    TEST ("abcdefghij", "j",             9),

    TEST ("edfcbbhjig", "cba",        NPOS),
    TEST ("edfcbahjig", "cba",           3),
    TEST ("edfcbahcba", "cba",           3),
    TEST ("cbacbahjig", "cba",           0),

    TEST ("e\0cb\0\0g", "b\0\0g",        3),
    TEST ("e\0cb\0\0g", "ecb",        NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",        0),
    TEST ("\0cb\0\0ge", "cb\0",          1),
    TEST ("e\0cbg\0\0", "bg",            3),
    TEST ("e\0cbg\0\0", "cba",        NPOS),

    TEST ("bcbedfbjih", "a",          NPOS),
    TEST ("bcaedfajih", "a",             2),
    TEST ("bcedfaajih", "a",             5),
    TEST ("bcaaedfaji", "a",             2),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",    0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaabaaaaa", "aaaaa",         5),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",     0),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa", NPOS),

    TEST ("x@4096",     "",              0),
    TEST ("x@4096",     "a",          NPOS),
    TEST ("x@4096",     "x",             0),
    TEST ("x@4096",     "xxx",           0),
    TEST ("x@4096",     "xxa",        NPOS),
    TEST ("abc",        "x@4096",     NPOS),
    TEST ("xxxxxxxxxx", "x@4096",     NPOS),

    TEST ("abcdefghij", 0,               0),
    TEST ("\0cb\0\0ge", 0,               0),
    TEST ("x@4096",     0,               0),

    TEST ("last test",  "test",          5)
};

/**************************************************************************/

// exercises:
// find (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res) {                   \
        __LINE__, -1, -1, -1, -1, -1,           \
        str, sizeof str - 1,                    \
        arg,  sizeof arg - 1, 0, res, 0         \
    }

    //    +------------------------------------ controlled sequence
    //    |             +---------------------- sequence to be found
    //    |             |                +----- expected result
    //    |             |                |
    //    |             |                |
    //    V             V                V
    TEST ("ab",         "a",             0),

    TEST ("",           "",              0),
    TEST ("",           "\0",         NPOS),
    TEST ("",           "a",          NPOS),

    TEST ("\0",         "",              0),
    TEST ("\0",         "\0",            0),
    TEST ("\0",         "\0\0",       NPOS),
    TEST ("\0",         "a",          NPOS),
    TEST ("\0a",        "a\0",        NPOS),

    TEST ("aababcabcd", "",              0),
    TEST ("aababcabcd", "a",             0),
    TEST ("aababcabcd", "ab",            1),
    TEST ("aababcabcd", "abc",           3),
    TEST ("aababcabcd", "abcd",          6),
    TEST ("aababcabcd", "abcde",      NPOS),

    TEST ("bbcdefghij", "a",          NPOS),
    TEST ("abcdefghij", "a",             0),
    TEST ("abcdefghij", "f",             5),
    TEST ("abcdefghij", "j",             9),

    TEST ("edfcbbhjig", "cba",        NPOS),
    TEST ("edfcbahjig", "cba",           3),
    TEST ("edfcbahcba", "cba",           3),
    TEST ("cbacbahjig", "cba",           0),

    TEST ("e\0cb\0\0g", "b\0\0g",        3),
    TEST ("e\0cb\0\0g", "ecb",        NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",        3),
    TEST ("\0cb\0\0ge", "cb\0",          1),
    TEST ("\0cb\0\0ge", "cb\0a",      NPOS),
    TEST ("e\0cbg\0\0", "bg",            3),
    TEST ("e\0cbg\0\0", "\0\0",          5),
    TEST ("e\0cbg\0\0", "\0\0a",      NPOS),
    TEST ("e\0cbg\0\0", "cba",        NPOS),

    TEST ("bcbedfbjih", "a",          NPOS),
    TEST ("bcaedfajih", "a",             2),
    TEST ("bcedfaajih", "a",             5),
    TEST ("bcaaedfaji", "a",             2),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",    0),
    TEST ("aaaaabaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaabaaaaa", "aaaaa",         5),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",     0),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa", NPOS),

    TEST ("x@4096",     "",              0),
    TEST ("x@4096",     "a",          NPOS),
    TEST ("x@4096",     "x",             0),
    TEST ("x@4096",     "xxx",           0),
    TEST ("x@4096",     "xxa",        NPOS),
    TEST ("abc",        "x@4096",     NPOS),
    TEST ("xxxxxxxxxx", "x@4096",     NPOS),

    TEST ("abcdefghij", 0,               0),
    TEST ("\0cb\0\0ge", 0,               0),
    TEST ("x@4096",     0,               0),

    TEST ("last test",  "test",          5)
};

/**************************************************************************/

// exercises:
// find (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res) {              \
        __LINE__, off, -1, -1, -1, -1,          \
        str, sizeof str - 1,                    \
        arg, sizeof arg - 1, 0, res, 0          \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be found
    //    |            |               +--------- find() off argument
    //    |            |               |      +-- expected result
    //    |            |               |      |
    //    |            |               |      |
    //    V            V               V      V
    TEST ("ab",        "a",            0,     0),

    TEST ("",           "",            0,     0),
    TEST ("",           "\0",          0,     0),
    TEST ("",           "a",           0,  NPOS),

    TEST ("\0",         "",            0,     0),
    TEST ("\0",         "\0",          0,     0),
    TEST ("\0",         "\0",          1,     1),
    TEST ("\0",         "a",           0,  NPOS),

    TEST ("aababcabcd", "",            0,     0),
    TEST ("aababcabcd", "a",           0,     0),
    TEST ("aababcabcd", "ab",          0,     1),
    TEST ("aababcabcd", "abc",         0,     3),
    TEST ("aababcabcd", "abcd",        0,     6),
    TEST ("aababcabcd", "abcde",       0,  NPOS),

    TEST ("bbcdefghij", "a",           0,  NPOS),
    TEST ("abcdefghij", "a",           0,     0),
    TEST ("abcdefghij", "a",           2,  NPOS),
    TEST ("abcdefghij", "f",           2,     5),
    TEST ("abcdefghij", "f",           7,  NPOS),
    TEST ("abcdefghij", "j",           9,     9),

    TEST ("edfcbbhjig", "cba",         0,  NPOS),
    TEST ("edfcbahjig", "cba",         1,     3),
    TEST ("edfcbahjig", "cba",         4,  NPOS),
    TEST ("edfcbahcba", "cba",         1,     3),
    TEST ("edfcbahcba", "cba",         5,     7),
    TEST ("cbacbahjig", "cba",         5,  NPOS),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,     3),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  NPOS),
    TEST ("e\0cb\0\0g", "ecb",         0,  NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,     6),
    TEST ("\0cb\0\0ge", "cb\0",        1,     1),
    TEST ("e\0cbg\0\0", "bg",          1,     3),
    TEST ("e\0cbg\0\0", "cba",         0,  NPOS),

    TEST ("bcbedfbjih", "a",           0,  NPOS),
    TEST ("bcaedfajih", "a",           1,     2),
    TEST ("bcedfaajih", "a",           6,     6),
    TEST ("bcaaedfaji", "a",           5,     7),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,     0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1,  NPOS),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0,  NPOS),
    TEST ("aaaabaaaaa", "aaaaa",       0,     5),
    TEST ("aaaabaaaaa", "aaaaa",       6,  NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,     0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,     1),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2,  NPOS),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0,  NPOS),

    TEST ("x@4096",     "",            0,     0),
    TEST ("x@4096",     "a",           0,  NPOS),
    TEST ("x@4096",     "x",           0,     0),
    TEST ("x@4096",     "xxx",        10,    10),
    TEST ("x@4096",     "xxa",        10,  NPOS),
    TEST ("abc",        "x@4096",      2,  NPOS),
    TEST ("xxxxxxxxxx", "x@4096",      0,  NPOS),

    TEST ("x@4096",     "xxx",      4092,  4092),
    TEST ("x@4096",     "xxx",      4094,  NPOS),

    TEST ("abcdefghij", 0,             0,     0),
    TEST ("abcdefghij", 0,             1,  NPOS),
    TEST ("\0cb\0\0ge", 0,             5,     5),
    TEST ("x@4096",     0,             0,     0),
    TEST ("x@4096",     0,             1,  NPOS),

    TEST ("",           "",            1,  NPOS),
    TEST ("abcdefghij", "abc",        10,  NPOS),
    TEST ("abcdefghij", "cba",        10,  NPOS),

    TEST ("last test", "test",         0,     5)
};

/**************************************************************************/

// exercises:
// find (const value_type*, size_type, size_type)
static const StringTestCase
cptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res) {        \
        __LINE__, off, size, -1, -1, -1,        \
        str, sizeof str - 1,                    \
        arg, sizeof arg - 1, 0, res, 0          \
    }

    //    +--------------------------------------- controlled sequence
    //    |            +-------------------------- sequence to be found
    //    |            |               +---------- find() off argument
    //    |            |               |   +------ find() n argument
    //    |            |               |   |     +-- expected result
    //    |            |               |   |     |
    //    |            |               |   |     |
    //    |            |               |   |     |
    //    |            |               |   |     |
    //    |            |               |   |     |
    //    V            V               V   V     V
    TEST ("ab",        "a",            0,  1,    0),

    TEST ("",           "",            0,  0,    0),
    TEST ("",           "\0",          0,  0,    0),
    TEST ("",           "\0",          0,  1, NPOS),
    TEST ("",           "a",           0,  0,    0),
    TEST ("",           "a",           0,  1, NPOS),

    TEST ("aababcabcd", "",            0,  0,    0),
    TEST ("aababcabcd", "a",           0,  1,    0),
    TEST ("aababcabcd", "ab",          0,  2,    1),
    TEST ("aababcabcd", "abc",         0,  3,    3),
    TEST ("aababcabcd", "abcd",        0,  4,    6),
    TEST ("aababcabcd", "abcde",       0,  5, NPOS),
    TEST ("aababcabcd", "aababcabcde", 0, 11, NPOS),

    TEST ("\0",         "",            0,  0,    0),
    TEST ("\0",         "\0",          0,  1,    0),
    TEST ("\0",         "\0",          1,  1, NPOS),
    TEST ("\0\0",       "\0\0",        1,  1,    1),
    TEST ("\0",         "a",           0,  1, NPOS),

    TEST ("edfcbbhjig", "cba",         0,  3, NPOS),
    TEST ("edfcbbhjig", "cba",         0,  2,    3),
    TEST ("edfcbahjig", "cba",         1,  3,    3),
    TEST ("edfcbahjig", "cba",         4,  3, NPOS),
    TEST ("edfcbahjig", "cba",         4,  1, NPOS),
    TEST ("edfcbahcba", "cba",         1,  3,    3),
    TEST ("edfcbehcba", "cba",         1,  2,    3),
    TEST ("edfcbahcba", "cba",         5,  3,    7),
    TEST ("cbacbahjig", "cba",         5,  3, NPOS),
    TEST ("cbacbahjcg", "cba",         5,  1,    8),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  4,    3),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  4, NPOS),
    TEST ("e\0cb\0\0b", "b\0\0g",      4,  1,    6),
    TEST ("\0b\0\0gb\0","b\0\0g",      2,  2,    5),
    TEST ("\0b\0\0gb\0","b\0\0g",      0,  2,    1),
    TEST ("\0b\0\0gb\0","b\0\0g",      0,  3,    1),
    TEST ("e\0cb\0\0g", "ecb",         0,  2, NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  4, NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  0,    6),
    TEST ("\0cb\0\0ge", "cb\0",        1,  3,    1),
    TEST ("e\0cbg\0\0", "bg",          1,  2,    3),
    TEST ("e\0cbg\0\0", "cba",         0,  3, NPOS),
    TEST ("e\0cbg\0\0", "cba",         0,  2,    2),

    TEST ("e\0a\0",     "e\0a\0\0",    0,  4,    0),
    TEST ("e\0a\0",     "e\0a\0\0",    0,  5, NPOS),
    TEST ("ee\0a\0",    "e\0a\0\0",    1,  4,    1),
    TEST ("ee\0a\0",    "e\0a\0\0",    1,  5, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, 10,    0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, 10, NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1,  9,    1),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0, 10, NPOS),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,  4,    1),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  2,  4,    6),
    TEST ("aaaabaaaaa", "aaaaa",       0,  5,    5),
    TEST ("aaaabaaaaa", "aaaaa",       0,  4,    0),
    TEST ("aaaabaaaaa", "aaaaa",       6,  5, NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,  9,    0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,  9,    1),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2,  9, NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2,  8,    2),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0, 10, NPOS),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0,  7,    0),

    TEST ("x@4096",     "",            0,  0,      0),
    TEST ("x@4096",     "a",           0,  1,   NPOS),
    TEST ("x@4096",     "x",           0,  1,      0),
    TEST ("x@4096",     "xxx",        10,  3,     10),
    TEST ("x@4096",     "xxa",        10,  3,   NPOS),
    TEST ("x@4096",     "xxa",        10,  2,     10),
    TEST ("abc",        "x@4096",      2, 10,   NPOS),
    TEST ("xxxxxxxxxx", "x@4096",      0, 4096, NPOS),
    TEST ("xxxxxxxxxx", "x@4096",      2,  4,      2),

    TEST ("x@4096",     "xxx",      4093,  3,   4093),
    TEST ("x@4096",     "xxx",      4094,  3,   NPOS),
    TEST ("x@4096",     "xxx",      4094,  2,   4094),

    TEST ("abcdefghij", 0,             0,  9,      0),
    TEST ("abcdefghij", 0,             1,  9,   NPOS),
    TEST ("\0cb\0\0ge", 0,             5,  7,   NPOS),
    TEST ("\0cb\0ge\0", 0,             6,  1,      6),
    TEST ("x@4096",     0,             0, 4096,    0),
    TEST ("x@4096",     0,             1, 4096, NPOS),
    TEST ("x@4096",     0,             5, 4091,    5),

    TEST ("",           "",            1,  0, NPOS),
    TEST ("abcdefghij", "abc",        10,  3, NPOS),
    TEST ("abcdefghij", "cba",        10,  1, NPOS),

    // excercise strictly undefined behavior
    TEST ("",           "cba",         0, -1, NPOS),
    TEST ("abcdefghij", "cba",         0, -1, NPOS),
    TEST ("x@4096",     "xxx",         0, -1, NPOS),
    TEST ("abcdefghij", "x@4096",      0, -1, NPOS),

    TEST ("last test", "test",         0,  4,    5)
};

/**************************************************************************/

// exercises:
// find (const basic_string&, size_type)
static const StringTestCase
cstr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res) {              \
        __LINE__, off, -1, -1, -1, -1,          \
        str, sizeof str - 1,                    \
        arg, sizeof arg - 1, 0, res, 0          \
    }

    //    +--------------------------------------- controlled sequence
    //    |             +------------------------- sequence to be found
    //    |             |              +---------- find() off argument
    //    |             |              |     +---- expected result
    //    |             |              |     |
    //    |             |              |     |
    //    V             V              V     V
    TEST ("ab",         "a",           0,    0),

    TEST ("",           "",            0,    0),
    TEST ("",           "\0",          0, NPOS),
    TEST ("",           "a",           0, NPOS),

    TEST ("\0",         "",            0,    0),
    TEST ("\0",         "\0",          0,    0),
    TEST ("\0",         "\0",          1, NPOS),
    TEST ("\0\0",       "\0",          1,    1),
    TEST ("\0",         "a",           0, NPOS),

    TEST ("bbcdefghij", "a",           0, NPOS),
    TEST ("abcdefghij", "a",           0,    0),
    TEST ("abcdefghij", "a",           2, NPOS),
    TEST ("abcdefghij", "f",           2,    5),
    TEST ("abcdefghij", "f",           7, NPOS),
    TEST ("abcdefghij", "j",           9,    9),

    TEST ("edfcbbhjig", "cba",         0, NPOS),
    TEST ("edfcbahjig", "cba",         1,    3),
    TEST ("edfcbahjig", "cba",         4, NPOS),
    TEST ("edfcbahcba", "cba",         1,    3),
    TEST ("edfcbahcba", "cba",         5,    7),
    TEST ("cbacbahjig", "cba",         5, NPOS),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    3),
    TEST ("e\0cb\0\0g", "b\0\0g",      4, NPOS),
    TEST ("e\0cb\0\0g", "ecb",         0, NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",      6, NPOS),
    TEST ("\0cb\0\0ge", "\0\0ge",      1,    3),
    TEST ("\0cb\0\0ge", "cb\0",        1,    1),
    TEST ("\0cbg\0\0e", "cb\0",        1, NPOS),
    TEST ("e\0cbg\0\0", "bg",          1,    3),
    TEST ("e\0cbg\0\0", "cba",         0, NPOS),

    TEST ("bcbedfbjih", "a",           0, NPOS),
    TEST ("bcaedfajih", "a",           1,    2),
    TEST ("bcedfaajih", "a",           6,    6),
    TEST ("bcaaedfaji", "a",           5,    7),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,    0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, NPOS),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  0, NPOS),
    TEST ("aaaabaaaaa", "aaaaa",       0,    5),
    TEST ("aaaabaaaaa", "aaaaa",       6, NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   0,    0),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   1,    1),
    TEST ("aaaaaaaaaa", "aaaaaaaaa",   2, NPOS),
    TEST ("aaaaaaaaa",  "aaaaaaaaaa",  0, NPOS),

    TEST ("x@4096",     "",            0,    0),
    TEST ("x@4096",     "a",           0, NPOS),
    TEST ("x@4096",     "x",           0,    0),
    TEST ("x@4096",     "xxx",        10,   10),
    TEST ("x@4096",     "xxa",        10, NPOS),
    TEST ("abc",        "x@4096",      2, NPOS),
    TEST ("xxxxxxxxxx", "x@4096",      0, NPOS),

    TEST ("x@4096",     "xxx",      4093, 4093),
    TEST ("x@4096",     "xxx",      4094, NPOS),

    TEST ("abcdefghij", 0,             0,    0),
    TEST ("abcdefghij", 0,             1, NPOS),
    TEST ("\0cb\0\0ge", 0,             5, NPOS),
    TEST ("\0cb\0\0ge", 0,             0,    0),
    TEST ("x@4096",     0,             0,    0),
    TEST ("x@4096",     0,             1, NPOS),

    TEST ("",           "",            1, NPOS),
    TEST ("abcdefghij", "abc",        10, NPOS),
    TEST ("abcdefghij", "cba",        10, NPOS),

    TEST ("last test",  "test",        0,    5)
};

/**************************************************************************/

// exercises:
// find (value_type)
static const StringTestCase
val_test_cases [] = {

#undef TEST
#define TEST(str, val, res) {                   \
        __LINE__, -1, -1, -1, -1, val,          \
        str, sizeof str - 1, 0, 0, 0, res, 0    \
    }

    //    +----------------------------- controlled sequence
    //    |              +-------------- character to be found
    //    |              |       +------ expected result
    //    |              |       |
    //    |              |       |
    //    V              V       V
    TEST ("ab",          'a',    0),

    TEST ("",            'a', NPOS),
    TEST ("",           '\0', NPOS),

    TEST ("\0",         '\0',    0),
    TEST ("\0\0",       '\0',    0),
    TEST ("\0",          'a', NPOS),

    TEST ("e\0cb\0\0g", '\0',    1),
    TEST ("e\0cb\0\0g",  'b',    3),
    TEST ("e\0cb\0\0g",  'a', NPOS),
    TEST ("\0cbge\0\0", '\0',    0),
    TEST ("\0cbge\0\0",  'b',    2),
    TEST ("\0cbge\0\0",  'a', NPOS),

    TEST ("x@4096",      'x',    0),
    TEST ("x@4096",     '\0', NPOS),
    TEST ("x@4096",      'a', NPOS),

    TEST ("last test",   't',    3)
};

/**************************************************************************/

// exercises:
// find (value_type, size_type)
static const StringTestCase
val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res) {              \
        __LINE__, off, -1, -1, -1, val,         \
        str, sizeof str - 1, 0, 0, 0, res, 0    \
    }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- find() off argument
    //    |              |     |     +--- expected result
    //    |              |     |     |
    //    |              |     |     |
    //    V              V     V     V
    TEST ("ab",          'a',  0,    0),

    TEST ("",            'a',  0, NPOS),
    TEST ("",           '\0',  0, NPOS),

    TEST ("\0",         '\0',  1, NPOS),
    TEST ("\0",          'a',  0, NPOS),
    TEST ("\0\0",       '\0',  1,    1),
    TEST ("\0\0",        'a',  3, NPOS),
    TEST ("\0\0",       '\0',  3, NPOS),

    TEST ("e\0cb\0\0g", '\0',  1,    1),
    TEST ("e\0cb\0\0g", '\0',  2,    4),
    TEST ("e\0cb\0\0g", '\0',  6, NPOS),
    TEST ("e\0cb\0\0g",  'b',  1,    3),
    TEST ("e\0cb\0\0g",  'b',  4, NPOS),
    TEST ("e\0cb\0\0g",  'a',  0, NPOS),
    TEST ("\0cbge\0\0", '\0',  0,    0),
    TEST ("\0cbge\0\0", '\0',  1,    5),
    TEST ("\0cbge\0\0", '\0',  9, NPOS),
    TEST ("\0cbge\0\0",  'b',  0,    2),
    TEST ("\0bgeb\0\0",  'b',  2,    4),
    TEST ("\0cbge\0\0",  'a',  1, NPOS),

    TEST ("x@4096",      'x',  0,    0),
    TEST ("x@4096",      'x',  5,    5),
    TEST ("x@4096",     '\0',  0, NPOS),
    TEST ("x@2048\0xxx",'\0',  0, 2048),
    TEST ("x@4096",      'a',  3, NPOS),
    TEST ("x@4096",      'x', 4096, NPOS),
    TEST ("x@4096",      'x', 4095, 4095),

    TEST ("last test",   't',  0,    3)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_find (charT, Traits*, Allocator*,
                const StringFunc     &func,
                const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    // construct the string object and the argument string
    const String  s_str (wstr, str_len);
    const String  s_arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));

    // string function argument
    const charT* const arg_ptr = tcase.arg ? s_arg.c_str () : s_str.c_str ();
    const String&      arg_str = tcase.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t size = tcase.size >= 0 ? tcase.size : s_arg.max_size () + 1;

#ifndef _RWSTD_NO_EXCEPTIONS

    // no exceptions expected
    const char* expected = 0;
    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        std::size_t res = 0;

        switch (func.which_) {

        case Find (cptr):
            res = s_str.find (arg_ptr);
            break;

        case Find (cstr):
            res = s_str.find (arg_str);
            break;

        case Find (cptr_size):
            res = s_str.find (arg_ptr, tcase.off);
            break;

        case Find (cptr_size_size):
            res = s_str.find (arg_ptr, tcase.off, size);
            break;

        case Find (cstr_size):
            res = s_str.find (arg_str, tcase.off);
            break;

        case Find (val):
            res = s_str.find (arg_val);
            break;

        case Find (val_size):
            res = s_str.find (arg_val, tcase.off);
            break;

        default:
            RW_ASSERT (!"logic error: unknown find overload");
            return;
        }

        const std::size_t exp_res =
            NPOS != tcase.nres ? tcase.nres : String::npos;

        // verify the returned value
        rw_assert (exp_res == res, 0, tcase.line,
                   "line %d. %{$FUNCALL} == %{?}%zu%{;}%{?}npos%{;}, "
                   "got %{?}%zu%{;}%{?}npos%{;}",
                   __LINE__, NPOS != tcase.nres, exp_res, NPOS == tcase.nres,
                   String::npos != res, res, String::npos == res);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

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
        str_state.assert_equal (rw_get_string_state (s_str),
                                __LINE__, tcase.line, caught);
    }
    else if (-1 != tcase.bthrow) {
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_find);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Find (sig), sig ## _test_cases,                         \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cptr_size_size),
        TEST (cstr_size),
        TEST (val),
        TEST (val_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.find",
                               test_find, tests, test_count);
}
#endif
