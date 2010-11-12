/***************************************************************************
 *
 * 21.string.replace.cpp - string test exercising lib.string.replace
 *
 * $Id: 21.string.replace.cpp 590052 2007-10-30 12:44:14Z faridz $
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
#include <stdexcept>        // for out_of_range, length_error
#include <cstddef>          // for ptrdiff_t, size_t

#include <21.strings.h>
#include <alg_test.h>       // for InputIter
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Replace(sig)    StringIds::replace_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, const value_type*)
// replace (iterator, iterator, const value_type*)
static const StringTestCase
iter_iter_cptr_test_cases [] = {

// iter_iter_cptr_test_cases serves a double duty
#define size_size_cptr_test_cases iter_iter_cptr_test_cases

#undef TEST
#define TEST(str, off, size, arg, res, bthrow) {                \
        __LINE__, off, size, -1, -1, -1,                        \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- sequence to be inserted
    //    |            |  |  |           +----------- expected result sequence
    //    |            |  |  |           |        +-- exception info
    //    |            |  |  |           |        |   -1   - no exceptions
    //    |            |  |  |           |        |    0   - exception safety
    //    |            |  |  |           |        |    1,2 - out_of_range
    //    |            |  |  |           |        |    3   - length_error
    //    |            |  |  |           |        |
    //    |            |  |  |           |        +-------+
    //    |            |  |  |           |                |
    //    V            V  V  V           V                V
    TEST ("ab",        0, 0, "c",        "cab",           0),

    TEST ("",          0, 0, "",         "",              0),
    TEST ("",          0, 0, "abc",      "abc",           0),

    TEST ("ab",        0, 2, "",         "",              0),
    TEST ("ab",        0, 1, "",         "b",             0),

    TEST ("<U0>",      0, 1, "",         "",              0),
    TEST ("<U0>",      0, 1, "a",        "a",             0),
    TEST ("<U0>",      0, 1, "<U0>@2",   "",              0),

    TEST ("ah",        0, 1, "bcdefg",   "bcdefgh",       0),
    TEST ("ah",        1, 1, "bcdefg",   "abcdefg",       0),
    TEST ("ah",        0, 2, "bcdefg",   "bcdefg",        0),

    TEST ("abc",       0, 2, "cc",       "ccc",           0),
    TEST ("abc",       1, 2, "cc",       "acc",           0),

    TEST ("abc",       0, 3, "defgh",    "defgh",         0),
    TEST ("abc",       2, 1, "defgh",    "abdefgh",       0),
    TEST ("abc",       2, 1, "de<U0>gh", "abde",          0),
    TEST ("abc",       2, 1, "",         "ab",            0),
    TEST ("abc",       1, 1, "defgh",    "adefghc",       0),

    TEST ("abc",       0, 0, "ee",       "eeabc",         0),
    TEST ("abc",       0, 0, "<U0>@2e<U0>", "abc",        0),
    TEST ("abc",       2, 0, "ee",       "abeec",         0),
    TEST ("abc",       1, 0, "ee",       "aeebc",         0),
    TEST ("abc",       1, 0, "e<U0>@2",  "aebc",          0),

    TEST ("a<U0>b<U0>@2c", 0, 3, "",      "<U0>@2c",      0),
    TEST ("a<U0>b<U0>@2c", 0, 3, "<U0>e", "<U0>@2c",      0),

    TEST ("<U0>ab<U0>@2c", 0, 0, "e<U0>",   "e<U0>ab<U0>@2c",   0),
    TEST ("a<U0>b<U0>c<U0>", 6, 0, "e<U0>", "a<U0>b<U0>c<U0>e", 0),
    TEST ("<U0>ab<U0>@2c", 5, 0, "e<U0>",   "<U0>ab<U0>@2ec",   0),

    TEST ("x@4096",    0, 4095, "ab",    "abx",           0),
    TEST ("<U0>@4096", 1, 4094, "abc",   "<U0>abc<U0>",   0),
    TEST ("x@4096",    1, 4094, "ab",    "xabx",          0),
    TEST ("x@4096",    0, 4094, "ab",    "abxx",          0),
    TEST ("x@4096",    1, 4093, "",      "xxx",           0),

    TEST ("x@20",      1,   10, "x@118", "x@128",         0),
    TEST ("x@128",   128,    0, "x@79",  "x@207",         0),
    TEST ("x@207",     0,    0, "x@127", "x@334",         0),
    TEST ("x@334",     1,    1, "x@207", "x@540",         0),
    TEST ("x@540",     0,  539, "x@334", "x@335",         0),
    TEST ("x@539",   539,    0, "x@873", "x@1412",        0),
    TEST ("x@3695",    0, 2283, "",      "x@1412",        0),
    TEST ("x@1412",  207,    1, "x@2284", "x@3695",       0),
    TEST ("x@207",     0,  207, "x@2284", "x@2284",       0),

    TEST ("",          0,    0, "x@4096", "x@4096",       0),
    TEST ("a",         0,    1, "x@4096", "x@4096",       0),
    TEST ("x@4096",    0, 4095, "x@4096", "x@4097",       0),
    TEST ("<U0>ab<U0>@2c", 0, 6, "x@4096", "x@4096",      0),

    TEST ("<U0>",       2,   0, "",       "<U0>",         1),
    TEST ("a",         10,   0, "",       "a",            1),
    TEST ("x@4096",  4106,   0, "",       "x@4096",       1),

    TEST ("",           0,   0, "x@4096", "x@4096",       0),

    // self-referential replacement
    TEST ("",          0,    0, 0,        "",             0),
    TEST ("abc",       0,    3, 0,        "abc",          0),
    TEST ("abc",       1,    1, 0,        "aabcc",        0),
    TEST ("a<U0>b<U0>c<U0>", 2,    3, 0,  "a<U0>a<U0>",       0),
    TEST ("a<U0>b<U0>c<U0>", 0,    0, 0,  "aa<U0>b<U0>c<U0>", 0),
    TEST ("a<U0>b<U0>c<U0>", 6,    0, 0,  "a<U0>b<U0>c<U0>a", 0),
    TEST ("<U0>ab<U0>c<U0>", 3,    3, 0,  "<U0>ab",           0),
    TEST ("x@4096",    0, 4095, 0,        "x@4097",       0),

    TEST ("last",      4, 0, "test",     "lasttest",      0)
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, const basic_string&)
// replace (iterator, iterator, const basic_string&)
static const StringTestCase
iter_iter_cstr_test_cases [] = {

// iter_iter_cstr_test_cases serves a double duty
#define size_size_cstr_test_cases iter_iter_cstr_test_cases

#undef TEST
#define TEST(s, off, size, arg, res, bthrow) {                  \
        __LINE__, off, size, -1, -1, -1,                        \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------- controlled sequence
    //    |             +----------------------------- replace() pos1 argument
    //    |             |  +-------------------------- replace() n1 argument
    //    |             |  |  +----------------------- sequence to be inserted
    //    |             |  |  |           +----------- expected result sequence
    //    |             |  |  |           |        +-- exception info
    //    |             |  |  |           |        |   -1   - no exceptions
    //    |             |  |  |           |        |    0   - exception safety
    //    |             |  |  |           |        |    1,2 - out_of_range
    //    |             |  |  |           |        |    3   - length_error
    //    |             |  |  |           |        |
    //    |             |  |  |           |        +-----------+
    //    |             |  |  |           |                    |
    //    V             V  V  V           V                    V
    TEST ("ab",         0, 0, "c",        "cab",               0),

    TEST ("",           0, 0, "",         "",                  0),
    TEST ("",           0, 0, "<U0>",       "<U0>",            0),
    TEST ("",           0, 0, "abc",      "abc",               0),

    TEST ("ab",         0, 2, "",         "",                  0),
    TEST ("ab",         0, 1, "",         "b",                 0),
    TEST ("ab",         1, 1, "<U0>",       "a<U0>",           0),

    TEST ("<U0>",       0, 1, "",         "",                  0),
    TEST ("<U0>",       0, 1, "a",        "a",                 0),
    TEST ("<U0>",       0, 1, "<U0>@2",     "<U0>@2",          0),

    TEST ("ah",         0, 1, "bcdefg",   "bcdefgh",           0),
    TEST ("ah",         1, 1, "bcdefg",   "abcdefg",           0),
    TEST ("ah",         0, 2, "bcdefg",   "bcdefg",            0),

    TEST ("abc",        0, 2, "cc",       "ccc",               0),
    TEST ("abc",        1, 2, "cc",       "acc",               0),

    TEST ("abc",        0, 3, "defgh",    "defgh",             0),
    TEST ("abc",        2, 1, "defgh",    "abdefgh",           0),
    TEST ("abc",        2, 1, "de<U0>gh", "abde<U0>gh",        0),
    TEST ("abc",        2, 1, "",         "ab",                0),
    TEST ("abc",        1, 1, "defgh",    "adefghc",           0),

    TEST ("abc",        0, 0, "ee",       "eeabc",             0),
    TEST ("abc",        0, 0, "<U0>@2e<U0>", "<U0>@2e<U0>abc", 0),
    TEST ("abc",        2, 0, "ee",       "abeec",             0),
    TEST ("abc",        1, 0, "ee",       "aeebc",             0),
    TEST ("abc",        1, 0, "e<U0>@2",  "ae<U0>@2bc",        0),

    TEST ("a<U0>b<U0>@2c",  0, 3, "",      "<U0>@2c",          0),
    TEST ("a<U0>b<U0>@2c",  0, 3, "<U0>e", "<U0>e<U0>@2c",     0),

    TEST ("a<U0>b<U0>@2c",  2, 3, "<U0>e",       "a<U0>@2ec",      0),
    TEST ("a<U0>b<U0>@2c",  0, 3, "<U0>@2e<U0>", "<U0>@2e<U0>@3c", 0),
    TEST ("<U0>ab<U0>@2c",  1, 2, "<U0>e<U0>@2", "<U0>@2e<U0>@4c", 0),

    TEST ("<U0>ab<U0>@2c",   0, 0, "<U0>e", "<U0>e<U0>ab<U0>@2c",   0),
    TEST ("a<U0>b<U0>c<U0>", 6, 0, "e<U0>", "a<U0>b<U0>c<U0>e<U0>", 0),
    TEST ("<U0>ab<U0>@2c",  5, 0, "<U0>e",      "<U0>ab<U0>@3ec",   0),

    TEST ("x@4096",     0, 4095, "ab",    "abx",               0),
    TEST ("x@4096",     1, 4094, "ab",    "xabx",              0),
    TEST ("x@4096",     0, 4094, "ab",    "abxx",              0),
    TEST ("x@4096",     1, 4093, "",      "xxx",               0),
    TEST ("x@4096",     1, 4092, "<U0>@2", "x<U0>@2xxx",       0),

    TEST ("x@20",       1,   10, "x@118", "x@128",             0),
    TEST ("x@128",    128,    0, "x@79",  "x@207",             0),
    TEST ("x@207",      0,    0, "x@127", "x@334",             0),
    TEST ("x@334",      1,    1, "x@207", "x@540",             0),
    TEST ("x@540",      0,  539, "x@334", "x@335",             0),
    TEST ("x@539",    539,    0, "x@873", "x@1412",            0),
    TEST ("x@3695",     0, 2283, "",      "x@1412",            0),
    TEST ("x@1412",   207,    1, "x@2284", "x@3695",           0),
    TEST ("x@207",      0,  207, "x@2284", "x@2284",           0),

    TEST ("",           0,    0, "x@4096", "x@4096",           0),
    TEST ("a",          0,    1, "x@4096", "x@4096",           0),
    TEST ("x@4096",     0, 4095, "x@4096", "x@4097",           0),
    TEST ("<U0>ab<U0>@2c",  0,    6, "x@4096", "x@4096",       0),

    TEST ("<U0>",         2,    0, "",        "<U0>",          1),
    TEST ("a",         10,    0, "",        "a",               1),
    TEST ("x@4096",  4106,    0, "",        "x@4096",          1),
    TEST ("",           0,    0, "x@4096",  "x@4096",          0),

    // self-referential replacement
    TEST ("abc",        0,    3, 0, "abc",                     0),
    TEST ("abc",        1,    1, 0, "aabcc",                   0),
    TEST ("a<U0>b<U0>c<U0>",  2, 3, 0, "a<U0>a<U0>b<U0>c<U0>@2",         0),
    TEST ("a<U0>b<U0>c<U0>",  0, 0, 0, "a<U0>b<U0>c<U0>a<U0>b<U0>c<U0>", 0),
    TEST ("a<U0>b<U0>c<U0>",  6, 0, 0, "a<U0>b<U0>c<U0>a<U0>b<U0>c<U0>", 0),
    TEST ("<U0>ab<U0>c<U0>",  3, 3, 0, "<U0>ab<U0>ab<U0>c<U0>",          0),
    TEST ("x@4096",     0, 4095, 0, "x@4097",                  0),

    TEST ("last",       4,    0, "test",     "lasttest",       0)
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, const value_type*, size_type)
// replace (iterator, iterator, const value_type*, size_type)
static const StringTestCase
iter_iter_cptr_size_test_cases [] = {

// iter_iter_cptr_size_test_cases serves a double duty
#define size_size_cptr_size_test_cases iter_iter_cptr_size_test_cases

#undef TEST
#define TEST(str, off, size, arg, count, res, bthrow) {         \
        __LINE__, off, size, -1, count, -1,                     \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- sequence to be inserted
    //    |            |  |  |            +---------- replace() n2 argument
    //    |            |  |  |            |  +------- expected result sequence
    //    |            |  |  |            |  |     +- exception info
    //    |            |  |  |            |  |     |  -1   - no exceptions
    //    |            |  |  |            |  |     |   0   - exception safety
    //    |            |  |  |            |  |     |   1,2 - out_of_range
    //    |            |  |  |            |  |     |   3   - length_error
    //    |            |  |  |            |  |     |
    //    |            |  |  |            |  |     +------------+
    //    |            |  |  |            |  |                  |
    //    V            V  V  V            V  V                  V
    TEST ("ab",        0, 0, "c",         1, "cab",             0),

    TEST ("",          0, 0, "",          0, "",                0),
    TEST ("",          0, 0, "abc",       3, "abc",             0),

    TEST ("ab",        0, 2, "",          0, "",                0),
    TEST ("ab",        0, 1, "",          0, "b",               0),

    TEST ("<U0>",      0, 1, "",          0, "",                0),
    TEST ("<U0>",      0, 1, "a",         1, "a",               0),
    TEST ("<U0>",      0, 1, "<U0>@2",    2, "<U0>@2",          0),

    TEST ("ah",        0, 1, "bcdefg",    3, "bcdh",            0),
    TEST ("ah",        1, 1, "bcdefg",    3, "abcd",            0),
    TEST ("ah",        0, 2, "bcdefg",    5, "bcdef",           0),

    TEST ("abc",       0, 2, "cc",        2, "ccc",             0),
    TEST ("abc",       1, 2, "cc",        2, "acc",             0),
    TEST ("abc",       2, 1, "defgh",     1, "abd",             0),
    TEST ("abc",       2, 1, "de<U0>gh",  3, "abde<U0>",        0),
    TEST ("abc",       2, 1, "",          0, "ab",              0),

    TEST ("abc",       0, 0, "ee",        2, "eeabc",           0),
    TEST ("abc",       0, 0, "<U0>@2e<U0>", 4, "<U0>@2e<U0>abc",0),
    TEST ("abc",       2, 0, "ee",        2, "abeec",           0),
    TEST ("abc",       1, 0, "ee",        1, "aebc",            0),

    TEST ("a<U0>b<U0>@2c", 0, 3, "",      0, "<U0>@2c",         0),
    TEST ("a<U0>b<U0>@2c", 0, 3, "e<U0>", 2, "e<U0>@3c",        0),
    TEST ("a<U0>b<U0>@2c", 2, 3, "e<U0>", 1, "a<U0>ec",         0),
    TEST ("a<U0>b<U0>@2c", 2, 3, "<U0>e", 2, "a<U0>@2ec",       0),
    TEST ("<U0>ab<U0>@2c", 2, 3, "<U0>e", 2, "<U0>a<U0>ec",     0),
    TEST ("a<U0>b<U0>@2c", 0, 6, "e<U0>", 2, "e<U0>",           0),

    TEST ("x@4096",    1, 4095, "<U0>",      1, "x<U0>",        0),
    TEST ("x@4096",    0, 4095, "ab",        2, "abx",          0),
    TEST ("x@4096",    1, 4094, "ab",        1, "xax",          0),
    TEST ("x@4096",    0, 4094, "ab",        2, "abxx",         0),
    TEST ("x@4096",    1, 4093, "",          0, "xxx",          0),
    TEST ("x@4096",    1, 4092, "<U0>@2",    2, "x<U0>@2xxx",   0),

    TEST ("",          0,    0, "x@873",   540, "x@540",        0),
    TEST ("x@20",      1,   10, "x@118",   118, "x@128",        0),
    TEST ("x@128",   128,    0, "x@79",     79, "x@207",        0),
    TEST ("x@207",     0,    0, "x@127",   127, "x@334",        0),
    TEST ("x@207",   207,  207, "x@207",   127, "x@334",        0),
    TEST ("x@334",     0,  207, "x@545",   540, "x@667",        0),
    TEST ("x@539",     0,  539, "x@873",   873, "x@873",        0),
    TEST ("x@878",     5,    5, "x@540",   539, "x@1412",       0),
    TEST ("x@872",     0,    0, "x@1412", 1412, "x@2284",       0),
    TEST ("x@1411", 1411,    0, "x@2284", 2284, "x@3695",       0),
    TEST ("x@1411",  873,  538, "x@3695", 2822, "x@3695",       0),
    TEST ("x@1412",    0,    0, "x@2284", 2284, "x@3696",       0),
    TEST ("",          0,    0, "x@3696", 3696, "x@3696",       0),

    TEST ("a",         0,    1, "x@4096", 4095, "x@4095",       0),
    TEST ("x@4096",    0, 4095, "y@4096", 4095, "y@4095x",      0),
    TEST ("x@4096",    0, 2047, "x@4096", 2047, "x@4096",       0),

    TEST ("x@4096", 2047, 2048, "x@4096", 2048,  "x@4096",      0),

    TEST ("<U0>",       2,   0, "",         0, "<U0>",          1),
    TEST ("a",         10,   0, "",         0, "a",             1),
    TEST ("x@4096",  4106,   0, "",         0, "x@4096",        1),

    TEST ("a",          0,   1, "x@4096", 4095, "x@4095",       0),

    // self-referential replacement
    TEST ("a",         0,    0, 0 /* self */,    1, "aa",              0),
    TEST ("a",         0,    1, 0 /* self */,    1, "a",               0),
    TEST ("a",         1,    0, 0 /* self */,    1, "aa",              0),
    TEST ("abc",       0,    3, 0 /* self */,    2, "ab",              0),
    TEST ("abc",       1,    1, 0 /* self */,    3, "aabcc",           0),
    TEST ("a<U0>b<U0>c<U0>", 2, 3, 0, 6, "a<U0>a<U0>b<U0>c<U0>@2",     0),
    TEST ("a<U0>b<U0>c<U0>", 0, 0, 0, 4, "a<U0>b<U0>a<U0>b<U0>c<U0>",  0),
    TEST ("<U0>ab<U0>c<U0>", 6, 0, 0 /* self */, 1, "<U0>ab<U0>c<U0>@2", 0),
    TEST ("<U0>ab<U0>c<U0>", 3, 3, 0 /* self */, 2, "<U0>ab<U0>a",       0),
    TEST ("a@4096",    0,    1, 0 /* self */, 1111, "a@5206",          0),
    TEST ("b@4096",    1,    2, 0 /* self */, 2222, "b@6316",          0),
    TEST ("x@4096",    0, 4095, 0 /* self */, 4095, "x@4096",          0),
    TEST ("x@4096",    0, 4095, 0 /* self */,    1, "xx",              0),

    TEST ("last",      4,    0, "test",          4, "lasttest",        0)
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, basic_string&, size_type, size_type)
// replace (iterator, Iterator, InputIterator, InputIterator)
static const StringTestCase
iter_iter_range_test_cases [] = {

// iter_iter_range_test_cases serves a double duty
#define size_size_cstr_size_size_test_cases iter_iter_range_test_cases

#undef TEST
#define TEST(str, off, size, arg, off2, size2, res, bthrow) {   \
        __LINE__, off, size, off2, size2, -1,                   \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------- controlled sequence
    //    |            +------------------------------ replace() pos argument
    //    |            |   +-------------------------- replace() n1 argument
    //    |            |   |  +----------------------- sequence to be inserted
    //    |            |   |  |           +----------- replace() pos2 argument
    //    |            |   |  |           |   +------- replace() n2 argument
    //    |            |   |  |           |   |  +---- expected result sequence
    //    |            |   |  |           |   |  | +-- exception info:
    //    |            |   |  |           |   | |  |   -1    - no exceptions
    //    |            |   |  |           |   |  | |    0    - exception safety
    //    |            |   |  |           |   |  | |    1, 2 - out_of_range
    //    |            |   |  |           |   |  | |    3    - length_error
    //    |            |   |  |           |   |  | |
    //    |            |   |  |           |   |  | +-----------------+
    //    |            |   |  |           |   |  |                   |
    //    V            V   V  V           V   V  V                   V
    TEST ("",          0,  0, "",         0,  0, "",                 0),
    TEST ("",          0,  0, "",         0, -1, "",                 0),
    TEST ("",          0,  0, "",         0,  1, "",                 0),

    TEST ("",          0,  1, "",         0,  0, "",                 0),
    TEST ("",          0,  1, "",         0,  1, "",                 0),
    TEST ("",          0,  1, "",         0, -1, "",                 0),

    TEST ("",          0, -1, "",         0,  0, "",                 0),
    TEST ("",          0, -1, "",         0,  1, "",                 0),
    TEST ("",          0, -1, "",         0, -1, "",                 0),

    TEST ("1",         0,  0, "",         0,  0, "1",                0),
    TEST ("2",         0,  0, "",         0, -1, "2",                0),
    TEST ("3",         0,  0, "",         0,  1, "3",                0),

    TEST ("4",         0,  1, "",         0,  0, "",                 0),
    TEST ("5",         0,  1, "",         0,  1, "",                 0),
    TEST ("6",         0,  1, "",         0, -1, "",                 0),

    TEST ("7",         0, -1, "",         0,  0, "",                 0),
    TEST ("8",         0, -1, "",         0,  1, "",                 0),
    TEST ("9",         0, -1, "",         0, -1, "",                 0),

    TEST ("1",         0,  0, "9",        0,  0, "1",                0),
    TEST ("2",         0,  0, "8",        0, -1, "82",               0),
    TEST ("3",         0,  0, "7",        0,  1, "73",               0),

    TEST ("4",         0,  1, "6",        0,  0, "",                 0),
    TEST ("5",         0,  1, "5",        0,  1, "5",                0),
    TEST ("6",         0,  1, "4",        0, -1, "4",                0),

    TEST ("7",         0, -1, "3",        0,  0, "",                 0),
    TEST ("8",         0, -1, "2",        0,  1, "2",                0),
    TEST ("9",         0, -1, "1",        0, -1, "1",                0),

    TEST ("",          0,  0, "abc",      0,  3, "abc",              0),

    TEST ("ab",        0,  0, "c",        0, 1, "cab",               0),
    TEST ("ab",        0,  2, "",         0, 0, "",                  0),
    TEST ("ab",        0,  1, "",         0, 0, "b",                 0),

    TEST ("<U0>",      0,  1, "",         0, 0, "",                  0),
    TEST ("<U0>",      0,  1, "a",        0, 1, "a",                 0),
    TEST ("<U0>",      0,  1, "<U0>@2",   1, 1, "<U0>",              0),
    TEST ("<U0>",      0,  1, "<U0>@2",   0, 2, "<U0>@2",            0),

    TEST ("ah",        0,  1, "bcdefg",   0, 3, "bcdh",              0),
    TEST ("ah",        1,  1, "bcdefg",   0, 3, "abcd",              0),
    TEST ("ah",        0,  1, "bcdefg",   1, 3, "cdeh",              0),
    TEST ("ah",        1,  1, "bcdefg",   1, 3, "acde",              0),
    TEST ("ah",        0,  1, "bcdefg",   0, 6, "bcdefgh",           0),
    TEST ("ah",        1,  1, "bcdefg",   0, 6, "abcdefg",           0),

    TEST ("abc",       0,  2, "cc",       0, 2, "ccc",               0),
    TEST ("abc",       1,  2, "cc",       0, 2, "acc",               0),

    TEST ("abc",       0,  3, "d",        0, 1, "d",                 0),
    TEST ("abc",       0,  3, "def",      0, 3, "def",               0),
    TEST ("abc",       0,  3, "defgh",    0, 5, "defgh",             0),
    TEST ("abc",       2,  1, "defgh",    4, 1, "abh",               0),
    TEST ("abc",       2,  1, "de<U0>gh",   2, 1, "ab<U0>",          0),
    TEST ("abc",       2,  1, "",         0, 0, "ab",                0),

    TEST ("abc",       1,  1, "defgh",    1, 2, "aefc",              0),
    TEST ("abc",       0,  0, "ee",       0, 2, "eeabc",             0),
    TEST ("abc",       0,  0, "<U0>@2e<U0>",  0, 4, "<U0>@2e<U0>abc",0),
    TEST ("abc",       2,  0, "ee",       0, 2, "abeec",             0),
    TEST ("abc",       2,  1, "<U0>e<U0>@2",  0, 4, "ab<U0>e<U0>@2", 0),
    TEST ("abc",       1,  0, "ee",       0, 2, "aeebc",             0),
    TEST ("abc",       1,  0, "<U0>e<U0>@2",  0, 4, "a<U0>e<U0>@2bc", 0),

    TEST ("a<U0>b<U0>@2c", 0,  3, "",      0, 0, "<U0>@2c",           0),
    TEST ("<U0>ab<U0>@2c", 0,  3, "",      0, 0, "<U0>@2c",           0),
    TEST ("a<U0>b<U0>@2c", 0,  3, "<U0>e", 0, 2, "<U0>e<U0>@2c",      0),

    TEST ("a<U0>b<U0>@2c", 2,  3, "<U0>e",      0, 2, "a<U0>@2ec",    0),
    TEST ("a<U0>b<U0>@2c", 2,  3, "<U0>e",      1, 1, "a<U0>ec",      0),
    TEST ("<U0>ab<U0>@2c", 2,  3, "<U0>e",      0, 2, "<U0>a<U0>ec",  0),
    TEST ("<U0>ab<U0>@2c", 2,  3, "<U0>e<U0>@2", 1, 3, "<U0>ae<U0>@2c", 0),

    TEST ("a<U0>b<U0>@2c", 0,  6, "<U0>e",      0, 2, "<U0>e",          0),
    TEST ("a<U0>b<U0>@2c", 0,  6, "<U0>e",      0, 1, "<U0>",           0),

    TEST ("<U0>ab<U0>@2c",   0,  0, "<U0>e",  0, 2, "<U0>e<U0>ab<U0>@2c",   0),
    TEST ("a<U0>b<U0>c<U0>", 6,  0, "e<U0>",  0, 2, "a<U0>b<U0>c<U0>e<U0>", 0),
    TEST ("<U0>ab<U0>@2c",   5,  0, "<U0>e",  0, 2, "<U0>ab<U0>@3ec",       0),

    ///////////////////////////////////////////////////////////////////////
    // very long strings
    TEST ("a",         0,     0, "b",      0, -1, "ba",               0),
    TEST ("a@0",       0,     0, "b@0",    0, -1, "",                 0),
    TEST ("a@0",       0,     0, "b@1",    0, -1, "b",                0),
    TEST ("a@1",       0,     0, "b@0",    0, -1, "a",                0),
    TEST ("a@1",       0,     0, "b@1",    0, -1, "ba",               0),
    TEST ("a@2",       0,     0, "b@2",    0, -1, "bbaa",             0),

    TEST ("a@1000",    0,     0, "b@1000", 0, -1, "b@1000a@1000",     0),
    TEST ("a@1000",    0,     1, "b@1001", 0, -1, "b@1001a@999",      0),
    TEST ("a@1000",    0,     2, "b@1002", 0, -1, "b@1002a@998",      0),
    TEST ("a@1000",    1,   998, "b@1003", 0, -1, "ab@1003a",         0),
    TEST ("a@1000",    2,   996, "b@1004", 0, -1, "aab@1004aa",       0),
    TEST ("a@1000",  500,   250, "b@1005", 0, -1, "a@500b@1005a@250", 0),
    TEST ("a@1000",  998,     1, "b@1006", 0, -1, "a@998b@1006a",     0),
    TEST ("a@2000", 1000,    -1, "b",      0, -1, "a@1000b",          0),
    TEST ("a@2000", 1000,   999, "bb",     0, -1, "a@1000bba",        0),

    TEST ("x@4096",    0,  4095, "ab",       0, 2, "abx",             0),
    TEST ("x@4096",    1,  4094, "ab",       0, 2, "xabx",            0),
    TEST ("x@4096",    0,  4094, "ab",       0, 2, "abxx",            0),
    TEST ("x@4096",    1,  4093, "",         0, 0, "xxx",             0),
    TEST ("x@4096",    1,  4092, "<U0>@2",   0, 2, "x<U0>@2xxx",      0),

    TEST ("a",      0,     1, "x@4096",   0, 4095, "x@4095",         0),
    TEST ("x@4096", 0,  4095, "x@4096",   0, 4095, "x@4096",         0),
    TEST ("x@4096", 0,  2047, "x@4096",   0, 2047, "x@4096",         0),

    TEST ("",       0,     0, "x@334",    0,  207, "x@207",          0),
    TEST ("x@20",   1,    10, "x@118",    0,  118, "x@128",          0),
    TEST ("x@128",  0,     0, "x@129",   50,   79, "x@207",          0),
    TEST ("x@207",  207,   0, "x@127",    0,  127, "x@334",          0),
    TEST ("x@208",  128,   1, "x@207",   10,  127, "x@334",          0),
    TEST ("x@335",  334,   1, "x@208",    2,  206, "x@540",          0),
    TEST ("x@540",    0,   0, "x@336",    3,  333, "x@873",          0),
    TEST ("x@334",    0, 334, "x@873",    0,  873, "x@873",          0),
    TEST ("x@873",    0, 873, "x@541",    1,  540, "x@540",          0),
    TEST ("x@872",    0,   0, "x@1412",   0, 1412, "x@2284",         0),
    TEST ("x@1411", 1411,  0, "x@2288",   4, 2284, "x@3695",         0),
    TEST ("x@873",   540,333, "x@3695",3695,    0, "x@540",          0),
    TEST ("x@1412",  0, 1412, "x@3696",    0,3695, "x@3695",         0),

    TEST ("x@4096", 2047, 2048, "x@4096", 0, 2048,  "x@4096",        0),

    TEST ("<U0>",         2, 0, "",           0, 0, "<U0>",          1),
    TEST ("<U0>",         0, 0, "<U0>",       2, 0, "<U0>",          2),

    TEST ("a",         10, 0, "",           0, 0, "a",               1),
    TEST ("a",          0, 0, "a",         10, 0, "a",               2),

    TEST ("x@4096",  4106, 0, "",           0, 0, "x@4096",          1),
    TEST ("x@4096",     0, 0, "x@4096",  4106, 0, "x@4096",          2),

    TEST ("a",          0, 1, "x@4096", 0, 4095, "x@4095",           0),

    ///////////////////////////////////////////////////////////////////////
    // self-referential replacement
    TEST ("",          0,  0, 0,          0,  0, "",                 0),
    TEST ("",          0,  0, 0,          0,  1, "",                 0),
    TEST ("",          0,  0, 0,          0, -1, "",                 0),

    TEST ("",          0,  1, 0,          0,  0, "",                 0),
    TEST ("",          0,  1, 0,          0,  1, "",                 0),
    TEST ("",          0,  1, 0,          0, -1, "",                 0),

    TEST ("",          0, -1, 0,          0,  0, "",                 0),
    TEST ("",          0, -1, 0,          0,  1, "",                 0),
    TEST ("",          0, -1, 0,          0, -1, "",                 0),

    TEST ("1",         0,  0, 0,          0,  0, "1",                0),
    TEST ("2",         0,  0, 0,          0, -1, "22",               0),
    TEST ("3",         0,  0, 0,          0,  1, "33",               0),

    TEST ("4",         0,  1, 0,          0,  0, "",                 0),
    TEST ("5",         0,  1, 0,          0,  1, "5",                0),
    TEST ("6",         0,  1, 0,          0, -1, "6",                0),

    TEST ("7",         0, -1, 0,          0,  0, "",                 0),
    TEST ("8",         0, -1, 0,          0,  1, "8",                0),
    TEST ("9",         0, -1, 0,          0, -1, "9",                0),

    TEST ("x@4096",    0,  0, 0,          0,  0, "x@4096",           0),
    TEST ("x@4096",    0,  1, 0,          0,  1, "x@4096",           0),
    TEST ("x@4096",    0,  2, 0,          0,  2, "x@4096",           0),
    TEST ("x@4096",    0, -1, 0,          0, -1, "x@4096",           0),

    TEST ("abc",       0, 0, 0,           1, 1,  "babc",             0),
    TEST ("abc",       2, 0, 0,           0, 2,  "ababc",            0),
    TEST ("a<U0>bc<U0>@2", 0, 0, 0,       4, 2,  "<U0>@2a<U0>bc<U0>@2", 0),
    TEST ("a<U0>bc<U0>@2", 6, 0, 0,       1, 3,  "a<U0>bc<U0>@3bc",     0),
    TEST ("abcdef",    0, 0, 0,           1, 2,  "bcabcdef",            0),

    TEST ("last",      4, 0, "test",      0, 4, "lasttest",          0)
};

