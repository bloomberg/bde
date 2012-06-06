/***************************************************************************
 *
 * 21.string.cons.cpp - test exercising string constructors [lib.string.cons]
 *
 * $Id: 21.string.cons.cpp 648752 2008-04-16 17:01:56Z faridz $
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
#include <stdexcept>        // for out_of_range, length_error
#include <cstddef>          // for size_t

#include <21.strings.h>
#include <alg_test.h>       // for InputIter
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Cons(sig)               StringIds::ctor_ ## sig
#define OpSet(sig)              StringIds::op_set_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// basic_string (void)
static const StringTestCase
void_test_cases [] = {

#define alloc_test_cases    void_test_cases

#undef TEST
#define TEST(dummy) {                            \
        __LINE__, -1, -1, -1, -1, -1,            \
        0, 0, dummy, sizeof dummy - 1,           \
        "", 0, 0                                 \
    }

    //    +--------------------- dummy argument
    //    |
    //    V
    TEST ("ab"),

    TEST ("last test")
};

/**************************************************************************/

// exercises:
// basic_string (const value_type*)
static const StringTestCase
cptr_test_cases [] = {

#define cptr_alloc_test_cases    cptr_test_cases

#undef TEST
#define TEST(arg, res) {                            \
        __LINE__, -1, -1, -1, -1, -1,               \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- expected result sequence
    //    |               |
    //    |               |
    //    V               V
    TEST ("ab",           "ab"),

    TEST ("",             ""),
    TEST ("<U0>",         ""),

    TEST ("a",            "a"),
    TEST ("bcd",          "bcd"),
    TEST ("cdefaihjb",    "cdefaihjb"),

    TEST ("<U0>@2ab",     ""),
    TEST ("a<U0>@2b",     "a"),
    TEST ("ab<U0>@2",     "ab"),
    TEST ("abefdcc<U0>a", "abefdcc"),

    TEST ("x@128",        "x@128"),
    TEST ("x@207",        "x@207"),
    TEST ("x@334",        "x@334"),
    TEST ("x@540",        "x@540"),
    TEST ("x@873",        "x@873"),
    TEST ("x@1412",       "x@1412"),
    TEST ("x@3695",       "x@3695"),
    TEST ("x@4096",       "x@4096"),

    TEST ("last test",    "last test")
};

/**************************************************************************/

// exercises:
// basic_string (const basic_string&)
static const StringTestCase
cstr_test_cases [] = {

#undef TEST
#define TEST(arg, res) {                            \
        __LINE__, -1, -1, -1, -1, -1,               \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- expected result sequence
    //    |               |
    //    |               |
    //    V               V
    TEST ("ab",           "ab"),

    TEST ("",             ""),
    TEST ("<U0>",         "<U0>"),
    TEST ("<U0>@2",       "<U0>@2"),

    TEST ("a",            "a"),
    TEST ("bcd",          "bcd"),
    TEST ("cdefaihjb",    "cdefaihjb"),

    TEST ("<U0>@2ab",     "<U0>@2ab"),
    TEST ("a<U0>@2b",     "a<U0>@2b"),
    TEST ("ab<U0>@2",     "ab<U0>@2"),
    TEST ("abefdcc<U0>a", "abefdcc<U0>a"),

    TEST ("x@128",        "x@128"),
    TEST ("x@207",        "x@207"),
    TEST ("x@334",        "x@334"),
    TEST ("x@540",        "x@540"),
    TEST ("x@873",        "x@873"),
    TEST ("x@1412",       "x@1412"),
    TEST ("x@3695",       "x@3695"),
    TEST ("x@4096",       "x@4096"),

    TEST ("last test",    "last test")
};

/**************************************************************************/

