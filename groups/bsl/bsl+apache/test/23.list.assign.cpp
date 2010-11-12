/***************************************************************************
 *
 * 23.list.assign.cpp - test exercising [lib.list.assign]
 *
 * $Id: 23.list.assign.cpp 580483 2007-09-28 20:55:52Z sebor $
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
#include <cstddef>        // for size_t

#include <23.list.h>      // for ListMembers
#include <alg_test.h>     // for InputIter
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Assign(sig) ListIds::assign_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise:
// assign (InputIterator, InputIterator)
static const ContainerTestCase
range_test_cases [] = {

#undef TEST
#define TEST(lst, arg, off, size, res, bthrow) {                \
        __LINE__, -1, -1, off, size, -1,                        \
        lst, sizeof lst - 1,                                    \
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
    //    |                |            |  |  |  +--------------------+
    //    |                |            |  |  +------+                |
    //    |                |            |  +------+  |                |
    //    |                |            +---+     |  |                |
    //    |                |                |     |  |                |
    //    |                |                |     |  |                |
    //    V                V                V     V  V                V
    TEST ("ab",            "c",             0,    1, "c",             0),
                                               
    TEST ("",              "",              0,    0, "",              0),
    TEST ("",              "abc",           1,    1, "b",             0),
    TEST ("",              "<U0>",          0,    1, "<U0>",          0),
                                               
    TEST ("<U0>",          "",              0,    0, "",              0),
                                               
    TEST ("abc",           "",              0,    0, "",              0),
                                               
    TEST ("<U0>",          "a",             0,    1, "a",             0),
    TEST ("<U0>",          "<U0>@2",        1,    1, "<U0>",          0),
    TEST ("<U0>",          "<U0>@2",        0,    2, "<U0>@2",        0),
    TEST ("<U0>",          "<U0>@2",        1,    5, "<U0>",          0),
                                               
    TEST ("cde",           "ab",            0,    2, "ab",            0),
    TEST ("cde",           "ab",            0,    1, "a",             0),
    TEST ("cde",           "ab",            1,    5, "b",             0),
                                               
    TEST ("ab",            "c<U0>e",        0,    3, "c<U0>e",        0),
    TEST ("ab",            "c<U0>e",        1,    2, "<U0>e",         0),
    TEST ("ab",            "c<U0>e",        0,    2, "c<U0>",         0),

    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 0,    9, "<U0>ab<U0>@2c", 0),
    TEST ("<U0>e<U0>",     "<U0>ab<U0>@2c", 0,    3, "<U0>ab",        0),
    TEST ("a<U0>b<U0>@2c", "<U0>e<U0>",     0,    3, "<U0>e<U0>",     0),
    TEST ("a<U0>b<U0>@2c", "<U0>@2e<U0>",   0,    2, "<U0>@2",        0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     2,    1, "<U0>",          0),
    TEST ("<U0>ab<U0>@2c", "<U0>e<U0>",     2,    9, "<U0>",          0),
    TEST ("a<U0>bc<U0>@2", "<U0>e",         0,    2, "<U0>e",         0),

#if TEST_RW_EXTENSIONS  // these test cases are too large and runs too slowly
    TEST ("x@4096",        "",              0,    0, "",              0),
    TEST ("",              "x@4096",        9,    2, "xx",            0),
    TEST ("",              "x@4096",        9,    0, "",              0),
    TEST ("abc",           "x@4096",        2,    1, "x",             0),
    TEST ("x@4096",        "x@4096",        2,    3, "xxx",           0),
    TEST ("",              "x@4096",        0, 4096, "x@4096",        0),
    TEST ("",              "x@4096",      100, 2000, "x@2000",        0),
                   
    TEST ("",              "x@207",         0,  207, "x@207",         0),
    TEST ("x@128",         "x@334",        10,  207, "x@207",         0),
    TEST ("x@540",         "x@207",        50,  128, "x@128",         0),
    TEST ("",              "x@1412",      128,  873, "x@873",         0),
    TEST ("x@128",         "x@1412",        0, 1412, "x@1412",        0),
    TEST ("x@3695",        "x@1412",      207,  540, "x@540",         0),
    TEST ("x@872",         "x@874",         1,  873, "x@873",         0),
    TEST ("x@873",         "x@3695",       10, 2284, "x@2284",        0),
                   
    TEST ("",              "<U0>",          2,    0, "",              1),
    TEST ("",              "a",             2,    0, "",              1),
    TEST ("",              "x@4096",     4106,    0, "",              1),
#else
    TEST ("x@64",          "",              0,    0, "",              0),
    TEST ("",              "x@64",          9,    2, "xx",            0),
    TEST ("",              "x@64",          9,    0, "",              0),
    TEST ("abc",           "x@64",          2,    1, "x",             0),
    TEST ("x@80",          "x@64",          2,    3, "xxx",           0),
    TEST ("",              "x@64",          0,   64, "x@64",          0),
    TEST ("",              "x@64",         10,   20, "x@20",          0),
                   
    TEST ("x@50",          "x@64",         10,   20, "x@20",          0),
    TEST ("x@80",          "x@64",         5,    32, "x@32",          0),
    TEST ("x@50",          "x@64",         0,    64, "x@64",          0),
    TEST ("x@50",          "x@64",         1,    63, "x@63",          0),
                   
    TEST ("",              "<U0>",          2,    0, "",              1),
    TEST ("",              "a",             2,    0, "",              1),
    TEST ("",              "x@64",         74,    0, "",              1),

#endif
    TEST ("last",          "test",          0,    4, "test",          0)
};                                       
                                         
/**************************************************************************/