/**************************************************************************/

// exercises:
// replace (size_type, size_type, value_type, size_type)
// replace (iterator, iterator, size_type, value_type)
static const StringTestCase
iter_iter_size_val_test_cases [] = {

// iter_iter_size_val_test_cases serves a double duty
#define size_size_size_val_test_cases iter_iter_size_val_test_cases

#undef TEST
#define TEST(str, off, size, count, val, res, bthrow) { \
        __LINE__, off, size, -1, count, val,            \
        str, sizeof str - 1,                            \
        0, 0, res, sizeof res - 1, bthrow               \
    }

    //    +------------------------------------------ controlled sequence
    //    |            +----------------------------- replace() pos1 argument
    //    |            |  +-------------------------- replace() n1 argument
    //    |            |  |  +----------------------- replace() count argument
    //    |            |  |  |   +------------------- character to be inserted
    //    |            |  |  |   |    +-------------- expected result sequence
    //    |            |  |  |   |    |       +------ exception info
    //    |            |  |  |   |    |       |       -1   - no exceptions
    //    |            |  |  |   |    |       |        0   - exception safety
    //    |            |  |  |   |    |       |        1,2 - out_of_range
    //    |            |  |  |   |    |       |        3   - length_error
    //    |            |  |  |   |    |       |
    //    |            |  |  |   |    |       +--------+
    //    |            |  |  |   |    |                |
    //    V            V  V  V   V    V                V
    TEST ("ab",        0, 0, 1, 'c',  "cab",           0),

    TEST ("",          0, 0, 0, 'c',  "",              0),
    TEST ("",          0, 0, 3, 'c',  "ccc",           0),

    TEST ("ab",        0, 2, 0, 'c',  "",              0),
    TEST ("ab",        0, 1, 0, 'c',  "b",             0),

    TEST ("<U0>",      0, 1, 0, ' ',  "",              0),
    TEST ("<U0>",      0, 1, 1, 'a',  "a",             0),
    TEST ("<U0>",      0, 1, 1, '\0', "<U0>",          0),
    TEST ("<U0>",      0, 1, 2, '\0', "<U0>@2",        0),

    TEST ("ah",        0, 1, 1, 'c',  "ch",            0),
    TEST ("ah",        1, 1, 1, 'c',  "ac",            0),
    TEST ("ah",        0, 1, 4, 'c',  "cccch",         0),
    TEST ("ah",        1, 1, 4, 'c',  "acccc",         0),

    TEST ("abc",       0, 2, 2, 'c',  "ccc",           0),
    TEST ("abc",       1, 2, 2, 'c',  "acc",           0),

    TEST ("abc",       0, 3, 1, 'c',  "c",             0),
    TEST ("abc",       0, 3, 5, 'c',  "ccccc",         0),
    TEST ("abc",       2, 1, 1, 'c',  "abc",           0),
    TEST ("abc",       2, 1, 0, 'c',  "ab",            0),

    TEST ("abc",       1, 1, 5, 'c',  "acccccc",       0),
    TEST ("abc",       0, 0, 2, 'c',  "ccabc",         0),
    TEST ("abc",       0, 0, 3, '\0', "<U0>@3abc",     0),
    TEST ("abc",       2, 0, 2, 'e',  "abeec",         0),
    TEST ("abc",       2, 0, 3, '\0', "ab<U0>@3c",     0),
    TEST ("abc",       1, 0, 1, '\0', "a<U0>bc",       0),

    TEST ("a<U0>b<U0>@2c", 0, 3, 1, '\0', "<U0>@3c",      0),
    TEST ("a<U0>b<U0>@2c", 2, 3, 2, '\0', "a<U0>@3c",     0),
    TEST ("a<U0>b<U0>@2c", 2, 2, 1, '\0', "a<U0>@3c",     0),
    TEST ("<U0>ab<U0>@2c", 2, 3, 0, '\0', "<U0>ac",       0),
    TEST ("<U0>ab<U0>@2c", 2, 1, 2, '\0', "<U0>a<U0>@4c", 0),

    TEST ("a<U0>b<U0>@2c", 0, 6, 2, '\0', "<U0>@2",       0),

    TEST ("<U0>ab<U0>@2c", 0, 0, 2, '\0', "<U0>@3ab<U0>@2c",     0),
    TEST ("a<U0>b<U0>c<U0>", 6, 0, 2, '\0', "a<U0>b<U0>c<U0>@3", 0),
    TEST ("<U0>ab<U0>@2c", 5, 0, 1, '\0', "<U0>ab<U0>@3c",       0),

    TEST ("x@4096",    0, 4095,  2, 'a',  "aax",       0),
    TEST ("x@4096",    1, 4094,  2, 'a',  "xaax",      0),
    TEST ("x@4096",    0, 4094,  2, 'a',  "aaxx",      0),
    TEST ("x@4096",    1, 4093,  0, 'a',  "xxx",       0),
    TEST ("x@4096",    1, 4092,  1, '\0', "x<U0>xxx",  0),

    TEST ("x@127",     0,    0,  1, 'x', "x@128",      0),
    TEST ("x@200",   128,    7, 14, 'x', "x@207",      0),
    TEST ("x@331",   331,    1,  3, 'x', "x@334",      0),
    TEST ("x@539",   539,    0,  1, 'x', "x@540",      0),
    TEST ("x@539",     0,    0,873, 'x', "x@1412",     0),
    TEST ("x@873",   873,  0, 1411, 'x', "x@2284",     0),
    TEST ("x@3694",  128,    1,  2, 'x', "x@3695",     0),
    TEST ("x@539",   537,    2,  3, 'a', "x@537aaa",   0),
    TEST ("",          0,  0, 3695, 'x', "x@3695",     0),

    TEST ("a",         0,    1, 4095, 'x', "x@4095",       0),
    TEST ("x@4096",    0, 4095, 4095, 'a', "a@4095x",      0),
    TEST ("x@4096",    0, 2047, 2047, 'b', "b@2047x@2049", 0),

    TEST ("x@4096", 2047, 2048, 2048,   'x', "x@4096",     0),

    TEST ("<U0>",       2,   0,    0, ' ',  "<U0>",        1),
    TEST ("a",         10,   0,    0, ' ',  "a",           1),
    TEST ("x@4096",  4106,   0,    0, ' ',  "x@4096",      1),

    TEST ("a",          0,   1, 4095, 'x', "x@4095",       0),

    TEST ("last",      4, 0, 4, 't', "lasttttt",           0)
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class String, class Iterator>
struct ReplaceRange: RangeBase<String> {

    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;