// exercises:
// basic_string (const value_type*, size_type)
static const StringTestCase
cptr_size_test_cases [] = {

#define cptr_size_alloc_test_cases    cptr_size_test_cases

#undef TEST
#define TEST(arg, size, res) {                      \
        __LINE__, -1, size, -1, -1, -1,             \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, 0                      \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- ctor n argument
    //    |               |  +---------------------- expected result sequence
    //    |               |  |
    //    |               |  |
    //    V               V  V
    TEST ("ab",           2, "ab"),

    TEST ("",             0, ""),
    TEST ("<U0>",         1, "<U0>"),

    TEST ("a",            1, "a"),
    TEST ("bcd",          3, "bcd"),
    TEST ("cdefaihjb",    9, "cdefaihjb"),

    TEST ("<U0>@2ab",     1, "<U0>"),
    TEST ("<U0>@2ab",     4, "<U0>@2ab"),
    TEST ("a<U0>@2b",     4, "a<U0>@2b"),
    TEST ("ab<U0>@2",     3, "ab<U0>"),
    TEST ("abefdcc<U0>a", 7, "abefdcc"),
    TEST ("abefdcc<U0>a", 8, "abefdcc<U0>"),
    TEST ("abefdcc<U0>a", 9, "abefdcc<U0>a"),

    TEST ("x@207",      207, "x@207"),
    TEST ("x@334",      207, "x@207"),
    TEST ("x@207",      128, "x@128"),
    TEST ("x@1412",     873, "x@873"),
    TEST ("x@1412",    1412, "x@1412"),
    TEST ("x@1412",     540, "x@540"),
    TEST ("x@873",      873, "x@873"),
    TEST ("x@3695",    2284, "x@2284"),
    TEST ("x@4096",    4096, "x@4096"),

    TEST ("last test",    9, "last test")
};

/**************************************************************************/

// exercises:
// basic_string (const basic_string&, size_type)
static const StringTestCase
cstr_size_test_cases [] = {

#undef TEST
#define TEST(arg, off, res, bthrow) {               \
        __LINE__, off, -1, -1, -1, -1,              \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- ctor off argument
    //    |               |  +---------------------- expected result sequence
    //    |               |  |               +------ exception info
    //    |               |  |               |          0 - no exception
    //    |               |  |               |          1 - out_of_range
    //    |               |  |               |
    //    |               |  |               |
    //    V               V  V               V
    TEST ("ab",           0, "ab",           0),

    TEST ("",             0, "",             0),
    TEST ("<U0>",         0, "<U0>",         0),
    TEST ("<U0>@2",       0, "<U0>@2",       0),
    TEST ("<U0>@2",       1, "<U0>",         0),
    TEST ("<U0>@2",       2, "",             0),

    TEST ("a",            0, "a",            0),
    TEST ("bcd",          0, "bcd",          0),
    TEST ("cdefaihjb",    0, "cdefaihjb",    0),

    TEST ("<U0>@2ab",     0, "<U0>@2ab",     0),
    TEST ("<U0>@2ab",     1, "<U0>ab",       0),
    TEST ("a<U0>@2b",     0, "a<U0>@2b",     0),
    TEST ("a<U0>@2b",     1, "<U0>@2b",      0),
    TEST ("a<U0>@2b",     2, "<U0>b",        0),
    TEST ("ab<U0>@2",     0, "ab<U0>@2",     0),
    TEST ("ab<U0>@2",     2, "<U0>@2",       0),
    TEST ("ab<U0>@2",     4, "",             0),
    TEST ("abefdcc<U0>a", 0, "abefdcc<U0>a", 0),
    TEST ("abefdcc<U0>a", 7, "<U0>a",        0),

    TEST ("x@207",        0, "x@207",        0),
    TEST ("x@334",      127, "x@207",        0),
    TEST ("x@1412",     872, "x@540",        0),
    TEST ("x@1412",       0, "x@1412",       0),
    TEST ("x@1412",     539, "x@873",        0),
    TEST ("x@873",        0, "x@873",        0),
    TEST ("x@3695",       0, "x@3695",       0),
    TEST ("x@4096",       0, "x@4096",       0),

    TEST ("abc",          5, "abc",          1),
    TEST ("x@4096",    4106, "x@4096",       1),

    TEST ("last test",    0, "last test",    0)
};

/**************************************************************************/

