/***************************************************************************
 *
 * 21.string.append.cpp - string test exercising lib.string.append
 *
 * $Id: 21.string.append.cpp 526961 2007-04-10 00:06:12Z sebor $
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

#include <21.strings.h>   // for StringMembers
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Append(sig)             StringIds::append_ ## sig
#define PushBack(sig)           StringIds::push_back_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// append (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {                           \
        __LINE__, -1, -1, -1, -1, -1,                           \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                  +----------------------- sequence to be appended
    //    |                  |            +---------- expected result sequence
    //    |                  |            |        +- exception info
    //    |                  |            |        |    0 - no exception
    //    |                  |            |        |    1 - out_of_range
    //    |                  |            |        |    2 - length_error
    //    |                  |            |        |   -1 - avoid exc. safety
    //    |                  |            |        |
    //    |                  |            |        +--------------+
    //    V                  V            V                       V
    TEST ("ab",             "c",          "abc",                  0),

    TEST ("",               "",           "",                     0),
    TEST ("",               "<U0>",       "",                     0),
    TEST ("",               "abc",        "abc",                  0),

    TEST ("<U0>",           "",           "<U0>",                 0),
    TEST ("<U0>",           "a",          "<U0>a",                0),
    TEST ("<U0>",           "<U0>@2",     "<U0>",                 0),

    TEST ("ab",             "cd",         "abcd",                 0),
    TEST ("bcd",            "a",          "bcda",                 0),
    TEST ("cde",            "ab",         "cdeab",                0),
    TEST ("abc",            "",           "abc",                  0),
    TEST ("ab",             "c<U0>e",     "abc",                  0),

    TEST ("<U0>@2ab",       "cdefghij",   "<U0>@2abcdefghij",     0),
    TEST ("a<U0>@2b",       "cdefghij",   "a<U0>@2bcdefghij",     0),
    TEST ("ab<U0>@2",       "cdefghij",   "ab<U0>@2cdefghij",     0),
    TEST ("a<U0>b<U0>@2c",  "e<U0>",      "a<U0>b<U0>@2ce",       0),
    TEST ("<U0>ab<U0>@2c",  "e<U0>",      "<U0>ab<U0>@2ce",       0),
    TEST ("abcdefghij",     "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",               "x@4096",     "x@4096",               0),
    TEST ("x@4096",         "",           "x@4096",               0),
    TEST ("x@4096",         "x@4096",     "x@8192",               0),

    TEST ("x@10",           "x@118",      "x@128",                0),
    TEST ("x@128",          "x@79",       "x@207",                0),
    TEST ("x@207",          "x@127",      "x@334",                0),
    TEST ("x@334",          "x@206",      "x@540",                0),
    TEST ("x@540",          "x@333",      "x@873",                0),
    TEST ("x@539",          "x@873",      "x@1412",               0),
    TEST ("x@872",          "x@1412",     "x@2284",               0),
    TEST ("x@1411",         "x@2284",     "x@3695",               0),
    TEST ("x@1412",         "x@2284",     "x@3696",               0),

    TEST ("",                      0,     "",                     0),
    TEST ("abc",                   0,     "abcabc",               0),
    TEST ("a<U0>@2bc",             0,     "a<U0>@2bca",           0),
    TEST ("<U0>@2abc",             0,     "<U0>@2abc",            0),
    TEST ("abc<U0>@2",             0,     "abc<U0>@2abc",         0),

    TEST ("last",             "test",     "lasttest",             0)
};

/**************************************************************************/