    ReplaceRange () { }

    virtual String&
    operator() (String &str, 
                const StringTestCaseData<StringChar>& tdata) const {

        const StringTestCase &tcase = tdata.tcase_;

        // create a pair of iterators into the string object being modified
        const StringIter first1 (str.begin () + tdata.off1_);
        const StringIter last1  (first1 + tdata.ext1_);

        const String &cstr = str;

        // when (0 == tcase.arg) exercise self-referential modification
        // (i.e., replacing a range of elements with a subrange of its
        // own elements)
        const StringChar* const beg =
            (tcase.arg ? tdata.arg_ : cstr.data ()) + tdata.off2_;

        const StringChar* const end = beg + tdata.ext2_;

        const Iterator first2 (beg, beg, end);
        const Iterator last2  (end, beg, end);
#if TEST_RW_EXTENSIONS
        return str.replace (first1, last1, first2, last2);
#else
        // The iterators had to be valid according to the standard.
        if ((&(*last1) < &(*first1))  // wrap around
         || (tdata.off1_ + tdata.ext1_ > str.size())) {  // length too long
            const StringIter tmpEnd(str.end());
            return str.replace(first1, str.end(), first2, last2);
        }

        return str.replace (first1, last1, first2, last2);
#endif
    }
};


// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class String, class Iterator>
struct ReplaceRangeOverload: RangeBase<String>
{
    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;