// exercises:
// basic_string (const basic_string&, size_type, size_type)
// basic_string (InputIterator, InputIterator)
static const StringTestCase
cstr_size_size_test_cases [] = {

#define range_test_cases                    cstr_size_size_test_cases
#define cstr_size_size_alloc_test_cases     cstr_size_size_test_cases
#define range_alloc_test_cases              cstr_size_size_test_cases

#undef TEST
#define TEST(arg, off, size, res, bthrow) {         \
        __LINE__, -1, -1, off, size, -1,            \
        0, 0, arg, sizeof arg - 1,                  \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |               +------------------------- ctor off argument
    //    |               |   +--------------------- ctor n argument
    //    |               |   |    +---------------- expected result sequence
    //    |               |   |    |                +--- exception info
    //    |               |   |    |                |       0 - no exception
    //    |               |   |    |                |       1 - out_of_range
    //    |               |   |    |                |
    //    |               |   |    |                |
    //    V               V   V    V                V
    TEST ("ab",           0,  2,   "ab",            0),

    TEST ("",             0,  0,   "",              0),
    TEST ("<U0>",         0,  1,   "<U0>",          0),
    TEST ("<U0>@2",       0,  2,   "<U0>@2",        0),

    TEST ("a",            0,  1,   "a",             0),
    TEST ("bcd",          0,  3,   "bcd",           0),
    TEST ("cdefaihjb",    0,  9,   "cdefaihjb",     0),

    TEST ("<U0>@2ab",     0,  4,   "<U0>@2ab",      0),
    TEST ("a<U0>@2b",     0,  4,   "a<U0>@2b",      0),
    TEST ("ab<U0>@2",     0,  4,   "ab<U0>@2",      0),
    TEST ("abefdcc<U0>a", 0,  9,   "abefdcc<U0>a",  0),

    TEST ("x@207",        0,  207, "x@207",         0),
    TEST ("x@334",       10,  207, "x@207",         0),
    TEST ("x@207",       50,  128, "x@128",         0),
    TEST ("x@1412",     128,  873, "x@873",         0),
    TEST ("x@1412",       0, 1412, "x@1412",        0),
    TEST ("x@1412",     207,  540, "x@540",         0),
    TEST ("x@874",        1,  873, "x@873",         0),
    TEST ("x@3695",      10, 2284, "x@2284",        0),
    TEST ("x@4096",       0, 4096, "x@4096",        0),

    TEST ("abc",          5,  3,   "",              1),
    TEST ("x@4096",    4106,  3,   "",              1),

    TEST ("last test",    0,  9,  "last test",      0)
};

/**************************************************************************/

// exercises:
// basic_string (size_type, value_type)
static const StringTestCase
size_val_test_cases [] = {

#define size_val_alloc_test_cases             size_val_test_cases

#undef TEST
#define TEST(size, val, res) {                      \
        __LINE__, -1, size, -1, -1, val,            \
        0, 0, 0, 0,                                 \
        res, sizeof res - 1, 0                      \
    }

    //    +---------------------------------------- ctor n argument
    //    |          +----------------------------- source value
    //    |          |    +------------------------ expected result sequence
    //    |          |    |
    //    |          |    |
    //    V          V    V
    TEST (1,        'a',  "a"),
    TEST (1,        '\0', "<U0>"),

    TEST (2,        'a',  "aa"),
    TEST (2,        '\0', "<U0>@2"),

    TEST (5,        '\0', "<U0>@5"),
    TEST (10,       'a',  "aaaaaaaaaa"),

    TEST (128,      'x',  "x@128"),
    TEST (207,      'x',  "x@207"),
    TEST (540,      'x',  "x@540"),
    TEST (873,      'x',  "x@873"),
    TEST (1412,     'x',  "x@1412"),
    TEST (3695,     'x',  "x@3695"),

    TEST (4096,     'x',  "x@4096"),

    TEST (4,        't',  "tttt")
};

