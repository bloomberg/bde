/***************************************************************************
 *
 * 21.string.assign.cpp - test exercising [lib.string.assign]
 *
 * $Id: 21.string.assign.cpp 424860 2006-07-23 23:50:25Z sebor $
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
#include <cstddef>        // for size_t
#include <stdexcept>      // for out_of_range, length_error

#include <21.strings.h>   // for StringMembers
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Assign(sig) StringIds::assign_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise:
// assign (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {                           \
        __LINE__, -1, -1, -1, -1, -1,                           \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                 +------------------------ sequence to be assigned
    //    |                 |             +---------- expected result sequence
    //    |                 |             |        +- exception info
    //    |                 |             |        |      0 - no exception
    //    |                 |             |        |      1 - out_of_range
    //    |                 |             |        |      2 - length_error
    //    |                 |             |        |     -1 - exc. safety
    //    |                 |             |        |
    //    |                 |             |        +-------+
    //    V                 V             V                V
    TEST ("ab",             "c",          "c",             0),

    TEST ("",               "",           "",              0),
    TEST ("",               "<U0>",       "",              0),
    TEST ("",               "abc",        "abc",           0),
    TEST ("abc",            "",           "",              0),

    TEST ("<U0>",           "",           "",              0),
    TEST ("<U0>",           "a",          "a",             0),
    TEST ("<U0>",           "<U0>@2",     "",              0),

    TEST ("<U0>@2ab",       "cdefghij",   "cdefghij",      0),
    TEST ("a<U0>@2b",       "cdefghij",   "cdefghij",      0),
    TEST ("ab<U0>@2",       "cdefghij",   "cdefghij",      0),
    TEST ("a<U0>b<U0>@2c",  "e<U0>",      "e",             0),
    TEST ("<U0>ab<U0>@2c",  "e<U0>",      "e",             0),

    TEST ("",               "x@4096",     "x@4096",        0),
    TEST ("x@4096",         "",           "",              0),
    TEST ("a@4096",         "b@4096",     "b@4096",        0),

    TEST ("",               "x@207",      "x@207",         0),
    TEST ("x@128",          "x@207",      "x@207",         0),
    TEST ("x@540",          "x@207",      "x@207",         0),
    TEST ("",               "x@1412",     "x@1412",        0),
    TEST ("x@128",          "x@1412",     "x@1412",        0),
    TEST ("x@3695",         "x@1412",     "x@1412",        0),
    TEST ("x@872",          "x@873",      "x@873",         0),
    TEST ("x@873",          "x@3695",     "x@3695",        0),

    TEST ("",               0,            "",              0),
    TEST ("abc",            0,            "abc",           0),
    TEST ("a<U0>@2bc",      0,            "a",             0),
    TEST ("<U0>@2abc",      0,            "",              0),
    TEST ("abc<U0>@2",      0,            "abc",           0),
    TEST ("x@4096",         0,            "x@4096",        0),

    TEST ("last",           "test",       "test",          0)
};

/**************************************************************************/

