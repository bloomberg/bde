/***************************************************************************
 *
 * 23.list.insert.cpp - test exercising [lib.list.insert]
 *
 * $Id: 23.list.insert.cpp 580483 2007-09-28 20:55:52Z sebor $
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

#include <list>           // for list
#include <stdexcept>      // for out_of_range, length_error
#include <cstddef>        // for size_t

#include <23.list.h>      // for ListMembers
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Insert(sig)   ListIds::insert_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception"
};

/**************************************************************************/

// exercises
// insert (iterator, InputIterator, InputIterator)
static const ContainerTestCase
iter_range_test_cases [] = {

#undef TEST
#define TEST(lst, off, arg, off2, size2, res, bthrow) {                 \
        __LINE__, off, -1, off2, size2, -1, lst, sizeof lst - 1,        \
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

    TEST ("cde",           0, "ab",        0, 2,  "abcde",           0),
    TEST ("cde",           1, "ab",        0, 1,  "cade",            0),
    TEST ("cde",           3, "ab",        1, 1,  "cdeb",            0),

    TEST ("ab",            0, "c<U0>e",    0, 3,  "c<U0>eab",        0),
    TEST ("ab",            1, "c<U0>e",    1, 2,  "a<U0>eb",         0),
    TEST ("ab",            2, "c<U0>e",    0, 2,  "abc<U0>",         0),

    TEST ("<U0>e<U0>",     1, "<U0>ab<U0>@2c", 0, 6, "<U0>@2ab<U0>@2ce<U0>",0),
    TEST ("<U0>e<U0>",     1, "<U0>ab<U0>@2c", 0, 3, "<U0>@2abe<U0>",       0),
    TEST ("a<U0>b<U0>@2c", 3, "<U0>e<U0>",     0, 3, "a<U0>b<U0>e<U0>@3c",  0),
    TEST ("a<U0>b<U0>@2c", 2, "<U0>@2e<U0>",   0, 2, "a<U0>@3b<U0>@2c",     0),
    TEST ("<U0>ab<U0>@2c", 0, "<U0>e<U0>",     2, 1, "<U0>@2ab<U0>@2c",     0),
    TEST ("a<U0>bc<U0>@2", 6, "<U0>e",         0, 2, "a<U0>bc<U0>@3e",      0),

#if TEST_RW_EXTENSIONS  // these tests are taking way too long, simplying them
    TEST ("",              0, "x@4096",    0, 4096, "x@4096",        0),
    TEST ("x@4096",        0, "",          0,    0, "x@4096",        0),
    TEST ("a@1000",        0, "b@1000",    0,  999, "b@999a@1000",   0),
    TEST ("a@1000",        1, "b@1001",    0, 1000, "ab@1000a@999",  0),
    TEST ("a@1000",        2, "b@1002",    0, 1001, "aab@1001a@998", 0),
    TEST ("a@1000",      998, "b@1000",    1,  999, "a@998b@999aa",  0),
    TEST ("a@1000",      999, "b@1001",    2,  999, "a@999b@999a",   0),
    TEST ("a@1000",        2, "b@1002",  999,    3, "aabbba@998",    0),

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
#else
    TEST ("",              0, "x@64",      0,   64, "x@64",          0),
    TEST ("x@64",          0, "",          0,    0, "x@64",          0),
    TEST ("a@20",          0, "b@20",      0,   19, "b@19a@20",      0),
    TEST ("a@20",          1, "b@21",      0,   20, "ab@20a@19",     0),
    TEST ("a@20",          2, "b@22",      0,   21, "aab@21a@18",    0),
    TEST ("a@20",         18, "b@20",      1,   19, "a@18b@19aa",    0),
    TEST ("a@20",         19, "b@21",      2,   19, "a@19b@19a",     0),
    TEST ("a@20",          2, "b@22",     19,    3, "aabbba@18",     0),

    TEST ("x@10",          1, "x@18",      0,   18, "x@28",          0),
    TEST ("x@18",          0, "x@19",      5,   14, "x@32",          0),
    TEST ("x@27",         27, "x@27",      0,   27, "x@54",          0),
    TEST ("x@27",         14, "x@54",     10,   27, "x@54",          0),
    TEST ("x@27",         27, "x@54",      2,   52, "x@79",          0),
#endif
    TEST ("<U0>",          2, "",          0,    0,  "<U0>",         1),
    TEST ("",              0, "<U0>",      2,    0,  "",             2),

    TEST ("a",             2, "",          0,    0,  "a",            1),
    TEST ("",              0, "a",         2,    0,  "",             2),

#if TEST_RW_EXTENSIONS
    TEST ("x@4096",     4106, "",          0,    0,  "x@4096",       1),
    TEST ("",              0, "x@4096", 4106,    0,  "",             2),
#else
    TEST ("x@64",         74, "",          0,    0,  "x@64",         1),
    TEST ("",              0, "x@64",     74,    0,  "",             2),
#endif
    TEST ("last",          4, "test",      0,    4,  "lasttest",     0)
};

