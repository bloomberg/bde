/***************************************************************************
 *
 * 23.list.erase.cpp - test exercising [lib.list::erase]
 *
 * $Id: 23.list.erase.cpp 523692 2007-03-29 13:01:55Z faridz $
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

#include <list>             // for list
#include <cstddef>          // for ptrdiff_t, size_t
#include <stdexcept>        // for out_of_range

#include <23.list.h>        // for ListMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Erase(sig)                ListIds::erase_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "exception"
};

/**************************************************************************/

// used to exercise
// erase (iterator, iterator)
static const ContainerTestCase
iter_iter_test_cases [] = {

#undef TEST
#define TEST(str, off, size, res) {                                        \
        __LINE__, off, size, -1, -1, -1, str, sizeof str - 1,              \
        0, 0, res, sizeof res - 1, 0 }

    //    +----------------------------------------- controlled sequence
    //    |                   +--------------------- erase() pos argument
    //    |                   |         +----------- erase() n argument
    //    |                   |         |   +------- expected result sequence
    //    |                   |         |   |
    //    |                   |         |   |
    //    |                   |         |   |
    //    |                   |         |   |
    //    |                   |         |   |
    //    V                   V         V   V
    TEST ("",                 0,        0,  ""              ),
    TEST ("<U0>",             0,        1,  ""              ),
    TEST ("<U0>",             0,        0,  "<U0>"          ),
    TEST ("<U0>",             1,        1,  "<U0>"          ),
    TEST ("<U0>@2",           1,        1,  "<U0>"          ),
    TEST ("<U0>@2",           0,        1,  "<U0>"          ),

    TEST ("a",                0,        1,  ""              ),
    TEST ("a",                0,        0,  "a"             ),
    TEST ("a",                1,        1,  "a"             ),

    TEST ("abc",              0,        3,  ""              ),
    TEST ("abc",              0,        2,  "c"             ),
    TEST ("abc",              1,        2,  "a"             ),
    TEST ("abc",              1,        1,  "ac"            ),
    TEST ("abc",              2,        1,  "ab"            ),
    TEST ("abc",              3,        0,  "abc"           ),

    TEST ("t<U0> s",          0,        3,  "s"             ),
    TEST ("t<U0> s",          0,        4,  ""              ),
    TEST ("t<U0> s",          0,        1,  "<U0> s"        ),
    TEST ("t<U0> s",          1,        3,  "t"             ),
    TEST ("t<U0> s",          1,        2,  "ts"            ),
    TEST ("t<U0> s",          2,        2,  "t<U0>"         ),
    TEST ("t<U0> s",          2,        1,  "t<U0>s"        ),
    TEST ("t<U0> s",          3,        2,  "t<U0> "        ),
    TEST ("t<U0> s",          4,        0,  "t<U0> s"       ),

    TEST ("a<U0>@3b",         2,        0,  "a<U0>@3b"      ),
    TEST ("a<U0>@3b",         2,        3,  "a<U0>"         ),
    TEST ("a<U0>@3b",         2,        2,  "a<U0>b"        ),
    TEST ("a<U0>@3b",         1,        4,  "a"             ),
    TEST ("a<U0>@3b",         0,        5,  ""              ),
    TEST ("a<U0>@3b",         0,        2,  "<U0>@2b"       ),
    TEST ("a<U0>b<U0>@2c",    0,        6,  ""              ),
    TEST ("a<U0>b<U0>@2c",    4,        2,  "a<U0>b<U0>"    ),
    TEST ("a<U0>b<U0>@2c",    4,        1,  "a<U0>b<U0>c"   ),
    TEST ("<U0>ab<U0>@2c",    2,        5,  "<U0>a"         ),
    TEST ("<U0>ab<U0>@2c",    0,        4,  "<U0>c"         ),
    TEST ("ab<U0>c<U0>@2",    5,        1,  "ab<U0>c<U0>"   ),
    TEST ("a",                0,        3,  ""              ),
    TEST ("t<U0> s",          0,        9,  ""              ),
    TEST ("ab<U0>c<U0>@2",    0,       10,  ""              ),
    TEST ("a",                3,        1,  "a"             ),
    TEST ("t<U0> s",          5,        1,  "t<U0> s"       ),
    TEST ("ab<U0>c<U0>@2",   10,        1,  "ab<U0>c<U0>@2" ),

#if TEST_RW_EXTENSIONS
    TEST ("x@4096",           0,     4105,  ""              ),
    TEST ("x@4096",        4106,        1,  "x@4096"        ),
    TEST ("x@4096",           0,     4096,  ""              ),
    TEST ("x@4096",           1,     4095,  "x"             ),
    TEST ("x@4096",           4,     4092,  "xxxx"          ),
    TEST ("x@4096",           4,     4090,  "xxxxxx"        ),
    TEST ("x@4096",        4096,     4096,  "x@4096"        ),
    TEST ("x@2048y@2048",     1,     4094,  "xy"            ),
