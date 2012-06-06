/***************************************************************************
 *
 * 21.string.find.last.not.of.cpp -
 *      string test exercising lib.string.find.last.not.of
 *
 * $Id: 21.string.find.last.not.of.cpp 590052 2007-10-30 12:44:14Z faridz $
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
#include <cstdlib>          // for free(), size_t
#include <stdexcept>        // for length_error

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define FindLastNotOf(sig)        StringIds::find_last_not_of_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// find_last_not_of (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                 \
    { __LINE__, -1, -1, -1, -1, -1,         \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |                +---- expected result
    //    |             |                |
    //    |             |                |
    //    V             V                V
    TEST ("ab",         "c",             1),

    TEST ("",           "",           NPOS),
    TEST ("",           "\0",         NPOS),
    TEST ("",           "a",          NPOS),

    TEST ("\0",         "",              0),
    TEST ("\0",         "\0",            0),
    TEST ("\0",         "a",             0),

    TEST ("bbcdefghij", "a",             9),
    TEST ("bcdefghija", "a",             8),
    TEST ("abcdedaaaa", "a",             5),
    TEST ("aabaaacaaa", "a",             6),

    TEST ("edfcbbhjig", "cba",           9),
    TEST ("edfcbaacba", "bca",           2),
    TEST ("edhcbahcba", "cba",           6),
    TEST ("cbacbbcbac", "cab",        NPOS),

    TEST ("e\0cb\0\0g", "b\0\0g",        6),
    TEST ("e\0cb\0\0g", "ecbg",          5),
    TEST ("\0cb\0\0ge", "\0\0ge",        6),
    TEST ("ge\0\0\0cb", "bc\0",          4),
    TEST ("e\0cdg\0\0", "abc",           6),
    TEST ("a\0b",       "e\0gbg\0\0",    2),
    TEST ("a\0b",       "ab\0gg\0\0",    1),
    TEST ("\0ab",       "b\0gg\0\0a",    1),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaaabaaaa", "ccccccccca",    5),
    TEST ("aabaabaaaa", "ccccccccca",    5),
    TEST ("bbb",        "aaaaaaaaba", NPOS),
    TEST ("bbb",        "aaaaaaaaaa",    2),
    TEST ("abcdefghij", "jihgfedcba", NPOS),

    TEST ("x@4096",     "",           4095),
    TEST ("x@4096",     "a",          4095),
    TEST ("x@4096",     "x",          NPOS),
    TEST ("x@4096",     "xxx",        NPOS),
    TEST ("x@4096",     "aax",        NPOS),
    TEST ("abc",        "x@4096",        2),
    TEST ("xxxxxxxxxx", "x@4096",     NPOS),
    TEST ("xxax@2048",  "x@4096",        2),

    TEST ("abcdefghij", 0,            NPOS),
    TEST ("\0cb\0\0ge", 0,               6),
    TEST ("x@4096",     0,            NPOS),

    TEST ("last test",  "test",          4)
};

/**************************************************************************/

