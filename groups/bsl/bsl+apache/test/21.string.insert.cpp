/***************************************************************************
 *
 * 21.string.insert.cpp - string test exercising lib.string.insert
 *
 * $Id: 21.string.insert.cpp 424860 2006-07-23 23:50:25Z sebor $
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

#include <string>         // for string
#include <stdexcept>      // for out_of_range, length_error
#include <cstddef>        // for size_t

#include <21.strings.h>   // for StringIds
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Insert(sig)   StringIds::insert_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// insert (size_type, const value_type*)
static const StringTestCase
size_cptr_test_cases [] = {

#undef TEST
#define TEST(str, off, arg, res, bthrow) {                      \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,     \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ insert() pos argument
    //    |                |  +--------------------- sequence to be inserted
    //    |                |  |           +--------- expected result sequence
    //    |                |  |           |       +- exception info
    //    |                |  |           |       |         0 - no exception
    //    |                |  |           |       |         1 - out_of_range
    //    |                |  |           |       |         2 - length_error
    //    |                |  |           |       |        -1 - exc. safety
    //    |                |  |           |       |
    //    |                |  |           |       +-------------+
    //    V                V  V           V                     V
    TEST ("ab",            0, "c",        "cab",                0),

    TEST ("",              0, "",         "",                   0),
    TEST ("",              0, "abc",      "abc",                0),

    TEST ("<U0>",          0, "",         "<U0>",               0),
    TEST ("",              0, "<U0>",     "",                   0),

    TEST ("abc",           0, "",         "abc",                0),
    TEST ("abc",           1, "",         "abc",                0),
    TEST ("abc",           3, "",         "abc",                0),

    TEST ("<U0>",          0, "a",        "a<U0>",              0),
    TEST ("<U0>",          1, "a",        "<U0>a",              0),
    TEST ("<U0>",          0, "<U0>",     "<U0>",               0),
    TEST ("<U0>",          1, "<U0>@2",   "<U0>",               0),

    TEST ("cde",           0, "ab",       "abcde",              0),
    TEST ("cde",           1, "ab",       "cabde",              0),
    TEST ("cde",           2, "ab",       "cdabe",              0),
    TEST ("cde",           3, "ab",       "cdeab",              0),

    TEST ("<U0>ab<U0>@2c", 0, "<U0>e<U0>",  "<U0>ab<U0>@2c",    0),
    TEST ("a<U0>b<U0>@2c", 2, "e<U0>e",   "a<U0>eb<U0>@2c",     0),
    TEST ("a<U0>bc<U0>@2", 6, "e<U0>e",   "a<U0>bc<U0>@2e",     0),

    TEST ("",              0, 0,          "",                   0),
    TEST ("abc",           0, 0,          "abcabc",             0),
    TEST ("abc",           2, 0,          "ababcc",             0),
    TEST ("a<U0>bc<U0>@2", 0, 0,          "aa<U0>bc<U0>@2",     0),
    TEST ("a<U0>bc<U0>@2", 6, 0,          "a<U0>bc<U0>@2a",     0),
    TEST ("x@4096",        5, 0,          "x@8192",             0),

    TEST ("",              0, "x@4096",   "x@4096",             0),
    TEST ("x@4096",        0, "",         "x@4096",             0),
    TEST ("x@4096",        2, "a@100",    "xxa@100x@4094",      0),
    TEST ("a@102",         2, "x@4096",   "aax@4096a@100",      0),

    TEST ("x@10",          1, "x@118",    "x@128",              0),
    TEST ("x@128",       128, "x@79",     "x@207",              0),
    TEST ("x@207",         0, "x@127",    "x@334",              0),
    TEST ("x@334",         1, "x@206",    "x@540",              0),
    TEST ("x@540",       539, "x@333",    "x@873",              0),
    TEST ("x@539",       539, "x@873",    "x@1412",             0),
    TEST ("x@872",         0, "x@1412",   "x@2284",             0),
    TEST ("x@1411",      207, "x@2284",   "x@3695",             0),
    TEST ("x@1412",        0, "x@2284",   "x@3696",             0),

    TEST ("<U0>",          2, "",         "",                   1),
    TEST ("a",             2, "",         "",                   1),
    TEST ("x@4096",     4106, "",         "",                   1),

    TEST ("last",          4, "test",     "lasttest",           0)
};

/**************************************************************************/