// used to exercise:
// assign (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(s, arg, res, bthrow) {                             \
        __LINE__, -1, -1, -1, -1, -1,                           \
        s, sizeof s - 1,                                        \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                 +------------------------ sequence to be assigned
    //    |                 |             +---------- expected result sequence
    //    |                 |             |        +- exception info:
    //    |                 |             |        |      0 - no exception
    //    |                 |             |        |      1 - out_of_range
    //    |                 |             |        |      2 - length_error
    //    |                 |             |        |     -1 - exc. safety
    //    |                 |             |        |
    //    |                 |             |        +-----+
    //    |                 |             |              |
    //    V                 V             V              V
    TEST ("ab",             "c",          "c",           0),

    TEST ("",               "",           "",            0),
    TEST ("",               "<U0>",       "<U0>",        0),
    TEST ("",               "abc",        "abc",         0),
    TEST ("abc",            "",           "",            0),

    TEST ("<U0>",           "",           "",            0),
    TEST ("<U0>",           "a",          "a",           0),
    TEST ("<U0>",           "<U0>@2",     "<U0>@2",      0),

    TEST ("ab",             "c<U0>e",     "c<U0>e",      0),

    TEST ("<U0>@2ab",       "cdefghij",   "cdefghij",    0),
    TEST ("a<U0>@2b",       "cdefghij",   "cdefghij",    0),
    TEST ("ab<U0>@2",       "cdefghij",   "cdefghij",    0),
    TEST ("a<U0>b<U0>@2c",  "e<U0>",      "e<U0>",       0),
    TEST ("<U0>ab<U0>@2c",  "e<U0>",      "e<U0>",       0),
    TEST ("ab<U0>@2c<U0>",  "<U0>e",      "<U0>e",       0),

    TEST ("e<U0>",          "a<U0>b<U0>@2c",  "a<U0>b<U0>@2c",   0),
    TEST ("e<U0>@2",        "<U0>ab<U0>@2c",  "<U0>ab<U0>@2c",   0),
    TEST ("<U0>e",          "ab<U0>@2c<U0>",  "ab<U0>@2c<U0>",   0),

    TEST ("",               "x@4096",     "x@4096",      0),
    TEST ("x@4096",         "",           "",            0),
    TEST ("a@4096",         "b@4096",     "b@4096",      0),

    TEST ("",               "x@207",      "x@207",       0),
    TEST ("x@128",          "x@207",      "x@207",       0),
    TEST ("x@540",          "x@207",      "x@207",       0),
    TEST ("",               "x@1412",     "x@1412",      0),
    TEST ("x@128",          "x@1412",     "x@1412",      0),
    TEST ("x@3695",         "x@1412",     "x@1412",      0),
    TEST ("x@872",          "x@873",      "x@873",       0),
    TEST ("x@873",          "x@3695",     "x@3695",      0),

    TEST ("",               0,            "",            0),
    TEST ("abc",            0,            "abc",         0),
    TEST ("a<U0>@2bc",      0,            "a<U0>@2bc",   0),
    TEST ("<U0>@2abc",      0,            "<U0>@2abc",   0),
    TEST ("abc<U0>@2",      0,            "abc<U0>@2",   0),
    TEST ("x@4096",         0,            "x@4096",      0),

    TEST ("last",           "test",       "test",        0)
};

/**************************************************************************/

// used to exercise:
// assign (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, size, res, bthrow) {                     \
        __LINE__, -1, size, -1, -1, -1,                         \
        str, sizeof str - 1,                                    \
        arg, sizeof arg - 1, res, sizeof res - 1, bthrow        \
    }

    //    +------------------------------------------ controlled sequence
    //    |                +------------------------- sequence to be assigned
    //    |                |            +------------ assign() n argument
    //    |                |            |   +-------- expected result sequence
    //    |                |            |   |     +-- exception info
    //    |                |            |   |     |        0 - no exception
    //    |                |            |   |     |        1 - out_of_range
    //    |                |            |   |     |        2 - length_error
    //    |                |            |   |     |       -1 - exc. safety
    //    |                |            |   |     |
    //    |                |            |   |     +----------+
    //    V                V            V   V                V
    TEST ("ab",            "c",         1,  "c",             0),

    TEST ("",              "",          0,  "",              0),
    TEST ("",              "abc",       1,  "a",             0),
    TEST ("",              "<U0>",      1,  "<U0>",          0),

    TEST ("<U0>",          "",          0,  "",              0),
    TEST ("<U0>",          "a",         0,  "",              0),
    TEST ("<U0>",          "a",         1,  "a",             0),
    TEST ("<U0>",          "<U0>@2",    1,  "<U0>",          0),
    TEST ("<U0>",          "<U0>@2",    2,  "<U0>@2",        0),

    TEST ("cde",           "ab",        2,  "ab",            0),
    TEST ("cde",           "ab",        1,  "a",             0),

    TEST ("<U0>e<U0>",     "a<U0>b<U0>@2c", 0,  "",          0),
    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 3,  "<U0>ab",    0),
    TEST ("<U0>e<U0>",     "a<U0>b<U0>@2c", 6,  "a<U0>b<U0>@2c", 0),

    TEST ("a<U0>b<U0>@2c", "<U0>e<U0>",     3,  "<U0>e<U0>", 0),
    TEST ("a<U0>b<U0>@2c", "<U0>@2e<U0>",   2,  "<U0>@2",    0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     1,  "<U0>",      0),
    TEST ("a<U0>bc<U0>@2", "<U0>e",         2,  "<U0>e",     0),

    TEST ("",              0,           0,  "",              0),
    TEST ("abc",           0,           0,  "",              0),
    TEST ("abc",           0,           1,  "a",             0),
    TEST ("abc",           0,           2,  "ab",            0),
    TEST ("a<U0>bc",       0,           2,  "a<U0>",         0),
    TEST ("<U0>abc<U0>@2", 0,           1,  "<U0>",          0),
    TEST ("a<U0>bc<U0>@2", 0,           3,  "a<U0>b",        0),
    TEST ("a<U0>bc<U0>@2", 0,           6,  "a<U0>bc<U0>@2", 0),
    TEST ("x@4096",        0,        2048,  "x@2048",        0),

    TEST ("",              "x@4096", 4096,  "x@4096",        0),
    TEST ("abcd",          "x@4096",    0,  "",              0),
    TEST ("x@4096",        "x@4096",    0,  "",              0),
    TEST ("x@4096",        "x@4096",    1,  "x",             0),
    TEST ("x@4096",        "",          0,  "",              0),
    TEST ("a@4096",        "b@4096", 2048,  "b@2048",        0),

    TEST ("",              "x@207",   207,  "x@207",         0),
    TEST ("x@128",         "x@334",   207,  "x@207",         0),
    TEST ("x@540",         "x@207",   128,  "x@128",         0),
    TEST ("",              "x@1412",  873,  "x@873",         0),
    TEST ("x@128",         "x@1412", 1412,  "x@1412",        0),
    TEST ("x@3695",        "x@1412",  540,  "x@540",         0),
    TEST ("x@872",         "x@873",   873,  "x@873",         0),
    TEST ("x@873",         "x@3695", 2284,  "x@2284",        0),

    TEST ("",              "",         -1,  "",              2),

    TEST ("last",          "test",      4, "test",           0)
};