    ReplaceRangeOverload () { }

    virtual String&
    operator() (String &str, 
                const StringTestCaseData<StringChar>& tdata) const {

        const StringTestCase &tcase = tdata.tcase_;

        // create a pair of iterators into the string object being modified
        const StringIter first1 (str.begin () + tdata.off1_);
        const StringIter last1  (first1 + tdata.ext1_);

        bool reverse_iter = StringIds::ReverseIterator == tdata.func_.iter_id_
            || StringIds::ConstReverseIterator == tdata.func_.iter_id_;

        const std::size_t srclen_ = tcase.arg ? tdata.arglen_ : str.size ();

        const std::size_t off = 
            reverse_iter ? srclen_ - tdata.off2_ - tdata.ext2_ : tdata.off2_;
        const std::size_t ext = tdata.ext2_;

        if (0 == tcase.arg) {
            // exercise self-referential modification (i.e., replacing
            // a range of elements with a subrange of its own elements)

            const Iterator first2 (this->begin (str, (Iterator*)0) + off);
            const Iterator last2 (first2 + ext);

            return str.replace (first1, last1, first2, last2);
        }

        String str_arg (tdata.arg_, tdata.arglen_);

        const Iterator first2 (this->begin (str_arg, (Iterator*)0) + off);
        const Iterator last2 (first2 + ext);

#if TEST_RW_EXTENSIONS
        return str.replace (first1, last1, first2, last2);
#else
        // The iterators had to be valid according to the standard.
        if ((&(*last1) < &(*first1))  // wrap around
         || (tdata.off1_ + tdata.ext1_ > str.size())) {  // length too long
            const StringIter tmpEnd(str.end());
            return str.replace(first1, str.end(), first2, last2);
        }

        return str.replace (first1, last1, first2, last2);
#endif

    }
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_replace (charT*, Traits*, Allocator*, const RangeBase<
                       std::basic_string <charT, Traits, Allocator> > &rng,
                   const StringTestCaseData<charT>                    &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;
    typedef typename UserTraits<charT>::MemFun           TraitsFunc;

    const StringFunc     &func  = tdata.func_;
    const StringTestCase &tcase = tdata.tcase_;

    // construct the string object to be modified
    // and the (possibly unused) argument string
    /* const */ String str (tdata.str_, tdata.strlen_);
    const       String arg (tdata.arg_, tdata.arglen_);

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    // create a pair of iterators into the string object being
    // modified (used only by the iterator overloads)
    const StringIter first (str.begin () + tdata.off1_);
    const StringIter last  (first + tdata.ext1_);

    // offset and extent function arguments
    const std::size_t arg_off  = std::size_t (tcase.off);
    const std::size_t arg_size = std::size_t (tcase.size);
    const std::size_t arg_off2 = std::size_t (tcase.off2);
    const std::size_t arg_size2 =
        0 <= tcase.size2 ? tcase.size2 : str.max_size () + 1;

    // string function argument
    const charT* const arg_ptr = tcase.arg ? arg.data () : str.data ();
    const String&      arg_str = tcase.arg ? arg : str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    // get a pointer to the Traits::length() function call counter
    // or 0 when no such counter exists (i.e., when Traits is not
    // UserTraits)
    std::size_t* length_calls =
           Replace (size_size_cptr) == func.which_
        || Replace (iter_iter_cptr) == func.which_ ?
        rw_get_call_counters ((Traits*)0, (charT*)0) : 0;

    if (length_calls) {
        length_calls += TraitsFunc::length;
        *length_calls = 0;
    }

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // out_of_range is only generated from size_type overloads
    // of replace() and not from the iterator equivalents of
    // the same functions
    const bool use_iters =
        StringIds::arg_iter == StringIds::arg_type (func.which_, 2);

    // iterate for`throw_count' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        if (1 == tcase.bthrow && !use_iters)
            expected = exceptions [1];      // out_of_range
        else if (2 == tcase.bthrow && !use_iters)
            expected = exceptions [1];      // out_of_range
        else if (3 == tcase.bthrow && !use_iters)
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

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        // pointer to the returned reference
        const String* ret_ptr = 0;

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        try {
            switch (func.which_) {
            case Replace (size_size_cptr):
                ret_ptr = &str.replace (arg_off, arg_size, arg_ptr);
                break;

            case Replace (size_size_cstr):
                ret_ptr = &str.replace (arg_off, arg_size, arg_str);
                break;

            case Replace (size_size_cptr_size):
                ret_ptr = &str.replace (arg_off, arg_size, arg_ptr, arg_size2);
                break;

            case Replace (size_size_cstr_size_size):
                ret_ptr = &str.replace (arg_off, arg_size, arg_str,
                                        arg_off2, arg_size2);
                break;

            case Replace (size_size_size_val):
                ret_ptr = &str.replace (arg_off, arg_size, arg_size2, arg_val);
                break;

            case Replace (iter_iter_cptr):
                ret_ptr = &str.replace (first, last, arg_ptr);
                break;

            case Replace (iter_iter_cstr):
                ret_ptr = &str.replace (first, last, arg_str);
                break;

            case Replace (iter_iter_cptr_size):
                ret_ptr = &str.replace (first, last, arg_ptr, arg_size2);
                break;

            case Replace (iter_iter_size_val):
                ret_ptr = &str.replace (first, last, arg_size2, arg_val);
                break;

            case Replace (iter_iter_range):
                ret_ptr = &rng (str, tdata);
                break;

            default:
                RW_ASSERT (!"logic error: unknown replace overload");
            }

            // verify that the reference returned from the function
            // refers to the modified string object (i.e., *this
            // within the function)
            const std::ptrdiff_t ret_off = ret_ptr - &str;

            // verify the returned value
            rw_assert (0 == ret_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} returned invalid reference, "
                       "offset is %td", __LINE__, ret_off);

            // for convenience
            static const int cwidth = sizeof (charT);

            // verfiy that the length of the resulting string
            rw_assert (tdata.reslen_ == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{/*.*Gs} with "
                       "length %zu, got %{/*.*Gs} with length %zu",
                       __LINE__,
                       cwidth, int (tdata.reslen_), tdata.res_,
                       tdata.reslen_, cwidth, int (str.size ()), str.data (),
                       str.size ());

            if (tdata.reslen_ == str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, str.data (), str.size ());
#if DRQS
// Not alias safe yet.
                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__,
                           cwidth, int (tdata.reslen_), tdata.res_,
                           cwidth, int (str.size ()),str.data (), match);
#endif
            }

            // verify that Traits::length() was used
            if (length_calls) {
                rw_assert (0 < *length_calls,
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits_type::length()", __LINE__);
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
            // verify that the exception didn't cause a change
            // in the state of the object
#if TEST_RW_EXTENSIONS
            str_state.assert_equal (rw_get_string_state (str),
                                    __LINE__, tcase.line, caught);
#else
    // standard doesn't say that we have to preserve state when an allocation
    // exception is thrown???
            if (caught != exceptions[3]) {
                str_state.assert_equal (rw_get_string_state (str),
                                    __LINE__, tcase.line, caught);
            }
#endif

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

    const std::size_t expect_throws = StringIds::UserAlloc == func.alloc_id_ ?
        str_state.capacity_ < str.capacity () : 0;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify number of exceptions thrown
    // for range version the allocation may take place several times
    if (Replace (iter_iter_range) != func.which_)
        rw_assert (expect_throws == throw_count, 0, tcase.line,
                   "line %d: %{$FUNCALL}: expected exactly 1 %s exception "
                   "while changing capacity from %zu to %zu, got %zu",
                   __LINE__, exceptions [3],
                   str_state.capacity_, str.capacity (), throw_count);
    else
        rw_assert (expect_throws <= throw_count, 0, tcase.line,
                   "line %d: %{$FUNCALL}: expected at least 1 %s exception "
                   "while changing capacity from %zu to %zu, got %zu",
                   __LINE__, exceptions [3],
                   str_state.capacity_, str.capacity (), throw_count);

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_replace (charT*, Traits*, Allocator*,
                   const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string<charT, Traits, Allocator> String;

    if (tdata.func_.which_ == Replace (iter_iter_range)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename String::Iterator Iter;                             \
        static const                                                        \
        ReplaceRangeOverload<String, Iter> rng;                             \
        test_replace ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);    \
    } while (0)

        case StringIds::Pointer: TEST (pointer); break;
        case StringIds::ConstPointer: TEST (const_pointer); break;
        case StringIds::Iterator: TEST (iterator); break;
        case StringIds::ConstIterator: TEST (const_iterator); break;

        case StringIds::ReverseIterator: TEST (reverse_iterator); break;
        case StringIds::ConstReverseIterator: TEST (const_reverse_iterator); 
            break;

        // exercise specializations of the member function template
        // on the required iterator categories
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef Iterator<charT> Iter;                                       \
        static const                                                        \
        ReplaceRange<String, Iter> rng;                                     \
        test_replace ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);    \
    } while (0)

        case StringIds::Input: TEST (InputIter); break;
        case StringIds::Forward: TEST (ConstFwdIter); break;
        case StringIds::Bidir: TEST (ConstBidirIter); break;
        case StringIds::Random: TEST (ConstRandomAccessIter); break;

        default:
            rw_error (0, 0, __LINE__, "bad iterator id");
        }
    }
    else {
        // exercise ordinary overloads of the member function
        static const RangeBase<String> rng;
        test_replace ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_replace);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Replace (sig), sig ## _test_cases,                      \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases, \
    }

        TEST (size_size_cptr),
        TEST (size_size_cstr),
        TEST (size_size_cptr_size),
        TEST (size_size_cstr_size_size),
        TEST (size_size_size_val),
        TEST (iter_iter_cptr),
        TEST (iter_iter_cstr),
        TEST (iter_iter_cptr_size),
        TEST (iter_iter_size_val),
        TEST (iter_iter_range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_string_test (argc, argv, __FILE__,
                            "lib.string.replace",
                            test_replace_func_array, tests, test_count);

    return status;
}