/**************************************************************************/

// exercises
// insert (iterator, size_type, const value_type&)
static const ContainerTestCase
iter_size_cref_test_cases [] = {

#undef TEST
#define TEST(lst, off, size, val, res, bthrow) {               \
        __LINE__, off, size, -1, -1, val, lst, sizeof lst - 1, \
        0, 0, res, sizeof res - 1, bthrow                      \
    }

    //    +---------------------------------------- controlled sequence
    //    |                +----------------------- insert() pos argument
    //    |                |  +-------------------- insert() size argument
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
#if TEST_RW_EXTENSIONS
    TEST ("x@4096",        0, 2,    'a',  "aax@4096",      0),

    TEST ("x@4096",        0, 2,    'a',  "aax@4096",      0),
    TEST ("x@4096",        1, 2,    'a',  "xaax@4095",     0),
    TEST ("x@4096",        1, 1,    '\0', "x<U0>x@4095",   0),

    TEST ("x@127",         0, 1,    'x',  "x@128",         0),
    TEST ("x@200",       128, 7,    'x',  "x@207",         0),
    TEST ("x@331",       331, 3,    'x',  "x@334",         0),
    TEST ("x@539",         0, 1,    'x',  "x@540",         0),

    TEST ("x@3694",      128, 1,    'x',  "x@3695",        0),
    TEST ("x@540",         0, 1,    'x',  "x@541",         0),

    TEST ("x@539",         0, 873,  'x',  "x@1412",        0),
    TEST ("",              0, 4106, 'x',  "x@4106",        0),
    TEST ("x@873",       873, 1411, 'x',  "x@2284",        0),
    TEST ("",              0, 3695, 'x',  "x@3695",        0),
    TEST ("a",             0, 4095, 'x',  "x@4095a",       0),
    TEST ("x@4096",        0, 2047, 'b',  "b@2047x@4096",  0),
    TEST ("x@4096",     2047, 2048, 'x',  "x@6144",        0),
#else
    TEST ("x@64",          0, 2,    'a',  "aax@64",        0),
    TEST ("x@64",          1, 2,    'a',  "xaax@63",       0),
    TEST ("x@64",          1, 1,    '\0', "x<U0>x@63",     0),

    TEST ("x@19",          0, 1,    'x',  "x@20",          0),
    TEST ("x@19",         13, 6,    'x',  "x@25",          0),
    TEST ("x@19",         19, 3,    'x',  "x@22",          0),

    TEST ("x@19",          0, 21,   'x',  "x@40",          0),
    TEST ("",              0, 20,   'x',  "x@20",          0),
    TEST ("x@19",         19, 21,   'x',  "x@40",          0),
    TEST ("a",             0, 20,   'x',  "x@20a",         0),
    TEST ("x@20",          0, 20,   'b',  "b@20x@20",      0),
    TEST ("x@20",         10, 11,   'x',  "x@31",          0),