/**************************************************************************/

// used to exercise:
// assign (const basic_string&, size_type, size_type)
// assign (InputIterator, InputIterator)
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
    //    |                +------------------------- sequence to be inserted
    //    |                |            +------------ assign() pos argument
    //    |                |            |  +--------- assign() n argument
    //    |                |            |  |  +------ expected result sequence
    //    |                |            |  |  |  +--- exception info
    //    |                |            |  |  |  |        0 - no exception
    //    |                |            |  |  |  |        1 - out_of_range
    //    |                |            |  |  |  |        2 - length_error
    //    |                |            |  |  |  |       -1 - exc. safety
    //    |                |            |  |  |  |
    //    |                |            |  |  |  +---------------+
    //    V                V            V  V  V                  V
    TEST ("ab",            "c",         0, 1, "c",               0),

    TEST ("",              "",          0, 0,  "",               0),
    TEST ("",              "abc",       1, 1,  "b",              0),
    TEST ("",              "<U0>",      0, 1,  "<U0>",           0),

    TEST ("<U0>",          "",          0, 0,  "",               0),

    TEST ("abc",           "",          0, 0,  "",               0),

    TEST ("<U0>",          "a",         0, 1,  "a",              0),
    TEST ("<U0>",          "<U0>@2",    1, 1,  "<U0>",           0),
    TEST ("<U0>",          "<U0>@2",    0, 2,  "<U0>@2",         0),
    TEST ("<U0>",          "<U0>@2",    1, 5,  "<U0>",           0),

    TEST ("cde",           "ab",        0, 2,  "ab",             0),
    TEST ("cde",           "ab",        0, 1,  "a",              0),
    TEST ("cde",           "ab",        1, 5,  "b",              0),

    TEST ("ab",            "c<U0>e",    0, 3,  "c<U0>e",         0),
    TEST ("ab",            "c<U0>e",    1, 2,  "<U0>e",          0),
    TEST ("ab",            "c<U0>e",    0, 2,  "c<U0>",          0),

    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 0, 9,  "<U0>ab<U0>@2c", 0),
    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 0, 3,  "<U0>ab",     0),
    TEST ("a<U0>b<U0>@2c", "<U0>e<U0>",     0, 3,  "<U0>e<U0>",  0),
    TEST ("a<U0>b<U0>@2c", "<U0>@2e<U0>",   0, 2,  "<U0>@2",     0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     2, 1,  "<U0>",       0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     2, 9,  "<U0>",       0),
    TEST ("a<U0>bc<U0>@2", "<U0>e",         0, 2,  "<U0>e",      0),

    TEST ("",              0,       0,     0,  "",               0),
    TEST ("abc",           0,       1,     0,  "",               0),
    TEST ("abc",           0,       1,     1,  "b",              0),
    TEST ("abc",           0,       0,     2,  "ab",             0),
    TEST ("a<U0>bc<U0>@2", 0,       4,     2,  "<U0>@2",         0),
    TEST ("a<U0>bc<U0>@2", 0,       1,     3,  "<U0>bc",         0),
    TEST ("a<U0>bc<U0>@2", 0,       0,     9,  "a<U0>bc<U0>@2",  0),
    TEST ("abcdef",        0,       1,     2,  "bc",             0),
    TEST ("x@4096",        0,       1,     2,  "xx",             0),

    TEST ("x@4096",    "",          0,     0,  "",               0),
    TEST ("",          "x@4096",    9,     2,  "xx",             0),
    TEST ("",          "x@4096",    9,     0,  "",               0),
    TEST ("abc",       "x@4096",    2,     1,  "x",              0),
    TEST ("x@4096",    "x@4096",    2,     3,  "xxx",            0),
    TEST ("",          "x@4096",    0,  4096, "x@4096",          0),
    TEST ("",          "x@4096",  100,  2000, "x@2000",          0),

    TEST ("",          "x@207",     0,   207,  "x@207",          0),
    TEST ("x@128",     "x@334",    10,   207,  "x@207",          0),
    TEST ("x@540",     "x@207",    50,   128,  "x@128",          0),
    TEST ("",          "x@1412",  128,   873,  "x@873",          0),
    TEST ("x@128",     "x@1412",    0,  1412,  "x@1412",         0),
    TEST ("x@3695",    "x@1412",  207,   540,  "x@540",          0),
    TEST ("x@872",     "x@874",     1,   873,  "x@873",          0),
    TEST ("x@873",     "x@3695",   10,  2284, "x@2284",          0),

    TEST ("",          "<U0>",      2,     0,  "",               1),
    TEST ("",          "a",         2,     0,  "",               1),
    TEST ("",          "x@4096", 4106,     0,  "",               1),

    TEST ("last",      "test",      0,     4, "test",            0)
};

