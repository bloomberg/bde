/***************************************************************************
 *
 * 21.string.find.first.not.of.cpp -
 *      string test exercising lib.string.find.first.not.of
 *
 * $Id: 21.string.find.first.not.of.cpp 580483 2007-09-28 20:55:52Z sebor $
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
#define FindFirstNotOf(sig)       StringIds::find_first_not_of_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// find_first_not_of (const value_type*)
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
    TEST ("ab",         "c",             0),

    TEST ("",           "",           NPOS),
    TEST ("",           "\0",         NPOS),
    TEST ("",           "a",          NPOS),

    TEST ("\0",         "",              0),
    TEST ("\0",         "\0",            0),
    TEST ("\0",         "a",             0),

    TEST ("bbcdefghij", "a",             0),
    TEST ("abcdefghij", "a",             1),
    TEST ("abcdefghij", "f",             0),

    TEST ("eaccbbhjig", "cba",           0),
    TEST ("ceabcbahca", "cba",           1),
    TEST ("bacbahjicg", "cba",           5),
    TEST ("abbcbacbca", "cba",        NPOS),
    TEST ("bcbedfbjih", "abce",          4),
    TEST ("bcaedfajih", "aabced",        5),
    TEST ("bcedfaacdh", "abcdef",        9),

    TEST ("e\0cb\0\0g", "b\0\0g",        0),
    TEST ("e\0cb\0\0g", "cbe",           1),
    TEST ("\0cb\0\0ge", "\0\0ge",        0),
    TEST ("bcbc\0\0be", "b\0c",          1),
    TEST ("gbg\0\0e\0", "bg",            3),
    TEST ("a\0b",       "e\0gbg\0\0",    0),
    TEST ("b\0a",       "eb\0gg\0\0",    1),
    TEST ("ab\0",       "ab\0gg\0\0",    2),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaaabaaaa", "cccccccccb",    0),
    TEST ("aabaabaaaa", "aaaaaaaaab", NPOS),
    TEST ("bbb",        "aaaaaaaaba", NPOS),
    TEST ("aab",        "aaaaaaaaaa",    2),

    TEST ("x@4096",     "",              0),
    TEST ("x@4096",     "a",             0),
    TEST ("x@4096",     "x",          NPOS),
    TEST ("x@4096",     "axa",        NPOS),
    TEST ("abc",        "x@4096",        0),
    TEST ("xabc",       "x@4096",        1),

    TEST ("abcdefghij", 0,            NPOS),
    TEST ("\0cb\0\0ge", 0,               0),
    TEST ("x@4096",     0,            NPOS),

    TEST ("last test",  "test",          0)
};

/**************************************************************************/

// used to exercise
// find_first_not_of (const basic_string&)
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
    TEST ("ab",         "c",             0),

    TEST ("",           "",           NPOS),
    TEST ("",           "\0",         NPOS),
    TEST ("",           "a",          NPOS),

    TEST ("\0",         "",              0),
    TEST ("\0",         "\0",         NPOS),
    TEST ("\0\0",       "\0",         NPOS),
    TEST ("\0",         "a",             0),

    TEST ("bbcdefghij", "a",             0),
    TEST ("abcdefghij", "a",             1),
    TEST ("abcdefghij", "f",             0),

    TEST ("eaccbbhjig", "cba",           0),
    TEST ("ceabcbahca", "cba",           1),
    TEST ("bacbahjicg", "cba",           5),
    TEST ("abbcbacbca", "cba",        NPOS),
    TEST ("bcbedfbjih", "abce",          4),
    TEST ("bcaedfajih", "aabced",        5),
    TEST ("bcedfaacdh", "abcdef",        9),

    TEST ("e\0cb\0\0g", "b\0\0g",        0),
    TEST ("\0cb\0\0ge", "b\0\0g",        1),
    TEST ("\0gb\0\0ge", "b\0\0g",        6),
    TEST ("e\0cb\0\0g", "cbe",           1),
    TEST ("\0cb\0\0ge", "\0\0ge",        1),
    TEST ("bcbc\0\0bc", "b\0c",       NPOS),
    TEST ("gbg\0\0e\0", "bg",            3),
    TEST ("a\0b",       "e\0gbg\0\0",    0),
    TEST ("b\0a",       "eb\0gg\0\0",    2),
    TEST ("ab\0",       "ab\0gg\0\0", NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa", NPOS),
    TEST ("aaaaabaaaa", "cccccccccb",    0),
    TEST ("aabaabaaaa", "aaaaaaaaab", NPOS),
    TEST ("bbb",        "aaaaaaaaba", NPOS),
    TEST ("aab",        "aaaaaaaaaa",    2),

    TEST ("x@4096",     "",              0),
    TEST ("x@4096",     "a",             0),
    TEST ("x@4096",     "x",          NPOS),
    TEST ("x@4096",     "axa",        NPOS),
    TEST ("abc",        "x@4096",        0),
    TEST ("xabc",       "x@4096",        1),

    TEST ("abcdefghij", 0,            NPOS),
    TEST ("\0cb\0\0ge", 0,            NPOS),
    TEST ("x@4096",     0,            NPOS),

    TEST ("last test",  "test",          0)
};