#endif
    TEST ("last",          4, 4,    't',  "lasttttt",      0)
};

/**************************************************************************/

// used to exercise
// insert (iterator, const value_type&)
static const ContainerTestCase
iter_cref_test_cases [] = {

#undef TEST
#define TEST(lst, off, val, res, bthrow)                                \
    { __LINE__, off, -1, -1, -1, val, lst, sizeof lst - 1, 0,           \
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
#if TEST_RW_EXTENSIONS
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
#else
    TEST ("x@64",          0,  'a', "ax@64",        0),
    TEST ("x@64",          1,  'a', "xax@63",       0),
    TEST ("x@64",          1, '\0', "x<U0>x@63",    0),

    TEST ("x@19",          0, 'x',  "x@20",         0),
    TEST ("x@19",          1, 'x',  "x@20",         0),
    TEST ("x@19",         19, 'x',  "x@20",         0),
    TEST ("x@19",         10, 'x',  "x@20",         0),
    TEST ("x@19",         18, 'x',  "x@20",         0),
#endif
    TEST ("last",          4,  't', "lastt",        0)
};

/**************************************************************************/

// used to exercise
// push_front (const value_type&)
static const ContainerTestCase
push_front_test_cases [] = {

#undef TEST
#define TEST(str, val, res) {                                       \
    __LINE__, -1, -1, -1, -1, val, str, sizeof str - 1,             \
    0, 0, res, sizeof res - 1, 0 }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ character to be inserted
    //    |                |     +------------------ expected result sequence
    //    |                |     |
    //    |                |     |
    //    |                |     |
    //    |                |     |
    //    |                |     |
    //    V                V     V
    TEST ("",              'x',  "x"              ),
    TEST ("<U0>",          '\0', "<U0>@2"         ),
    TEST ("",              'a',  "a"              ),
    TEST ("bc",            'a',  "abc"            ),
    TEST ("<U0> s",        't',  "t<U0> s"        ),
    TEST ("<U0>@3b",       'a',  "a<U0>@3b"       ),
    TEST ("<U0>b<U0>@2c",  'a',  "a<U0>b<U0>@2c"  ),
    TEST ("ab<U0>@2c",     '\0', "<U0>ab<U0>@2c"  ),
    TEST ("b<U0>c<U0>@2",  'a',  "ab<U0>c<U0>@2"  ),
#if TEST_RW_EXTENSIONS
    TEST ("x@4095",        'x',  "x@4096"         ),
#else
    TEST ("x@63",          'x',  "x@64"           ),
#endif
    TEST ("ast test",      'l',  "last test"      )
};

/**************************************************************************/

// used to exercise
// push_back (const value_type&)
static const ContainerTestCase
push_back_test_cases [] = {

#undef TEST
#define TEST(str, val, res) {                                       \
    __LINE__, -1, -1, -1, -1, val, str, sizeof str - 1,             \
    0, 0, res, sizeof res - 1, 0 }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ character to be inserted
    //    |                |     +------------------ expected result sequence
    //    |                |     |
    //    |                |     |
    //    |                |     |
    //    |                |     |
    //    |                |     |
    //    V                V     V
    TEST ("",              'x',  "x"              ),
    TEST ("<U0>",          '\0', "<U0>@2"         ),
    TEST ("",              'a',  "a"              ),
    TEST ("ab",            'c',  "abc"            ),
    TEST ("t<U0> ",        's',  "t<U0> s"        ),
    TEST ("a<U0>@3",       'b',  "a<U0>@3b"       ),
    TEST ("a<U0>b<U0>@2",  'c',  "a<U0>b<U0>@2c"  ),
    TEST ("<U0>ab<U0>@2",  'c',  "<U0>ab<U0>@2c"  ),
    TEST ("ab<U0>c<U0>",   '\0', "ab<U0>c<U0>@2"  ),
#if TEST_RW_EXTENSIONS
    TEST ("x@4095",        'x',  "x@4096"         ),
#else
    TEST ("x@63",          'x',  "x@64"           ),