/**************************************************************************/

// used to exercise:
// assign (size_type, value_type)
static const StringTestCase
size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {     \
        __LINE__, -1, size, -1, -1, val,        \
        str, sizeof str - 1,                    \
        0, 0, res, sizeof res - 1, bthrow       \
    }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ assign() count argument
    //    |                |   +-------------------- character to be assigned
    //    |                |   |   +---------------- expected result sequence
    //    |                |   |   |       +-------- exception info
    //    |                |   |   |       |              0 - no exception
    //    |                |   |   |       |              1 - out_of_range
    //    |                |   |   |       |              2 - length_error
    //    |                |   |   |       |             -1 - exc. safety
    //    |                |   |   |       |
    //    |                |   |   |       +--------+
    //    V                V   V   V                V
    TEST ("ab",            1, 'c', "c",             0),

    TEST ("",              0, ' ',  "",             0),
    TEST ("",              1, 'b',  "b",            0),
    TEST ("",              3, 'b',  "bbb",          0),

    TEST ("<U0>",          0, ' ',  "",             0),
    TEST ("",              2, '\0', "<U0>@2",       0),

    TEST ("<U0>",          1, 'a',  "a",            0),
    TEST ("<U0>",          1, '\0', "<U0>",         0),
    TEST ("<U0>",          2, '\0', "<U0>@2",       0),
    TEST ("<U0>",          0, '\0', "",             0),

    TEST ("cde",           3, 'a',  "aaa",          0),
    TEST ("ab",            2, '\0', "<U0>@2",       0),
    TEST ("ab",            1, '\0', "<U0>",         0),

    TEST ("a<U0>b<U0>@2c", 2, '\0', "<U0>@2",       0),
    TEST ("a<U0>b<U0>@2c", 1, '\0', "<U0>",         0),
    TEST ("<U0>ab<U0>@2c", 3, '\0', "<U0>@3",       0),
    TEST ("a<U0>bc<U0>@2", 2, 'a',  "aa",           0),

    TEST ("",           4096, 'x',  "x@4096",       0),
    TEST ("x@4096",        0, 'x',  "",             0),

    TEST ("x@127",       128, 'x', "x@128",         0),
    TEST ("x@200",       207, 'x', "x@207",         0),
    TEST ("x@334",       128, 'x', "x@128",         0),
    TEST ("",            540, 'x', "x@540",         0),
    TEST ("xx",          873, 'x', "x@873",         0),
    TEST ("x@873",      1412, 'x', "x@1412",        0),
    TEST ("x@3695",      207, 'x', "x@207",         0),
    TEST ("x@540",      3695, 'x', "x@3695",        0),

    TEST ("",             -1, 'x',  "",             2),

    TEST ("last",          4, 't',  "tttt",         0)
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class String, class Iterator>
struct AssignRange: RangeBase<String> {

    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;