// exercises:
// append (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(s, arg, res, bthrow) {                             \
        __LINE__, -1, -1, -1, -1, -1,                           \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                  +----------------------- sequence to be appended
    //    |                  |            +---------- expected result sequence
    //    |                  |            |        +- exception info
    //    |                  |            |        |    0 - no exception
    //    |                  |            |        |    1 - out_of_range
    //    |                  |            |        |    2 - length_error
    //    |                  |            |        |   -1 - avoid exc. safety
    //    |                  |            |        |
    //    |                  |            |        +--------------+
    //    V                  V            V                       V
    TEST ("ab",             "c",          "abc",                  0),

    TEST ("",               "",           "",                     0),
    TEST ("",               "<U0>",       "<U0>",                 0),
    TEST ("",               "abc",        "abc",                  0),

    TEST ("<U0>",           "",           "<U0>",                 0),
    TEST ("<U0>",           "a",          "<U0>a",                0),
    TEST ("<U0>",           "<U0>@2",     "<U0>@3",               0),

    TEST ("ab",             "cd",         "abcd",                 0),
    TEST ("bcd",            "a",          "bcda",                 0),
    TEST ("cde",            "ab",         "cdeab",                0),
    TEST ("abc",            "",           "abc",                  0),
    TEST ("ab",             "c<U0>e",     "abc<U0>e",             0),

    TEST ("<U0>@2ab",       "cdefghij",   "<U0>@2abcdefghij",     0),
    TEST ("a<U0>@2b",       "cdefghij",   "a<U0>@2bcdefghij",     0),
    TEST ("ab<U0>@2",       "cdefghij",   "ab<U0>@2cdefghij",     0),
    TEST ("a<U0>b<U0>@2c",  "e<U0>",      "a<U0>b<U0>@2ce<U0>",   0),
    TEST ("<U0>ab<U0>@2c",  "e<U0>",      "<U0>ab<U0>@2ce<U0>",   0),
    TEST ("ab<U0>@2c<U0>",  "<U0>e",      "ab<U0>@2c<U0>@2e",     0),
    TEST ("abcdefghij",     "abcdefghij", "abcdefghijabcdefghij", 0),

    TEST ("",               "x@4096",     "x@4096",               0),
    TEST ("x@4096",         "",           "x@4096",               0),
    TEST ("x@4096",         "x@4096",     "x@8192",               0),

    TEST ("x@10",           "x@118",      "x@128",                0),
    TEST ("x@128",          "x@79",       "x@207",                0),
    TEST ("x@207",          "x@127",      "x@334",                0),
    TEST ("x@334",          "x@206",      "x@540",                0),
    TEST ("x@540",          "x@333",      "x@873",                0),
    TEST ("x@539",          "x@873",      "x@1412",               0),
    TEST ("x@872",          "x@1412",     "x@2284",               0),
    TEST ("x@1411",         "x@2284",     "x@3695",               0),
    TEST ("x@1412",         "x@2284",     "x@3696",               0),

    TEST ("",               0,            "",                     0),
    TEST ("abc",            0,            "abcabc",               0),
    TEST ("a<U0>@2bc",      0,            "a<U0>@2bca<U0>@2bc",   0),
    TEST ("<U0>@2abc",      0,            "<U0>@2abc<U0>@2abc",   0),
    TEST ("abc<U0>@2",      0,            "abc<U0>@2abc<U0>@2",   0),

    TEST ("last",           "test",       "lasttest",             0)
};

/**************************************************************************/