#endif
    TEST ("last tes",      't',  "last test"      )
};

/**************************************************************************/

template <class InputIterator, class Distance>
inline void
_rw_advance (InputIterator& it, Distance dist) {

    while (0 < dist) {
        --dist;
        ++it;
    }
}

template<class InputIterator>
inline std::size_t
_rw_distance(InputIterator first, InputIterator last)
{
    std::size_t dist = 0;

    for (; first != last; ++first)
        ++dist;

    return dist;
}

// invokes specializations of the member function template
// on the required iterator categories
template <class List, class Iterator>
struct InsertRange: ContRangeBase<List> {

    typedef typename List::iterator       ListIter;
    typedef typename List::value_type     ListVal;

    InsertRange () { }

    virtual List&
    operator() (List &lst, 
                const ContainerTestCaseData<ListVal>& tdata) const {

        // create a pair of iterators into the list object being modified
        ListIter first1 (lst.begin ());
        _rw_advance (first1, tdata.off1_);

        const ListVal* const beg = tdata.arg_ + tdata.off2_;
        const ListVal* const end = beg + tdata.ext2_;

        const Iterator first2 (beg, beg, end);
        const Iterator last2  (end, beg, end);

        lst.insert (first1, first2, last2);
        return lst;
    }
};


// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class List, class Iterator>
struct InsertRangeOverload: ContRangeBase<List>
{
    typedef typename List::iterator       ListIter;
    typedef typename List::value_type     ListVal;

    InsertRangeOverload () { }

    virtual List&
    operator() (List &lst, 
                const ContainerTestCaseData<ListVal>& tdata) const {

        // create a pair of iterators into the list object being modified
        ListIter first1 (lst.begin ());
        _rw_advance (first1, tdata.off1_);

        bool reverse_iter = ListIds::ReverseIterator == tdata.func_.iter_id_
            || ListIds::ConstReverseIterator == tdata.func_.iter_id_;

        const std::size_t srclen_ = tdata.arglen_;

        const std::size_t off = 
            reverse_iter ? srclen_ - tdata.off2_ - tdata.ext2_ : tdata.off2_;
        const std::size_t ext = tdata.ext2_;

        List str_arg (tdata.arg_, tdata.arg_ + tdata.arglen_);

        Iterator first2 (this->begin (str_arg, (Iterator*)0));
        _rw_advance (first2, off);

        Iterator last2 (first2);
        _rw_advance (last2, ext);

        if (ListIds::UserClass == tdata.func_.elem_id_)
            UserClass::reset_totals ();

        lst.insert (first1, first2, last2);
        return lst;
    }
};

// invokes specializations of the member function template
// on the required iterator categories
template <class List, class Iterator>
struct InsertRangePtrOverload: ContRangeBase<List> {

    typedef typename List::iterator       ListIter;
    typedef typename List::value_type     ListVal;

    InsertRangePtrOverload () { }

    virtual List&
    operator() (List& lst,
                const ContainerTestCaseData<ListVal>& tdata) const {

        ListIter first1 (lst.begin ());
        _rw_advance (first1, tdata.off1_);

        const ListVal* const beg = tdata.arg_ + tdata.off2_;
        const ListVal* const end = beg + tdata.ext2_;

        const Iterator first2 = _RWSTD_CONST_CAST (Iterator, beg);
        const Iterator last2  = _RWSTD_CONST_CAST (Iterator, end);

        lst.insert (first1, first2, last2);
        return lst;
    }
};

/**************************************************************************/