/**************************************************************************/

// used to exercise
// find_first_not_of (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)            \
    { __LINE__, off, -1, -1, -1, -1,        \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +---------------------------------- controlled sequence
    //    |            +--------------------- sequence to be found
    //    |            |               +----- find_first_not_of() off argument
    //    |            |               |     +- expected result
    //    |            |               |     |
    //    |            |               |     |
    //    V            V               V     V
    TEST ("ab",        "c",            0,    0),

    TEST ("",           "",            0, NPOS),
    TEST ("",           "\0",          0, NPOS),
    TEST ("",           "a",           0, NPOS),

    TEST ("\0",         "",            0,    0),
    TEST ("\0",         "\0",          0,    0),
    TEST ("\0",         "\0",          1, NPOS),
    TEST ("\0",         "a",           0,    0),

    TEST ("bbcdefghij", "a",           0,    0),
    TEST ("abcdefghij", "a",           0,    1),
    TEST ("bcaaaaaaaa", "a",           2, NPOS),
    TEST ("bcaaafaaaa", "a",           2,    5),
    TEST ("abcdefghij", "j",           9, NPOS),

    TEST ("cbbedfhjig", "cba",         0,    3),
    TEST ("edfcbahjig", "cba",         4,    6),
    TEST ("edfcbaaabc", "cba",         6, NPOS),
    TEST ("edfcbahcba", "cba",         9, NPOS),
    TEST ("cbacbabjig", "cbaig",       0,    7),
    TEST ("bcaedfajih", "aabced",      0,    5),
    TEST ("bcedfaacdh", "abcdef",      3,    9),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    0),
    TEST ("\0cbe\0\0g", "b\0\0g",      4,    4),
    TEST ("e\0cb\0\0g", "cbe",         0,    1),
    TEST ("\0cb\0\0ge", "\0\0ge",      0,    0),
    TEST ("\0cb\0\0ge", "cb\0",        6,    6),
    TEST ("e\0gbg\0\0", "bg",          2,    5),
    TEST ("a\0b",       "a\0gbg\0\0",  0,    1),
    TEST ("ab\0",       "ab\0gg\0\0",  1,    2),
    TEST ("a\0b",       "e\0gg\0\0a",  3, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, NPOS),
    TEST ("aaabaaaaaa", "aaaaaaaaaa",  4, NPOS),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,    5),
    TEST ("aabaabaaaa", "aaaaaaaaaa",  0,    2),
    TEST ("aabaabaaaa", "aaaaaaaaaa",  3,    5),
    TEST ("aabaabaaaa", "aaaaaaaaab",  0, NPOS),
    TEST ("cbb",        "aaaaaaaaba",  2, NPOS),
    TEST ("bac",        "aaaaaaaaba",  0,    2),
    TEST ("baa",        "aaaaaaaaaa",  0,    0),

    TEST ("x@4096",     "",            0,    0),
    TEST ("x@4096",     "a",           0,    0),
    TEST ("x@4096",     "x",           0, NPOS),
    TEST ("x@4096",     "xxx",        10, NPOS),
    TEST ("x@4096",     "axa",        10, NPOS),
    TEST ("abc",        "x@4096",      2,    2),
    TEST ("xxxxxxxxxx", "x@4096",      0, NPOS),

    TEST ("x@4096",     "xxx",      4094, NPOS),
    TEST ("x@4096",     "xxx",      4095, NPOS),
    TEST ("x@2048axxx", "x",           1, 2048),

    TEST ("abcdefghij", 0,             0, NPOS),
    TEST ("abcdefghij", 0,             1, NPOS),
    TEST ("\0cb\0\0ge", 0,             5,    5),
    TEST ("x@4096",     0,             0, NPOS),
    TEST ("x@4096",     0,             7, NPOS),

    TEST ("",           "",            1, NPOS),
    TEST ("abcdefghij", "abc",        10, NPOS),
    TEST ("abcdefghij", "cba",        10, NPOS),

    TEST ("last test", "test",         0,    0)
};