// used to exercise
// find_last_not_of (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                 \
    { __LINE__, -1, -1, -1, -1, -1,         \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +------------------------------------ controlled sequence
    //    |             +---------------------- sequence to be found
    //    |             |                +----- expected result
    //    |             |                |
    //    |             |                |
    //    V             V                V
    TEST ("ab",         "c",             1),

    TEST ("",           "",           NPOS),
    TEST ("",           "\0",         NPOS),
    TEST ("",           "a",          NPOS),

    TEST ("\0",         "",              0),
    TEST ("\0",         "\0",         NPOS),
    TEST ("\0\0",       "\0",         NPOS),
    TEST ("\0",         "a",             0),
    TEST ("a",          "\0",            0),

    TEST ("bbcdefghij", "a",             9),
    TEST ("bcdefghija", "a",             8),
    TEST ("abcdedaaaa", "a",             5),
    TEST ("aabaaacaaa", "a",             6),

    TEST ("edfcbbhjig", "cba",           9),
    TEST ("edfcbaacba", "bca",           2),
    TEST ("edhcbahcba", "cba",           6),
    TEST ("cbacbbcbac", "cab",        NPOS),

    TEST ("e\0bb\0\0g", "b\0\0g",        0),
    TEST ("e\0cb\0\0g", "ecbg",          5),
    TEST ("\0cb\0\0ge", "\0\0ge",        2),
    TEST ("be\0\0\0cb", "bc\0",          1),
    TEST ("e\0cdg\0\0", "abc",           6),
    TEST ("a\0b",       "e\0gbg\0\0",    0),
    TEST ("a\0b",       "ab\0gg\0\0", NPOS),
    TEST ("\0ab",       "abcdefghij",    0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaaabaaaa", "ccccccccca",    5),
    TEST ("aabaabaaaa", "ccccccccca",    5),
    TEST ("bbb",        "aaaaaaaaba", NPOS),
    TEST ("bbb",        "aaaaaaaaaa",    2),
    TEST ("abcdefghij", "jihgfedcba", NPOS),

    TEST ("x@4096",     "",           4095),
    TEST ("x@4096",     "a",          4095),
    TEST ("x@4096",     "x",          NPOS),
    TEST ("x@4096",     "xxx",        NPOS),
    TEST ("x@4096",     "aax",        NPOS),
    TEST ("abc",        "x@4096",        2),
    TEST ("xxxxxxxxxx", "x@4096",     NPOS),
    TEST ("xxax@2048",  "x@4096",        2),

    TEST ("abcdefghij", 0,            NPOS),
    TEST ("\0cb\0\0ge", 0,            NPOS),
    TEST ("x@4096",     0,            NPOS),

    TEST ("last test",  "test",          4)
};

/**************************************************************************/

// used to exercise
// find_last_not_of (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)            \
    { __LINE__, off, -1, -1, -1, -1,        \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +----------------------------------- controlled sequence
    //    |            +---------------------- sequence to be found
    //    |            |               +------ find_last_not_of() off argument
    //    |            |               |     +-- expected result
    //    |            |               |     |
    //    |            |               |     |
    //    V            V               V     V
    TEST ("ab",        "c",            0,    0),

    TEST ("",           "",            0, NPOS),
    TEST ("",           "\0",          0, NPOS),
    TEST ("",           "a",           0, NPOS),

    TEST ("\0",         "",            0,    0),
    TEST ("\0",         "",            1,    0),
    TEST ("\0",         "\0",          0,    0),
    TEST ("\0",         "\0",          1,    0),
    TEST ("\0",         "a",           0,    0),

    TEST ("bbcdefghij", "a",           9,    9),
    TEST ("bcdefghija", "a",           9,    8),
    TEST ("bbcdefghij", "a",           0,    0),
    TEST ("bcadefghij", "a",           2,    1),
    TEST ("aaadefghij", "a",           2, NPOS),
    TEST ("abcdeaaaij", "a",           7,    4),

    TEST ("edfcbbhjig", "cba",         9,    9),
    TEST ("edfcbacbag", "cba",         7,    2),
    TEST ("ebccbahjig", "cba",         4,    0),
    TEST ("edfcbahcba", "cba",         9,    6),
    TEST ("cbacbahcba", "cba",         5, NPOS),
    TEST ("edfcbahcba", "cba",         3,    2),
    TEST ("cbahbahabc", "cba",         9,    6),

    TEST ("e\0cb\0\0g", "b\0\0g",      5,    5),
    TEST ("b\0bc\0\0g", "b\0\0g",      2,    1),
    TEST ("e\0cb\0\0g", "ecb",         7,    6),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,    6),
    TEST ("b\0\0\0gec", "cb\0",        0, NPOS),
    TEST ("\0cb\0\0ge", "cb\0",        1,    0),
    TEST ("\0cb\0\0ge", "cb\0",        2,    0),
    TEST ("e\0cbg\0\0", "bg",          4,    2),
    TEST ("e\0\0\0bbg", "bg",          5,    3),
    TEST ("a\0b",       "b\0gbg\0\0",  3,    1),
    TEST ("\0ba",       "ab\0gg\0\0",  3,    0),
    TEST ("ab\0",       "ba\0d\0\0a",  1, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  9, NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  4, NPOS),
    TEST ("aaaaabaaaa", "ccccccccca",  8,    5),
    TEST ("aabaabaaaa", "ccccccccca",  9,    5),
    TEST ("aabaabaaaa", "ccccccccca",  3,    2),
    TEST ("bbb",        "aaaaaaaaba",  2, NPOS),
    TEST ("cab",        "aaaaaaaaba",  0,    0),
    TEST ("bbb",        "aaaaaaaaaa",  3,    2),

    TEST ("x@4096",     "",         4096, 4095),
    TEST ("x@4096",     "a",        4096, 4095),
    TEST ("x@4096",     "x",        4096, NPOS),
    TEST ("x@4096",     "xxx",      4095, NPOS),
    TEST ("x@4096",     "xxx",         0, NPOS),
    TEST ("x@4096",     "xxx",      4087, NPOS),
    TEST ("x@4096",     "aax",      4087, NPOS),
    TEST ("abc",        "x@4096",      2,    2),
    TEST ("xxxxxxxxxx", "x@4096",      6, NPOS),
    TEST ("xxxxaxxxxx", "x@4096",      9,    4),

    TEST ("x@4096",     "xxx",         3, NPOS),
    TEST ("x@4096",     "xxx",         2, NPOS),
    TEST ("xxax@2048",  "x@4096",   2052,    2),

    TEST ("abcdefghij", 0,             0, NPOS),
    TEST ("abcdefghij", 0,             9, NPOS),
    TEST ("\0cb\0\0ge", 0,             5,    5),
    TEST ("x@4096",     0,             0, NPOS),
    TEST ("x@4096",     0,             1, NPOS),

    TEST ("",           "",            1, NPOS),
    TEST ("defghijabc", "abc",        15,    6),
    TEST ("defgabcabc", "cba",        15,    3),

    TEST ("last test", "test",         9,    4)
};