// used to exercise
// insert (size_type, const basic_string&)
static const StringTestCase
size_cstr_test_cases [] = {

#undef TEST
#define TEST(str, off, arg, res, bthrow) {                      \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,     \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ insert() pos argument
    //    |                |  +--------------------- sequence to be inserted
    //    |                |  |           +--------- expected result sequence
    //    |                |  |           |       +- exception info
    //    |                |  |           |       |     0 - no exception
    //    |                |  |           |       |     1 - out_of_range
    //    |                |  |           |       |     2 - length_error
    //    |                |  |           |       |    -1 - exc. safety
    //    |                |  |           |       |
    //    |                |  |           |       -+------------+
    //    V                V  V           V                     V
    TEST ("ab",            0, "c",        "cab",                0),

    TEST ("",              0, "",         "",                   0),
    TEST ("",              0, "abc",      "abc",                0),

    TEST ("<U0>",          0, "",         "<U0>",               0),
    TEST ("",              0, "<U0>",     "<U0>",               0),

    TEST ("abc",           0, "",         "abc",                0),
    TEST ("abc",           1, "",         "abc",                0),
    TEST ("abc",           3, "",         "abc",                0),

    TEST ("<U0>",          0, "a",        "a<U0>",              0),
    TEST ("<U0>",          1, "a",        "<U0>a",              0),
    TEST ("<U0>",          0, "<U0>",     "<U0>@2",             0),
    TEST ("<U0>",          1, "<U0>@2",   "<U0>@3",             0),

    TEST ("cde",           0, "ab",       "abcde",              0),
    TEST ("cde",           1, "ab",       "cabde",              0),
    TEST ("cde",           2, "ab",       "cdabe",              0),
    TEST ("cde",           3, "ab",       "cdeab",              0),

    TEST ("<U0>ab<U0>@2c", 0, "<U0>e<U0>",    "<U0>e<U0>@2ab<U0>@2c",      0),
    TEST ("a<U0>b<U0>@2c", 2, "e<U0>e",       "a<U0>e<U0>eb<U0>@2c",       0),
    TEST ("a<U0>bc<U0>@2", 6, "e<U0>@2e<U0>", "a<U0>bc<U0>@2e<U0>@2e<U0>", 0),

    TEST ("abc",           0, 0,          "abcabc",             0),
    TEST ("abc",           2, 0,          "ababcc",             0),
    TEST ("a<U0>bc<U0>@2", 0, 0,          "a<U0>bc<U0>@2a<U0>bc<U0>@2", 0),
    TEST ("a<U0>bc<U0>@2", 6, 0,          "a<U0>bc<U0>@2a<U0>bc<U0>@2", 0),
    TEST ("x@4096",        5, 0,          "x@8192",             0),

    TEST ("",              0, "x@4096",   "x@4096",             0),
    TEST ("x@4096",        0, "",         "x@4096",             0),
    TEST ("x@4096",        2, "a@100",    "xxa@100x@4094",      0),
    TEST ("a@102",         2, "x@4096",   "aax@4096a@100",      0),

    TEST ("x@10",          1, "x@118",    "x@128",              0),
    TEST ("x@128",       128, "x@79",     "x@207",              0),
    TEST ("x@207",         0, "x@127",    "x@334",              0),
    TEST ("x@334",         1, "x@206",    "x@540",              0),
    TEST ("x@540",       539, "x@333",    "x@873",              0),
    TEST ("x@539",       539, "x@873",    "x@1412",             0),
    TEST ("x@872",         0, "x@1412",   "x@2284",             0),
    TEST ("x@1411",      207, "x@2284",   "x@3695",             0),
    TEST ("x@1412",        0, "x@2284",   "x@3696",             0),

    TEST ("<U0>",          2, "",         "",                   1),
    TEST ("a",             2, "",         "",                   1),
    TEST ("x@4096",     4106, "",         "",                   1),

    TEST ("last",          4, "test",     "lasttest",           0)
};

/**************************************************************************/

