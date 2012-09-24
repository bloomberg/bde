/***************************************************************************
 *
 * 21.string.io.cpp - string test exercising lib.string.io
 *
 * $Id: 21.string.io.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 *
 **************************************************************************/

#include <string>         // for string
#include <cstddef>        // for size_t
#include <istream>        // for istream
#include <ostream>        // for ostream
#include <iostream>
#include <locale>         // for locale

#include <21.strings.h>   // for StringMembers
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_ctype.h>     // for UserCtype
#include <rw_new.h>       // for bad_alloc, replacement operator new
#include <rw_streambuf.h> // for MyStreambuf

#include <bsls_platform.h>


#if !defined(BSLS_PLATFORM_CMP_IBM) && !defined(BSLS_PLATFORM_CMP_MSVC)
    // On IBM, ctype_base inherits from 'facet', which causes an ambiguous
    // base class error in 'xlocale', when the code refers to 'UserCtype::id'.

/**************************************************************************/

static const char* const exceptions[] = {
    "unknown exception", "ios_base::failure", "bad_alloc", "exception"
};

/**************************************************************************/

// for convenience
#define Good    std::ios_base::goodbit
#define Eof     std::ios_base::eofbit
#define Fail    std::ios_base::failbit
#define Bad     std::ios_base::badbit
#define NotGood (Eof | Fail | Bad)

#define Skipws   std::ios_base::skipws
#define Left     std::ios_base::left
#define Right    std::ios_base::right

#define WHITESPACE " \f\n\r\t\v"

const char SYMB_THROW = '!';

// off   - width
// size  - fmtflags
// off2  - iostate or exceptions mask
// size2 - expected iostate or -1 if not verifyed
// val   - expected width
// str   - controlled sequence
// arg   - sequence in stream
// res   - expected result

// bthrow == 0 - off2 is initial stream state
//        == 1 - off2 is exceptions mask

// used to exercise
// operator>> (istream&, string&)
static const StringTestCase
extractor_test_cases [] = {

#undef TEST
#define TEST(str, arg, width, flags, state, res, res_width, res_st, bthrow) { \
    __LINE__, width, flags, state, res_st, res_width,                         \
    str, sizeof (str) - 1, arg, sizeof (arg) - 1,                             \
    res, sizeof (res) - 1, bthrow                                             \
}

    // 1.
    // the string argument and stream width should be unchanged when
    // the stream's initial state is not good, or the initial state
    // is good and skipws is set and the stream's buffer contains only
    // whitespace characters or is empty

    //    +----------------------------------------------- initial string
    //    |    +------------------------------------------ sequence in stream
    //    |    |           +------------------------------ stream width
    //    |    |           |   +-------------------------- stream flags
    //    |    |           |   |       +------------------ stream state
    //    |    |           |   |       |                     or exceptions
    //    |    |           |   |       |        +--------- expected result
    //    |    |           |   |       |        | +------- expected width
    //    |    |           |   |       |        | |   +--- expected state
    //    |    |           |   |       |        | |   | +- exception info:
    //    |    |           |   |       |        | |   | |  0 - no exception
    //    |    |           |   |       |        | |   | |  1 - ios_base::failure
    //    |    |           |   |       |        | |   | |  -1 - exc. safety
    //    |    |           |   |       |        | |   | |
    //    |    |           |   |       |        | |   | +------------+
    //    |    |           |   |       |        | |   +--+           |
    //    |    |           |   |       |        | +--+   |           |
    //    |    |           |   |       |        |    |   |           |
    //    V    V           V   V       V        V    V   V           V
#undef TEST1
#define TEST1(str)                                                       \
    TEST (str, "",         0,  0,      Eof,     str, 0,  Eof,        0), \
    TEST (str, "",         1,  0,      Fail,    str, 1,  Fail,       0), \
    TEST (str, "",         2,  0,      Bad,     str, 2,  Bad,        0), \
    TEST (str, "",         3,  Skipws, Eof,     str, 3,  Eof,        0), \
    TEST (str, "",         4,  Skipws, Fail,    str, 4,  Fail,       0), \
    TEST (str, "",         5,  Skipws, Bad,     str, 5,  Bad,        0), \
    TEST (str, "",         6,  Skipws, Good,    str, 6,  Eof | Fail, 0), \
    TEST (str, WHITESPACE, 7,  0,      Eof,     str, 7,  Eof,        0), \
    TEST (str, WHITESPACE, 8,  0,      Fail,    str, 8,  Fail,       0), \
    TEST (str, WHITESPACE, 9,  0,      Bad,     str, 9,  Bad,        0), \
    TEST (str, WHITESPACE, 10, Skipws, Eof,     str, 10, Eof,        0), \
    TEST (str, WHITESPACE, 11, Skipws, Fail,    str, 11, Fail,       0), \
    TEST (str, WHITESPACE, 12, Skipws, Bad,     str, 12, Bad,        0), \
    TEST (str, WHITESPACE, 13, Skipws, Good,    str, 13, Eof | Fail, 0), \
    TEST (str, "",         14, Skipws, NotGood, str, 14, Eof | Fail, 1), \
    TEST (str, WHITESPACE, 15, Skipws, NotGood, str, 15, Eof | Fail, 1)

#if DRQS  // streams not respecting ios state when initializing streaming
    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V

    TEST1 (""             ),
    TEST1 ("ab"           ),
    TEST1 ("abc"          ),
    TEST1 ("<U0>"         ),
    TEST1 ("<U0>@2ab"     ),
    TEST1 ("a<U0>@2b"     ),
    TEST1 ("ab<U0>@2"     ),
    TEST1 ("a<U0>b<U0>@2c"),
    TEST1 ("<U0>ab<U0>@2c"),
    TEST1 ("x@4096"       ),

#endif  // DRQS

    // 2.
    // the string argument should be cleared when the stream's initial
    // state is good and skipws is not set and the stream's buffer is
    // empty or contains a string which begins with whitespace
    // width should be reset to 0, and the stream's state should have
    // failbit set

    //    +----------------------------------------------- initial string
    //    |    +------------------------------------------ sequence in stream
    //    |    |                      +------------------- stream width
    //    |    |                      |   +--------------- stream flags
    //    |    |                      |   |  +------------ stream state
    //    |    |                      |   |  |               or exceptions
    //    |    |                      |   |  |   +-------- expected result
    //    |    |                      |   |  |   |  +----- expected width
    //    |    |                      |   |  |   |  | +--- expected state
    //    |    |                      |   |  |   |  | | +- exception info:
    //    |    |                      |   |  |   |  | | |  0 - no exception
    //    |    |                      |   |  |   |  | | |  1 - ios_base::failure
    //    |    |                      |   |  |   |  | | |  -1 - exc. safety
    //    |    |                      |   |  |   |  | | |
    //    |    |                      |   |  |   |  | | +-----------------+
    //    |    |                      |   |  |   |  | +-------+           |
    //    |    |                      |   |  |   |  +-----+   |           |
    //    |    |                      |   |  |   +----+   |   |           |
    //    |    |                      |   |  |        |   |   |           |
    //    V    V                      V   V  V        V   V   V           V

#if DRQS  // streams not respecting ios state when initializing streaming