/**************************************************************************/

// used to exercise
// find_last_not_of (const value_type*, size_type, size_type)
static const StringTestCase
cptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)      \
    { __LINE__, off, size, int (NPOS), -1, -1,      \
      str, sizeof str - 1, arg,                     \
      sizeof arg - 1, 0, res, bthrow                \
    }

    //    +----------------------------------- controlled sequence
    //    |            +---------------------- sequence to be found
    //    |            |               +------ find_last_not_of() off argument
    //    |            |               |   +-- find_last_not_of() n argument
    //    |            |               |   |     +-- expected result
    //    |            |               |   |     |   exception info
    //    |            |               |   |     |   |   0 - no exception
    //    |            |               |   |     |   |   1 - length_error
    //    |            |               |   |     |   |
    //    |            |               |   |     |   +----+
    //    V            V               V   V     V        V
    TEST ("ab",        "c",            0,  1,    0,       0),

    TEST ("",           "",            0,  0, NPOS,       0),
    TEST ("",           "\0",          0,  0, NPOS,       0),
    TEST ("",           "\0",          0,  1, NPOS,       0),
    TEST ("",           "a",           0,  0, NPOS,       0),
    TEST ("",           "a",           0,  1, NPOS,       0),

    TEST ("\0",         "",            0,  0,    0,       0),
    TEST ("\0",         "\0",          0,  1, NPOS,       0),
    TEST ("\0",         "\0",          1,  1, NPOS,       0),
    TEST ("\0\0",       "\0\0",        1,  1, NPOS,       0),
    TEST ("\0",         "a",           0,  1,    0,       0),
    TEST ("a",          "\0",          0,  1,    0,       0),

    TEST ("adfcbbhjig", "cba",         0,  3, NPOS,       0),
    TEST ("bdfcbbhjig", "cba",         0,  2, NPOS,       0),
    TEST ("edfhjabcbb", "cba",         9,  2,    5,       0),
    TEST ("edacbdcbag", "cba",         8,  3,    5,       0),
    TEST ("edfcbaabcg", "cba",         8,  3,    2,       0),
    TEST ("cccfehjigb", "cba",         2,  1, NPOS,       0),
    TEST ("edfcbahcba", "cba",         9,  3,    6,       0),
    TEST ("edfcbehcba", "cab",         9,  2,    8,       0),
    TEST ("edfcbacbah", "cba",         9,  3,    9,       0),
    TEST ("cbacbahjig", "cba",         5,  3, NPOS,       0),
    TEST ("accacbahji", "cba",         2,  1,    0,       0),
    TEST ("cbacbaccdg", "cba",         7,  1,    5,       0),

    TEST ("e\0cb\0\0g", "b\0\0g",      6,  4,    2,       0),
    TEST ("eg\0b\0\0g", "b\0\0g",      7,  4,    0,       0),
    TEST ("e\0cb\0\0b", "b\0\0g",      4,  1,    4,       0),
    TEST ("g\0b\0\0b\0","b\0\0g",      7,  2,    0,       0),
    TEST ("\0bg\0\0b\0","b\0\0g",      2,  2,    2,       0),
    TEST ("\0b\0\0b\0g","b\0g\0",      7,  3, NPOS,       0),
    TEST ("e\0cbg\0\0", "ecb",         7,  2,    6,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  4,    2,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      2,  0,    2,       0),
    TEST ("\0cb\0\0ge", "cb\0",        4,  3, NPOS,       0),
    TEST ("e\0bcg\0\0", "bg",          1,  2,    1,       0),
    TEST ("e\0bcg\0\0", "bg",          6,  2,    6,       0),
    TEST ("a\0b",       "a\0gbg\0\0",  2,  1,    2,       0),
    TEST ("\0ba",       "ab\0gg\0\0",  1,  2,    0,       0),
    TEST ("ba\0",       "e\0gg\0\0a",  3,  7,    0,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  0,  7, NPOS,       0),
    TEST ("a\0b",       "eb\0gg\0\0",  1,  3,    0,       0),

    TEST ("e\0a\0",     "e\0a\0\0",    2,  4, NPOS,       0),
    TEST ("e\0a\0",     "e\0a\0\0",    3,  5, NPOS,       0),
    TEST ("ee\0a\0",    "b\0c\0\0",    3,  4,    3,       0),
    TEST ("e\0\0cb",    "fdbcb\0a",    3,  5,    2,       0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  9, 10, NPOS,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  8, 10, NPOS,       0),
    TEST ("aaaaabaaaa", "ccccccccca",  9, 10,    5,       0),
    TEST ("aabaabaaab", "cccacccccc",  8,  4,    5,       0),
    TEST ("aaaabaaaab", "cccca",       9,  5,    9,       0),
    TEST ("aabaabaaaa", "cacccccccb",  7,  5,    5,       0),
    TEST ("bbb",        "aaaaaaaaba",  1,  2,    1,       0),
    TEST ("bbb",        "aaaaaaaaab",  1,  9,    1,       0),
    TEST ("bab",        "aaaaaaaaba",  0,  0,    0,       0),
    TEST ("bab",        "ccccccccba",  1, 10, NPOS,       0),
    TEST ("abcdefghij", "jihgfedcba",  9, 10, NPOS,       0),
    TEST ("abcdefghij", "jihgfedcba",  9,  9,    0,       0),

    TEST ("x@4096",     "",         4096,  0, 4095,       0),
    TEST ("x@4096",     "a",        4096,  1, 4095,       0),
    TEST ("x@4096",     "x",        4096,  1, NPOS,       0),
    TEST ("x@4096",     "xxx",      4088,  3, NPOS,       0),
    TEST ("x@4096",     "aax",      4088,  2, 4088,       0),
    TEST ("abc",        "x@4096",      2, 10,    2,       0),
    TEST ("xxxxxxxxxx", "x@4096",      9, 4096, NPOS,     0),
    TEST ("xxxxxxxxxx", "x@4096",      2,  4, NPOS,       0),

    TEST ("x@4096",     "xxx",      4093,  3, NPOS,       0),
    TEST ("x@4096",     "xxx",      4094,  0, 4094,       0),
    TEST ("xxax@2048",  "x@4096a",  2052, 4096,  2,       0),

    TEST ("abcdefghij", 0,             9, 10, NPOS,       0),
    TEST ("abcdefghij", 0,             8,  6,    8,       0),
    TEST ("\0cb\0\0ge", 0,             5,  7, NPOS,       0),
    TEST ("\0cb\0ge\0", 0,             6,  1,    5,       0),
    TEST ("\0c\0eg\0c", 0,             6,  2,    4,       0),
    TEST ("x@4096",     0,             0, 4096, NPOS,     0),
    TEST ("x@4096",     0,             1, 4096, NPOS,     0),
    TEST ("x@4096",     0,          4091,  0, 4091,       0),

    TEST ("",           "",            1,  0, NPOS,       0),
    TEST ("defghijabc", "abc",        15,  3,    6,       0),
    TEST ("defghijabc", "cba",        15,  1,    8,       0),

    TEST ("",           "cba",         0, -1, NPOS,       0),

    TEST ("last test",  "test",        9,  4,    4,       0)
};