template <class T, class Allocator>
void test_insert (T*, Allocator*,
                  const ContRangeBase< std::list <T, Allocator> > &rng,
                  const ContainerTestCaseData<T>                  &tdata)
{
    typedef std::list <T, Allocator>      List;
    typedef typename List::iterator       ListIter;
    typedef typename List::const_iterator ListCIter;
    typedef ListState<List>               ListState;

    const ContainerFunc     &func  = tdata.func_;
    const ContainerTestCase &tcase = tdata.tcase_;

    // construct the list object to be modified
    List lst (tdata.str_, tdata.str_ + tdata.strlen_);

    // save the state of the list object before the call
    // to detect exception safety violations (changes to
    // the state of the object after an exception)
    const ListState lst_state (lst);

    std::ptrdiff_t exp_off = Insert (iter_cref) == func.which_ ? tcase.off : 0;

    // compute the offset and the extent (the number of elements)
    // of the first range into the list object being modified
    const std::size_t size1 = tdata.strlen_;
    const std::size_t off1 =
        std::size_t (tcase.off) < size1 ? std::size_t (tcase.off) : size1;

    const T arg_val = T::from_char (char (tcase.val));

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
            // by default exercise the exception safety of the function
            // by iteratively inducing an exception at each call to operator
            // new or Allocator::allocate() until the call succeeds
            expected = exceptions [1];      // bad_alloc
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
        const List* ret_ptr = 0;

        // start checking for memory leaks
        rw_check_leaks (lst.get_allocator ());

        const bool is_class = ListIds::UserClass == func.elem_id_;

        const std::size_t x_count_save = UserClass::count_;

        try {
            // reset function call counters
            if (is_class)
                UserClass::reset_totals ();

            std::size_t newcnt = 0;

            switch (func.which_) {

            case Insert (iter_size_cref): {
                ListIter it (lst.begin ());
                _rw_advance (it, off1);
                lst.insert (it, tcase.size, arg_val);
                ret_ptr = &lst;   // function returns void
                newcnt = tcase.size;
                break;
            }

            case Insert (iter_cref): {
                ListIter it (lst.begin ());
                _rw_advance (it, off1);
                it = lst.insert (it, arg_val);
                ret_ptr = &lst + _rw_distance (lst.begin (), it);
                newcnt = 1;
                break;
            }

            case Insert (iter_range): {
                ret_ptr = &rng (lst, tdata);  // function returns void
                newcnt = tcase.size2;
                break;
            }

            case ListIds::push_front_cref:
                lst.push_front (arg_val);
                ret_ptr = &lst;   // function returns void
                newcnt = 1;
                break;

            case ListIds::push_back_cref:
                lst.push_back (arg_val);
                ret_ptr = &lst;   // function returns void
                newcnt = 1;
                break;

            default:
                RW_ASSERT (!"logic error: unknown insert overload");
                return;
            }

            if (is_class) {
                // verify that the erase method calls only dtor
                // of UserClass and only the given number of times each
                bool success = UserClass::is_total (x_count_save + newcnt,
                                                    0, newcnt, 0, 0, 0);

                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL} called default/copy ctor "
                           "and operator=() %zu, %zu and %zu times, "
                           "respectively, 0, %zu, 0, expected",
                           __LINE__,
                           UserClass::n_total_def_ctor_,
                           UserClass::n_total_copy_ctor_,
                           UserClass::n_total_op_assign_,
                           newcnt);
            }

            // verify that the reference returned from the function
            // refers to the modified list object (i.e., *this
            // within the function)
            const std::ptrdiff_t ret_off = ret_ptr - &lst;

            // verify the returned value
            rw_assert (exp_off == ret_off, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}== begin() + %td, got %td%{;}"
                       "%{?}returned invalid reference, offset is %td%{;}",
                       __LINE__, Insert (iter_cref) == func.which_,
                       exp_off, ret_off,
                       Insert (iter_cref) != func.which_, ret_off);

            // for convenience
            static const int cwidth = sizeof (T);

            const std::size_t got_size = lst.size ();
            char* const got = new char [got_size + 1];

            std::size_t index = 0;
            for (ListCIter it = lst.begin (),
                end = lst.end (); it != end; ++it) {
                got [index++] = char (it->data_.val_);
            }

            got [got_size] = '\0';

            // verify that strings length are equal
            rw_assert (tdata.reslen_ == got_size, 0, tcase.line,
                       "line %d. %{$FUNCALL} expected \"%{X=*}\" with length "
                       "%zu, got %{/*.*Gs} with length %zu", __LINE__, 
                       cwidth, int (tdata.reslen_), tdata.res_, tdata.reslen_, 
                       1, int (got_size), got, got_size);

            if (tdata.reslen_ == got_size) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // list matches the expected result
                const T* pmatch = T::mismatch (tdata.res_, got, got_size);

                rw_assert (0 == pmatch, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected \"%{X=*}\", "
                           "got %{/*.*Gs}, difference at offset %zu",
                           __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                           1, int (got_size), got, pmatch - tdata.res_);
            }

            delete [] got;
        }