// exercises:
// append (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow) {                     \
        __LINE__, -1, size, -1, -1, -1,                         \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                +------------------------- sequence to be appended
    //    |                |            +------------ append() n argument
    //    |                |            |  +--------- expected result sequence
    //    |                |            |  |    +---- exception info
    //    |                |            |  |    |       0 - no exception
    //    |                |            |  |    |       1 - out_of_range
    //    |                |            |  |    |       2 - length_error
    //    |                |            |  |    |      -1 - avoid exc. safety
    //    |                |            |  |    |
    //    |                |            |  |    +------------+
    //    V                V            V  V                 V
    TEST ("ab",            "c",         1, "abc",            0),

    TEST ("",              "",          0,  "",              0),
    TEST ("",              "abc",       1,  "a",             0),
    TEST ("",              "<U0>",      1,  "<U0>",          0),

    TEST ("<U0>",          "",          0,  "<U0>",          0),
    TEST ("<U0>",          "a",         0,  "<U0>",          0),
    TEST ("<U0>",          "a",         1,  "<U0>a",         0),
    TEST ("<U0>",          "<U0>@2",    1,  "<U0>@2",        0),
    TEST ("<U0>",          "<U0>@2",    2,  "<U0>@3",        0),

    TEST ("cde",           "ab",        2,  "cdeab",         0),
    TEST ("cde",           "ab",        1,  "cdea",          0),

    TEST ("<U0>e<U0>",     "a<U0>b<U0>@2c", 0,  "<U0>e<U0>",              0),
    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 3,  "<U0>e<U0>@2ab",          0),

    TEST ("a<U0>b<U0>@2c", "<U0>e<U0>",     3,  "a<U0>b<U0>@2c<U0>e<U0>", 0),
    TEST ("a<U0>b<U0>@2c", "<U0>@2e<U0>",   2,  "a<U0>b<U0>@2c<U0>@2",    0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     1,  "<U0>ab<U0>@2c<U0>",      0),
    TEST ("a<U0>bc<U0>@2", "<U0>e",         2,  "a<U0>bc<U0>@3e",         0),

    TEST ("x@10",          "x@118",   118,  "x@128",         0),
    TEST ("x@128",         "x@79",     79,  "x@207",         0),
    TEST ("x@207",         "x@127",   127,  "x@334",         0),
    TEST ("x@207",         "x@207",   127,  "x@334",         0),
    TEST ("x@334",         "x@206",   206,  "x@540",         0),
    TEST ("x@540",         "x@333",   333,  "x@873",         0),
    TEST ("x@539",         "x@873",   873,  "x@1412",        0),
    TEST ("x@873",         "x@540",   539,  "x@1412",        0),
    TEST ("x@872",         "x@1412", 1412,  "x@2284",        0),
    TEST ("x@1411",        "x@2284", 2284,  "x@3695",        0),
    TEST ("x@1411",        "x@3695", 2284,  "x@3695",        0),
    TEST ("x@1412",        "x@2284", 2284,  "x@3696",        0),

    TEST ("",              0,           0,  "",              0),
    TEST ("abc",           0,           0,  "abc",           0),
    TEST ("abc",           0,           1,  "abca",          0),
    TEST ("abc",           0,           2,  "abcab",         0),
    TEST ("a<U0>bc",       0,           2,  "a<U0>bca<U0>",         0),
    TEST ("<U0>abc<U0>@2", 0,           1,  "<U0>abc<U0>@3",        0),
    TEST ("a<U0>bc<U0>@2", 0,           3,  "a<U0>bc<U0>@2a<U0>b",  0),
    TEST ("a@4096",        0,        1111,  "a@5207",        0),
    TEST ("b@4096",        0,        2222,  "b@6318",        0),

    TEST ("",              "x@4096", 4096,  "x@4096",        0),
    TEST ("x@4096",        "",          0,  "x@4096",        0),

    TEST ("last",          "test",      4,  "lasttest",      0)
};

/**************************************************************************/