/**************************************************************************/

// used to exercise
// find_last_not_of (const basic_string&, size_type)
static const StringTestCase
cstr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)            \
    { __LINE__, off, -1, -1, -1, -1,        \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |              +------ find_last_not_of() off argument
    //    |             |              |     +-- expected result
    //    |             |              |     |
    //    |             |              |     |
    //    V             V              V     V
    TEST ("ab",         "c",           1,    1),

    TEST ("",           "",            0, NPOS),
    TEST ("",           "\0",          0, NPOS),
    TEST ("",           "a",           0, NPOS),

    TEST ("\0",         "",            0,    0),
    TEST ("\0",         "",            1,    0),
    TEST ("\0",         "\0",          0, NPOS),
    TEST ("\0",         "\0",          1, NPOS),
    TEST ("\0\0",       "\0",          1, NPOS),
    TEST ("\0",         "a",           0,    0),
    TEST ("a",          "\0",          1,    0),

    TEST ("bbcdefghij", "a",           9,    9),
    TEST ("bcdefghija", "a",           9,    8),
    TEST ("bbcdefghij", "a",           0,    0),
    TEST ("bcadefghij", "a",           2,    1),
    TEST ("aaadefghij", "a",           2, NPOS),
    TEST ("abcdeaaaij", "a",           7,    4),

    TEST ("edfcbbhjig", "cba",         9,    9),
    TEST ("edfcbacbag", "cba",         7,    2),
    TEST ("ebccbahjig", "cba",         4,    0),
    TEST ("edfcbahcba", "cba",         9,    6),
    TEST ("cbacbahcba", "cba",         5, NPOS),
    TEST ("edfcbahcba", "cba",         3,    2),
    TEST ("cbahbahabc", "cba",         9,    6),

    TEST ("e\0cb\0\0g", "b\0\0g",      5,    2),
    TEST ("c\0bg\0\0g", "b\0\0g",      2,    0),
    TEST ("e\0cb\0\0g", "ecb",         7,    6),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,    2),
    TEST ("b\0\0\0gec", "cb\0",        2, NPOS),
    TEST ("\0cb\0\0ge", "cb\0",        1, NPOS),
    TEST ("\0cb\0\0ge", "cb\0",        6,    6),
    TEST ("e\0cbg\0\0", "bg",          4,    2),
    TEST ("e\0\0\0bbg", "bg",          5,    3),
    TEST ("a\0b",       "b\0gbg\0\0",  3,    0),
    TEST ("\0ba",       "ab\0gg\0\0",  3, NPOS),
    TEST ("ab\0",       "ba\0d\0\0a",  2, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  9, NPOS),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  4, NPOS),
    TEST ("aaaaabaaaa", "ccccccccca",  8,    5),
    TEST ("aabaabaaaa", "ccccccccca",  9,    5),
    TEST ("aabaabaaaa", "ccccccccca",  3,    2),
    TEST ("bbb",        "aaaaaaaaba",  2, NPOS),
    TEST ("cab",        "aaaaaaaaba",  0,    0),
    TEST ("bbb",        "aaaaaaaaaa",  3,    2),

    TEST ("x@4096",     "",         4096, 4095),
    TEST ("x@4096",     "a",        4096, 4095),
    TEST ("x@4096",     "x",        4096, NPOS),
    TEST ("x@4096",     "xxx",      4095, NPOS),
    TEST ("x@4096",     "xxx",         0, NPOS),
    TEST ("x@4096",     "xxx",      4087, NPOS),
    TEST ("x@4096",     "aax",      4087, NPOS),
    TEST ("abc",        "x@4096",      2,    2),
    TEST ("xxxxxxxxxx", "x@4096",      6, NPOS),
    TEST ("xxxxaxxxxx", "x@4096",      9,    4),
    TEST ("xxax@2048",  "x@4096",   2050,    2),

    TEST ("x@4096",     "xxx",         3, NPOS),
    TEST ("x@4096",     "xxx",         2, NPOS),

    TEST ("abcdefghij", 0,             0, NPOS),
    TEST ("abcdefghij", 0,             9, NPOS),
    TEST ("\0cb\0\0ge", 0,             5, NPOS),
    TEST ("x@4096",     0,             0, NPOS),
    TEST ("x@4096",     0,             1, NPOS),

    TEST ("",           "",            1, NPOS),
    TEST ("defghijabc", "abc",        15,    6),
    TEST ("defgabcabc", "cba",        15,    3),

    TEST ("last test",  "test",        9,    4)
};