#else
    TEST ("x@64",             0,       74,  ""              ),
    TEST ("x@64",            74,        1,  "x@64"          ),
    TEST ("x@64",             0,       64,  ""              ),
    TEST ("x@64",             1,       63,  "x"             ),
    TEST ("x@64",             4,       60,  "xxxx"          ),
    TEST ("x@64",             4,       58,  "xxxxxx"        ),
    TEST ("x@64",            64,       64,  "x@64"          ),
    TEST ("x@32y@32",         1,       62,  "xy"            ),
#endif

    TEST ("last test",        4,        1,  "lasttest"      )
};


/**************************************************************************/

// used to exercise
// erase (iterator)
static const ContainerTestCase
iter_test_cases [] = {

#undef TEST
#define TEST(str, off, res) {                                      \
        __LINE__, off, -1, -1, -1, -1, str, sizeof str - 1,        \
        0, 0, res, sizeof res - 1, 0 }

    //    +-------------------------------------- controlled sequence
    //    |                   +------------------ iterator offset
    //    |                   |    +------------- expected result sequence
    //    |                   |    |
    //    V                   V    V
    TEST ("a",                0,   ""),

    TEST ("<U0>",             0,   ""),
    TEST ("<U0>@2",           0,   "<U0>"),
    TEST ("<U0>@2",           1,   "<U0>"),

    TEST ("abc",              0,   "bc"),
    TEST ("abc",              1,   "ac"),
    TEST ("abc",              2,   "ab"),

    TEST ("t<U0> s",          0,   "<U0> s"),
    TEST ("t<U0> s",          1,   "t s"),
    TEST ("t<U0> s",          2,   "t<U0>s"),
    TEST ("t<U0> s",          3,   "t<U0> "),

    TEST ("a<U0>@3b",         4,   "a<U0>@3"),
    TEST ("a<U0>@3b",         2,   "a<U0>@2b"),
    TEST ("a<U0>@3b",         1,   "a<U0>@2b"),
    TEST ("a<U0>@3b",         0,   "<U0>@3b"),
    TEST ("a<U0>b<U0>@2c",    4,   "a<U0>b<U0>c"),
    TEST ("<U0>ab<U0>@2c",    0,   "ab<U0>@2c"),
    TEST ("<U0>ab<U0>@2c",    2,   "<U0>a<U0>@2c"),
    TEST ("ab<U0>c<U0>@2",    5,   "ab<U0>c<U0>"),
#if TEST_RW_EXTENSIONS
    TEST ("x@4096y",       4096,   "x@4096"),
    TEST ("x@4096",        4088,   "x@4095"),
    TEST ("ax@4096",          0,   "x@4096"),
    TEST ("x@4096",           9,   "x@4095"),
#else
    TEST ("x@64y",           64,   "x@64"),
    TEST ("x@64",            58,   "x@63"),
    TEST ("ax@64",            0,   "x@64"),
    TEST ("x@64",             9,   "x@63"),
#endif
    TEST ("last test",        4,   "lasttest")
};

/**************************************************************************/

// used to exercise
// pop_front ()
static const ContainerTestCase
pop_front_test_cases [] = {

#undef TEST
#define TEST(str, res) {                                            \
    __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1,              \
    0, 0, res, sizeof res - 1, 0 }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ expected result sequence
    //    |                |
    //    |                |
    //    |                |
    //    |                |
    //    |                |
    //    V                V
    TEST ("<U0>",          ""              ),
    TEST ("<U0>@2",        "<U0>"          ),
    TEST ("a",             ""              ),
    TEST ("abc",           "bc"            ),
    TEST ("t<U0> s",       "<U0> s"        ),
    TEST ("a<U0>@3b",      "<U0>@3b"       ),
    TEST ("a<U0>b<U0>@2c", "<U0>b<U0>@2c"  ),
    TEST ("<U0>ab<U0>@2c", "ab<U0>@2c"     ),
    TEST ("ab<U0>c<U0>@2", "b<U0>c<U0>@2"  ),
#if TEST_RW_EXTENSIONS
    TEST ("x@4096",        "x@4095"        ),
#else
    TEST ("x@64",          "x@63"          ),
#endif

    TEST (" last test",    "last test"     )
};

/**************************************************************************/

// used to exercise
// pop_back ()
static const ContainerTestCase
pop_back_test_cases [] = {

#undef TEST
#define TEST(str, res) {                                            \
    __LINE__, -1, -1, -1, -1, -1, str, sizeof str - 1,              \
    0, 0, res, sizeof res - 1, 0 }

    //    +----------------------------------------- controlled sequence
    //    |                +------------------------ expected result sequence
    //    |                |
    //    |                |
    //    |                |
    //    |                |
    //    |                |
    //    V                V
    TEST ("<U0>",          ""              ),
    TEST ("<U0>@2",        "<U0>"          ),
    TEST ("a",             ""              ),
    TEST ("abc",           "ab"            ),
    TEST ("t<U0> s",       "t<U0> "        ),
    TEST ("a<U0>@3b",      "a<U0>@3"       ),
    TEST ("a<U0>b<U0>@2c", "a<U0>b<U0>@2"  ),
    TEST ("<U0>ab<U0>@2c", "<U0>ab<U0>@2"  ),
    TEST ("ab<U0>c<U0>@2", "ab<U0>c<U0>"   ),