#undef TEST2
#define TEST2(str)                                                            \
    TEST (str, "",                    0,  0, Good,    "", 0 , Eof | Fail, 0), \
    TEST (str, "",                    10, 0, Good,    "", 0 , Eof | Fail, 0), \
    TEST (str, WHITESPACE,            0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE,            2,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE,            20, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc",      0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc",      2,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc",      10, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc\ndef", 0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc\ndef", 3,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc\ndef", 30, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "<U0>@3",   0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "<U0>@3",   2,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "<U0>@3",   10, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, "",                    0,  0, NotGood, "", 0 , Eof | Fail, 1), \
    TEST (str, "",                    10, 0, NotGood, "", 0 , Eof | Fail, 1), \
    TEST (str, WHITESPACE,            0,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE,            2,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE,            20, 0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "abc",      0,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "abc",      2,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "abc",      10, 0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "abc\ndef", 0,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "abc\ndef", 3,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "abc\ndef", 30, 0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "<U0>@3",   0,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "<U0>@3",   2,  0, NotGood, "", 0 , Fail,       1), \
    TEST (str, WHITESPACE "<U0>@3",   10, 0, NotGood, "", 0 , Fail,       1)

#else

#undef TEST2
#define TEST2(str)                                                            \
    TEST (str, "",                    0,  0, Good,    "", 0 , Eof | Fail, 0), \
    TEST (str, "",                    10, 0, Good,    "", 0 , Eof | Fail, 0), \
    TEST (str, WHITESPACE,            0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE,            2,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE,            20, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc",      0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc",      2,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc",      10, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc\ndef", 0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc\ndef", 3,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "abc\ndef", 30, 0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "<U0>@3",   0,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "<U0>@3",   2,  0, Good,    "", 0 , Fail,       0), \
    TEST (str, WHITESPACE "<U0>@3",   10, 0, Good,    "", 0 , Fail,       0)

#endif  // DRQS


    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V
    TEST2 (""             ),
    TEST2 ("ab"           ),
    TEST2 ("abc"          ),
    TEST2 ("<U0>"         ),
    TEST2 ("<U0>@2ab"     ),
    TEST2 ("a<U0>@2b"     ),
    TEST2 ("ab<U0>@2"     ),
    TEST2 ("a<U0>b<U0>@2c"),
    TEST2 ("<U0>ab<U0>@2c"),
    TEST2 ("x@4096"       ),


    // 3.
    // the string argument should contain the extracted characters when
    // the stream's initial state is good and either
    //     skipws is clear and the stream's buffer doesn't contain whitespace
    // or
    //     skipws is set
    // width should be reset to 0

    //    +----------------------------------------------- initial string
    //    |                +------------------------------ sequence in stream
    //    |                |    +------------------------- stream width
    //    |                |    |      +------------------ stream flags
    //    |                |    |      |      +----------- stream state
    //    |                |    |      |      |              or exceptions
    //    |                |    |      |      |  +-------- expected result
    //    |                |    |      |      |  |  +----- expected width
    //    |                |    |      |      |  |  | +--- expected state
    //    |                |    |      |      |  |  | | +- exception info:
    //    |                |    |      |      |  |  | | |  0 - no exception
    //    |                |    |      |      |  |  | | |  1 - ios_base::failure
    //    |                |    |      |      |  |  | | |  -1 - exc. safety
    //    |                |    |      |      |  |  | | |
    //    |                |    |      |      |  |  | | +----------+
    //    |                |    |      |      |  |  | +--------+   |
    //    |                |    |      |      |  |  +------+   |   |
    //    |                |    |      |      |  +-----+   |   |   |
    //    |                |    |      |      |        |   |   |   |
    //    V                V    V      V      V        V   V   V   V

#if DRQS  // streams not respecting ios state when initializing streaming

#undef TEST3
#define TEST3(arg, width, flags, res)                                  \
    TEST ("",              arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("ab",            arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("abc",           arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("<U0>",          arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("<U0>@2ab",      arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("a<U0>@2b",      arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("ab<U0>@2",      arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("a<U0>b<U0>@2c", arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("<U0>ab<U0>@2c", arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("x@4096",        arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("",              arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("ab",            arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("abc",           arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("<U0>",          arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("<U0>@2ab",      arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("a<U0>@2b",      arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("ab<U0>@2",      arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("a<U0>b<U0>@2c", arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("<U0>ab<U0>@2c", arg, width, flags, NotGood, res, 0, -1, 1), \
    TEST ("x@4096",        arg, width, flags, NotGood, res, 0, -1, 1)

#else

#undef TEST3
#define TEST3(arg, width, flags, res)                                  \
    TEST ("",              arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("ab",            arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("abc",           arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("<U0>",          arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("<U0>@2ab",      arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("a<U0>@2b",      arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("ab<U0>@2",      arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("a<U0>b<U0>@2c", arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("<U0>ab<U0>@2c", arg, width, flags, Good,    res, 0, -1, 0), \
    TEST ("x@4096",        arg, width, flags, Good,    res, 0, -1, 0)

#endif  // DRQS

    //     +------------------------------------------------- sequence in stream
    //     |                            +-------------------- stream width
    //     |                            |   +---------------- stream flags
    //     |                            |   |       +-------- expected result
    //     |                            |   |       |
    //     |                            |   |       |
    //     V                            V   V       V
    TEST3 ("a!bc",                      1,  0,      "a"   ),
    TEST3 ("a!bc",                      2,  0,      "a!"  ),
    TEST3 ("a!bc",                      10, 0,      "a!bc"),
    TEST3 ("ab!c ",                     10, 0,      "ab!c"),
    TEST3 ("ab!c\f",                    10, 0,      "ab!c"),
    TEST3 ("ab!c\n",                    10, 0,      "ab!c"),
    TEST3 ("ab!c\r",                    10, 0,      "ab!c"),
    TEST3 ("ab!c\t",                    10, 0,      "ab!c"),
    TEST3 ("ab!c\v",                    10, 0,      "ab!c"),

    TEST3 ("a!bc" WHITESPACE,           0,  0,      "a!bc"),
    TEST3 ("ab!c" WHITESPACE,           2,  0,      "ab"  ),
    TEST3 ("abc!" WHITESPACE,           10, 0,      "abc!"),

    TEST3 ("abc!",                      0,  Skipws, "abc!"),
    TEST3 ("ab!c",                      2,  Skipws, "ab"  ),
    TEST3 ("a!bc",                      10, Skipws, "a!bc"),

    TEST3 ("abc" WHITESPACE,            0,  Skipws, "abc" ),
    TEST3 ("abc" WHITESPACE,            2,  Skipws, "ab"  ),
    TEST3 ("abc" WHITESPACE,            10, Skipws, "abc" ),

    TEST3 (WHITESPACE "abc",            0,  Skipws, "abc" ),
    TEST3 (WHITESPACE "abc",            2,  Skipws, "ab"  ),
    TEST3 (WHITESPACE "abc",            30, Skipws, "abc" ),

    TEST3 (WHITESPACE "abc" WHITESPACE, 0,  Skipws, "abc" ),
    TEST3 (WHITESPACE "abc" WHITESPACE, 2,  Skipws, "ab"  ),
    TEST3 (WHITESPACE "abc" WHITESPACE, 30, Skipws, "abc" )
};

/**************************************************************************/

// off   - width
// size  - fmtflags
// off2  - iostate or exceptions mask
// size2 - expected iostate or -1 if not verifyed
// val   - unused
// str   - controlled sequence
// arg   - sequence in stream
// res   - expected result

// bthrow == 0 - off2 is initial stream state
//        == 1 - off2 is exceptions mask

// used to exercise
// getline (istream&, string&)
static const StringTestCase
getline_test_cases [] = {

#undef TEST
#define TEST(str, arg, width, flags, state, res, res_width, res_st, bthrow) { \
    __LINE__, width, flags, state, res_st, res_width,                         \
    str, sizeof (str) - 1, arg, sizeof (arg) - 1,                             \
    res, sizeof (res) - 1, bthrow                                             \
}

    // 1.
    // string and width should be unchanged when initial iostate is not good

    //    +---------------------------------------------- controlled sequence
    //    |    +----------------------------------------- sequence in stream
    //    |    |              +-------------------------- stream width
    //    |    |              |   +---------------------- stream flags
    //    |    |              |   |       +-------------- stream state
    //    |    |              |   |       |                 or exceptions
    //    |    |              |   |       |     +-------- expected result
    //    |    |              |   |       |     |  +----- unused
    //    |    |              |   |       |     |  | +--- expected state
    //    |    |              |   |       |     |  | | +- exception info:
    //    |    |              |   |       |     |  | | |  0 - no exception
    //    |    |              |   |       |     |  | | |  1 - ios_base::failure
    //    |    |              |   |       |     |  | | |  -1 - exc. safety
    //    |    |              |   |       |     |  | | |
    //    |    |              |   |       |     |  | | +---------+
    //    |    |              |   |       |     |  | +-----+     |
    //    |    |              |   |       |     |  +---+   |     |
    //    |    |              |   |       |     +--+   |   |     |
    //    |    |              |   |       |        |   |   |     |
    //    V    V              V   V       V        V   V   V     V

#undef TEST1
#define TEST1(str)                                                   \
    TEST (str, "",            0,  0,      Eof,     str, 0, Eof,  0), \
    TEST (str, "",            1,  0,      Fail,    str, 0, Fail, 0), \
    TEST (str, "",            2,  0,      Bad,     str, 0, Bad,  0), \
    TEST (str, "",            3,  Skipws, Eof,     str, 0, Eof,  0), \
    TEST (str, "",            4,  Skipws, Fail,    str, 0, Fail, 0), \
    TEST (str, "",            5,  Skipws, Bad,     str, 0, Bad,  0), \
    TEST (str, WHITESPACE,    7,  0,      Eof,     str, 0, Eof,  0), \
    TEST (str, WHITESPACE,    8,  0,      Fail,    str, 0, Fail, 0), \
    TEST (str, WHITESPACE,    9,  0,      Bad,     str, 0, Bad,  0), \
    TEST (str, WHITESPACE,    10, Skipws, Eof,     str, 0, Eof,  0), \
    TEST (str, WHITESPACE,    11, Skipws, Fail,    str, 0, Fail, 0), \
    TEST (str, WHITESPACE,    12, Skipws, Bad,     str, 0, Bad,  0), \
    TEST (str, "abc",         13, 0,      Eof,     str, 0, Eof,  0), \
    TEST (str, "x@4096",      14, 0,      Fail,    str, 0, Fail, 0), \
    TEST (str, "<U0>abc<U0>", 15, 0,      Bad,     str, 0, Bad,  0), \
    TEST (str, "abc\ndef",    16, Skipws, Eof,     str, 0, Eof,  0), \
    TEST (str, " \tabc",      17, Skipws, Fail,    str, 0, Fail, 0), \
    TEST (str, "\n\n<U0>\n",  18, Skipws, Bad,     str, 0, Bad,  0)

#if DRQS  // streams not respecting ios state when initializing streaming
    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V
    TEST1 (""             ),
    TEST1 ("ab"           ),
    TEST1 ("abc"          ),
    TEST1 ("<U0>"         ),
    TEST1 ("<U0>@2ab"     ),
    TEST1 ("a<U0>@2b"     ),
    TEST1 ("ab<U0>@2"     ),
    TEST1 ("a<U0>b<U0>@2c"),
    TEST1 ("<U0>ab<U0>@2c"),
    TEST1 ("x@4096"       ),
#endif

    // 2.
    // string should be erased when initial iostate is good and
    // stream empty or contains string which begins from '\n'
    // width should be unchanged, iostate should have failbit setted

    //    +--------------------------------------------- controlled sequence
    //    |    +---------------------------------------- sequence in stream
    //    |    |             +-------------------------- stream width
    //    |    |             |   +---------------------- stream flags
    //    |    |             |   |       +-------------- stream state
    //    |    |             |   |       |                 or exceptions
    //    |    |             |   |       |     +-------- expected result
    //    |    |             |   |       |     |  +----- unused
    //    |    |             |   |       |     |  | +--- expected state
    //    |    |             |   |       |     |  | | +- exception info:
    //    |    |             |   |       |     |  | | |  0 - no exception
    //    |    |             |   |       |     |  | | |  1 - ios_base::failure
    //    |    |             |   |       |     |  | | |  -1 - exc. safety
    //    |    |             |   |       |     |  | | |
    //    |    |             |   |       |     |  | | +--------------+
    //    |    |             |   |       |     |  | +----+           |
    //    |    |             |   |       |     |  +---+  |           |
    //    |    |             |   |       |     +--+   |  |           |
    //    |    |             |   |       |        |   |  |           |
    //    V    V             V   V       V        V   V  V           V

#if DRQS  // streams not respecting ios state when initializing streaming

#undef TEST2
#define TEST2(str)                                                       \
    TEST (str, "",           0,  Skipws, Good,    "", 0, Eof | Fail, 0), \
    TEST (str, "",           10, 0,      Good,    "", 0, Eof | Fail, 0), \
    TEST (str, "\n",         0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\n",         2,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "\nabc",      0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\nabc",      2,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "\nabc\ndef", 0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\nabc\ndef", 3,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "\n<U0>@3",   0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\n<U0>@3",   2,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "",           0,  Skipws, NotGood, "", 0, Eof | Fail, 1), \
    TEST (str, "",           10, 0,      NotGood, "", 0, Eof | Fail, 1), \
    TEST (str, "\n",         0,  Skipws, NotGood, "", 0, Good,       1), \
    TEST (str, "\n",         2,  0,      NotGood, "", 0, Good,       1), \
    TEST (str, "\nabc",      0,  Skipws, NotGood, "", 0, Good,       1), \
    TEST (str, "\nabc",      2,  0,      NotGood, "", 0, Good,       1), \
    TEST (str, "\nabc\ndef", 0,  Skipws, NotGood, "", 0, Good,       1), \
    TEST (str, "\nabc\ndef", 3,  0,      NotGood, "", 0, Good,       1), \
    TEST (str, "\n<U0>@3",   0,  Skipws, NotGood, "", 0, Good,       1), \
    TEST (str, "\n<U0>@3",   2,  0,      NotGood, "", 0, Good,       1)

#else

#undef TEST2
#define TEST2(str)                                                       \
    TEST (str, "",           0,  Skipws, Good,    "", 0, Eof | Fail, 0), \
    TEST (str, "",           10, 0,      Good,    "", 0, Eof | Fail, 0), \
    TEST (str, "\n",         0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\n",         2,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "\nabc",      0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\nabc",      2,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "\nabc\ndef", 0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\nabc\ndef", 3,  0,      Good,    "", 0, Good,       0), \
    TEST (str, "\n<U0>@3",   0,  Skipws, Good,    "", 0, Good,       0), \
    TEST (str, "\n<U0>@3",   2,  0,      Good,    "", 0, Good,       0)

#endif

    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V
    TEST2 (""             ),
    TEST2 ("ab"           ),
    TEST2 ("abc"          ),
    TEST2 ("<U0>"         ),
    TEST2 ("<U0>@2ab"     ),
    TEST2 ("a<U0>@2b"     ),
    TEST2 ("ab<U0>@2"     ),
    TEST2 ("a<U0>b<U0>@2c"),
    TEST2 ("<U0>ab<U0>@2c"),
    TEST2 ("x@4096"       ),


    // 3.
    // string should be compared with result when initial iostate is good
    // and stream string begins from not '\n'
    // width should be unchanged

    //    +----------------------------------------------- controlled sequence
    //    |                +------------------------------ sequence in stream
    //    |                |    +------------------------- stream width
    //    |                |    |   +--------------------- stream flags
    //    |                |    |   |       +------------- stream state
    //    |                |    |   |       |                or exceptions
    //    |                |    |   |       |    +-------- expected result
    //    |                |    |   |       |    |  +----- unused
    //    |                |    |   |       |    |  | +--- expected state
    //    |                |    |   |       |    |  | | +- exception info:
    //    |                |    |   |       |    |  | | |  0 - no exception
    //    |                |    |   |       |    |  | | |  1 - ios_base::failure
    //    |                |    |   |       |    |  | | |  -1 - exc. safety
    //    |                |    |   |       |    |  | | |
    //    |                |    |   |       |    |  | | +--------+
    //    |                |    |   |       |    |  | +------+   |
    //    |                |    |   |       |    |  +-----+  |   |
    //    |                |    |   |       |    +---+    |  |   |
    //    |                |    |   |       |        |    |  |   |
    //    V                V    V   V       V        V    V  V   V

#if DRQS  // streams not respecting ios state when initializing streaming

#undef TEST3
#define TEST3(arg, res)                                              \
    TEST ("",              arg, 0,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("ab",            arg, 1,  0,      Good,    res, 0, -1, 0), \
    TEST ("abc",           arg, 2,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("<U0>",          arg, 3,  0,      Good,    res, 0, -1, 0), \
    TEST ("<U0>@2ab",      arg, 4,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("a<U0>@2b",      arg, 5,  0,      Good,    res, 0, -1, 0), \
    TEST ("ab<U0>@2",      arg, 6,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("a<U0>b<U0>@2c", arg, 7,  0,      Good,    res, 0, -1, 0), \
    TEST ("<U0>ab<U0>@2c", arg, 8,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("x@4096",        arg, 9,  0,      Good,    res, 0, -1, 0), \
    TEST ("",              arg, 10, Skipws, NotGood, res, 0, -1, 1), \
    TEST ("ab",            arg, 11, 0,      NotGood, res, 0, -1, 1), \
    TEST ("abc",           arg, 12, Skipws, NotGood, res, 0, -1, 1), \
    TEST ("<U0>",          arg, 13, 0,      NotGood, res, 0, -1, 1), \
    TEST ("<U0>@2ab",      arg, 14, Skipws, NotGood, res, 0, -1, 1), \
    TEST ("a<U0>@2b",      arg, 15, 0,      NotGood, res, 0, -1, 1), \
    TEST ("ab<U0>@2",      arg, 16, Skipws, NotGood, res, 0, -1, 1), \
    TEST ("a<U0>b<U0>@2c", arg, 17, 0,      NotGood, res, 0, -1, 1), \
    TEST ("<U0>ab<U0>@2c", arg, 18, Skipws, NotGood, res, 0, -1, 1), \
    TEST ("x@4096",        arg, 19, 0,      NotGood, res, 0, -1, 1)

#else

#undef TEST3
#define TEST3(arg, res)                                              \
    TEST ("",              arg, 0,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("ab",            arg, 1,  0,      Good,    res, 0, -1, 0), \
    TEST ("abc",           arg, 2,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("<U0>",          arg, 3,  0,      Good,    res, 0, -1, 0), \
    TEST ("<U0>@2ab",      arg, 4,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("a<U0>@2b",      arg, 5,  0,      Good,    res, 0, -1, 0), \
    TEST ("ab<U0>@2",      arg, 6,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("a<U0>b<U0>@2c", arg, 7,  0,      Good,    res, 0, -1, 0), \
    TEST ("<U0>ab<U0>@2c", arg, 8,  Skipws, Good,    res, 0, -1, 0), \
    TEST ("x@4096",        arg, 9,  0,      Good,    res, 0, -1, 0)

#endif

    //     +------------------------------------------------- sequence in stream
    //     |                         +----------------------- expected result
    //     |                         |
    //     |                         |
    //     V                         V
    TEST3 ("ab!c",                   "ab!c"          ),
    TEST3 ("a!b\nc",                 "a!b"           ),
    TEST3 ("a\n!bc",                 "a"             ),
    TEST3 (" !\t\n",                 " !\t"          ),
    TEST3 (" \f\r\t\v\n",            " \f\r\t\v"     ),
    TEST3 (" \f\r\n\t\v\n",          " \f\r"         ),
    TEST3 ("<U0>a<U0>b<U0>\n<U0>@2", "<U0>a<U0>b<U0>"),
    TEST3 ("<U0>@7",                 "<U0>@7"        ),
    TEST3 ("x@4096",                 "x@4096"        ),
    TEST3 ("x@4096\nx@4096",         "x@4096"        )
};

/**************************************************************************/

// off   - width
// size  - fmtflags
// off2  - iostate or exceptions mask
// size2 - expected iostate or -1 if not verifyed
// val   - char delim
// str   - controlled sequence
// arg   - sequence in stream
// res   - expected result

// bthrow == 0 - off2 is initial stream state
//        == 1 - off2 is exceptions mask

// used to exercise
// getline (istream&, string&, char)
static const StringTestCase
getline_val_test_cases [] = {

#undef TEST
#define TEST(str, arg, width, flags, state, res, res_width, res_st, bthrow) { \
    __LINE__, width, flags, state, res_st, res_width,                         \
    str, sizeof (str) - 1, arg, sizeof (arg) - 1,                             \
    res, sizeof (res) - 1, bthrow                                             \
}

    // 1.
    // string and width should be unchanged when initial iostate is not good

    //    +---------------------------------------------- controlled sequence
    //    |    +----------------------------------------- sequence in stream
    //    |    |              +-------------------------- stream width
    //    |    |              |   +---------------------- stream flags
    //    |    |              |   |       +-------------- stream state
    //    |    |              |   |       |                 or exceptions
    //    |    |              |   |       |     +-------- expected result
    //    |    |              |   |       |     |  +----- delim
    //    |    |              |   |       |     |  | +--- expected state
    //    |    |              |   |       |     |  | | +- exception info:
    //    |    |              |   |       |     |  | | |  0 - no exception
    //    |    |              |   |       |     |  | | |  1 - ios_base::failure
    //    |    |              |   |       |     |  | | |  -1 - exc. safety
    //    |    |              |   |       |     |  | | |
    //    |    |              |   |       |     |  | | +------------+
    //    |    |              |   |       |     |  | +--------+     |
    //    |    |              |   |       |     |  +----+     |     |
    //    |    |              |   |       |     +--+    |     |     |
    //    |    |              |   |       |        |    |     |     |
    //    V    V              V   V       V        V    V     V     V

#undef TEST1
#define TEST1(str)                                                      \
    TEST (str, "",            0,  0,      Eof,     str, '\n', Eof,  0), \
    TEST (str, "",            1,  0,      Fail,    str, '\0', Fail, 0), \
    TEST (str, "",            2,  0,      Bad,     str, '\r', Bad,  0), \
    TEST (str, "",            3,  Skipws, Eof,     str, '\a', Eof,  0), \
    TEST (str, "",            4,  Skipws, Fail,    str, '\b', Fail, 0), \
    TEST (str, "",            5,  Skipws, Bad,     str, '\t', Bad,  0), \
    TEST (str, WHITESPACE,    7,  0,      Eof,     str, '\v', Eof,  0), \
    TEST (str, WHITESPACE,    8,  0,      Fail,    str, 'a',  Fail, 0), \
    TEST (str, WHITESPACE,    9,  0,      Bad,     str, ' ',  Bad,  0), \
    TEST (str, WHITESPACE,    10, Skipws, Eof,     str, '1',  Eof,  0), \
    TEST (str, WHITESPACE,    11, Skipws, Fail,    str, '5',  Fail, 0), \
    TEST (str, WHITESPACE,    12, Skipws, Bad,     str, 'w',  Bad,  0), \
    TEST (str, "abc",         13, 0,      Eof,     str, 'b',  Eof,  0), \
    TEST (str, "x@4096",      14, 0,      Fail,    str, 'x',  Fail, 0), \
    TEST (str, "<U0>abc<U0>", 15, 0,      Bad,     str, 'c',  Bad,  0), \
    TEST (str, "abc\ndef",    16, Skipws, Eof,     str, 'e',  Eof,  0), \
    TEST (str, " \tabc",      17, Skipws, Fail,    str, '\t', Fail, 0), \
    TEST (str, "\n\n<U0>\n",  18, Skipws, Bad,     str, '\0', Bad,  0)

#if DRQS  // streams not respecting ios state when initializing streaming
    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V
    TEST1 (""             ),
    TEST1 ("ab"           ),
    TEST1 ("abc"          ),
    TEST1 ("<U0>"         ),
    TEST1 ("<U0>@2ab"     ),
    TEST1 ("a<U0>@2b"     ),
    TEST1 ("ab<U0>@2"     ),
    TEST1 ("a<U0>b<U0>@2c"),
    TEST1 ("<U0>ab<U0>@2c"),
    TEST1 ("x@4096"       ),
#endif

    // 2.
    // string should be erased when initial iostate is good and
    // stream empty or contains string which begins from delim
    // width should be unchanged, iostate should have failbit setted

    //    +--------------------------------------------- controlled sequence
    //    |    +---------------------------------------- sequence in stream
    //    |    |             +-------------------------- stream width
    //    |    |             |   +---------------------- stream flags
    //    |    |             |   |       +-------------- stream state
    //    |    |             |   |       |                 or exceptions
    //    |    |             |   |       |     +-------- expected result
    //    |    |             |   |       |     |  +----- delim
    //    |    |             |   |       |     |  | +--- expected state
    //    |    |             |   |       |     |  | | +- exception info:
    //    |    |             |   |       |     |  | | |  0 - no exception
    //    |    |             |   |       |     |  | | |  1 - ios_base::failure
    //    |    |             |   |       |     |  | | |  -1 - exc. safety
    //    |    |             |   |       |     |  | | |
    //    |    |             |   |       |     |  | | +-----------------+
    //    |    |             |   |       |     |  | +-------+           |
    //    |    |             |   |       |     |  +---+     |           |
    //    |    |             |   |       |     +--+   |     |           |
    //    |    |             |   |       |        |   |     |           |
    //    V    V             V   V       V        V   V     V           V

#if DRQS  // streams not respecting ios state when initializing streaming

#undef TEST2
#define TEST2(str)                                                          \
    TEST (str, "",           0,  Skipws, Good,    "", '\a', Eof | Fail, 0), \
    TEST (str, "",           10, 0,      Good,    "", '\0', Eof | Fail, 0), \
    TEST (str, "<U0>",       0,  Skipws, Good,    "", '\0', Good,       0), \
    TEST (str, "\n",         2,  0,      Good,    "", '\n', Good,       0), \
    TEST (str, "\rabc",      0,  Skipws, Good,    "", '\r', Good,       0), \
    TEST (str, "abc",        2,  0,      Good,    "", 'a',  Good,       0), \
    TEST (str, "\babc\ndef", 0,  Skipws, Good,    "", '\b', Good,       0), \
    TEST (str, "\tabc\ndef", 3,  0,      Good,    "", '\t', Good,       0), \
    TEST (str, "\v<U0>@3",   0,  Skipws, Good,    "", '\v', Good,       0), \
    TEST (str, " <U0>@3",    2,  0,      Good,    "", ' ',  Good,       0), \
    TEST (str, "",           0,  Skipws, NotGood, "", '\a', Eof | Fail, 1), \
    TEST (str, "",           10, 0,      NotGood, "", '\0', Eof | Fail, 1), \
    TEST (str, "<U0>",       0,  Skipws, NotGood, "", '\0', Good,       1), \
    TEST (str, "\n",         2,  0,      NotGood, "", '\n', Good,       1), \
    TEST (str, "\rabc",      0,  Skipws, NotGood, "", '\r', Good,       1), \
    TEST (str, "abc",        2,  0,      NotGood, "", 'a',  Good,       1), \
    TEST (str, "\babc\ndef", 0,  Skipws, NotGood, "", '\b', Good,       1), \
    TEST (str, "\tabc\ndef", 3,  0,      NotGood, "", '\t', Good,       1), \
    TEST (str, "\v<U0>@3",   0,  Skipws, NotGood, "", '\v', Good,       1), \
    TEST (str, " <U0>@3",    2,  0,      NotGood, "", ' ',  Good,       1)

#else

#undef TEST2
#define TEST2(str)                                                          \
    TEST (str, "<U0>",       0,  Skipws, Good,    "", '\0', Good,       0), \
    TEST (str, "\n",         2,  0,      Good,    "", '\n', Good,       0), \
    TEST (str, "\rabc",      0,  Skipws, Good,    "", '\r', Good,       0), \
    TEST (str, "abc",        2,  0,      Good,    "", 'a',  Good,       0), \
    TEST (str, "\babc\ndef", 0,  Skipws, Good,    "", '\b', Good,       0), \
    TEST (str, "\tabc\ndef", 3,  0,      Good,    "", '\t', Good,       0), \
    TEST (str, "\v<U0>@3",   0,  Skipws, Good,    "", '\v', Good,       0), \
    TEST (str, " <U0>@3",    2,  0,      Good,    "", ' ',  Good,       0)

#endif  // DRQS

    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V
    TEST2 (""             ),
    TEST2 ("ab"           ),
    TEST2 ("abc"          ),
    TEST2 ("<U0>"         ),
    TEST2 ("<U0>@2ab"     ),
    TEST2 ("a<U0>@2b"     ),
    TEST2 ("ab<U0>@2"     ),
    TEST2 ("a<U0>b<U0>@2c"),
    TEST2 ("<U0>ab<U0>@2c"),
    TEST2 ("x@4096"       ),


    // 3.
    // string should be compared with result when initial iostate is good
    // and stream string begins from not delim
    // width should be unchanged

    //    +----------------------------------------------- controlled sequence
    //    |                +------------------------------ sequence in stream
    //    |                |    +------------------------- stream width
    //    |                |    |   +--------------------- stream flags
    //    |                |    |   |       +------------- stream state
    //    |                |    |   |       |                or exceptions
    //    |                |    |   |       |    +-------- expected result
    //    |                |    |   |       |    |  +----- delim
    //    |                |    |   |       |    |  | +--- expected state
    //    |                |    |   |       |    |  | | +- exception info:
    //    |                |    |   |       |    |  | | |  0 - no exception
    //    |                |    |   |       |    |  | | |  1 - ios_base::failure
    //    |                |    |   |       |    |  | | |  -1 - exc. safety
    //    |                |    |   |       |    |  | | |
    //    |                |    |   |       |    |  | | +------------+
    //    |                |    |   |       |    |  | +----------+   |
    //    |                |    |   |       |    |  +-----+      |   |
    //    |                |    |   |       |    +---+    |      |   |
    //    |                |    |   |       |        |    |      |   |
    //    V                V    V   V       V        V    V      V   V

#if DRQS  // streams not respecting ios state when initializing streaming

#undef TEST3
#define TEST3(arg, delim, res)                                           \
    TEST ("",              arg, 0,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("ab",            arg, 1,  0,      Good,    res, delim, -1, 0), \
    TEST ("abc",           arg, 2,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("<U0>",          arg, 3,  0,      Good,    res, delim, -1, 0), \
    TEST ("<U0>@2ab",      arg, 4,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("a<U0>@2b",      arg, 5,  0,      Good,    res, delim, -1, 0), \
    TEST ("ab<U0>@2",      arg, 6,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("a<U0>b<U0>@2c", arg, 7,  0,      Good,    res, delim, -1, 0), \
    TEST ("<U0>ab<U0>@2c", arg, 8,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("x@4096",        arg, 9,  0,      Good,    res, delim, -1, 0), \
    TEST ("",              arg, 10, Skipws, NotGood, res, delim, -1, 1), \
    TEST ("ab",            arg, 11, 0,      NotGood, res, delim, -1, 1), \
    TEST ("abc",           arg, 12, Skipws, NotGood, res, delim, -1, 1), \
    TEST ("<U0>",          arg, 13, 0,      NotGood, res, delim, -1, 1), \
    TEST ("<U0>@2ab",      arg, 14, Skipws, NotGood, res, delim, -1, 1), \
    TEST ("a<U0>@2b",      arg, 15, 0,      NotGood, res, delim, -1, 1), \
    TEST ("ab<U0>@2",      arg, 16, Skipws, NotGood, res, delim, -1, 1), \
    TEST ("a<U0>b<U0>@2c", arg, 17, 0,      NotGood, res, delim, -1, 1), \
    TEST ("<U0>ab<U0>@2c", arg, 18, Skipws, NotGood, res, delim, -1, 1), \
    TEST ("x@4096",        arg, 19, 0,      NotGood, res, delim, -1, 1)

#else

#undef TEST3
#define TEST3(arg, delim, res)                                           \
    TEST ("",              arg, 0,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("ab",            arg, 1,  0,      Good,    res, delim, -1, 0), \
    TEST ("abc",           arg, 2,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("<U0>",          arg, 3,  0,      Good,    res, delim, -1, 0), \
    TEST ("<U0>@2ab",      arg, 4,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("a<U0>@2b",      arg, 5,  0,      Good,    res, delim, -1, 0), \
    TEST ("ab<U0>@2",      arg, 6,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("a<U0>b<U0>@2c", arg, 7,  0,      Good,    res, delim, -1, 0), \
    TEST ("<U0>ab<U0>@2c", arg, 8,  Skipws, Good,    res, delim, -1, 0), \
    TEST ("x@4096",        arg, 9,  0,      Good,    res, delim, -1, 0)

#endif  // DRQS

    //     +------------------------------------------------- sequence in stream
    //     |                         +----------------------- delim
    //     |                         |     +----------------- expected result
    //     |                         |     |
    //     |                         |     |
    //     V                         V     V
    TEST3 ("ab!c",                   '\t', "ab!c"     ),
    TEST3 ("a!b<U0>c",               '\0', "a!b"      ),
    TEST3 ("a\n!bc",                 'b',  "a\n!"     ),
    TEST3 (" \t\n",                  '\n', " \t"      ),
    TEST3 (" \f\r\t\v\n",            '\v', " \f\r\t"  ),
    TEST3 (" \f\r\n\t\v\n",          '\r', " \f"      ),
    TEST3 ("<U0>a<U0>b<U0>\n<U0>@2", 'b',  "<U0>a<U0>"),
    TEST3 ("<U0>@7",                 '1',  "<U0>@7"   ),
    TEST3 ("x@4096",                 ' ',  "x@4096"   ),
    TEST3 ("x@4096yx@4096",          'y',  "x@4096"   )
};

/**************************************************************************/

// off     - width
// size    - fmtflags
// off2    - iostate or exceptions mask
// size2   - expected iostate or -1 if not verifyed
// val     - expected width
// str     - controlled sequence
// arg_len - size of streambuf
// res     - expected result

// bthrow == 0 - off2 is initial stream state
//        == 1 - off2 is exceptions mask

// used to exercise
// operator<< (ostream&, const string&)
static const StringTestCase
inserter_test_cases [] = {

#undef TEST
#define TEST(str, arg, width, flags, state, res, res_width, res_st, bthrow) { \
    __LINE__, width, flags, state, res_st, res_width,                         \
    str, sizeof (str) - 1, 0, arg, res, sizeof (res) - 1, bthrow              \
}

    // 1. if initial state is not good then width should be
    // unchanged and no data in output stream

    //    +----------------------------------------- controlled sequence
    //    |    +------------------------------------ size of streambuf
    //    |    |          +------------------------- stream width
    //    |    |          |   +--------------------- stream flags
    //    |    |          |   |       +------------- stream state
    //    |    |          |   |       |                 or exceptions
    //    |    |          |   |       |     +------- expected result
    //    |    |          |   |       |     | +----- expected width
    //    |    |          |   |       |     | | +--- expected state
    //    |    |          |   |       |     | | | +- exception info:
    //    |    |          |   |       |     | | | |  0 - no exception
    //    |    |          |   |       |     | | | |  1 - ios_base::failure
    //    |    |          |   |       |     | | | |  -1 - exc. safety
    //    |    |          |   |       |     | | | |
    //    |    |          |   |       |     | | | +-------+
    //    |    |          |   |       |     | | +---+     |
    //    |    |          |   |       |     | +-+   |     |
    //    |    |          |   |       |     |   |   |     |
    //    |    |          |   |       |     |   |   |     |
    //    V    V          V   V       V     V   V   V     V

#undef TEST1
#define TEST1(str)                                            \
    TEST (str, 0,         0,  0,      Eof,  "", 0,  Eof,  0), \
    TEST (str, 0,         1,  0,      Fail, "", 1,  Fail, 0), \
    TEST (str, 0,         2,  0,      Bad,  "", 2,  Bad,  0)

    //     +----------------------------------------- controlled sequence
    //     |
    //     |
    //     V

#if DRQS  // streams not respecting ios state when initializing streaming
    TEST1 (""             ),
    TEST1 ("ab"           ),
    TEST1 ("abc"          ),
    TEST1 ("<U0>"         ),
    TEST1 ("<U0>@2ab"     ),
    TEST1 ("a<U0>@2b"     ),
    TEST1 ("ab<U0>@2"     ),
    TEST1 ("a<U0>b<U0>@2c"),
    TEST1 ("<U0>ab<U0>@2c"),
    TEST1 ("x@4096"       ),
#endif  // DRQS

    // 2. if initial state is good then:
    // if stream has not enough size to get string then badbit should be set
    // width should be 0

    //    +---------------------------------------- controlled sequence
    //    |    +----------------------------------- size of streambuf
    //    |    |    +------------------------------ stream width
    //    |    |    |      +----------------------- stream flags
    //    |    |    |      |      +---------------- stream state
    //    |    |    |      |      |                   or exceptions
    //    |    |    |      |      |        +------- expected result
    //    |    |    |      |      |        | +----- expected width
    //    |    |    |      |      |        | | +--- expected state
    //    |    |    |      |      |        | | | +- exception info:
    //    |    |    |      |      |        | | | |  0 - no exception
    //    |    |    |      |      |        | | | |  1 - ios_base::failure
    //    |    |    |      |      |        | | | |  -1 - exc. safety
    //    |    |    |      |      |        | | | |
    //    |    |    |      |      |        | | | +-----------------+
    //    |    |    |      |      |        | | +-----------+       |
    //    |    |    |      |      |        | +--+          |       |
    //    |    |    |      |      |        |    |          |       |
    //    |    |    |      |      |        |    |          |       |
    //    V    V    V      V      V        V    V          V       V

#undef TEST2
#define TEST2(str, arg, width, flags, res, res_width, res_st)          \
    TEST (str, arg, width, flags, Good,    res, res_width, res_st, 0), \
    TEST (str, arg, width, flags, NotGood, res, res_width, res_st, 1)

    //     +------------------------------------------------ controlled sequence
    //     |                +------------------------------- size of streambuf
    //     |                |     +------------------------- stream width
    //     |                |     |     +------------------- stream flags
    //     |                |     |     |      +------------ expected result
    //     |                |     |     |      |      +----- expected width
    //     |                |     |     |      |      |  +-- expected state
    //     |                |     |     |      |      |  |
    //     |                |     |     |      |      |  +---------------+
    //     |                |     |     |      |      +------------+     |
    //     |                |     |     |      |                   |     |
    //     V                V     V     V      V                   V     V
    TEST2 ("",              0,    0,    0,     "",                 0,    Good),
    TEST2 ("",              0,    0,    Right, "",                 0,    Good),
    TEST2 ("",              0,    0,    Left,  "",                 0,    Good),

    TEST2 ("",              20,   10,   0,     " @10",             0,    Good),
    TEST2 ("",              20,   10,   Right, " @10",             0,    Good),
    TEST2 ("",              20,   10,   Left,  " @10",             0,    Good),

    TEST2 ("",              5,    10,   0,     " @5",              0,    Bad ),
    TEST2 ("",              5,    10,   Right, " @5",              0,    Bad ),
    TEST2 ("",              5,    10,   Left,  " @5",              0,    Bad ),

    TEST2 (" @10",          20,   10,   0,     " @10",             0,    Good),
    TEST2 (" @10",          20,   10,   Right, " @10",             0,    Good),
    TEST2 (" @10",          20,   10,   Left,  " @10",             0,    Good),

    TEST2 ("abc def",       20,   0,    0,     "abc def",          0,    Good),
    TEST2 ("abc def",       20,   0,    Right, "abc def",          0,    Good),
    TEST2 ("abc def",       20,   0,    Left,  "abc def",          0,    Good),

    TEST2 ("abc def",       5,    0,    0,     "abc d",            0,    Bad ),
    TEST2 ("abc def",       5,    0,    Right, "abc d",            0,    Bad ),
    TEST2 ("abc def",       5,    0,    Left,  "abc d",            0,    Bad ),

    TEST2 ("abc def",       20,   5,    0,     "abc def",          0,    Good),
    TEST2 ("abc def",       20,   5,    Right, "abc def",          0,    Good),
    TEST2 ("abc def",       20,   5,    Left,  "abc def",          0,    Good),

    TEST2 ("abc def",       20,   10,   0,     "   abc def",       0,    Good),
    TEST2 ("abc def",       20,   10,   Right, "   abc def",       0,    Good),
    TEST2 ("abc def",       20,   10,   Left,  "abc def   ",       0,    Good),

    TEST2 ("<U0>",          20,   10,   0,     " @9<U0>",          0,    Good),
    TEST2 ("<U0>",          20,   10,   Right, " @9<U0>",          0,    Good),
    TEST2 ("<U0>",          20,   10,   Left,  "<U0> @9",          0,    Good),

    TEST2 ("<U0>ab<U0>@2c", 20,   10,   0,     " @4<U0>ab<U0>@2c", 0,    Good),
    TEST2 ("<U0>ab<U0>@2c", 20,   10,   Right, " @4<U0>ab<U0>@2c", 0,    Good),
    TEST2 ("<U0>ab<U0>@2c", 20,   10,   Left,  "<U0>ab<U0>@2c @4", 0,    Good),

    TEST2 ("x@4096",        4096, 10,   0,     "x@4096",           0,    Good),
    TEST2 ("x@4096",        4096, 10,   Right, "x@4096",           0,    Good),
    TEST2 ("x@4096",        4096, 10,   Left,  "x@4096",           0,    Good),

    TEST2 ("x@4096",        4097, 4097, 0,     " x@4096",          0,    Good),
    TEST2 ("x@4096",        4097, 4097, Right, " x@4096",          0,    Good),
    TEST2 ("x@4096",        4097, 4097, Left,  "x@4096 ",          0,    Good)
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_io (charT*, Traits*, Allocator*,
              const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef MyStreambuf<charT, Traits>                   Streambuf;
    typedef std::basic_ios<charT, Traits>                BasicIos;
    typedef std::basic_istream<charT, Traits>            Istream;
    typedef std::basic_ostream<charT, Traits>            Ostream;

    typedef std::ios_base::fmtflags                      Fmtflags;
    typedef std::ios_base::iostate                       Iostate;

    const StringFunc     &func  = tdata.func_;
    const StringTestCase &tcase = tdata.tcase_;

    bool test_inserter = func.which_ == StringIds::inserter_ostream_cstr;

    // construct the string object
    String str (tdata.str_, tdata.strlen_);
    // construct a const reference to the string object
    // for tests exercising output
    const String& cstr (str);

    static const std::size_t BUFSIZE = 256;

    static char arg_buf [BUFSIZE];

    std::size_t arg_len = 0;
    const char* arg = rw_expand (arg_buf, tcase.arg, tcase.arg_len, &arg_len);

    // prepare arrays of indexes on which force the exception throwing
    const charT* arg_throw = test_inserter ? tdata.str_ : tdata.arg_;
    const std::size_t arg_throw_len =
        test_inserter ? tdata.strlen_ : tdata.arglen_;

    std::size_t* throw_on = new std::size_t [arg_throw_len + 1];
    std::size_t pthrow = 0;
    for (std::size_t k = 0; k < arg_throw_len; ++k) {
        if (1 == rw_match (&SYMB_THROW, arg_throw + k, 1))
            throw_on [pthrow++] = k + 1;
    }
    throw_on [pthrow] = NPOS;

    // construct ctype facet to install in each stream's locale
    // (the facet's lifetime must exceed that of the locale in
    // which it is installed, which might include both the stream
    // and the stream's streambuf)
#if TEST_RW_PEDANTIC // Must locale work with const facets?
    const UserCtype<charT> ctyp (1);
#else
    UserCtype<charT> ctyp (1);
#endif


    // set the initial formatting flags in both streams
    const Fmtflags flags = Fmtflags (tcase.size);

    // save the state of the const string object before the call
    // to any (changes to the state of the object after a call)
    const StringState cstr_state (rw_get_string_state (cstr));

    //                             Xsgetn, Sync, Xsputn
    std::size_t throw_count [] = {    1,     1,     1   };
    std::size_t throw_inx = 0;

    while (1) {

        std::string s;
    s.insert(0, arg, arg_len);
        std::cout << "arg_len: " << arg_len << std::endl;
    std::cout << s << std::endl;

        Streambuf inbuf (arg, arg_len, Throw | Underflow | Sync | Xsgetn, -1);

        Streambuf outbuf (std::streamsize (tcase.arg_len),
                          Throw | Overflow | Sync | Xsputn, -1);

        Streambuf& sbuf = test_inserter ? outbuf : inbuf;

#ifndef _RWSTD_NO_EXCEPTIONS

        // (name of) expected and caught exception
        const char* expected = 0;
        const char* caught   = 0;

        if (1 == tcase.bthrow) {
            expected = exceptions [1];   // ios_base::failure
        }
        else if (0 == tcase.bthrow) {
            // set on which call of which method to throw
            sbuf.throw_when_ [sbuf.memfun_inx (
                test_inserter ? Overflow : Underflow) ] = throw_on [throw_inx];

            sbuf.throw_when_ [sbuf.memfun_inx (Xsgetn)] =
                throw_count [0];
            sbuf.throw_when_ [sbuf.memfun_inx (Sync)] =
                throw_count [1];
            sbuf.throw_when_ [sbuf.memfun_inx (Xsputn)] =
                throw_count [2];
        }
        else {
            // exceptions disabled for this test case
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        // construct both stream objects even though only one will be used
        Istream is (&inbuf);
        Ostream os (&outbuf);

        Istream is2 (&inbuf);
        String str2 = str;

        // base referes to the basic_ios subobject of the stream used in
        // each test case (to avoid unnecessarily manipulating both streams)
        BasicIos &strm = test_inserter ? (BasicIos&)os : (BasicIos&)is;

        // install std::ctype<UserChar> facet
        strm.imbue (std::locale (strm.getloc (), &ctyp));

        // set the initial width of both streams
        strm.width (tcase.off);

        strm.flags (flags);

        const Iostate state = Iostate (tcase.off2);

        if (tcase.bthrow) {
            // set exception bits leaving the initial stream state good
            strm.exceptions (state);
        }
        else {
            // set the initial stream state leaving exceptions clear
            strm.clear (state);
        }

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        bool streambuf_threw = false;

        try {

            // the offset of the address of the returned reference
            // from the address of the stream object argument (the
            // two must be the same)
            std::ptrdiff_t ret_off = 0;

            try {
                switch (func.which_) {

                case StringIds::extractor_istream_str:
                    ret_off = &is - &(is >> str);
                    break;

                case StringIds::getline_istream_str: {
                    Istream *isp = &is;
                    Istream *isp2 = &std::getline (is, str);
                    ret_off = isp - isp2;
                    }
                    break;

                case StringIds::getline_istream_str_val: {
                    const charT delim = make_char (tcase.val, (charT*)0);
                    ret_off = &is - &std::getline (is, str, delim);
                    break;
                }

                case StringIds::inserter_ostream_cstr:
                    ret_off = &os - &(os << cstr);
                    break;

                default:
                    RW_ASSERT (!"test logic error: unknown io overload");
                    return;
                }
            }
#ifndef _RWSTD_NO_EXCEPTIONS

            catch (std::ios_base::failure& ex) {
                caught = exceptions [1];
                rw_assert (caught == expected, 0, tcase.line,
                           "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                           "unexpectedly%{;} caught std::%s(%#s)",
                           __LINE__, 0 != expected, expected, caught,
                           ex.what ());

            }
            catch (...) {
                caught = exceptions [0];
#if TEST_RW_EXTENSIONS  // It seems like this test is designed to throw
                        // exceptions.
                rw_assert (0, 0, tcase.line,
                           "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                           "unexpectedly%{;} caught %s",
                           __LINE__, 0 != expected, expected, caught);
#endif
            }

#endif   // _RWSTD_NO_EXCEPTIONS

            const std::streamsize ret_width = strm.width ();
            const Iostate         ret_state = strm.rdstate ();
            const charT*          ret_str   = str.data ();
            std::size_t                ret_sz    = str.size ();

            if (test_inserter) {
                ret_str = outbuf.pubpbase ();
                ret_sz  = outbuf.pubepptr () - ret_str;
            }

            streambuf_threw = None != sbuf.threw_;

            if (streambuf_threw) {
                if (Overflow == sbuf.threw_ || Underflow == sbuf.threw_)
                    throw_inx++;
                if (Xsgetn == sbuf.threw_)
                    throw_count [0] ++;
                if (Sync == sbuf.threw_)
                    throw_count [1] ++;
                if (Xsputn == sbuf.threw_)
                    throw_count [2] ++;
            }

            // character width (for convenience)
            static const int cwidth = sizeof (charT);

            // verify that the reference returned from the function
            // refers to the passed stream object
            bool success = 0 == ret_off;
            rw_assert (success, 0, tcase.line,
                    "line %d. %{$FUNCALL} returned invalid reference, "
                    "offset is %td", __LINE__, ret_off);

            if (!streambuf_threw) {
                // verify the width

                const std::streamsize width =
                       func.which_ == StringIds::getline_istream_str
                    || func.which_ == StringIds::getline_istream_str_val
#ifndef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
                    || func.which_ == StringIds::inserter_ostream_cstr
                    && ret_state != Good
#endif  // _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
                    ? tcase.off : tcase.val;

                success = width == ret_width;
                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected width() == %td, "
                           "got %td", __LINE__, width, ret_width);
            }

            // tcase.size2 is the expected iostate
            if (0 <= tcase.size2) {

                // verify the iostate
                const Iostate res_state =
                    streambuf_threw ? Bad : Iostate (tcase.size2);

                success = res_state ?
                    (res_state == (ret_state & res_state)) : (0 == ret_state);
#if DRQS  // bad bit not set after an exception being thrown for _string_io
                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected rdstate() "
                           "== %{Is}, got %{Is}",
                           __LINE__, res_state, ret_state);
#endif
            }

            if (   !streambuf_threw
                || Overflow == sbuf.threw_ || Underflow == sbuf.threw_) {

                std::size_t res_sz = streambuf_threw ?
                    sbuf.throw_when_ [sbuf.memfun_inx (sbuf.threw_)] - 1 :
                    tdata.reslen_;

                // verify that strings length are equal
                success = res_sz == ret_sz;
                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected %{/*.*Gs} with "
                           "length %zu, got %{/*.*Gs} with length %zu",
                           __LINE__,
                           cwidth, int (res_sz), tdata.res_, res_sz,
                           cwidth, int (ret_sz), ret_str, ret_sz);

                if (res_sz == ret_sz) {
                    // if the result length matches the expected length
                    // (and only then), also verify that the modified
                    // string matches the expected result
                    const std::size_t match = rw_match (tcase.res,
                                                        ret_str, ret_sz);

                    success = match == res_sz;
                    rw_assert (success, 0, tcase.line,
                               "line %d. %{$FUNCALL}: expected %{/*.*Gs}, "
                               "got %{/*.*Gs}, difference at off %zu",
                               __LINE__, cwidth, int (res_sz), tdata.res_,
                               cwidth, int (ret_sz), ret_str, match);
                }
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::bad_alloc &ex) {
            caught = exceptions [2];
            rw_assert (0, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());
        }
        catch (const std::exception &ex) {
            caught = exceptions [3];
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

        if (streambuf_threw && 0 == tcase.bthrow) {
            // call the function again until streambuf methods
            // not throws the exception
            continue;
        }
        else if (1 < tcase.bthrow) {
            rw_assert (caught == expected, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                       "%{:}unexpectedly caught %s%{;}",
                       __LINE__, 0 != expected, expected, caught, caught);
        }

        break;
    }

    if (StringIds::fid_inserter == (StringIds::fid_mask & int (func.which_))) {
        // verify that const string object was not modified during
        // the call to the inserter (input functions may, obviously,
        // modify it)
        cstr_state.assert_equal (rw_get_string_state (cstr),
                                 __LINE__, tcase.line, "call");
    }

    if (arg != arg_buf)
        delete[] arg;

    arg = 0;

    if (throw_on)
        delete[] throw_on;
}


/**************************************************************************/

#if !TEST_RW_EXTENSIONS
// RW has default meanings for locals other than for 'char' and 'wchar_t'
#define UserChar char
#endif

DEFINE_STRING_TEST_FUNCTIONS (test_io);

int main (int argc, char** argv)
{
    static const StringTest


    tests [] = {

#undef TEST
#define TEST(gsig, sig) {                                       \
    gsig, sig ## _test_cases,                                   \
    sizeof sig ## _test_cases / sizeof *sig ## _test_cases      \
}

        TEST (StringIds::extractor_istream_str, extractor),
        TEST (StringIds::inserter_ostream_cstr, inserter),
        TEST (StringIds::getline_istream_str, getline),
        TEST (StringIds::getline_istream_str_val, getline_val)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.io",
                               test_io_func_array, tests, test_count);

}

#else

int main ()
{
}
#endif