/**************************************************************************/
// exercises:
// operator= (const value_type*)
static const StringTestCase
cptr_op_set_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {               \
        __LINE__, -1, -1, -1, -1, -1,               \
        str, sizeof str - 1, arg, sizeof arg - 1,   \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |          +------------------------------ argument sequence
    //    |          |               +-------------- expected result sequence
    //    |          |               |             +--- exception info
    //    |          |               |             |      0 - no exception
    //    |          |               |             |     -1 - excpetion safety
    //    |          |               |             |
    //    |          |               |             |
    //    V          V               V             V
    TEST ("",       "ab",           "ab",          0),

    TEST ("",       "",             "",            0),
    TEST ("abc",    "",             "",            0),
    TEST ("",       "<U0>",         "",            0),
    TEST ("abc",    "<U0>",         "",            0),

    TEST ("",       "a",            "a",           0),
    TEST ("<U0>@2", "a",            "a",           0),
    TEST ("a",      "bcd",          "bcd",         0),
    TEST ("x@4096", "bcd",          "bcd",         0),
    TEST ("",       "cdefaihjb",    "cdefaihjb",   0),
    TEST ("a<U0>b", "cdefaihjb",    "cdefaihjb",   0),

    TEST ("",       "<U0>@2ab",     "",            0),
    TEST ("c<U0>d", "<U0>@2ab",     "",            0),
    TEST ("",       "a<U0>@2b",     "a",           0),
    TEST ("bcd",    "a<U0>@2b",     "a",           0),
    TEST ("<U0>",   "ab<U0>@2",     "ab",          0),
    TEST ("x@4096", "ab<U0>@2",     "ab",          0),
    TEST ("<U0>a",  "abefdcc<U0>a", "abefdcc",     0),
    TEST ("x@4096", "abefdcc<U0>a", "abefdcc",     0),

    TEST ("",       "x@207",        "x@207",       0),
    TEST ("x@128",  "x@207",        "x@207",       0),
    TEST ("x@540",  "x@207",        "x@207",       0),
    TEST ("",       "x@1412",       "x@1412",      0),
    TEST ("x@128",  "x@1412",       "x@1412",      0),
    TEST ("x@3695", "x@1412",       "x@1412",      0),
    TEST ("x@872",  "x@873",        "x@873",       0),
    TEST ("x@873",  "x@3695",       "x@3695",      0),

    TEST ("abc",    "x@4096",       "x@4096",      0),

    TEST ("",              0,       "",            0),
    TEST ("a<U0>b<U0>",    0,       "a",           0),
    TEST ("x@4096",        0,       "x@4096",      0),

    TEST ("abcd",   "x@4096",       "x@4096",      0),

    TEST ("",       "last test",    "last test",   0)
};

/**************************************************************************/
// exercises:
// operator= (const basic_string&)
static const StringTestCase
cstr_op_set_test_cases [] = {

#undef TEST
#define TEST(str, arg, res, bthrow) {               \
        __LINE__, -1, -1, -1, -1, -1,               \
        str, sizeof str - 1, arg, sizeof arg - 1,   \
        res, sizeof res - 1, bthrow                 \
    }

    //    +----------------------------------------- source sequence
    //    |          +------------------------------ argument sequence
    //    |          |              +--------------- expected result sequence
    //    |          |              |                +- exception info
    //    |          |              |                |    0 - no exception
    //    |          |              |                |   -1 - excpetion safety
    //    |          |              |                |
    //    |          |              |                |
    //    V          V              V                V
    TEST ("",       "ab",           "ab",            0),

    TEST ("",       "",             "",              0),
    TEST ("abc",    "",             "",              0),
    TEST ("",       "<U0>",         "<U0>",          0),
    TEST ("abc",    "<U0>",         "<U0>",          0),

    TEST ("",       "a",            "a",             0),
    TEST ("<U0>@2", "a",            "a",             0),
    TEST ("a",      "bcd",          "bcd",           0),
    TEST ("x@4096", "bcd",          "bcd",           0),
    TEST ("",       "cdefaihjb",    "cdefaihjb",     0),
    TEST ("a<U0>b", "cdefaihjb",    "cdefaihjb",     0),

    TEST ("",       "<U0>@2ab",     "<U0>@2ab",      0),
    TEST ("c<U0>d", "<U0>@2ab",     "<U0>@2ab",      0),
    TEST ("",       "a<U0>@2b",     "a<U0>@2b",      0),
    TEST ("bcd",    "a<U0>@2b",     "a<U0>@2b",      0),
    TEST ("<U0>",   "ab<U0>@2",     "ab<U0>@2",      0),
    TEST ("x@4096", "ab<U0>@2",     "ab<U0>@2",      0),
    TEST ("<U0>a",  "abefdcc<U0>a", "abefdcc<U0>a",  0),
    TEST ("x@4096", "abefdcc<U0>a", "abefdcc<U0>a",  0),

    TEST ("",       "x@207",        "x@207",         0),
    TEST ("x@128",  "x@207",        "x@207",         0),
    TEST ("x@540",  "x@207",        "x@207",         0),
    TEST ("",       "x@1412",       "x@1412",        0),
    TEST ("x@128",  "x@1412",       "x@1412",        0),
    TEST ("x@3695", "x@1412",       "x@1412",        0),
    TEST ("x@872",  "x@873",        "x@873",         0),
    TEST ("x@873",  "x@3695",       "x@3695",        0),

    TEST ("abc",    "x@4096",       "x@4096",        0),

    TEST ("",           0,          "",              0),
    TEST ("a<U0>b<U0>", 0,          "a<U0>b<U0>",    0),
    TEST ("x@4096",     0,          "x@4096",        0),

    TEST ("",       "last test",    "last test",     0)
};


