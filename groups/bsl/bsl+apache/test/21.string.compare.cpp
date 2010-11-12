/***************************************************************************
 *
 * 21.string.compare.cpp - string test exercising lib.string.compare
 *
 * $Id: 21.string.compare.cpp 590052 2007-10-30 12:44:14Z faridz $
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
#include <cstddef>          // for size_t
#include <stdexcept>        // for out_of_range, length_error

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_assert()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define Compare(sig)              StringIds::compare_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// compare (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {           \
        __LINE__, -1, -1, -1, -1, -1,           \
        str, sizeof str - 1,                    \
        arg, sizeof arg - 1, 0, res, bthrow     \
    }

    //    +------------------------------------------ controlled sequence
    //    |                  +----------------------- sequence to be inserted
    //    |                  |                +------ expected result
    //    |                  |                |       +-- exception info
    //    |                  |                |       |     0   - no exception
    //    |                  |                |       |     1,2 - out_of_range
    //    |                  |                |       |     3   - length_error
    //    |                  |                |       |
    //    |                  |                |       |
    //    V                  V                V       V
    TEST ("ab",              "ab",            0,      0),

    TEST ("",                "",              0,      0),
    TEST ("",                "\0",            0,      0),
    TEST ("",                "a",          NPOS,      0),

    TEST ("\0",              "",              1,      0),
    TEST ("\0",              "\0",            1,      0),
    TEST ("\0",              "a",          NPOS,      0),

    TEST ("abcdefghij",      "abcdefghij",    0,      0),
    TEST ("abcdefghij",      "abcdefghi",     1,      0),
    TEST ("abcdefghi",       "abcdefghij", NPOS,      0),

    TEST ("abbdefghij",      "abcdefghij", NPOS,      0),
    TEST ("abcdeeghij",      "abcdefghij", NPOS,      0),
    TEST ("abcdefghii",      "abcdefghij", NPOS,      0),

    TEST ("bbcdefghij",      "a",             1,      0),
    TEST ("eeeeeeghij",      "aeeee",         1,      0),
    TEST ("a",               "bbcdefghij", NPOS,      0),
    TEST ("aeeee",           "eeeeeeghij", NPOS,      0),

    TEST ("bbcdefghij",      "abcdefghij",    1,      0),
    TEST ("abcdffghij",      "abcdefghij",    1,      0),
    TEST ("abcdefghjj",      "abcdefghij",    1,      0),

    TEST ("a\0b\0\0c",       "a\0b\0\0c",     1,      0),
    TEST ("abc\0\0\0",       "abc\0\0\0",     1,      0),
    TEST ("\0ab\0\0c",       "e\0ab\0\0c", NPOS,      0),

    TEST ("a\0b",            "a\0b\0\0c",     1,      0),
    TEST ("ab\0",            "abc\0\0\0",  NPOS,      0),
    TEST ("\0ab",            "e\0ab\0\0c", NPOS,      0),

    TEST ("x@4096",          "x@4096",        0,      0),
    TEST ("xx",              "x@4096",     NPOS,      0),
    TEST ("x@4096",          "xxxxxx",        1,      0),
    TEST ("a\0b",            "x@4096",     NPOS,      0),
    TEST ("x@4096",          "a\0b",          1,      0),

    TEST ("a@2048b@2048",    "a@2048b@2047",  1,      0),
    TEST ("a@2048b@2048",    "a@2048b@2047c", NPOS,   0),
    TEST ("a@2048cb@2047",   "a@2048cb@2047", 0,      0),

    TEST ("last",            "last",          0,      0)
};

/**************************************************************************/

// exercises:
// compare (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {           \
        __LINE__, -1, -1, -1, -1, -1,           \
        str, sizeof str - 1,                    \
        arg, sizeof arg - 1, 0, res, bthrow     \
    }

    //    +------------------------------------------- controlled sequence
    //    |                  +------------------------ sequence to be compared
    //    |                  |                +------- expected result
    //    |                  |                |       +- exception info
    //    |                  |                |       |   0   - no exception
    //    |                  |                |       |   1,2 - out_of_range
    //    |                  |                |       |   3   - length_error
    //    |                  |                |       |
    //    |                  |                |       |
    //    V                  V                V       V
    TEST ("ab",              "ab",            0,      0),

    TEST ("",                "",              0,      0),
    TEST ("",                "\0",         NPOS,      0),
    TEST ("",                "a",          NPOS,      0),

    TEST ("\0",              "",              1,      0),
    TEST ("\0",              "\0",            0,      0),
    TEST ("\0",              "a",          NPOS,      0),

    TEST ("abcdefghij",      "abcdefghij",    0,      0),
    TEST ("abcdefghij",      "abcdefghi",     1,      0),
    TEST ("abcdefghi",       "abcdefghij", NPOS,      0),

    TEST ("abbdefghij",      "abcdefghij", NPOS,      0),
    TEST ("abcdeeghij",      "abcdefghij", NPOS,      0),
    TEST ("abcdefghii",      "abcdefghij", NPOS,      0),

    TEST ("bbcdefghij",      "a",             1,      0),
    TEST ("eeeeeeghij",      "aeeee",         1,      0),
    TEST ("a",               "bbcdefghij", NPOS,      0),
    TEST ("aeeee",           "eeeeeeghij", NPOS,      0),

    TEST ("bbcdefghij",      "abcdefghij",    1,      0),
    TEST ("abcdffghij",      "abcdefghij",    1,      0),
    TEST ("abcdefghjj",      "abcdefghij",    1,      0),

    TEST ("a\0b\0\0c",       "a\0b\0\0c",     0,      0),
    TEST ("abc\0\0\0",       "bac\0\0\0",  NPOS,      0),
    TEST ("\0ab\0\0c",       "e\0ab\0\0c", NPOS,      0),
    TEST ("\0ab\0\0c",       "\0ab\0\0b",     1,      0),

    TEST ("a\0b",            "a\0b\0\0c",  NPOS,      0),
    TEST ("ab\0",            "abc\0\0\0",  NPOS,      0),
    TEST ("\0ab",            "e\0ab\0\0c", NPOS,      0),
    TEST ("bb\0",            "ab\0\0\0c",     1,      0),
    TEST ("\0ac",            "\0abe\0\0c",    1,      0),

    TEST ("x@4096",          "x@4096",        0,      0),
    TEST ("xx",              "x@4096",     NPOS,      0),
    TEST ("x@4096",          "xxxxxx",        1,      0),
    TEST ("a\0b",            "x@4096",     NPOS,      0),
    TEST ("x@4096",          "a\0b",          1,      0),

    TEST ("a@2048b@2048",    "a@2048b@2047",  1,      0),
    TEST ("a@2048b@2048",    "a@2048b@2047c", NPOS,   0),
    TEST ("a@2048cb@2047",   "a@2048cb@2047", 0,      0),

    TEST ("last",            "last",          0,      0)
};