#ifndef _RWSTD_NO_EXCEPTIONS

        catch (const std::bad_alloc &ex) {
            caught = exceptions [1];
            rw_assert (0 == tcase.bthrow, 0, tcase.line,
                       "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                       "unexpectedly%{;} caught std::%s(%#s)",
                       __LINE__, 0 != expected, expected, caught, ex.what ());

// Reset state of list since test case assumes inserting multiple
// items have strong exception guarantee.
#if TEST_RW_EXTENSIONS
            lst;  // do nothing
#else
            lst.clear();
            lst.insert(lst.begin(), tdata.str_, tdata.str_ + tdata.strlen_);
#endif
        }
        catch (const std::exception &ex) {
            caught = exceptions [2];
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
        rw_check_leaks (lst.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object

// State will change because an strong exception guarantee for multi-item
// insert is not implemented.
#if TEST_RW_EXTENSIONS
            lst_state.assert_equal (ListState (lst),
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

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

template <class T, class Allocator>
void test_insert (T*, Allocator*,
                  const ContainerTestCaseData<T> &tdata)
{
    typedef std::list<T, Allocator> List;

    if (tdata.func_.which_ == Insert (iter_range)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename List::Iterator Iter;                               \
        static const                                                        \
        InsertRangePtrOverload<List, Iter> rng;                             \
        test_insert ((T*)0, (Allocator*)0, rng, tdata);                     \
    } while (0)

        case ListIds::Pointer: TEST (pointer); break;
        case ListIds::ConstPointer: TEST (const_pointer); break;

#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename List::Iterator Iter;                               \
        static const                                                        \
        InsertRangeOverload<List, Iter> rng;                                \
        test_insert ((T*)0, (Allocator*)0, rng, tdata);                     \
    } while (0)

        case ListIds::Iterator: TEST (iterator); break;
        case ListIds::ConstIterator: TEST (const_iterator); break;

        case ListIds::ReverseIterator: TEST (reverse_iterator); break;
        case ListIds::ConstReverseIterator: TEST (const_reverse_iterator);
            break;

        // exercise specializations of the member function template
        // on the required iterator categories
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef Iterator<T> Iter;                                           \
        static const                                                        \
        InsertRange<List, Iter> rng;                                        \
        test_insert ((T*)0, (Allocator*)0, rng, tdata);                     \
    } while (0)

        case ListIds::Input: TEST (InputIter); break;
        case ListIds::Forward: TEST (ConstFwdIter); break;
        case ListIds::Bidir: TEST (ConstBidirIter); break;
        case ListIds::Random: TEST (ConstRandomAccessIter); break;

        default:
            rw_error (0, 0, __LINE__, "bad iterator id");
        }
    }
    else {
        // exercise ordinary overloads of the member function
        static const ContRangeBase<List> rng;
        test_insert ((T*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_insert);

int main (int argc, char** argv)
{
    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Insert (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (iter_cref),
        TEST (iter_size_cref),
        TEST (iter_range),

#undef TEST
#define TEST(sig) {                                             \
        ListIds::sig ## _cref, sig ## _test_cases,              \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (push_front),
        TEST (push_back)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_cont_test (argc, argv, __FILE__,
                          "lib.list.insert",
                          ContainerIds::List,
                          test_insert_func_array, tests, test_count);

    return status;
}