// exercises:
// append (const basic_string&, size_type, size_type)
// append (InputIterator, InputIterator)
static const StringTestCase
range_test_cases [] = {

// range_test_cases serves a double duty
#define cstr_size_size_test_cases range_test_cases

#undef TEST
#define TEST(str, arg, off, size, res, bthrow) {                \
        __LINE__, -1, -1, off, size, -1,                        \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                +------------------------- sequence to be appended
    //    |                |            +------------ append() pos argument
    //    |                |            |  +--------- append() n argument
    //    |                |            |  |  +------ expected result sequence
    //    |                |            |  |  |  +--- exception info
    //    |                |            |  |  |  |       0 - no exception
    //    |                |            |  |  |  |       1 - out_of_range
    //    |                |            |  |  |  |       2 - length_error
    //    |                |            |  |  |  |      -1 - exc. safety
    //    |                |            |  |  |  |
    //    |                |            |  |  |  +----------------+
    //    V                V            V  V  V                   V
    TEST ("ab",            "c",         0, 1, "abc",              0),

    TEST ("",              "",          0, 0,  "",                0),
    TEST ("",              "abc",       1, 1,  "b",               0),
    TEST ("",              "<U0>",      0, 1,  "<U0>",            0),

    TEST ("<U0>",          "",          0, 0,  "<U0>",            0),

    TEST ("abc",           "",          0, 0,  "abc",             0),

    TEST ("<U0>",          "a",         0, 1,  "<U0>a",           0),
    TEST ("<U0>",          "<U0>@2",    1, 1,  "<U0>@2",          0),
    TEST ("<U0>",          "<U0>@2",    0, 2,  "<U0>@3",          0),
    TEST ("<U0>",          "<U0>@2",    1, 5,  "<U0>@2",          0),

    TEST ("cde",           "ab",        0, 2,  "cdeab",           0),
    TEST ("cde",           "ab",        0, 1,  "cdea",            0),
    TEST ("cde",           "ab",        1, 5,  "cdeb",            0),

    TEST ("ab",            "c<U0>e",    0, 3,  "abc<U0>e",        0),
    TEST ("ab",            "c<U0>e",    1, 2,  "ab<U0>e",         0),
    TEST ("ab",            "c<U0>e",    0, 2,  "abc<U0>",         0),

    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 0, 9, "<U0>e<U0>@2ab<U0>@2c",   0),
    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 0, 3, "<U0>e<U0>@2ab",          0),
    TEST ("a<U0>b<U0>@2c", "<U0>e<U0>",     0, 3, "a<U0>b<U0>@2c<U0>e<U0>", 0),
    TEST ("a<U0>b<U0>@2c", "<U0>@2e<U0>",   0, 2, "a<U0>b<U0>@2c<U0>@2",    0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     2, 1, "<U0>ab<U0>@2c<U0>",      0),
    TEST ("a<U0>bc<U0>@2", "<U0>e",         0, 2, "a<U0>bc<U0>@3e",         0),

    TEST ("",              0,           0, 0,  "",                0),
    TEST ("abc",           0,           1, 0,  "abc",             0),
    TEST ("abc",           0,           1, 1,  "abcb",            0),
    TEST ("abc",           0,           0, 2,  "abcab",           0),
    TEST ("a<U0>bc<U0>@2", 0,           4, 2,  "a<U0>bc<U0>@3<U0>",     0),
    TEST ("a<U0>bc<U0>@2", 0,           1, 3,  "a<U0>bc<U0>@3bc",       0),
    TEST ("a<U0>bc<U0>@2", 0,           3, 9,  "a<U0>bc<U0>@2c<U0>@2",  0),
    TEST ("abcdef",        0,           1, 2,  "abcdefbc",        0),

    TEST ("a@1000",        "b@1000",    0,  999, "a@1000b@999",   0),
    TEST ("a@1000",        "b@1001",    0, 1000, "a@1000b@1000",  0),
    TEST ("a@1000",        "b@1002",    0,  102, "a@1000b@102",   0),
    TEST ("",              "x@4096",    0, 4096, "x@4096",        0),
    TEST ("x@4096",        "",          0,    0, "x@4096",        0),
    TEST ("x@4096",        "a@4096",  100,   10, "x@4096a@10",    0),

    TEST ("x@10",          "x@118",     0,  118, "x@128",         0),
    TEST ("x@128",         "x@129",    50,   79, "x@207",         0),
    TEST ("x@207",         "x@127",     0,  127, "x@334",         0),
    TEST ("x@207",         "x@207",    10,  127, "x@334",         0),
    TEST ("x@334",         "x@208",     2,  206, "x@540",         0),
    TEST ("x@540",         "x@336",     3,  333, "x@873",         0),
    TEST ("x@539",         "x@873",     0,  873, "x@1412",        0),
    TEST ("x@873",         "x@540",     1,  539, "x@1412",        0),
    TEST ("x@872",         "x@1412",    0, 1412, "x@2284",        0),
    TEST ("x@1411",        "x@2288",    4, 2284, "x@3695",        0),
    TEST ("x@1411",        "x@3695",  128, 2284, "x@3695",        0),
    TEST ("x@1412",        "x@2284",    0, 2284, "x@3696",        0),

    TEST ("",              "<U0>",      2,    0, "",              1),
    TEST ("",              "a",         2,    0, "",              1),
    TEST ("",              "x@4096", 4106,    0, "",              1),

    TEST ("last",          "test",      0,    4, "lasttest",      0)
};

/**************************************************************************/