/**************************************************************************/

// exercises:
// compare (size_type, size_type, const value_type*)
static const StringTestCase
size_size_cptr_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, res, bthrow) {        \
        __LINE__, off, size, -1, -1, -1,                \
        str, sizeof str - 1,                            \
        arg, sizeof arg - 1, 0, res, bthrow             \
    }

    //    +------------------------------------------ controlled sequence
    //    |              +--------------------------- compare() off argument
    //    |              |   +----------------------- compare() n1 argument
    //    |              |   |  +-------------------- sequence to be compared
    //    |              |   |  |                +--- expected result
    //    |              |   |  |                |   +- exception info
    //    |              |   |  |                |   |    0   - no exception
    //    |              |   |  |                |   |    1,2 - out_of_range
    //    |              |   |  |                |   |    3   - length_error
    //    |              |   |  |                |   |
    //    |              |   |  |                |   |
    //    V              V   V  V                V   V
    TEST ("ab",          0,  2, "ab",            0,  0),

    TEST ("",            0,  0, "",              0,  0),
    TEST ("",            0,  0, "\0",            0,  0),
    TEST ("",            0,  0, "a",          NPOS,  0),

    TEST ("\0",          0,  1, "",              1,  0),
    TEST ("\0",          0,  1, "\0",            1,  0),
    TEST ("\0",          0,  1, "a",          NPOS,  0),

    TEST ("abcdefghij",  0, 10, "abcdefghij",    0,  0),
    TEST ("abcdefghij",  0, 10, "abcdefghi",     1,  0),
    TEST ("abcdefghij",  1,  9, "abcdefghi",     1,  0),
    TEST ("abcdefghij",  1,  8, "abcdefghi",     1,  0),
    TEST ("abcdefghij",  2,  5, "abcdefghi",     1,  0),
    TEST ("abcdefghi",   0,  9, "abcdefghij", NPOS,  0),

    TEST ("abbdefghij",  0, 10, "abcdefghij", NPOS,  0),
    TEST ("abbdefghij",  1,  9, "abcdefghij",    1,  0),
    TEST ("abcdeeghij",  0, 10, "abcdefghij", NPOS,  0),
    TEST ("abcdeeghij",  5, 10, "abcdefghij",    1,  0),
    TEST ("abcdeeghij",  0,  5, "abcdefghij", NPOS,  0),
    TEST ("abcdefghii",  0, 10, "abcdefghij", NPOS,  0),
    TEST ("abcdefghii",  9, 10, "abcdefghij",    1,  0),

    TEST ("abcdefghij",  1,  1, "b",             0,  0),
    TEST ("abcdefghij",  0,  1, "b",          NPOS,  0),
    TEST ("abcdefghij",  1,  4, "bcde",          0,  0),
    TEST ("abcdefghij",  1,  3, "bcde",       NPOS,  0),

    TEST ("bbcdefghij",  2,  4, "a",             1,  0),
    TEST ("bbcdefghij",  2,  0, "a",          NPOS,  0),
    TEST ("aeeeeeghij",  6,  1, "aeeee",         1,  0),
    TEST ("aeeeeeghij",  0,  2, "aeeee",      NPOS,  0),
    TEST ("a",           0,  2, "bbcdefghij", NPOS,  0),
    TEST ("jeeee",       1,  2, "eeeeeeghij", NPOS,  0),

    TEST ("a\0b\0\0c",   0,  6, "a\0b\0\0c",     1,  0),
    TEST ("a\0b\0\0c",   0,  1, "a\0b\0\0c",     0,  0),
    TEST ("abc\0\0\0",   0,  4, "abc\0\0\0",     1,  0),
    TEST ("abc\0\0\0",   0,  3, "abc\0\0\0",     0,  0),
    TEST ("\0af\0\0c",   0,  6, "e\0ab\0\0c", NPOS,  0),
    TEST ("\0af\0\0c",   2,  2, "e\0ab\0\0c",    1,  0),

    TEST ("a\0b",        0,  3, "a\0b\0\0c",     1,  0),
    TEST ("a\0b",        0,  1, "a\0b\0\0c",     0,  0),
    TEST ("ab\0",        0,  5, "abc\0\0\0",  NPOS,  0),
    TEST ("\0eb",        0,  5, "e\0ab\0\0c", NPOS,  0),
    TEST ("\0eb",        1,  5, "e\0ab\0\0c",    1,  0),

    TEST ("x@4096",    0, 4096, "x@4096",        0,  0),
    TEST ("x@4096",    1, 4096, "x@4096",     NPOS,  0),
    TEST ("xx",          0,  2, "x@4096",     NPOS,  0),
    TEST ("x@4096",    0, 4096, "xxxxx",         1,  0),
    TEST ("x@4096",      2,  5, "xxxxx",         0,  0),
    TEST ("x@4096",      5,  2, "xxxxx",      NPOS,  0),
    TEST ("a\0x",        0,  3, "x@4096",     NPOS,  0),
    TEST ("x@4096",      0, 10, "x\0b",          1,  0),
    TEST ("x@4096",      0,  1, "x\0b",          0,  0),

    TEST ("\0",          2,  0, "",              1,  1),
    TEST ("a",          10,  0, "",              1,  1),
    TEST ("x@4096",   4106,  0, "",              1,  1),

    TEST ("last",        0,  4, "last",          0,  0)
};