    AssignRange () { }

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

        return str.assign (first, last);
    }
};

/**************************************************************************/

// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class String, class Iterator>
struct AssignRangeOverload: RangeBase<String>
{
    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;

    AssignRangeOverload () { }

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

            return str.assign (first, last);
        }

        String str_arg (tdata.arg_, tdata.arglen_);

        const Iterator first (this->begin (str_arg, (Iterator*)0) + off);
        const Iterator last (first + ext);

        return str.assign (first, last);
    }
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_assign (charT*, Traits*, Allocator*, const RangeBase<
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

    std::size_t size  = tcase.size  >= 0 ? tcase.size  : str.max_size () + 1;
    std::size_t size2 = tcase.size2 >= 0 ? tcase.size2 : str.max_size () + 1;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    // first function argument
    const charT* const arg_ptr = tcase.arg ? arg.c_str () : str.c_str ();
    const String&      arg_str = tcase.arg ? arg : str;

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

        if (1 == tcase.bthrow && Assign (cstr_size_size) == func.which_)
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

            case Assign (cptr):
                ret_ptr = &str.assign (arg_ptr);
                if (rg_calls)
                    n_length_calls = rg_calls [TraitsFunc::length];
                break;

            case Assign (cstr):
                ret_ptr = &str.assign (arg_str);
                break;

            case Assign (cptr_size):
                ret_ptr = &str.assign (arg_ptr, size);
                break;

            case Assign (cstr_size_size):
                ret_ptr = &str.assign (arg_str, tcase.off2, size2);
                break;

            case Assign (size_val): {
                const charT val = make_char (char (tcase.val), (charT*)0);
                ret_ptr = &str.assign (size, val);
                break;
            }

            case Assign (range):
                ret_ptr = &rng (str, tdata);
                break;

            default:
                RW_ASSERT (!"test logic error: unknown assign overload");
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

            // verfiy that strings length are equal
            rw_assert (tdata.reslen_ == str.size (), 0, tcase.line,
                       "line %d. %{$FUNCALL}: expected %{/*.*Gs} with length "
                       "%zu, got %{/*.*Gs} with length %zu", __LINE__,
                       cwidth, int (tdata.reslen_), tdata.res_, tdata.reslen_,
                       cwidth, int (str.size ()), str.c_str (), str.size ());

            if (tdata.reslen_ == str.size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, str.c_str(), str.size ());
#if DRQS
// Not alias safe yet.
                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at off %zu",
                           __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                           cwidth, int (str.size ()), str.c_str (), match);
#endif
            }

            // verify that Traits::length was used
            if (Assign (cptr) == func.which_ && rg_calls) {
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

    std::size_t expect_throws = 0;

// Our string implementation doesn't use reference counting.

//#ifndef _RWSTD_NO_STRING_REF_COUNT
//    if (Assign (cstr) != func.which_) {
//#endif

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

    // verify that if exceptions are enabled and when capacity changes
    // at least one exception is thrown
    expect_throws = str_state.capacity_ < str.capacity ();

#else   // if defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

    expect_throws = (StringIds::UserAlloc == func.alloc_id_)
        ? str_state.capacity_ < str.capacity (): 0;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

//#ifndef _RWSTD_NO_STRING_REF_COUNT
//    }
//#endif

    // verify number of exceptions thrown
    // for range version the allocation may take place several times
    if (Assign (range) != func.which_)
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
void test_assign (charT*, Traits*, Allocator*,
                  const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string<charT, Traits, Allocator> String;

    if (tdata.func_.which_ == Assign (range)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename String::Iterator Iter;                             \
        static const                                                        \
        AssignRangeOverload<String, Iter> rng;                              \
        test_assign ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);     \
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
        AssignRange<String, Iter> rng;                                      \
        test_assign ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);     \
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
        test_assign ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_assign);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Assign (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cstr_size_size),
        TEST (size_val),
        TEST (range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_string_test (argc, argv, __FILE__,
                            "lib.string.assign",
                            test_assign_func_array, tests, test_count);

    return status;
}