// exercises:
// append (value_type, size_type)
static const StringTestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {     \
        __LINE__, -1, size, -1, -1, val,        \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +------------------------------------------ controlled sequence
    //    |                +------------------------- append() count argument
    //    |                |   +--------------------- character to be appended
    //    |                |   |   +----------------- expected result sequence
    //    |                |   |   |       +--------- exception info
    //    |                |   |   |       |             0 - no exception
    //    |                |   |   |       |             1 - out_of_range
    //    |                |   |   |       |             2 - length_error
    //    |                |   |   |       |            -1 - exc. safety
    //    |                |   |   |       |
    //    |                |   |   |       +-----------+
    //    V                V   V   V                   V
    TEST ("ab",            1, 'c', "abc",              0),

    TEST ("",              0, ' ',  "",                0),
    TEST ("",              1, 'b',  "b",               0),
    TEST ("",              3, 'b',  "bbb",             0),

    TEST ("<U0>",          0, ' ',  "<U0>",            0),
    TEST ("",              2, '\0', "<U0>@2",          0),

    TEST ("<U0>",          1, 'a',  "<U0>a",           0),
    TEST ("<U0>",          1, '\0', "<U0>@2",          0),
    TEST ("<U0>",          2, '\0', "<U0>@3",          0),
    TEST ("<U0>",          0, '\0', "<U0>",            0),

    TEST ("cde",           1, 'a',  "cdea",            0),
    TEST ("cde",           2, 'a',  "cdeaa",           0),
    TEST ("cde",           3, 'a',  "cdeaaa",          0),

    TEST ("ab",            2, '\0', "ab<U0>@2",        0),
    TEST ("ab",            1, '\0', "ab<U0>",          0),
    TEST ("ab",            2, '\0', "ab<U0>@2",        0),

    TEST ("a<U0>b<U0>@2c", 2, '\0', "a<U0>b<U0>@2c<U0>@2", 0),
    TEST ("a<U0>b<U0>@2c", 1, '\0', "a<U0>b<U0>@2c<U0>",   0),
    TEST ("<U0>ab<U0>@2c", 3, '\0', "<U0>ab<U0>@2c<U0>@3", 0),
    TEST ("a<U0>bc<U0>@2", 2, 'a',  "a<U0>bc<U0>@2aa",     0),

    TEST ("",           4096, 'x', "x@4096",           0),
    TEST ("x@4096",        0, 'x', "x@4096",           0),

    TEST ("x@127",         1, 'x', "x@128",            0),
    TEST ("x@200",         7, 'x', "x@207",            0),
    TEST ("x@331",         3, 'x', "x@334",            0),
    TEST ("x@539",         1, 'x', "x@540",            0),
    TEST ("x@539",       873, 'x', "x@1412",           0),
    TEST ("x@873",      1411, 'x', "x@2284",           0),
    TEST ("x@3694",        1, 'x', "x@3695",           0),
    TEST ("x@540",         1, 'x', "x@541",            0),

    TEST ("last",          4, 't', "lasttttt",         0)
};

/**************************************************************************/