/**************************************************************************/

// used to exercise
// find_last_not_of (value_type)
static const StringTestCase
val_test_cases [] = {

#undef TEST
#define TEST(str, val, res)             \
    { __LINE__, -1, -1, -1, -1,         \
      val, str, sizeof str - 1,         \
      0, 0, 0, res, 0                   \
    }

    //    +----------------------------- controlled sequence
    //    |              +-------------- character to be found
    //    |              |       +------ expected result
    //    |              |       |
    //    |              |       |
    //    V              V       V
    TEST ("ab",          'c',    1),

    TEST ("",            'a', NPOS),
    TEST ("",           '\0', NPOS),

    TEST ("\0",         '\0', NPOS),
    TEST ("\0\0",       '\0', NPOS),
    TEST ("\0",          'a',    0),
    TEST ("a\0",        '\0',    0),

    TEST ("e\0cbg\0\0", '\0',    4),
    TEST ("e\0cgg\0\0", '\0',    4),
    TEST ("e\0cb\0\0b",  'b',    5),
    TEST ("\0cbge\0\0", '\0',    4),
    TEST ("\0cbgb\0\0",  'b',    6),
    TEST ("aaaaaaaaaa",  'a', NPOS),
    TEST ("\0aaaaaaaa",  'a',    0),

    TEST ("x@4096",      'x', NPOS),
    TEST ("x@4096",     '\0', 4095),
    TEST ("x@4096",      'a', 4095),
    TEST ("xxax@2048",   'x',    2),

    TEST ("last test",   't',    7)
};