/**************************************************************************/

// exercises:
// operator= (value_type)
static const StringTestCase
val_op_set_test_cases [] = {

#undef TEST
#define TEST(str, val, res) {                 \
        __LINE__, -1, -1, -1, -1, val,        \
        str, sizeof str - 1, 0, 0,            \
        res, sizeof res - 1, 0                \
    }

    //    +----------------------------------- initial sequence
    //    |          +------------------------ source value
    //    |          |    +------------------- expected result sequence
    //    |          |    |
    //    |          |    |
    //    V          V    V
    TEST ("",       'a',  "a"),
    TEST ("abcdef", 'a',  "a"),
    TEST ("",       '\0', "<U0>"),
    TEST ("a",      '\0', "<U0>"),
    TEST ("<U0>@2", 'x',  "x"),

    TEST ("x@207",  'a',  "a"),
    TEST ("x@873",  '\0', "<U0>"),
    TEST ("x@2284", 't',  "t"),
    TEST ("x@4096", 'x',  "x"),

    TEST ("",       't',  "t")
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class String, class Iterator>
struct ConsRange: RangeBase<String> {

    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;
    typedef typename String::allocator_type StringAlloc;

    ConsRange () { }

    virtual String&
    operator() (String&, const StringTestCaseData<StringChar>& tdata) const {

        const StringChar* const beg = tdata.arg_ + tdata.off2_;
        const StringChar* const end = beg + tdata.ext2_;

        const Iterator first (beg, beg, end);
        const Iterator last  (end, beg, end);

        if (Cons (range_alloc) == tdata.func_.which_) {
            StringAlloc alloc;
            String* tmp = new String (first, last, alloc);
            return *tmp;
        }

        String* tmp = new String (first, last);
        return *tmp;
    }
};

/**************************************************************************/

// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class String, class Iterator>
struct ConsRangeOverload: RangeBase<String>
{
    typedef typename String::iterator       StringIter;
    typedef typename String::value_type     StringChar;
    typedef typename String::allocator_type StringAlloc;

    ConsRangeOverload () { }

    virtual String&
    operator() (String& str_arg,
                const StringTestCaseData<StringChar>& tdata) const {

        bool reverse_iter = StringIds::ReverseIterator == tdata.func_.iter_id_
            || StringIds::ConstReverseIterator == tdata.func_.iter_id_;

        const std::size_t off = reverse_iter ?
            tdata.arglen_ - tdata.off2_ - tdata.ext2_ : tdata.off2_;
        const std::size_t ext = tdata.ext2_;

        const Iterator first (this->begin (str_arg, (Iterator*)0) + off);
        const Iterator last (first + ext);

        if (Cons (range_alloc) == tdata.func_.which_) {
            StringAlloc alloc;
            String* tmp = new String (first, last, alloc);
            return *tmp;
        }

        String* tmp = new String (first, last);
        return *tmp;
    }
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_cons (charT*, Traits*, Allocator*, const RangeBase<
                    std::basic_string <charT, Traits, Allocator> > &rng,
                const StringTestCaseData<charT>                    &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;

    const StringFunc     &func  = tdata.func_;
    const StringTestCase &tcase = tdata.tcase_;

    // construct the argument string
    /* const */ String arg (tdata.arg_, tdata.arglen_);

    // offset and extent function arguments
    const std::size_t arg_off  = -1 != tcase.off ?
        std::size_t (tcase.off) : std::size_t (tcase.off2);
    const std::size_t arg_size = -1 != tcase.size ?
        std::size_t (tcase.size) : std::size_t (tcase.size2);

    // string function argument
    const charT* const arg_ptr = arg.c_str ();
    const String&      arg_str = arg;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);
    const Allocator    arg_alc;