/**************************************************************************/

// exercises:
// compare (size_type, size_type, basic_string&)
static const StringTestCase
size_size_cstr_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, res, bthrow) {        \
        __LINE__, off, size, -1, -1, -1,                \
        str, sizeof str - 1,                            \
        arg, sizeof arg - 1, 0, res, bthrow             \
    }

    //    +------------------------------------------ controlled sequence
    //    |              +--------------------------- compare() off argument
    //    |              |   +----------------------- compare() n1 argument
    //    |              |   |  +-------------------- sequence to be compared
    //    |              |   |  |                +--- expected result
    //    |              |   |  |                |   +- exception info
    //    |              |   |  |                |   |    0   - no exception
    //    |              |   |  |                |   |    1,2 - out_of_range
    //    |              |   |  |                |   |    3   - length_error
    //    |              |   |  |                |   |
    //    |              |   |  |                |   |
    //    V              V   V  V                V   V
    TEST ("ab",          0,  2, "ab",            0,  0),

    TEST ("",            0,  0, "",              0,  0),
    TEST ("",            0,  0, "\0",         NPOS,  0),
    TEST ("",            0,  0, "a",          NPOS,  0),

    TEST ("\0",          0,  1, "",              1,  0),
    TEST ("\0",          0,  1, "\0",            0,  0),
    TEST ("\0",          0,  1, "\0\0",       NPOS,  0),
    TEST ("\0",          0,  1, "a",          NPOS,  0),

    TEST ("abcdefghij",  0, 10, "abcdefghij",    0,  0),
    TEST ("abcdefghij",  0, 10, "abcdefghi",     1,  0),
    TEST ("abcdefghij",  1,  9, "abcdefghi",     1,  0),
    TEST ("abcdefghij",  1,  8, "abcdefghi",     1,  0),
    TEST ("abcdefghij",  2,  5, "abcdefghi",     1,  0),
    TEST ("abcdefghi",   0,  9, "abcdefghij", NPOS,  0),

    TEST ("abbdefghij",  0, 10, "abcdefghij", NPOS,  0),
    TEST ("abbdefghij",  1,  9, "abcdefghij",    1,  0),
    TEST ("abcdeeghij",  0, 10, "abcdefghij", NPOS,  0),
    TEST ("abcdeeghij",  5, 10, "abcdefghij",    1,  0),
    TEST ("abcdeeghij",  0,  5, "abcdefghij", NPOS,  0),
    TEST ("abcdefghii",  0, 10, "abcdefghij", NPOS,  0),
    TEST ("abcdefghii",  9, 10, "abcdefghij",    1,  0),

    TEST ("abcdefghij",  1,  1, "b",             0,  0),
    TEST ("abcdefghij",  0,  1, "b",          NPOS,  0),
    TEST ("abcdefghij",  1,  4, "bcde",          0,  0),
    TEST ("abcdefghij",  1,  3, "bcde",       NPOS,  0),

    TEST ("bbcdefghij",  2,  4, "a",             1,  0),
    TEST ("bbcdefghij",  2,  0, "a",          NPOS,  0),
    TEST ("aeeeeeghij",  6,  1, "aeeee",         1,  0),
    TEST ("aeeeeeghij",  0,  2, "aeeee",      NPOS,  0),
    TEST ("a",           0,  2, "bbcdefghij", NPOS,  0),
    TEST ("jeeee",       1,  2, "eeeeeeghij", NPOS,  0),

    TEST ("a\0b\0\0c",   0,  6, "a\0b\0\0c",     0,  0),
    TEST ("a\0b\0\0c",   2,  4, "a\0b\0\0c",     1,  0),
    TEST ("e\0b\0\0c",   0,  1, "a\0b\0\0c",     1,  0),
    TEST ("abc\0\0\0",   0,  4, "abc\0\0\0",  NPOS,  0),
    TEST ("ab\0c\0\0",   2,  2, "ab\0b\0\0",  NPOS,  0),
    TEST ("abc\0\0\0",   3,  3, "abc\0\0\0",  NPOS,  0),
    TEST ("\0af\0\0c",   0,  6, "e\0ab\0\0c", NPOS,  0),
    TEST ("\0af\0\0c",   0,  1, "\0ab\0\0ce", NPOS,  0),
    TEST ("\0af\0\0c",   2,  2, "e\0ab\0\0c",    1,  0),
    TEST ("\0ae\0\0c",   2,  4, "e\0\0b\0ca",    1,  0),

    TEST ("a\0b",        0,  3, "a\0b\0\0c",  NPOS,  0),
    TEST ("a\0b",        0,  1, "a\0b\0\0c",  NPOS,  0),
    TEST ("ab\0",        0,  5, "abc\0\0\0",  NPOS,  0),
    TEST ("\0eb",        0,  5, "e\0ab\0\0c", NPOS,  0),
    TEST ("a\0b\0\0c",   0,  3, "a\0b",          0,  0),
    TEST ("a\0b\0\0c",   0,  1, "a",             0,  0),
    TEST ("abc\0\0\0",   0,  5, "abc\0\0",       0,  0),
    TEST ("e\0ab\0\0c",  0,  7, "e\0ab\0\0c",    0,  0),
    TEST ("a\0b\0\0c",   2,  3, "a\0b",          1,  0),
    TEST ("a\0b\0\0c",   2,  1, "a",             1,  0),
    TEST ("abc\0\0\0",   2,  5, "abc\0\0",       1,  0),
    TEST ("e\0ag\0\0c",  3,  6, "e\0ab\0\0c",    1,  0),

    TEST ("x@4096",    0, 4096, "x@4096",        0,  0),
    TEST ("x@4096",    1, 4096, "x@4096",     NPOS,  0),
    TEST ("xx",          0,  2, "x@4096",     NPOS,  0),
    TEST ("x@4096",    0, 4096, "xxxxx",         1,  0),
    TEST ("x@4096",      2,  5, "xxxxx",         0,  0),
    TEST ("x@4096",      5,  2, "xxxxx",      NPOS,  0),
    TEST ("a\0x",        0,  3, "x@4096",     NPOS,  0),
    TEST ("x@4096",      0, 10, "x\0b",          1,  0),
    TEST ("x@4096",      0,  1, "x\0b",       NPOS,  0),
    TEST ("a@4096\0", 4096,  1, "\0",            0,  0),

    TEST ("\0",          2,  0, "",              1,  1),
    TEST ("a",          10,  0, "",              1,  1),
    TEST ("x@4096",   4106,  0, "",              1,  1),

    TEST ("last",        0, 4, "last",           0,  0)
};