/**************************************************************************/

// used to exercise
// find_last_not_of (value_type, size_type)
static const StringTestCase
val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res)        \
    { __LINE__, off, -1, -1, -1,        \
      val, str, sizeof str - 1,         \
      0, 0, 0, res, 0                   \
    }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- find_last_not_of() off argument
    //    |              |     |     +--- expected result
    //    |              |     |     |
    //    |              |     |     |
    //    V              V     V     V
    TEST ("ab",          'c',  1,    1),

    TEST ("",            'a',  0, NPOS),
    TEST ("",           '\0',  0, NPOS),

    TEST ("\0",         '\0',  1, NPOS),
    TEST ("\0",          'a',  0,    0),
    TEST ("\0\0",       '\0',  2, NPOS),
    TEST ("\0\0",        'a',  3,    1),
    TEST ("\0\0",       '\0',  3, NPOS),

    TEST ("e\0cb\0\0g", '\0',  1,    0),
    TEST ("e\0cb\0\0g", '\0',  5,    3),
    TEST ("\0ecb\0\0g", '\0',  0, NPOS),
    TEST ("e\0cb\0\0b",  'b',  7,    5),
    TEST ("e\0cb\0\0g",  'b',  4,    4),
    TEST ("e\0cbg\0\0",  'a',  6,    6),
    TEST ("\0cbee\0\0", '\0',  0, NPOS),
    TEST ("\0cbee\0\0", '\0',  6,    4),
    TEST ("\0cb\0\0\0", '\0',  9,    2),
    TEST ("aaaaaaaaaa",  'a',  6, NPOS),
    TEST ("\0aaaaaaaa",  'a',  6,    0),

    TEST ("x@4096",      'x',    0, NPOS),
    TEST ("x@4096",      'x', 4096, NPOS),
    TEST ("x@4096",     '\0', 4096, 4095),
    TEST ("x@4096",      'a', 4094, 4094),
    TEST ("x@4096",      'x', 4106, NPOS),
    TEST ("xxax@2048",   'x', 2050,    2),

    TEST ("last test",   't',  9,    7)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_find_last_not_of (charT, Traits*, Allocator*,
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

    const charT* const arg_ptr = tcase.arg ? s_arg.c_str () : s_str.c_str ();
    const String&      arg_str = tcase.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t size = tcase.size >= 0 ? tcase.size : s_arg.max_size () + 1;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == tcase.bthrow)
        expected = exceptions [2];

    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        std::size_t res = 0;

        switch (func.which_) {

        case FindLastNotOf (cptr):
            res = s_str.find_last_not_of (arg_ptr);
            break;

        case FindLastNotOf (cstr):
            res = s_str.find_last_not_of (arg_str);
            break;

        case FindLastNotOf (cptr_size):
            res = s_str.find_last_not_of (arg_ptr, tcase.off);
            break;

        case FindLastNotOf (cptr_size_size):
            res = s_str.find_last_not_of (arg_ptr, tcase.off, size);
            break;

        case FindLastNotOf (cstr_size):
            res = s_str.find_last_not_of (arg_str, tcase.off);
            break;

        case FindLastNotOf (val):
            res = s_str.find_last_not_of (arg_val);
            break;

        case FindLastNotOf (val_size):
            res = s_str.find_last_not_of (arg_val, tcase.off);
            break;

        default:
            RW_ASSERT ("test logic error: unknown find_last_not_of overload");
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

    catch (const std::length_error &ex) {
        caught = exceptions [2];
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
        str_state.assert_equal (rw_get_string_state (s_str),
                                __LINE__, tcase.line, caught);
    }
    else if (-1 != tcase.bthrow) {
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_find_last_not_of);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        FindLastNotOf (sig), sig ## _test_cases,                \
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
                               "lib.string.find.last.not.of",
                               test_find_last_not_of, tests, test_count);
}