    // (name of) expected and caught exception
    const char* expected = 0;
    const char* caught   = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

    if (1 == tcase.bthrow && Cons (range) != func.which_
                          && Cons (range_alloc) != func.which_)
        expected = exceptions [1];      // out_of_range
    else if (2 == tcase.bthrow)
        expected = exceptions [2];      // length_error

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    // pointer to the returned reference
    String* ret_ptr = 0;

    // start checking for memory leaks
    rwt_check_leaks (0, 0);

    try {
        switch (func.which_) {

        case Cons (void):
            ret_ptr = new String ();
            break;

        case Cons (alloc):
            ret_ptr = new String (arg_alc);
            break;

        case Cons (cptr):
            ret_ptr = new String (arg_ptr);
            break;

        case Cons (cptr_alloc):
            ret_ptr = new String (arg_ptr, arg_alc);
            break;

        case Cons (cstr):
            ret_ptr = new String (arg_str);
            break;

        case Cons (cptr_size):
            ret_ptr = new String (arg_ptr, arg_size);
            break;

        case Cons (cptr_size_alloc):
            ret_ptr = new String (arg_ptr, arg_size, arg_alc);
            break;

        case Cons (cstr_size):
            ret_ptr = new String (arg_str, arg_off);
            break;

        case Cons (cstr_size_size):
            ret_ptr = new String (arg_str, arg_off, arg_size);
            break;

        case Cons (cstr_size_size_alloc):
            ret_ptr = new String (arg_str, arg_off, arg_size, arg_alc);
            break;

        case Cons (size_val):
            ret_ptr = new String (arg_size, arg_val);
            break;

        case Cons (size_val_alloc):
            ret_ptr = new String (arg_size, arg_val, arg_alc);
            break;

        case Cons (range):
            ret_ptr = &rng (arg, tdata);
            break;

        case Cons (range_alloc):
            ret_ptr = &rng (arg, tdata);
            break;

        default:
            RW_ASSERT (!"logic error: unknown constructor overload");
        }

        // for convenience
        static const int cwidth = sizeof (charT);

        // verify that returned pointer is valid
        rw_assert (0 != ret_ptr, 0, tcase.line,
                   "line %d. %{$FUNCALL} expected %{/*.*Gs}, got null",
                   __LINE__, cwidth, int (tdata.reslen_), tdata.res_);

        if (0 != ret_ptr) {

            // verfiy that the length of the resulting string
            rw_assert (tdata.reslen_ == ret_ptr->size (), 0, tcase.line,
                       "line %d. %{$FUNCALL} expected %{/*.*Gs} with "
                       "length %zu, got %{/*.*Gs} with length %zu",
                       __LINE__,
                       cwidth, int (tdata.reslen_), tdata.res_,
                       tdata.reslen_, cwidth, int (ret_ptr->size ()),
                       ret_ptr->data (), ret_ptr->size ());

            if (Cons (void) != func.which_) {
                // verify the capacity of the resulting string
                rw_assert (ret_ptr->size () <= ret_ptr->capacity (), 0,
                           tcase.line, "line %d. %{$FUNCALL} expected "
                           "capacity () >= size (), got capacity () == "
                           "%zu, size () == %zu", __LINE__,
                           ret_ptr->capacity (), ret_ptr->size ());
            }

            if (tdata.reslen_ == ret_ptr->size ()) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // string matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, ret_ptr->c_str (), ret_ptr->size ());

                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__,
                           cwidth, int (tdata.reslen_), tdata.res_,
                           cwidth, int (ret_ptr->size ()),
                           ret_ptr->data (), match);
            }