// exrcises
// insert (size_type, basic_string&, size_type, size_type)
// insert (iterator, InputIterator, InputIterator)
static const StringTestCase
iter_range_test_cases [] = {

// range_test_cases serves a double duty
#define size_cstr_size_size_test_cases iter_range_test_cases

#undef TEST
#define TEST(str, off, arg, off2, size2, res, bthrow) {                 \
        __LINE__, off, -1, off2, size2, -1, str, sizeof str - 1,        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow                \
    }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ insert() pos argument
    //    |                |  +--------------------- sequence to be inserted
    //    |                |  |            +-------- insert() off2 argument
    //    |                |  |            |  +----- insert() num argument
    //    |                |  |            |  |  +-- expected result sequence
    //    |                |  |            |  |  |  +--- exception info
    //    |                |  |            |  |  |  |       0 - no exception
    //    |                |  |            |  |  |  |       1 - out_of_range
    //    |                |  |            |  |  |  |       2 - length_error
    //    |                |  |            |  |  |  |      -1 - exc. safety
    //    |                |  |            |  |  |  |
    //    |                |  |            |  |  |  +----------------+
    //    V                V  V            V  V  V                   V
    TEST ("ab",            0, "c",         0, 1, "cab",              0),

    TEST ("",              0, "",          0, 0,  "",                0),
    TEST ("",              0, "abc",       1, 1,  "b",               0),
    TEST ("",              0, "<U0>",      0, 1,  "<U0>",            0),

    TEST ("<U0>",          0, "",          0, 0,  "<U0>",            0),
    TEST ("<U0>",          1, "",          0, 0,  "<U0>",            0),

    TEST ("abc",           0, "",          0, 0,  "abc",             0),
    TEST ("abc",           1, "",          0, 0,  "abc",             0),
    TEST ("abc",           3, "",          0, 0,  "abc",             0),

    TEST ("<U0>",          0, "a",         0, 1,  "a<U0>",           0),
    TEST ("<U0>",          1, "a",         0, 1,  "<U0>a",           0),
    TEST ("<U0>",          0, "<U0>@2",    1, 1,  "<U0>@2",          0),
    TEST ("<U0>",          1, "<U0>@2",    0, 2,  "<U0>@3",          0),
    TEST ("<U0>",          1, "<U0>@2",    1, 5,  "<U0>@2",          0),

    TEST ("cde",           0, "ab",        0, 2,  "abcde",           0),
    TEST ("cde",           1, "ab",        0, 1,  "cade",            0),
    TEST ("cde",           2, "ab",        1, 5,  "cdbe",            0),
    TEST ("cde",           3, "ab",        1, 1,  "cdeb",            0),

    TEST ("ab",            0, "c<U0>e",    0, 3,  "c<U0>eab",        0),
    TEST ("ab",            1, "c<U0>e",    1, 2,  "a<U0>eb",         0),
    TEST ("ab",            2, "c<U0>e",    0, 2,  "abc<U0>",         0),

    TEST ("<U0>e<U0>",     1, "<U0>ab<U0>@2c", 0, 9, "<U0>@2ab<U0>@2ce<U0>",0),
    TEST ("<U0>e<U0>",     1, "<U0>ab<U0>@2c", 0, 3, "<U0>@2abe<U0>",       0),
    TEST ("a<U0>b<U0>@2c", 3, "<U0>e<U0>",     0, 3, "a<U0>b<U0>e<U0>@3c",  0),
    TEST ("a<U0>b<U0>@2c", 2, "<U0>@2e<U0>",   0, 2, "a<U0>@3b<U0>@2c",     0),
    TEST ("<U0>ab<U0>@2c", 0, "<U0>e<U0>",     2, 1, "<U0>@2ab<U0>@2c",     0),
    TEST ("a<U0>bc<U0>@2", 6, "<U0>e",         0, 2, "a<U0>bc<U0>@3e",      0),

    TEST ("abc",           0, 0,           1, 1,  "babc",                0),
    TEST ("abc",           2, 0,           0, 2,  "ababc",               0),
    TEST ("a<U0>bc<U0>@2", 0, 0,           4, 2,  "<U0>@2a<U0>bc<U0>@2", 0),
    TEST ("a<U0>bc<U0>@2", 6, 0,           1, 3,  "a<U0>bc<U0>@3bc",     0),
    TEST ("abcdef",        0, 0,           1, 2,  "bcabcdef",            0),

    TEST ("",              0, "x@4096",    0, 4096, "x@4096",        0),
    TEST ("x@4096",        0, "",          0,    0, "x@4096",        0),
    TEST ("a@1000",        0, "b@1000",    0,  999, "b@999a@1000",   0),
    TEST ("a@1000",        1, "b@1001",    0, 1000, "ab@1000a@999",  0),
    TEST ("a@1000",        2, "b@1002",    0, 1001, "aab@1001a@998", 0),
    TEST ("a@1000",      998, "b@1000",    1,  999, "a@998b@999aa",  0),
    TEST ("a@1000",      999, "b@1001",    2, 1000, "a@999b@999a",   0),
    TEST ("a@1000",        2, "b@1002",  999, 1001, "aabbba@998",    0),

    TEST ("x@10",          1, "x@118",     0,  118, "x@128",         0),
    TEST ("x@128",         0, "x@129",    50,   79, "x@207",         0),
    TEST ("x@207",       207, "x@127",     0,  127, "x@334",         0),
    TEST ("x@207",       128, "x@207",    10,  127, "x@334",         0),
    TEST ("x@334",       334, "x@208",     2,  206, "x@540",         0),
    TEST ("x@540",         0, "x@336",     3,  333, "x@873",         0),
    TEST ("x@539",       538, "x@873",     0,  873, "x@1412",        0),
    TEST ("x@873",       540, "x@540",     1,  539, "x@1412",        0),
    TEST ("x@872",         0, "x@1412",    0, 1412, "x@2284",        0),
    TEST ("x@1411",     1411, "x@2288",    4, 2284, "x@3695",        0),
    TEST ("x@1411",      872, "x@3695",  128, 2284, "x@3695",        0),
    TEST ("x@1412",        0, "x@2284",    0, 2284, "x@3696",        0),

    TEST ("<U0>",          2, "",          0,    0,  "<U0>",         1),
    TEST ("",              0, "<U0>",      2,    0,  "",             2),

    TEST ("a",             2, "",          0,    0,  "a",            1),
    TEST ("",              0, "a",         2,    0,  "",             2),

    TEST ("x@4096",     4106, "",          0,    0,  "x@4096",       1),
    TEST ("",              0, "x@4096", 4106,    0,  "",             2),

    TEST ("last",          4, "test",      0,    4,  "lasttest",     0)
};