/**************************************************************************/

// used to exercise
// find_first_not_of (const value_type*, size_type, size_type)
static const StringTestCase
cptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)      \
    { __LINE__, off, size, -1, -1, -1,              \
      str, sizeof str - 1, arg,                     \
      sizeof arg - 1, 0, res, bthrow                \
    }

    //    +---------------------------------- controlled sequence
    //    |            +--------------------- sequence to be found
    //    |            |               +----- find_first_not_of() off argument
    //    |            |               |   +- find_first_not_of() n argument
    //    |            |               |   |     +- expected result
    //    |            |               |   |     |  exception info
    //    |            |               |   |     |  |   0 - no exception
    //    |            |               |   |     |  |   1 - length_error
    //    |            |               |   |     |  |
    //    |            |               |   |     |  +-----+
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

    TEST ("cbbedfhjig", "cba",         0,  3,    3,       0),
    TEST ("edfcbbhjig", "bac",         0,  2,    0,       0),
    TEST ("edfcbahaib", "cba",         7,  3,    8,       0),
    TEST ("edfcbahbcb", "cba",         7,  2, NPOS,       0),
    TEST ("edfcbahbcb", "cba",         3,  2,    5,       0),
    TEST ("edfcbahcba", "bac",         3,  3,    6,       0),
    TEST ("edacbehcba", "abc",         2,  2,    3,       0),
    TEST ("babcbahcba", "cba",         0,  3,    6,       0),
    TEST ("hjigcbacba", "cba",         4,  3, NPOS,       0),
    TEST ("cbacbcccbc", "cba",         5,  1,    8,       0),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  4,    0,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  4, NPOS,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  1,    4,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  2,    6,       0),
    TEST ("\0b\0\0gb\0","bg\0",        2,  2,    2,       0),
    TEST ("\0b\0\0gb\0","bg\0",        0,  2,    0,       0),
    TEST ("\0b\0\0gb\0","bg\0",        0,  3, NPOS,       0),
    TEST ("e\0cb\0\0g", "a\0e",        0,  3,    2,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      7,  4, NPOS,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  0,    6,       0),
    TEST ("a\0b",       "e\0gbg\0\0",  0,  1,    0,       0),
    TEST ("a\0b",       "ab\0gg\0\0",  1,  2,    1,       0),
    TEST ("a\0b",       "\0ba\0\0fe",  1,  2, NPOS,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  3,  6, NPOS,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  0,  7,    2,       0),
    TEST ("\0baa",      "b\0g\0\0ac",  0,  4,    2,       0),

    TEST ("e\0a\0",     "e\0a\0\0",    0,  4, NPOS,       0),
    TEST ("\0\0ea",     "b\0c\0\0",    0,  5,    2,       0),
    TEST ("ee\0b\0",    "e\0a\0b",     1,  4,    3,       0),
    TEST ("be\0a\0",    "\0acefdg",    1,  5, NPOS,       0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, 10, NPOS,       0),
    TEST ("baaaaaaaaa", "aaaaaaaaaa",  1, 10, NPOS,       0),
    TEST ("aaaaabaaaa", "cccccccccc",  0, 10,    0,       0),
    TEST ("aaaaabaaaa", "aaaaaccbcc",  1,  4,    5,       0),
    TEST ("aaaabaaaaa", "cccca",       0,  5,    4,       0),
    TEST ("aabaabadca", "acaaacccab",  0,  5,    2,       0),
    TEST ("abcdefghij", "jihgfedcba",  0, 10, NPOS,       0),
    TEST ("abcdefghij", "jihgfedcba",  0,  9,    0,       0),
    TEST ("bbb",        "aaaaaaaaba",  2,  2,    2,       0),
    TEST ("bbb",        "aaaaaaaaba",  0,  9, NPOS,       0),
    TEST ("bab",        "baaaaaaaaa",  0,  0,    0,       0),
    TEST ("bab",        "ccccccccba",  1, 10, NPOS,       0),

    TEST ("x@4096",     "",            0,  0,    0,       0),
    TEST ("x@4096",     "a",           0,  1,    0,       0),
    TEST ("x@4096",     "x",           0,  1, NPOS,       0),
    TEST ("x@4096",     "xxx",        10,  3, NPOS,       0),
    TEST ("x@4096",     "axx",        10,  1,   10,       0),
    TEST ("x@4096",     "xxa",        10,  0,   10,       0),
    TEST ("x@4096",     "xxa",        10,  1, NPOS,       0),
    TEST ("abc",        "x@4096",      2, 10,    2,       0),
    TEST ("xxxxxxxxxx", "x@4096",      0, 4096, NPOS,     0),
    TEST ("xxxxxxxxxx", "x@4096",      2,  4, NPOS,       0),
    TEST ("xxxxxxxxxa", "x@4096",      0,  4,    9,       0),

    TEST ("x@4096",     "xxx",      4094,  3, NPOS,       0),
    TEST ("x@4096",     "xxx",      4093,  0, 4093,       0),

    TEST ("abcdefghij", 0,             0, 10, NPOS,       0),
    TEST ("abcdefghij", 0,             1,  9,    9,       0),
    TEST ("abcdefghij", 0,             0,  8,    8,       0),
    TEST ("\0cb\0\0ge", 0,             5,  7, NPOS,       0),
    TEST ("\0cb\0\0ge", 0,             5,  5,    5,       0),
    TEST ("\0cb\0\0cb", 0,             3,  5, NPOS,       0),
    TEST ("\0cb\0ge\0", 0,             6,  1, NPOS,       0),
    TEST ("x@4096",     0,             0, 4096, NPOS,     0),
    TEST ("x@4096",     0,          4096, 4096, NPOS,     0),
    TEST ("x@4096",     0,          4095,  1,   NPOS,     0),

    TEST ("",           "",            1,  0, NPOS,       0),
    TEST ("abcdefghij", "abc",        10,  3, NPOS,       0),
    TEST ("abcdefghij", "cba",        10,  1, NPOS,       0),

    TEST ("",           "cba",         0, -1, NPOS,       0),

    TEST ("last test", "test",         0,  4,    0,       0)
};