/**************************************************************************/

// exercises:
// compare (size_type, size_type, const value_type*, size_type)
static const StringTestCase
size_size_cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, size2, res, bthrow) { \
        __LINE__, off, size, -1, size2, -1,             \
        str, sizeof str - 1,                            \
        arg, sizeof arg - 1, 0, res, bthrow             \
    }

    //    +------------------------------------------- controlled sequence
    //    |              +---------------------------- compare() off argument
    //    |              |   +------------------------ compare() n1 argument
    //    |              |   |  +--------------------- sequence to be compared
    //    |              |   |  |              +------ compare() n2 argument
    //    |              |   |  |              |     +----- expected result
    //    |              |   |  |              |     |   +- exception info
    //    |              |   |  |              |     |   |  0   - no exception
    //    |              |   |  |              |     |   |  1,2 - out_of_range
    //    |              |   |  |              |     |   |  3   - length_error
    //    |              |   |  |              |     |   |
    //    |              |   |  |              |     |   |
    //    V              V   V  V              V     V   V
    TEST ("ab",          0,  2, "ab",          2,    0,  0),

    TEST ("",            0,  0, "",            0,    0,  0),
    TEST ("",            0,  0, "\0",          1, NPOS,  0),
    TEST ("",            0,  0, "a",           0,    0,  0),
    TEST ("",            0,  0, "a",           1, NPOS,  0),

    TEST ("\0",          0,  1, "",            0,    1,  0),
    TEST ("\0",          0,  1, "\0",          1,    0,  0),
    TEST ("\0",          0,  1, "\0\0",        2, NPOS,  0),
    TEST ("\0",          0,  1, "\0\0",        1,    0,  0),
    TEST ("\0\0",        1,  1, "\0",          1,    0,  0),
    TEST ("\0",          0,  1, "a",           1, NPOS,  0),

    TEST ("abcdefghij",  0, 10, "abcdefghij", 10,    0,  0),
    TEST ("abcdefghij",  0, 10, "abcdefghi",   9,    1,  0),
    TEST ("abcdefghij",  1,  9, "abcdefghi",   9,    1,  0),
    TEST ("abcdefghij",  1,  8, "abcdefghi",   9,    1,  0),
    TEST ("abcdefghij",  2,  5, "abcdefghi",   9,    1,  0),
    TEST ("abcdefghi",   0,  9, "abcdefghij", 10, NPOS,  0),
    TEST ("abcdefghi",   0,  4, "abcdefghij",  4,    0,  0),
    TEST ("abcdefghi",   0,  5, "abcdefghij",  4,    1,  0),

    TEST ("abbdefghij",  0, 10, "abcdefghij", 10, NPOS,  0),
    TEST ("abbdefghij",  1,  9, "abcdefghij", 10,    1,  0),
    TEST ("abcdeeghij",  0, 10, "abcdefghij", 10, NPOS,  0),
    TEST ("abcdeeghij",  5, 10, "abcdefghij", 10,    1,  0),
    TEST ("abcdeeghij",  0,  3, "abcdefghij",  3,    0,  0),
    TEST ("abcdeeghij",  0,  5, "abcdefghij", 10, NPOS,  0),
    TEST ("abcdefghii",  0, 10, "abcdefghij", 10, NPOS,  0),
    TEST ("abcdefghii",  0,  9, "abcdefghij",  9,    0,  0),
    TEST ("abcdefghii",  9, 10, "abcdefghij", 10,    1,  0),

    TEST ("abcdefghij",  1,  1, "b",           1,    0,  0),
    TEST ("abcdefghij",  1,  1, "g",           1, NPOS,  0),
    TEST ("abcdefghij",  1,  1, "g",           0,    1,  0),
    TEST ("abcdefghij",  1,  1, "bfg",         1,    0,  0),
    TEST ("abcdefghij",  0,  1, "b",           1, NPOS,  0),
    TEST ("abcdefghij",  1,  4, "bcde",        4,    0,  0),
    TEST ("abcdefghij",  1,  3, "bcde",        4, NPOS,  0),
    TEST ("abcdefghij",  1,  3, "bcde",        2,    1,  0),

    TEST ("bbcdefghij",  2,  4, "a",           1,    1,  0),
    TEST ("bbcdefghij",  2,  0, "a",           1, NPOS,  0),
    TEST ("bbcaefghij",  3,  1, "age",         1,    0,  0),
    TEST ("aeeeeeghij",  6,  1, "aeeee",       2,    1,  0),
    TEST ("aeeeeeghij",  0,  2, "aeeee",       3, NPOS,  0),
    TEST ("aeeeeeghij",  2,  3, "aeeee",       5,    1,  0),
    TEST ("a",           0,  2, "bbcdefghij", 10, NPOS,  0),
    TEST ("jeeee",       1,  2, "eeeeeeghij", 10, NPOS,  0),
    TEST ("jeeee",       1,  4, "eeeeeeghij",  4,    0,  0),

    TEST ("a\0b\0\0c",   0,  6, "a\0b\0\0c",   6,    0,  0),
    TEST ("a\0b\0\0c",   2,  4, "a\0b\0\0c",   6,    1,  0),
    TEST ("e\0b\0\0c",   0,  1, "a\0b\0\0c",   6,    1,  0),
    TEST ("e\0b\0\0c",   3,  2, "\0\0a\0bc",   2,    0,  0),
    TEST ("e\0b\0\0c",   3,  3, "\0\0c\0ba",   4, NPOS,  0),
    TEST ("abc\0\0\0",   0,  4, "abc\0\0\0",   5, NPOS,  0),
    TEST ("ab\0c\0\0",   2,  2, "ab\0b\0\0",   4, NPOS,  0),
    TEST ("abc\0\0\0",   3,  3, "abc\0\0\0",   3, NPOS,  0),
    TEST ("\0af\0\0c",   0,  6, "e\0ab\0\0c",  6, NPOS,  0),
    TEST ("\0af\0\0c",   0,  1, "\0ab\0\0ce",  2, NPOS,  0),
    TEST ("\0\0c\0af",   3,  3, "\0ab\0\0ce",  3,    1,  0),
    TEST ("\0af\0\0c",   2,  2, "e\0ab\0\0c",  2,    1,  0),
    TEST ("\0ae\0\0c",   2,  4, "e\0\0b\0ca",  4,    1,  0),

    TEST ("a\0b",        0,  3, "a\0b\0\0c",   4, NPOS,  0),
    TEST ("a\0b",        0,  1, "a\0b\0\0c",   2, NPOS,  0),
    TEST ("ab\0",        0,  5, "abc\0\0\0",   3, NPOS,  0),
    TEST ("\0eb",        0,  5, "e\0ab\0\0c",  1, NPOS,  0),
    TEST ("a\0b\0\0c",   0,  3, "a\0b",        3,    0,  0),
    TEST ("a\0b\0\0c",   0,  1, "a",           1,    0,  0),
    TEST ("abc\0\0\0",   0,  5, "abc\0\0",     5,    0,  0),
    TEST ("e\0ab\0\0c",  0,  7, "e\0ab\0\0c",  7,    0,  0),
    TEST ("a\0b\0\0c",   2,  3, "a\0b",        2,    1,  0),
    TEST ("a\0b\0\0c",   2,  1, "a",           0,    1,  0),
    TEST ("abc\0\0\0",   2,  5, "c\0\0ab",     3,    1,  0),
    TEST ("e\0ag\0\0c",  3,  6, "e\0ab\0\0c",  7,    1,  0),
    TEST ("abc\0\0\0",   2,  5, "c\0\0ab",     4, NPOS,  0),

    TEST ("x@4096",    0, 4096, "x@4096",   4096,    0,  0),
    TEST ("x@4096",    1, 4096, "x@4096",   4096, NPOS,  0),
    TEST ("x@4096",    1, 4096, "x@4096",   4092,    1,  0),
    TEST ("xx",          0,  2, "x@4096",   4092, NPOS,  0),
    TEST ("x@4096",    0, 4096, "xxxxx",       5,    1,  0),
    TEST ("x@4096",   4090,  5, "xxxxx",       5,    0,  0),
    TEST ("x@4096",      2,  5, "xxxxx",       5,    0,  0),
    TEST ("x@4096",      5,  2, "xxxxx",       3, NPOS,  0),
    TEST ("a\0x",        0,  3, "x@4096",     10, NPOS,  0),
    TEST ("x@4096",      0, 10, "x\0b",        2,    1,  0),
    TEST ("x@4096",      0,  1, "x\0b",        2, NPOS,  0),

    TEST ("a@4096\0", 4096,  1, "\0b",         1,    0,  0),

    TEST ("\0",          2,  0, "",            0,    1,  1),
    TEST ("a",          10,  0, "",            0,    1,  1),
    TEST ("x@4096",   4106,  0, "",            0,    1,  1),

    TEST ("last",        0,  4, "last",        4,    0,  0)
};