/**************************************************************************/

// used to exercise
// insert (size_type, const value_type*, size_type)
static const StringTestCase
size_cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, off, arg, size2, res, bthrow) {               \
        __LINE__, off, -1, -1, size2, -1, str, sizeof str - 1,  \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ insert() pos argument
    //    |                |  +--------------------- sequence to be inserted
    //    |                |  |            +-------- insert() num argument
    //    |                |  |            |  +----- expected result sequence
    //    |                |  |            |  |     +--- exception info
    //    |                |  |            |  |     |      0 - no exception
    //    |                |  |            |  |     |      1 - out_of_range
    //    |                |  |            |  |     |      2 - length_error
    //    |                |  |            |  |     |     -1 - exc. safety
    //    |                |  |            |  |     |
    //    |                |  |            |  |     +-----------+
    //    V                V  V            V  V                 V
    TEST ("ab",            0, "c",         1, "cab",            0),

    TEST ("",              0, "",          0, "",               0),
    TEST ("",              0, "abc",       1, "a",              0),

    TEST ("<U0>",          0, "",          0, "<U0>",           0),
    TEST ("<U0>",          1, "",          0, "<U0>",           0),
    TEST ("",              0, "<U0>",      1, "<U0>",           0),

    TEST ("<U0>",          0, "a",         0, "<U0>",           0),
    TEST ("<U0>",          1, "a",         1, "<U0>a",          0),
    TEST ("<U0>",          0, "<U0>@2",    1, "<U0>@2",         0),
    TEST ("<U0>",          1, "<U0>@2",    2, "<U0>@3",         0),

    TEST ("cde",           0, "ab",        2, "abcde",          0),
    TEST ("cde",           1, "ab",        1, "cade",           0),
    TEST ("cde",           3, "ab",        1, "cdea",           0),
    TEST ("cde",           2, "ab",        2, "cdabe",          0),

    TEST ("<U0>e<U0>",     3, "a<U0>b<U0>@2c", 0,  "<U0>e<U0>",          0),
    TEST ("<U0>e<U0>",     3, "<U0>ab<U0>@2c", 3,  "<U0>e<U0>@2ab",      0),

    TEST ("a<U0>b<U0>@2c", 3, "<U0>e<U0>",     3,  "a<U0>b<U0>e<U0>@3c", 0),
    TEST ("a<U0>b<U0>@2c", 2, "<U0>@2e<U0>",   2,  "a<U0>@3b<U0>@2c",    0),
    TEST ("<U0>ab<U0>@2c", 0, "<U0>e<U0>",     1,  "<U0>@2ab<U0>@2c",    0),
    TEST ("a<U0>bc<U0>@2", 6, "<U0>e",         2,  "a<U0>bc<U0>@3e",     0),

    TEST ("abc",           0, 0,           1, "aabc",            0),
    TEST ("abc",           2, 0,           2, "ababc",           0),
    TEST ("<U0>abc<U0>@2", 0, 0,           1, "<U0>@2abc<U0>@2", 0),
    TEST ("a<U0>bc<U0>@2", 6, 0,           3, "a<U0>bc<U0>@2a<U0>b",  0),
    TEST ("a@4096",        0, 0,        1111, "a@5207",          0),
    TEST ("b@4096",        1, 0,        2222, "b@6318",          0),

    TEST ("x@4096",        1, "<U0>",      1, "x<U0>x@4095",     0),
    TEST ("x@4096",        0, "<U0>",      1, "<U0>x@4096",      0),
    TEST ("x@4096",     4096, "<U0>",      1, "x@4096<U0>",      0),
    TEST ("x@4096",        0, "ab",        2, "abx@4096",        0),
    TEST ("x@4096",     4095, "ab",        1, "x@4095ax",        0),
    TEST ("x@4096",     4094, "<U0>@2",    2, "x@4094<U0>@2xx",  0),

    TEST ("",              0, "x@4096", 4096, "x@4096",          0),
    TEST ("x@4096",        0, "",          0, "x@4096",          0),

    TEST ("",              0, "x@873",   540, "x@540",           0),
    TEST ("x@10",          1, "x@118",   118, "x@128",           0),
    TEST ("x@128",       128, "x@79",     79, "x@207",           0),
    TEST ("x@207",         0, "x@127",   127, "x@334",           0),
    TEST ("x@207",       207, "x@207",   127, "x@334",           0),
    TEST ("x@334",       128, "x@206",   206, "x@540",           0),
    TEST ("x@540",       540, "x@333",   333, "x@873",           0),
    TEST ("x@539",         0, "x@873",   873, "x@1412",          0),
    TEST ("x@873",         5, "x@540",   539, "x@1412",          0),
    TEST ("x@872",         0, "x@1412", 1412, "x@2284",          0),
    TEST ("x@1411",     1411, "x@2284", 2284, "x@3695",          0),
    TEST ("x@1411",      873, "x@3695", 2284, "x@3695",          0),
    TEST ("x@1412",        0, "x@2284", 2284, "x@3696",          0),
    TEST ("",              0, "x@3696", 3696, "x@3696",          0),

    TEST ("<U0>",          2, "",          0,  "",               1),
    TEST ("a",             2, "",          0,  "",               1),
    TEST ("x@4096",     4106, "",          0,  "",               1),

    TEST ("last",          4, "test",      4,  "lasttest",       0)
};