/**************************************************************************/

// used to exercise
// find_first_not_of (const basic_string&, size_type)
static const StringTestCase
cstr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)            \
    { __LINE__, off, -1, -1, -1, -1,        \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +---------------------------------- controlled sequence
    //    |             +-------------------- sequence to be found
    //    |             |              +----- find_first_not_of() off argument
    //    |             |              |     +- expected result
    //    |             |              |     |
    //    |             |              |     |
    //    V             V              V     V
    TEST ("ab",         "c",           0,    0),

    TEST ("",           "",            0, NPOS),
    TEST ("",           "\0",          0, NPOS),
    TEST ("",           "a",           0, NPOS),

    TEST ("\0",         "",            0,    0),
    TEST ("\0",         "\0",          0, NPOS),
    TEST ("\0",         "\0",          1, NPOS),
    TEST ("\0\0",       "\0",          0, NPOS),
    TEST ("\0",         "a",           0,    0),

    TEST ("bbcdefghij", "a",           0,    0),
    TEST ("abcdefghij", "a",           0,    1),
    TEST ("bcaaaaaaaa", "a",           2, NPOS),
    TEST ("bcaaafaaaa", "a",           2,    5),
    TEST ("abcdefghij", "j",           9, NPOS),

    TEST ("cbbedfhjig", "cba",         0,    3),
    TEST ("edfcbahjig", "cba",         4,    6),
    TEST ("edfcbaaabc", "cba",         6, NPOS),
    TEST ("edfcbahcba", "cba",         9, NPOS),
    TEST ("cbacbabjig", "cbaig",       0,    7),
    TEST ("bcaedfajih", "aabced",      0,    5),
    TEST ("bcedfaacdh", "abcdef",      3,    9),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    0),
    TEST ("\0cbe\0\0g", "b\0\0g",      4, NPOS),
    TEST ("e\0cb\0\0g", "cbe",         0,    1),
    TEST ("\0ge\0\0cb", "\0\0ge",      0,    5),
    TEST ("\0cb\0\0ge", "cb\0",        6,    6),
    TEST ("\0cb\0\0ce", "cb\0",        0,    6),
    TEST ("e\0gbg\0\0", "bg",          2,    5),
    TEST ("a\0c",       "a\0gbg\0\0",  0,    2),
    TEST ("a\0b",       "a\0gbg\0\0",  0, NPOS),
    TEST ("ab\0",       "ab\0gg\0\0",  1, NPOS),
    TEST ("a\0b",       "e\0gg\0\0a",  3, NPOS),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, NPOS),
    TEST ("aaabaaaaaa", "aaaaaaaaaa",  4, NPOS),
    TEST ("aaaaabaaaa", "aaaaaaaaaa",  1,    5),
    TEST ("aabaabaaaa", "aaaaaaaaaa",  0,    2),
    TEST ("aabaabaaaa", "aaaaaaaaaa",  3,    5),
    TEST ("aabaabaaaa", "aaaaaaaaab",  0, NPOS),
    TEST ("cbb",        "aaaaaaaaba",  2, NPOS),
    TEST ("bac",        "aaaaaaaaba",  0,    2),
    TEST ("baa",        "aaaaaaaaaa",  0,    0),

    TEST ("x@4096",     "",            0,    0),
    TEST ("x@4096",     "a",           0,    0),
    TEST ("x@4096",     "x",           0, NPOS),
    TEST ("x@4096",     "xxx",        10, NPOS),
    TEST ("x@4096",     "axa",        10, NPOS),
    TEST ("abc",        "x@4096",      2,    2),
    TEST ("xxxxxxxxxx", "x@4096",      0, NPOS),

    TEST ("x@4096",     "xxx",      4094, NPOS),
    TEST ("x@4096",     "xxx",      4096, NPOS),
    TEST ("x@2048axxx", "xxx",        10, 2048),

    TEST ("abcdefghij", 0,             0, NPOS),
    TEST ("abcdefghij", 0,             1, NPOS),
    TEST ("\0cb\0\0ge", 0,             5, NPOS),
    TEST ("x@4096",     0,             0, NPOS),
    TEST ("x@4096",     0,             7, NPOS),

    TEST ("",           "",            1, NPOS),
    TEST ("abcdefghij", "abc",        10, NPOS),
    TEST ("abcdefghij", "cba",        10, NPOS),

    TEST ("last test",  "test",        0,    0)
};