#if TEST_RW_EXTENSIONS
    TEST ("x@4096",        "x@4095"        ),
#else
    TEST ("x@64",          "x@63"          ),
#endif

    TEST ("last test ",    "last test"     )
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

template <class T, class Allocator>
void test_erase (T*, Allocator*,
                 const ContainerTestCaseData<T> &tdata)
{
    typedef std::list <T, Allocator>      List;
    typedef typename List::iterator       ListIter;
    typedef typename List::const_iterator ListCIter;
    typedef ListState<List>               ListState;

    const ContainerFunc     &func  = tdata.func_;
    const ContainerTestCase &tcase = tdata.tcase_;

    // construct the list object to be modified
    List lst (tdata.str_, tdata.str_ + tdata.strlen_);

    // compute the offset and the extent (the number of elements)
    // of the first range into the list object being modified
    const std::size_t size1 = tdata.strlen_;
    const std::size_t off1 =
        std::size_t (tcase.off) < size1 ? std::size_t (tcase.off) : size1;
    const std::size_t ext1 =
        off1 + tcase.size < size1 ? tcase.size : size1 - off1;

    // create a pair of iterators into the list object being modified
    ListIter it_first (lst.begin ());
    _rw_advance (it_first, off1);

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    // start checking for memory leaks
    rwt_check_leaks (0, 0);

    ListIter res_iter;

    const bool is_class = ListIds::UserClass == func.elem_id_;

    const std::size_t x_count_save = UserClass::count_;

    try {
        // reset function call counters
        if (is_class)
            UserClass::reset_totals ();

        std::size_t delcnt = 0;

        switch (func.which_) {

        case Erase (iter):
            res_iter = lst.erase (it_first);
            delcnt = 1;
            break;

        case Erase (iter_iter): {
            ListIter it_last (it_first);
            _rw_advance (it_last, ext1);
            res_iter = lst.erase (it_first, it_last);
            delcnt = ext1;
            break;
        }

        case ListIds::pop_front_void:
            lst.pop_front ();
            delcnt = 1;
            break;

        case ListIds::pop_back_void:
            lst.pop_back ();
            delcnt = 1;
            break;

        default:
            RW_ASSERT (!"test logic error: unknown erase overload");
            return;
        }

        if (is_class) {
            // verify that the erase method calls only dtor
            // of UserClass and only the given number of times each
            bool success = UserClass::is_total (x_count_save - delcnt,
                                                0, 0, 0, 0, 0)
                        && UserClass::n_total_dtor_ == delcnt;

            rw_assert (success, 0, tcase.line,
                       "line %d. %{$FUNCALL} called default/copy ctor, "
                       "operator=() and dtor %zu, %zu, %zu, and %zu times, "
                       "respectively, 0, 0, 0, %zu expected",
                       __LINE__,
                       UserClass::n_total_def_ctor_,
                       UserClass::n_total_copy_ctor_,
                       UserClass::n_total_op_assign_,
                       UserClass::n_total_dtor_, delcnt);
        }

        if (ListIds::pop_front_void != func.which_
         && ListIds::pop_back_void != func.which_) {
             // verify the returned value
             std::size_t dist = _rw_distance (lst.begin (), res_iter);
             rw_assert (dist <= lst.size(), 0, tcase.line,
                 "line %d. %{$FUNCALL} returned invalid iterator, "
                 "difference with begin is %zu",
                 __LINE__, dist);

             if (std::size_t (tcase.off) >= tdata.reslen_) {
                 rw_assert (res_iter == lst.end (), 0, tcase.line,
                     "line %d. %{$FUNCALL} != end()", __LINE__);
             }
             else {
                 bool success =
                     tdata.res_ [tcase.off].data_.val_ == res_iter->data_.val_;

                 rw_assert (success, 0, tcase.line,
                     "line %d. %{$FUNCALL} == %{#c}, got %{#c}",
                     __LINE__, char (tdata.res_ [tcase.off].data_.val_),
                     char (res_iter->data_.val_));
             }
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

        // verify that strings length are equal
        rw_assert (tdata.reslen_ == got_size, 0, tcase.line,
                   "line %d. %{$FUNCALL} expected \"%{X=*}\" with length "
                   "%zu, got %{/*.*Gs} with length %zu",
                   __LINE__, cwidth, int (tdata.reslen_), tdata.res_,
                   tdata.reslen_, 1, int (got_size), got, got_size);

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

    catch (const std::exception &ex) {
        caught = exceptions [1];
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
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_erase);

int main (int argc, char** argv)
{
    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Erase (sig), sig ## _test_cases,                        \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (iter),
        TEST (iter_iter),

#undef TEST
#define TEST(sig) {                                             \
        ListIds::sig ## _void, sig ## _test_cases,              \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (pop_front),
        TEST (pop_back)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_cont_test (argc, argv, __FILE__,
                             "lib.list.erase",
                             ContainerIds::List,
                             test_erase_func_array, tests, test_count);
}