/**************************************************************************/

// exrecises
// insert (size_type, size_type, value_type)
// insert (iterator, size_type, value_type)
static const StringTestCase
iter_size_val_test_cases [] = {

// size_val_test_cases serves a double duty
#define size_size_val_test_cases iter_size_val_test_cases

#undef TEST
#define TEST(str, off, size2, val, res, bthrow) {               \
        __LINE__, off, -1, -1, size2, val, str, sizeof str - 1, \
        0, 0, res, sizeof res - 1, bthrow                       \
    }

    //    +---------------------------------------- controlled sequence
    //    |                +----------------------- insert() pos argument
    //    |                |  +-------------------- insert() size2 argument
    //    |                |  |      +------------- character to be inserted
    //    |                |  |      |   +--------- expected result sequence
    //    |                |  |      |   |       +- exception info
    //    |                |  |      |   |       |      0 - no exception
    //    |                |  |      |   |       |      1 - out_of_range
    //    |                |  |      |   |       |      2 - length_error
    //    |                |  |      |   |       |     -1 - exc. safety
    //    |                |  |      |   |       |
    //    |                |  |      |   |       +---------+
    //    V                V  V      V   V                 V
    TEST ("ab",            0, 1,    'c', "cab",            0),

    TEST ("",              0, 0,    ' ',  "",              0),
    TEST ("",              0, 1,    'b',  "b",             0),

    TEST ("<U0>",          0, 0,    ' ',  "<U0>",          0),
    TEST ("<U0>",          1, 0,    ' ',  "<U0>",          0),
    TEST ("",              0, 2,    '\0', "<U0>@2",        0),

    TEST ("<U0>",          0, 1,    'a',  "a<U0>",         0),
    TEST ("<U0>",          1, 1,    'a',  "<U0>a",         0),
    TEST ("<U0>",          0, 1,    '\0', "<U0>@2",        0),
    TEST ("<U0>",          1, 2,    '\0', "<U0>@3",        0),
    TEST ("<U0>",          1, 0,    '\0', "<U0>",          0),

    TEST ("cde",           0, 2,    'a',  "aacde",         0),
    TEST ("cde",           1, 1,    'a',  "cade",          0),
    TEST ("cde",           3, 3,    'a',  "cdeaaa",        0),
    TEST ("cde",           2, 3,    'a',  "cdaaae",        0),

    TEST ("ab",            0, 2,    '\0', "<U0>@2ab",      0),
    TEST ("ab",            1, 1,    '\0', "a<U0>b",        0),
    TEST ("ab",            2, 2,    '\0', "ab<U0>@2",      0),

    TEST ("a<U0>b<U0>@2c", 3, 2,    '\0', "a<U0>b<U0>@3<U0>c",   0),
    TEST ("a<U0>b<U0>@2c", 2, 1,    '\0', "a<U0>@2b<U0>@2c",     0),
    TEST ("<U0>ab<U0>@2c", 0, 3,    '\0', "<U0>@3<U0>ab<U0>@2c", 0),
    TEST ("a<U0>bc<U0>@2", 6, 2,    'a',  "a<U0>bc<U0>@2aa",     0),

    TEST ("",              0, 4106, 'x',  "x@4106",        0),
    TEST ("x@4096",        0, 2,    'a',  "aax@4096",      0),

    TEST ("x@4096",        0, 2,    'a',  "aax@4096",      0),
    TEST ("x@4096",        1, 2,    'a',  "xaax@4095",     0),
    TEST ("x@4096",        1, 1,    '\0', "x<U0>x@4095",   0),

    TEST ("x@127",         0, 1,    'x',  "x@128",         0),
    TEST ("x@200",       128, 7,    'x',  "x@207",         0),
    TEST ("x@331",       331, 3,    'x',  "x@334",         0),
    TEST ("x@539",         0, 1,    'x',  "x@540",         0),
    TEST ("x@539",         0, 873,  'x',  "x@1412",        0),
    TEST ("x@873",       873, 1411, 'x',  "x@2284",        0),
    TEST ("x@3694",      128, 1,    'x',  "x@3695",        0),
    TEST ("x@540",         0, 1,    'x',  "x@541",         0),
    TEST ("",              0, 3695, 'x',  "x@3695",        0),

    TEST ("a",             0, 4095, 'x',  "x@4095a",       0),
    TEST ("x@4096",        0, 2047, 'b',  "b@2047x@4096",  0),
    TEST ("x@4096",     2047, 2048, 'x',  "x@6144",        0),

    TEST ("last",          4, 4,    't',  "lasttttt",      0)
};