// exercises:
// push_back (value_type)
static const StringTestCase
push_back_val_test_cases [] = {

#undef TEST
#define TEST(str, val, res, bthrow) {           \
        __LINE__, -1, -1, -1, -1, val,          \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +---------------------------------------- controlled sequence
    //    |                 +---------------------- character to be appended
    //    |                 |    +----------------- expected result sequence
    //    |                 |    |       +--------- exception info
    //    |                 |    |       |             0 - no exception
    //    |                 |    |       |            -1 - exc. safety
    //    |                 |    |       |
    //    |                 |    |       +------------+
    //    V                 V    V                    V
    TEST ("ab",            'c',  "abc",               0),

    TEST ("",              'a',  "a",                 0),
    TEST ("",              '\0', "<U0>",              0),
    TEST ("<U0>",          'a',  "<U0>a",             0),
    TEST ("<U0>",          '\0', "<U0>@2",            0),

    TEST ("a<U0>b<U0>@2c", '\0', "a<U0>b<U0>@2c<U0>", 0),
    TEST ("a<U0>bc<U0>@2", 'a',  "a<U0>bc<U0>@2a",    0),
    TEST ("<U0>abc<U0>@2", 'a',  "<U0>abc<U0>@2a",    0),

    TEST ("x@4095",        'x',  "x@4096",            0),

    TEST ("x@127",         'x',  "x@128",             0),
    TEST ("x@206",         'x',  "x@207",             0),
    TEST ("x@333",         'x',  "x@334",             0),
    TEST ("x@539",         'x',  "x@540",             0),
    TEST ("x@1411",        'x',  "x@1412",            0),
    TEST ("x@2283",        'x',  "x@2284",            0),
    TEST ("x@3694",        'x',  "x@3695",            0),
    TEST ("x@540",         'x',  "x@541",             0),

    TEST ("last",          't',  "lastt",             0)
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class String, class Iterator>
struct AppendRange: RangeBase<String> {

    typedef typename String::iterator     StringIter;
    typedef typename String::value_type   StringChar;

    AppendRange () { }

    virtual String&
    operator() (String &str,
                const StringTestCaseData<StringChar>& tdata) const {

        const StringTestCase &tcase = tdata.tcase_;

        const String &cstr = str;

        // when (0 == tcase.arg) exercise self-referential modification
        // (i.e., replacing a range of elements with a subrange of its
        // own elements)
        const StringChar* const beg =
            (tcase.arg ? tdata.arg_ : cstr.data ()) + tdata.off2_;

        const StringChar* const end = beg + tdata.ext2_;

        const Iterator first (beg, beg, end);
        const Iterator last  (end, beg, end);

        return str.append (first, last);
    }
};

/**************************************************************************/

// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class String, class Iterator>
struct AppendRangeOverload: RangeBase<String>
{
    typedef typename String::iterator     StringIter;
    typedef typename String::value_type   StringChar;

    AppendRangeOverload () { }

    virtual String&
    operator() (String &str,
                const StringTestCaseData<StringChar>& tdata) const {

        const StringTestCase &tcase = tdata.tcase_;

        bool reverse_iter = StringIds::ReverseIterator == tdata.func_.iter_id_
            || StringIds::ConstReverseIterator == tdata.func_.iter_id_;

        const std::size_t srclen_ = tcase.arg ? tdata.arglen_ : str.size ();

        const std::size_t off =
            reverse_iter ? srclen_ - tdata.off2_ - tdata.ext2_ : tdata.off2_;
        const std::size_t ext = tdata.ext2_;

        if (0 == tcase.arg) {
            // exercise self-referential modification (i.e., replacing
            // a range of elements with a subrange of its own elements)

            const Iterator first (this->begin (str, (Iterator*)0) + off);
            const Iterator last (first + ext);

            return str.append (first, last);
        }

        String str_arg (tdata.arg_, tdata.arglen_);

        const Iterator first (this->begin (str_arg, (Iterator*)0) + off);
        const Iterator last (first + ext);

        return str.append (first, last);
    }
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_append (charT*, Traits*, Allocator*, const RangeBase<
                      std::basic_string <charT, Traits, Allocator> > &rng,
                  const StringTestCaseData<charT>                    &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::size_type                   SizeT;
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

    const charT* const ptr_arg = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      str_arg = tcase.arg ? arg : str;
    const charT        val_arg = (make_char (char (tcase.val), (charT*)0));

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

        if (1 == tcase.bthrow && Append (cstr_size_size) == func.which_)
            expected = exceptions [1];   // out_of_range
        else if (2 == tcase.bthrow)
            expected = exceptions [2];   // length_error
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

            case Append (cptr):
                ret_ptr = &str.append (ptr_arg);
                if (rg_calls)
                    n_length_calls = rg_calls [TraitsFunc::length];
                break;

            case Append (cstr):
                ret_ptr = &str.append (str_arg);
                break;

            case Append (cptr_size):
                ret_ptr = &str.append (ptr_arg, tcase.size);
                break;

            case Append (cstr_size_size):
                ret_ptr = &str.append (str_arg,
                                       SizeT (tcase.off2), SizeT (tcase.size2));
                break;

            case Append (size_val):
                ret_ptr = &str.append (tcase.size, val_arg);
                break;

            case Append (range):
                ret_ptr = &rng (str, tdata);
                break;

            case PushBack (val):
                str.push_back (val_arg);
                ret_ptr = &str;
                break;

            default:
                RW_ASSERT (!"logic error: unknown append overload");
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

            // verify that strings are of equal length
            rw_assert (tdata.reslen_ == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                       "length %zu, got %{/*.*Gs}, length %zu",
                       __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                       tdata.reslen_, cwidth,
                       int (str.size ()), str.c_str (), str.size ());

            if (tdata.reslen_ == str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, str.c_str (), str.size ());

                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                           cwidth, int (str.size ()), str.c_str (), match);
            }

            // verify that Traits::length was used
            if (Append (cptr) == func.which_ && rg_calls) {
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
    if (Append (range) != func.which_)
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
void test_append (charT*, Traits*, Allocator*,
                  const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string<charT, Traits, Allocator> String;

    if (tdata.func_.which_ == Append (range)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename String::Iterator Iter;                             \
        static const                                                        \
        AppendRangeOverload<String, Iter> rng;                              \
        test_append ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);     \
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
        AppendRange<String, Iter> rng;                                      \
        test_append ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);     \
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
        static const RangeBase<String > rng;
        test_append ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_append);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Append (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cstr_size_size),
        TEST (size_val),
        TEST (range),

        {
            StringIds::push_back_val, push_back_val_test_cases,
            sizeof push_back_val_test_cases / sizeof *push_back_val_test_cases
        }
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_string_test (argc, argv, __FILE__,
                            "lib.string.append",
                            test_append_func_array, tests, test_count);

    return status;
}