            delete ret_ptr;
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
        rw_assert (-1 == tcase.bthrow, 0, tcase.line,
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

    /* const */ std::size_t nbytes;
    const       std::size_t nblocks = rwt_check_leaks (&nbytes, 0);

    // FIXME: verify the number of blocks the function call
    // is expected to allocate and detect any memory leaks
    const std::size_t expect_blocks = nblocks;

    rw_assert (nblocks == expect_blocks, 0, tcase.line,
               "line %d. %{$FUNCALL} allocated %td bytes in %td blocks",
               __LINE__, nbytes, expect_blocks);
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_op_set (charT*, Traits*, Allocator*,
                  const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename UserTraits<charT>::MemFun           TraitsFunc;

    const StringFunc     &func  = tdata.func_;
    const StringTestCase &tcase = tdata.tcase_;

    // construct the string object to be modified
    // and the (possibly unused) argument string
    /* const */ String str (tdata.str_, tdata.strlen_, Allocator ());
    const       String arg (tdata.arg_, tdata.arglen_, Allocator ());

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

    // string function argument
    const charT* const arg_ptr = tcase.arg ? arg.data () : str.data ();
    const String&      arg_str = tcase.arg ? arg : str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t total_length_calls = 0;
    std::size_t n_length_calls = 0;
    std::size_t* const rg_calls = OpSet (cptr) == func.which_ ?
        rw_get_call_counters ((Traits*)0, (charT*)0) : 0;

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

        if (0 == tcase.bthrow) {
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

        if (tcase.bthrow) {
            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        try {
            switch (func.which_) {

            case OpSet (cptr):
                str = arg_ptr;
                break;

            case OpSet (cstr):
                str = arg_str;
                break;

            case OpSet (val):
                str = arg_val;
                break;

            default:
                RW_ASSERT (!"logic error: unknown operator= overload");
            }

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
                    rw_match (tcase.res, str.c_str (), str.size ());

                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %{/*.*Gs}, "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__,
                           cwidth, int (tdata.reslen_), tdata.res_,
                           cwidth, int (str.size ()),str.data (), match);
            }

            // verify that Traits::length was used
            if (rg_calls) {
                rw_assert (n_length_calls - total_length_calls > 0,
                           0, tcase.line, "line %d. %{$FUNCALL} doesn't "
                           "use traits::length()", __LINE__);
            }
        }

#ifndef _RWSTD_NO_EXCEPTIONS

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
#if TEST_RW_EXTENSIONS  // shouldn't need such strong exception guarantee.
            str_state.assert_equal (rw_get_string_state (str),
                                    __LINE__, tcase.line, caught);
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

#if TEST_RW_EXTENSIONS  // We don't use reference counting strings.  On
                        // assignment memory is allocated.
    if (OpSet (cstr) != func.which_) {
#endif

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE

        // verify that if exceptions are enabled and when capacity changes
        // at least one exception is thrown
        expect_throws = str_state.capacity_ < str.capacity ();

#else   // if defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)

        expect_throws = StringIds::UserAlloc == func.alloc_id_ ?
            str_state.capacity_ < str.capacity () : 0;

#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

#if TEST_RW_EXTENSIONS
    }
#endif

    rw_assert (expect_throws == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected exactly 1 %s exception "
               "while changing capacity from %zu to %zu, got %zu",
               __LINE__, exceptions [3],
               str_state.capacity_, str.capacity (), throw_count);

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_cons (charT*, Traits*, Allocator*,
                const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string<charT, Traits, Allocator> String;

    const int fid = int (tdata.func_.which_) & StringIds::fid_mask;

    if (StringIds::fid_op_set == fid) {
        test_op_set ((charT*)0, (Traits*)0, (Allocator*)0, tdata);
        return;
    }

    if (tdata.func_.which_ == Cons (range)
        || tdata.func_.which_ == Cons (range_alloc)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename String::Iterator Iter;                             \
        static const                                                        \
        ConsRangeOverload<String, Iter> rng;                                \
        test_cons ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);       \
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
        ConsRange<String, Iter> rng;                                        \
        test_cons ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);       \
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
        test_cons ((charT*)0, (Traits*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_cons);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Cons (sig), sig ## _test_cases,                         \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (void),
        TEST (alloc),
        TEST (cptr),
        TEST (cptr_alloc),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cptr_size_alloc),
        TEST (cstr_size),
        TEST (cstr_size_size),
        TEST (cstr_size_size_alloc),
        TEST (size_val),
        TEST (size_val_alloc),
        TEST (range),
        TEST (range_alloc),

#undef TEST
#define TEST(sig) {                             \
        OpSet (sig), sig ## _op_set_test_cases, \
          sizeof sig ## _op_set_test_cases      \
        / sizeof *sig ## _op_set_test_cases     \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (val)

    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_string_test (argc, argv, __FILE__,
                            "lib.string.cons",
                            test_cons_func_array, tests, test_count);

    return status;
}