/**************************************************************************/

// used to exercise
// insert (iterator, value_type)
static const StringTestCase
iter_val_test_cases [] = {

#undef TEST
#define TEST(str, off, val, res, bthrow)                                \
    { __LINE__, off, -1, -1, -1, val, str, sizeof str - 1, 0,           \
      0, res, sizeof res - 1, bthrow }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ insert() pos argument
    //    |                |    +------------------- character to be inserted
    //    |                |    |   +--------------- expected result sequence
    //    |                |    |   |               +--- exception info
    //    |                |    |   |               |       0 - no exception
    //    |                |    |   |               |       1 - out_of_range
    //    |                |    |   |               |       2 - length_error
    //    |                |    |   |               |      -1 - exc. safety
    //    |                |    |   |               |
    //    |                |    |   |               |
    //    V                V    V   V               V
    TEST ("ab",            0,  'c', "cab",          0),

    TEST ("",              0,  'b', "b",            0),
    TEST ("",              0, '\0', "<U0>",         0),

    TEST ("<U0>",          0,  'a', "a<U0>",        0),
    TEST ("<U0>",          1,  'a', "<U0>a",        0),
    TEST ("<U0>",          0, '\0', "<U0>@2",       0),
    TEST ("<U0>",          1, '\0', "<U0>@2",       0),

    TEST ("cde",           0,  'a', "acde",         0),
    TEST ("cde",           1,  'a', "cade",         0),
    TEST ("cde",           2,  'a', "cdae",         0),
    TEST ("cde",           3,  'a', "cdea",         0),

    TEST ("a<U0>b<U0>@2c", 3, '\0', "a<U0>b<U0>@3c",    0),
    TEST ("<U0>ab<U0>@2c", 0, '\0', "<U0>@2ab<U0>@2c",  0),
    TEST ("a<U0>bc<U0>@2", 6,  'a', "a<U0>bc<U0>@2a",   0),

    TEST ("x@4096",        0,  'a', "ax@4096",      0),
    TEST ("x@4096",        1,  'a', "xax@4095",     0),
    TEST ("x@4096",        1, '\0', "x<U0>x@4095",  0),

    TEST ("x@127",         0, 'x',  "x@128",        0),
    TEST ("x@206",         1, 'x',  "x@207",        0),
    TEST ("x@333",       333, 'x',  "x@334",        0),
    TEST ("x@539",       128, 'x',  "x@540",        0),
    TEST ("x@1411",        0, 'x',  "x@1412",       0),
    TEST ("x@2283",       10, 'x',  "x@2284",       0),
    TEST ("x@3694",     3694, 'x',  "x@3695",       0),
    TEST ("x@540",       538, 'x',  "x@541",        0),

    TEST ("last",          4,  't', "lastt",        0)
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class String, class Iterator>
struct InsertRange: RangeBase<String> {

    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;

    InsertRange () { }

    virtual String&
    operator() (String &str,
                const StringTestCaseData<StringChar>& tdata) const {

        const StringTestCase &tcase = tdata.tcase_;

        // create a pair of iterators into the string object being modified
        const StringIter first1 (str.begin () + tdata.off1_);

        const String &cstr = str;

        // when (0 == tcase.arg) exercise self-referential modification
        // (i.e., replacing a range of elements with a subrange of its
        // own elements)
        const StringChar* const beg =
            (tcase.arg ? tdata.arg_ : cstr.data ()) + tdata.off2_;

        const StringChar* const end = beg + tdata.ext2_;

        const Iterator first2 (beg, beg, end);
        const Iterator last2  (end, beg, end);

        str.insert (first1, first2, last2);
        return str;
    }
};


// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class String, class Iterator>
struct InsertRangeOverload: RangeBase<String>
{
    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;

    InsertRangeOverload () { }

    virtual String&
    operator() (String &str,
                const StringTestCaseData<StringChar>& tdata) const {

        const StringTestCase &tcase = tdata.tcase_;

        // create a pair of iterators into the string object being modified
        const StringIter first1 (str.begin () + tdata.off1_);

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

            str.insert (first1, first2, last2);
            return str;
        }

        String str_arg (tdata.arg_, tdata.arglen_);

        const Iterator first2 (this->begin (str_arg, (Iterator*)0) + off);
        const Iterator last2 (first2 + ext);

        str.insert (first1, first2, last2);
        return str;
    }
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_insert (charT*, Traits*, Allocator*, const RangeBase<
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

    std::ptrdiff_t exp_off = Insert (iter_val) == func.which_ ? tcase.off : 0;

    // compute the offset and the extent (the number of elements)
    // of the first range into the string object being modified
    const std::size_t size1 = tdata.strlen_;
    const std::size_t off1 =
        std::size_t (tcase.off) < size1 ? std::size_t (tcase.off) : size1;

    const charT* const arg_ptr = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      arg_str = tcase.arg ? arg : str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = rw_get_call_counters ((Traits*)0, (charT*)0);

    if (rg_calls)
        total_length_calls = rg_calls [TraitsFunc::length];

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

        const bool use_iters =
            StringIds::arg_iter == StringIds::arg_type (func.which_, 1);

        if (1 == tcase.bthrow && !use_iters)
            expected = exceptions [1];      // out_of_range
        else if (   2 == tcase.bthrow
                 && Insert (size_cstr_size_size) == func.which_)
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

            case Insert (size_cptr):
                ret_ptr = &str.insert (tcase.off, arg_ptr);
                if (rg_calls)
                    n_length_calls = rg_calls [TraitsFunc::length];
                break;

            case Insert (size_cstr):
                ret_ptr = &str.insert (tcase.off, arg_str);
                break;

            case Insert (size_cptr_size):
                ret_ptr = &str.insert (tcase.off, arg_ptr, tcase.size2);
                break;

            case Insert (size_cstr_size_size):
                ret_ptr =
                    &str.insert (tcase.off, arg_str, tcase.off2, tcase.size2);
                break;

            case Insert (size_size_val):
                ret_ptr = &str.insert (tcase.off, tcase.size2, arg_val);
                break;

            case Insert (iter_size_val): {
                const StringIter it (str.begin () + off1);
                str.insert (it, tcase.size2, arg_val);
                ret_ptr = &str;   // function returns void
                break;
            }

            case Insert (iter_val): {
                StringIter it (str.begin () + off1);
                it = str.insert (it, arg_val);
                ret_ptr = &str + (it - str.begin ());
                break;
            }

            case Insert (iter_range): {
                ret_ptr = &rng (str, tdata);  // function returns void
                break;
            }

            default:
                RW_ASSERT (!"logic error: unknown insert overload");
                return;
            }

            // verify that the reference returned from the function
            // refers to the modified string object (i.e., *this
            // within the function)
            const std::ptrdiff_t ret_off = ret_ptr - &str;

            // verify the returned value
            rw_assert (exp_off == ret_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}== begin() + %td, got %td%{;}"
                       "%{?}returned invalid reference, offset is %td%{;}",
                       __LINE__, Insert (iter_val) == func.which_,
                       exp_off, ret_off,
                       Insert (iter_val) != func.which_, ret_off);

            // for convenience
            static const int cwidth = sizeof (charT);

            // verfiy that strings length are equal
            rw_assert (tdata.reslen_ == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{/*.*Gs} with length "
                       "%zu, got %{/*.*Gs} with length %zu", __LINE__,
                       cwidth, int (tdata.reslen_), tdata.res_, tdata.reslen_,
                       cwidth, int (str.size ()), str.c_str (), str.size ());

            if (tdata.reslen_ == str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, str.c_str(), str.size ());

                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                           cwidth, int (str.size ()), str.c_str (), match);
            }

            // verify that Traits::length was used
            if (Insert (size_cptr) == func.which_ && rg_calls) {
                rw_assert (n_length_calls - total_length_calls > 0,
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits::length()", __LINE__);
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
      ? str_state.capacity_ < str.capacity (): 0;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify number of exceptions thrown
    // for range version the allocation may take place several times
    if (Insert (iter_range) != func.which_)
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
void test_insert (charT*, Traits*, Allocator*,
                  const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string<charT, Traits, Allocator> String;

    if (tdata.func_.which_ == Insert (iter_range)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename String::Iterator Iter;                             \
        static const                                                        \
        InsertRangeOverload<String, Iter> rng;                              \
        test_insert ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);     \
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
        InsertRange<String, Iter> rng;                                      \
        test_insert ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);     \
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
        test_insert ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_insert);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Insert (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (size_cptr),
        TEST (size_cstr),
        TEST (size_cptr_size),
        TEST (size_cstr_size_size),
        TEST (size_size_val),
        TEST (iter_val),
        TEST (iter_size_val),
        TEST (iter_range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_string_test (argc, argv, __FILE__,
                            "lib.string.insert",
                            test_insert_func_array, tests, test_count);

    return status;
}