/**************************************************************************/

// used to exercise
// find_first_not_of (value_type)
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
    TEST ("ab",          'c',    0),

    TEST ("",            'a', NPOS),
    TEST ("",           '\0', NPOS),

    TEST ("\0",         '\0', NPOS),
    TEST ("\0\0",       '\0', NPOS),
    TEST ("\0",          'a',    0),
    TEST ("a\0",         'a',    1),

    TEST ("e\0cb\0\0g", '\0',    0),
    TEST ("\0ecb\0\0g", '\0',    1),
    TEST ("bbb\0cb\0e",  'b',    3),
    TEST ("\0ecb\0\0g",  'a',    0),
    TEST ("\0cbge\0\0", '\0',    1),

    TEST ("x@4096",      'x', NPOS),
    TEST ("x@4096",     '\0',    0),
    TEST ("x@4096",      'a',    0),

    TEST ("last test",   't',    0)
};

/**************************************************************************/

// used to exercise
// find_first_not_of (value_type, size_type)
static const StringTestCase
val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res)          \
    { __LINE__, off, -1, -1, -1,          \
      val, str, sizeof str - 1,           \
      0, 0, 0, res, 0                     \
    }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- find_first_not_of() off argument
    //    |              |     |     +--- expected result
    //    |              |     |     |
    //    |              |     |     |
    //    V              V     V     V
    TEST ("ab",          'c',  0,    0),

    TEST ("",            'a',  0, NPOS),
    TEST ("",           '\0',  0, NPOS),

    TEST ("\0",         '\0',  1, NPOS),
    TEST ("\0",          'a',  0,    0),
    TEST ("\0\0",       '\0',  1, NPOS),
    TEST ("\0\0",        'a',  3, NPOS),
    TEST ("\0\0",        'a',  1,    1),
    TEST ("\0\0",       '\0',  3, NPOS),

    TEST ("e\0cb\0\0g", '\0',  1,    2),
    TEST ("ecb\0\0\0g", '\0',  3,    6),
    TEST ("e\0cbg\0\0", '\0',  5, NPOS),
    TEST ("eb\0\0\0cg",  'b',  1,    2),
    TEST ("e\0\0\0bbb",  'b',  4, NPOS),
    TEST ("e\0cb\0\0g",  'a',  0,    0),
    TEST ("\0cbge\0\0", '\0',  0,    1),
    TEST ("\0\0\0cbge", '\0',  0,    3),
    TEST ("\0cbge\0\0", '\0',  9, NPOS),

    TEST ("x@4096",      'x',  0, NPOS),
    TEST ("x@4096",      'x',  5, NPOS),
    TEST ("x@4096",     '\0',  0,    0),
    TEST ("x@4096",      'a',  3,    3),
    TEST ("x@4096",      'x', 4096, NPOS),
    TEST ("x@4096",      'x', 4095, NPOS),
    TEST ("x@2048axxx",  'x',  1, 2048),

    TEST ("last test",   't',  0,    0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_find_first_not_of (charT, Traits*, Allocator*,
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
        case FindFirstNotOf (cptr):
            res = s_str.find_first_not_of (arg_ptr);
            break;

        case FindFirstNotOf (cstr):
            res = s_str.find_first_not_of (arg_str);
            break;

        case FindFirstNotOf (cptr_size):
            res = s_str.find_first_not_of (arg_ptr, tcase.off);
            break;

        case FindFirstNotOf (cptr_size_size):
            res = s_str.find_first_not_of (arg_ptr, tcase.off, size);
            break;

        case FindFirstNotOf (cstr_size):
            res = s_str.find_first_not_of (arg_str, tcase.off);
            break;

        case FindFirstNotOf (val):
            res = s_str.find_first_not_of (arg_val);
            break;

        case FindFirstNotOf (val_size):
            res = s_str.find_first_not_of (arg_val, tcase.off);
            break;

        default:
            RW_ASSERT (!"logic error: unknown find_first_not_of overload");
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

DEFINE_STRING_TEST_DISPATCH (test_find_first_not_of);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        FindFirstNotOf (sig), sig ## _test_cases,               \
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
                               "lib.string.find.first.not.of",
                               test_find_first_not_of, tests, test_count);
}