/**************************************************************************/

// exercises:
// compare (size_type, size_type, basic_string&, size_type, size_type)
static const StringTestCase
size_size_cstr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, off, size, arg, off2, size2, res, bthrow) {   \
        __LINE__, off, size, off2, size2, -1,                   \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, 0, res, bthrow                     \
    }

    //    +------------------------------------------- controlled sequence
    //    |              +---------------------------- compare() off argument
    //    |              |   +------------------------ compare() n1 argument
    //    |              |   |  +--------------------- sequence to be inserted
    //    |              |   |  |              +------ compare() off2 argument
    //    |              |   |  |              |   +-- compare() n2 argument
    //    |              |   |  |              |   |     +-- expected result
    //    |              |   |  |              |   |     |   +- exception info
    //    |              |   |  |              |   |     |   | 0-no exception
    //    |              |   |  |              |   |     |   | 1-out_of_range
    //    |              |   |  |              |   |     |   | 2-out_of_range
    //    |              |   |  |              |   |     |   | 3-length_error
    //    |              |   |  |              |   |     |   |
    //    |              |   |  |              |   |     |   |
    //    V              V   V  V              V   V     V   V
    TEST ("ab",          0,  2, "ab",          0,  2,    0,  0),

    TEST ("",            0,  0, "",            0,  0,    0,  0),
    TEST ("",            0,  0, "\0",          0,  1, NPOS,  0),
    TEST ("",            0,  0, "a",           0,  0,    0,  0),
    TEST ("",            0,  0, "a",           0,  1, NPOS,  0),

    TEST ("\0",          0,  1, "",            0,  0,    1,  0),
    TEST ("\0",          0,  1, "\0",          0,  1,    0,  0),
    TEST ("\0",          0,  1, "\0",          0,  0,    1,  0),
    TEST ("\0",          0,  1, "\0\0",        0,  2, NPOS,  0),
    TEST ("\0",          0,  1, "\0\0",        1,  1,    0,  0),
    TEST ("\0",          0,  1, "\0\0",        2,  2,    1,  0),
    TEST ("\0\0",        1,  1, "\0",          0,  1,    0,  0),
    TEST ("\0",          0,  1, "a",           0,  1, NPOS,  0),

    TEST ("abcdefghij",  0, 10, "abcdefghij",  0, 10,    0,  0),
    TEST ("abcdefghij",  0, 10, "abcdefghij",  1, 10, NPOS,  0),
    TEST ("abcdefghij",  0, 10, "abcdefghi",   0,  9,    1,  0),
    TEST ("abcdefghij",  5,  4, "abcdefghi",   5,  9,    0,  0),
    TEST ("abcdefghij",  1,  9, "abcdefghi",   0,  9,    1,  0),
    TEST ("abcdefghij",  1,  9, "abcdefghi",   1,  8,    1,  0),
    TEST ("abcdefghij",  1,  8, "abcdefghi",   0,  9,    1,  0),
    TEST ("abcdefghij",  1,  8, "abcdefghi",   2,  9, NPOS,  0),
    TEST ("abcdefghij",  2,  5, "abcdefghi",   0,  9,    1,  0),
    TEST ("abcdefghij",  2,  5, "abcdefghi",   2,  5,    0,  0),
    TEST ("abcdefghi",   0,  9, "abcdefghij",  0, 10, NPOS,  0),
    TEST ("abcdefghi",   0,  9, "abcdefghij",  3, 10, NPOS,  0),
    TEST ("abcdefghi",   0,  4, "abcdefghij",  0,  4,    0,  0),
    TEST ("abcdefghi",   0,  4, "abcdefghij",  9,  1, NPOS,  0),
    TEST ("abcdefghi",   0,  5, "abcdefghij",  0,  4,    1,  0),

    TEST ("abbdefghij",  0, 10, "abcdefghij",  0, 10, NPOS,  0),
    TEST ("abbdefghij",  0, 10, "abcdefghij",  1,  2, NPOS,  0),
    TEST ("abbdefghij",  1,  9, "abcdefghij",  0, 10,    1,  0),
    TEST ("abbdefghij",  1,  9, "abcdefghij",  5, 10, NPOS,  0),
    TEST ("abcdeeghij",  0, 10, "abcdefghij",  0, 10, NPOS,  0),
    TEST ("abcdeeghij",  0, 10, "abcdefghij",  1,  5, NPOS,  0),
    TEST ("abcdeeghij",  5, 10, "abcdefghij",  0, 10,    1,  0),
    TEST ("abcdeeghij",  0,  3, "abcdefghij",  0,  3,    0,  0),
    TEST ("abcdeeghij",  0,  5, "abcdefghij",  0, 10, NPOS,  0),
    TEST ("abcdeeghij",  0,  5, "abcdeabcde",  5, 10,    0,  0),
    TEST ("abcdefghii",  0, 10, "abcdefghij",  0, 10, NPOS,  0),
    TEST ("abcdefghii",  0, 10, "abcdefghij",  8,  2, NPOS,  0),
    TEST ("abcdefghii",  0,  9, "abcdefghij",  0,  9,    0,  0),
    TEST ("abcdefghii",  9, 10, "abcdefghij",  0, 10,    1,  0),
    TEST ("abcdefghii",  9, 10, "abcdefghij",  9, 10, NPOS,  0),

    TEST ("abcdefghij",  1,  1, "b",           0,  1,    0,  0),
    TEST ("abcdefghij",  1,  1, "g",           0,  1, NPOS,  0),
    TEST ("abcdefghij",  1,  1, "g",           0,  0,    1,  0),
    TEST ("abcdefghij",  1,  1, "bfg",         0,  1,    0,  0),
    TEST ("abcdefghij",  1,  1, "bfg",         2,  1, NPOS,  0),
    TEST ("abcdefghij",  0,  1, "b",           0,  1, NPOS,  0),
    TEST ("abcdefghij",  1,  4, "bcde",        0,  4,    0,  0),
    TEST ("abcdefghij",  1,  4, "bcde",        2,  4, NPOS,  0),
    TEST ("abcdefghij",  1,  3, "bcde",        0,  4, NPOS,  0),
    TEST ("abcdefghij",  1,  3, "aebcd",       2,  3,    0,  0),
    TEST ("abcdefghij",  1,  3, "bcde",        0,  2,    1,  0),
    TEST ("abcdefghij",  1,  3, "bcde",        1,  2, NPOS,  0),

    TEST ("bbcdefghij",  2,  4, "a",           0,  1,    1,  0),
    TEST ("bbcdefghij",  2,  0, "a",           0,  1, NPOS,  0),
    TEST ("bbcaefghij",  3,  1, "age",         0,  1,    0,  0),
    TEST ("aeeeeeghij",  6,  1, "aeeee",       0,  2,    1,  0),
    TEST ("aeeeeeghij",  0,  2, "aeeee",       0,  3, NPOS,  0),
    TEST ("aeeeeeghij",  2,  3, "aeeee",       0,  5,    1,  0),
    TEST ("aeeeeeghij",  2,  3, "eeeea",       1,  3,    0,  0),
    TEST ("a",           0,  2, "bbcdefghij",  0, 10, NPOS,  0),
    TEST ("jeeee",       1,  2, "eeeeeeghij",  0, 10, NPOS,  0),
    TEST ("jeeee",       1,  2, "eeeeeeghij",  1,  1,    1,  0),
    TEST ("jeeee",       1,  4, "eeeeeeghij",  0,  4,    0,  0),

    TEST ("a\0b\0\0c",   0,  6, "a\0b\0\0c",   0,  6,    0,  0),
    TEST ("a\0b\0\0c",   1,  1, "a\0b\0\0c",   3,  1,    0,  0),
    TEST ("a\0b\0\0c",   2,  4, "a\0b\0\0c",   0,  6,    1,  0),
    TEST ("a\0b\0\0c",   2,  4, "ab\0\0c\0",   1,  4,    0,  0),
    TEST ("e\0b\0\0c",   0,  1, "a\0b\0\0c",   0,  6,    1,  0),
    TEST ("e\0b\0\0c",   3,  2, "\0\0a\0bc",   0,  2,    0,  0),
    TEST ("e\0b\0\0c",   3,  3, "\0\0c\0ba",   0,  4, NPOS,  0),
    TEST ("e\0b\0\0c",   3,  3, "a\0\0c\0b",   1,  3,    0,  0),
    TEST ("abc\0\0\0",   0,  4, "abc\0\0\0",   0,  5, NPOS,  0),
    TEST ("abc\0\0\0",   3,  4, "a\0\0\0bc",   1,  3,    0,  0),
    TEST ("ab\0c\0\0",   2,  2, "ab\0b\0\0",   0,  4, NPOS,  0),
    TEST ("ab\0c\0\0",   2,  4, "ab\0b\0\0",   2,  4,    1,  0),
    TEST ("\0af\0\0c",   0,  6, "e\0ab\0\0c",  0,  6, NPOS,  0),
    TEST ("\0af\0\0c",   2,  6, "e\0ab\0\0c",  2,  6,    1,  0),
    TEST ("\0af\0\0c",   0,  1, "\0ab\0\0ce",  0,  2, NPOS,  0),
    TEST ("\0af\0\0c",   3,  1, "\0ab\0\0ce",  0,  1,    0,  0),
    TEST ("\0\0c\0af",   3,  3, "\0ab\0\0ce",  0,  3,    1,  0),
    TEST ("\0\0c\0af",   1,  2, "\0ab\0\0ec",  4,  3, NPOS,  0),
    TEST ("\0af\0\0c",   2,  2, "e\0ab\0\0c",  0,  2,    1,  0),
    TEST ("\0ae\0\0c",   0,  3, "e\0\0b\0ca",  4,  4, NPOS,  0),

    TEST ("a\0b",        0,  3, "a\0b\0\0c",   0,  4, NPOS,  0),
    TEST ("a\0b",        1,  3, "a\0b\0\0c",   4,  4, NPOS,  0),
    TEST ("a\0b",        0,  1, "a\0b\0\0c",   0,  2, NPOS,  0),
    TEST ("ab\0",        0,  5, "abc\0\0\0",   0,  3, NPOS,  0),
    TEST ("ab\0",        2,  1, "abc\0\0\0",   3,  3, NPOS,  0),
    TEST ("\0eb",        0,  5, "e\0ab\0\0c",  0,  1, NPOS,  0),
    TEST ("a\0b\0\0c",   0,  3, "a\0b",        0,  3,    0,  0),
    TEST ("a\0b\0\0c",   3,  2, "a\0b",        1,  2, NPOS,  0),
    TEST ("a\0b\0\0c",   0,  1, "a",           0,  1,    0,  0),
    TEST ("abc\0\0\0",   0,  5, "abc\0\0",     0,  5,    0,  0),
    TEST ("e\0ab\0\0c",  0,  7, "e\0ab\0\0c",  0,  7,    0,  0),
    TEST ("e\0ab\0\0c",  0,  7, "e\0ab\0\0c",  2,  7,    1,  0),
    TEST ("a\0b\0\0c",   2,  3, "a\0b",        0,  2,    1,  0),
    TEST ("a\0b\0\0c",   2,  1, "a",           0,  0,    1,  0),
    TEST ("abc\0\0\0",   2,  5, "c\0\0ab",     0,  3,    1,  0),
    TEST ("e\0ag\0\0c",  3,  6, "e\0ab\0\0c",  0,  7,    1,  0),
    TEST ("abc\0\0\0",   2,  5, "c\0\0ab",     0,  4, NPOS,  0),

    TEST ("x@4096",    0, 4096, "x@4096",    0, 4096,    0,  0),
    TEST ("x@4096",    1, 4096, "x@4096",    0, 4096, NPOS,  0),
    TEST ("x@4096",    1, 4096, "x@4096",    0, 4092,    1,  0),
    TEST ("x@4096",    1, 4090, "x@4096",    2, 4090,    0,  0),
    TEST ("x@4096",    1, 4102, "x@4096",    2, 4102,    1,  0),
    TEST ("xx",          0,  2, "x@4096",    0, 4092, NPOS,  0),
    TEST ("xx",          0,  2, "x@4096", 4090,    2,    0,  0),
    TEST ("x@4096",    0, 4096, "xxxxx",     0,    5,    1,  0),
    TEST ("x@4096",   4090,  5, "xxxxx",     0,    5,    0,  0),
    TEST ("x@4096",      2,  5, "xxxxx",     0,    5,    0,  0),
    TEST ("x@4096",      5,  2, "xxxxx",     0,    3, NPOS,  0),
    TEST ("a\0x",        0,  3, "x@4096",    0,   10, NPOS,  0),
    TEST ("a\0x",        2,  1, "x@4096", 4095,    1,    0,  0),
    TEST ("x@4096",      0, 10, "x\0b",      0,    2,    1,  0),
    TEST ("x@4096",      0,  1, "x\0b",      0,    2, NPOS,  0),

    TEST ("a@4096\0", 4096,  1, "b\0",       1,    1,    0,  0),

    TEST ("\0",          2,  0, "",            0,  0,    1,  1),
    TEST ("a",          10,  0, "",            0,  0,    1,  1),
    TEST ("x@4096",   4106,  0, "",            0,  0,    1,  1),

    TEST ("",            0,  0, "\0",          2,  0,    1,  2),
    TEST ("",            0,  0, "a",          10,  0,    1,  2),
    TEST ("",            0,  0, "x@4096",   4106,  0,    1,  2),

    TEST ("last",        0, 4, "last",         0, 4,     0,  0)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_compare (charT, Traits*, Allocator*,
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
    const String str (wstr, str_len);
    const String arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    int res = 0;

    // string function argument
    const charT* const arg_ptr = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      arg_str = tcase.arg ? arg : str;

    std::size_t total_compare_calls = 0;
    std::size_t n_compare_calls = 0;
    std::size_t* const rg_calls =
        rw_get_call_counters ((Traits*)0, (charT*)0);

    if (rg_calls)
        total_compare_calls = rg_calls[UTMemFun::compare];

    // (name of) expected and caught exception
    const char* expected = 0;
    const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    if (1 == tcase.bthrow)
        expected = exceptions [1];
    else if (   2 == tcase.bthrow
             && Compare (size_size_cstr_size_size) == func.which_)
        expected = exceptions [1];
    else if (3 == tcase.bthrow)
        expected = exceptions [2];

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        switch (func.which_) {
            case Compare (cptr):
                res = str.compare (arg_ptr);
                break;

            case Compare (cstr):
                res = str.compare (arg_str);
                if (rg_calls)
                    n_compare_calls = rg_calls[UTMemFun::compare];
                break;

            case Compare (size_size_cptr):
                res = str.compare (tcase.off, tcase.size, arg_ptr);
                break;

            case Compare (size_size_cstr):
                res = str.compare (tcase.off, tcase.size, arg_str);
                break;

            case Compare (size_size_cptr_size):
                res = str.compare (tcase.off, tcase.size,
                                    arg_ptr, tcase.size2);
                break;

            case Compare (size_size_cstr_size_size):
                res = str.compare (tcase.off, tcase.size,
                                    arg_str, tcase.off2, tcase.size2);
                break;

            default:
                RW_ASSERT ("test logic error: unknown compare overload");
                return;
        }

        // verify the returned value
        const bool success =    res < 0 && tcase.nres == NPOS
                             || res > 0 && tcase.nres > 0
                             || res == 0 && tcase.nres == 0;

        rw_assert (success, 0, tcase.line,
                   "line %d. %{$FUNCALL} == %d, got %d",
                   __LINE__, tcase.nres == NPOS ? -1 : int (tcase.nres), res);

        // verify that Traits::length was used
        if (Compare (cstr) == func.which_ && rg_calls) {
            rw_assert (n_compare_calls - total_compare_calls > 0,
                       0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                       "use traits::compare()", __LINE__);
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

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (str),
                                __LINE__, tcase.line, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_compare);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Compare (sig), sig ## _test_cases,                      \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (size_size_cptr),
        TEST (size_size_cstr),
        TEST (size_size_cptr_size),
        TEST (size_size_cstr_size_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.compare",
                               test_compare, tests, test_count);
}