// used to exercise:
// assign (size_type, const value_type&)
static const ContainerTestCase
size_cref_test_cases [] = {

#undef TEST
#define TEST(lst, size, val, res, bthrow) {     \
        __LINE__, -1, size, -1, -1, val,        \
        lst, sizeof lst - 1,                    \
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

#if TEST_RW_EXTENSIONS  // these test runs too slowly, simplify
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
#else
    TEST ("",             64, 'x',  "x@64",         0),
    TEST ("x@64",          0, 'x',  "",             0),

    TEST ("x@63",         64, 'x', "x@64",          0),
    TEST ("x@20",         27, 'x', "x@27",          0),
    TEST ("x@44",         33, 'x', "x@33",          0),
    TEST ("",             54, 'x', "x@54",          0),
    TEST ("xx",           30, 'x', "x@30",          0),

#endif

    TEST ("last",          4, 't',  "tttt",         0)
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

// invokes specializations of the member function template
// on the required iterator categories
template <class List, class Iterator>
struct AssignRange: ContRangeBase<List> {

    typedef typename List::iterator       ListIter;
    typedef typename List::value_type     ListVal;

    AssignRange () { }

    virtual List&
    operator() (List &lst, 
                const ContainerTestCaseData<ListVal>& tdata) const {

        const ListVal* const beg = tdata.arg_ + tdata.off2_;
        const ListVal* const end = beg + tdata.ext2_;

        const Iterator first (beg, beg, end);
        const Iterator last  (end, beg, end);

        lst.assign (first, last);
        return lst;
    }
};

/**************************************************************************/

// invokes possible overloads of the member function template
// on common RandomAccessIterator types
template <class List, class Iterator>
struct AssignRangeOverload: ContRangeBase<List>
{
    typedef typename List::iterator       ListIter;
    typedef typename List::value_type     ListVal;

    AssignRangeOverload () { }

    virtual List&
    operator() (List &lst, 
                const ContainerTestCaseData<ListVal>& tdata) const {

        const bool reverse_iter =
               ListIds::ReverseIterator == tdata.func_.iter_id_
            || ListIds::ConstReverseIterator == tdata.func_.iter_id_;

        const std::size_t off = tdata.arglen_ - tdata.off2_ - tdata.ext2_;

        List str_arg (tdata.arg_, tdata.arg_ + tdata.arglen_);

        Iterator first (this->begin (str_arg, (Iterator*)0));
        _rw_advance (first, reverse_iter ? off : tdata.off2_);

        Iterator last (first);
        _rw_advance (last, tdata.ext2_);

        lst.assign (first, last);
        return lst;
    }
};

/**************************************************************************/

// invokes specializations of the member function template
// on the required iterator categories
template <class List, class Iterator>
struct AssignRangePtrOverload: ContRangeBase<List> {

    typedef typename List::value_type     ListVal;

    AssignRangePtrOverload () { }

    virtual List&
    operator() (List& lst,
                const ContainerTestCaseData<ListVal>& tdata) const {

        const ListVal* const beg = tdata.arg_ + tdata.off2_;
        const ListVal* const end = beg + tdata.ext2_;

        const Iterator first = _RWSTD_CONST_CAST (Iterator, beg);
        const Iterator last  = _RWSTD_CONST_CAST (Iterator, end);

        lst.assign (first, last);
        return lst;
    }
};

/**************************************************************************/

template <class T, class Allocator>
void test_assign (T*, Allocator*,
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

    std::size_t size = tcase.size >= 0 ? tcase.size : 0;

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

        // start checking for memory leaks
        rw_check_leaks (lst.get_allocator ());

        try {
            switch (func.which_) {

            case Assign (size_cref): {
                const T val = T::from_char (char (tcase.val));
                lst.assign (size, val);
                break;
            }

            case Assign (range):
                rng (lst, tdata);
                break;

            default:
                RW_ASSERT (!"test logic error: unknown assign overload");
            }

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

            // verify that list length are equal to the expected
            rw_assert (tdata.reslen_ == got_size, 0, tcase.line,
                       "line %d. %{$FUNCALL}: expected \"%{X=*}\" with length "
                       "%zu, got %{/*.*Gs} with length %zu", __LINE__, 
                       cwidth, int (tdata.reslen_), tdata.res_, tdata.reslen_,
                       1, int (got_size), got, got_size);

            if (tdata.reslen_ == got_size) {
                // if the result length matches the expected length
                // (and only then), also verify that the modified
                // list matches the expected result
                const std::size_t match =
                    rw_match (tcase.res, got, got_size);

                rw_assert (match == tdata.reslen_, 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected \"%{X=*}\", "
                           "got %{/*.*Gs}, difference at off %zu",
                           __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                           1, int (got_size), got, match);
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
            if (0 == tcase.bthrow && caught == exceptions [1]) {
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
void test_assign (T*, Allocator*,
                  const ContainerTestCaseData<T> &tdata)
{
    typedef std::list<T, Allocator> List;

    if (tdata.func_.which_ == Assign (range)) {

        switch (tdata.func_.iter_id_) {

        // exercise possible overloads of the member function template
        // on common RandomAccessIterator types
#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename List::Iterator Iter;                               \
        static const                                                        \
        AssignRangePtrOverload<List, Iter> rng;                                \
        test_assign ((T*)0, (Allocator*)0, rng, tdata);                     \
    } while (0)

        case ListIds::Pointer: TEST (pointer); break;
        case ListIds::ConstPointer: TEST (const_pointer); break;

#undef TEST
#define TEST(Iterator) do {                                                 \
        typedef typename List::Iterator Iter;                               \
        static const                                                        \
        AssignRangeOverload<List, Iter> rng;                                \
        test_assign ((T*)0, (Allocator*)0, rng, tdata);                     \
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
        AssignRange<List, Iter> rng;                                        \
        test_assign ((T*)0, (Allocator*)0, rng, tdata);                     \
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
        test_assign ((T*)0, (Allocator*)0, rng, tdata);
    }
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_assign);

int main (int argc, char** argv)
{
    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Assign (sig), sig ## _test_cases,                       \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (size_cref),
        TEST (range)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_cont_test (argc, argv, __FILE__,
                          "lib.list.assign",
                          ContainerIds::List,
                          test_assign_func_array, tests, test_count);

    return status;
}
