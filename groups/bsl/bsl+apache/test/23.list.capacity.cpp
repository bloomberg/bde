/***************************************************************************
 *
 * 23.list.capacity.cpp - test exercising [lib.list.capacity]
 *
 * $Id: 23.list.capacity.cpp 523692 2007-03-29 13:01:55Z faridz $
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
#include <cstddef>          // for size_t
#include <stdexcept>        // for exception
#include <23.list.h>        // for ListMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Size(sig)                 ListIds::size_ ## sig
#define MaxSize(sig)              ListIds::max_size_ ## sig
#define Resize(sig)               ListIds::resize_ ## sig
#define Clear(sig)                ListIds::clear_ ## sig
#define Empty(sig)                ListIds::empty_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// size ()
static const ContainerTestCase
size_void_test_cases [] = {

#undef TEST
#define TEST(str, res) {                    \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        0, res, -1                          \
    }

    //    +--------------------------------------- controlled sequence
    //    |                    +------------------ expected result
    //    |                    |                   
    //    |                    |                   
    //    V                    V                   
    TEST (0,                   0),
    TEST ("",                  0),

    TEST ("<U0>",              1),
    TEST ("a",                 1),
    TEST (" ",                 1),
    TEST ("ab",                2),
    TEST ("bc",                2),

    TEST ("test string",      11),
    TEST ("Test String",      11),

    TEST ("t<U0> s",           4),
    TEST ("Test<U0>string",   11),

    TEST ("<U0>a<U0>b",        4),
    TEST ("a<U0>@2b",          4),
    TEST ("a<U0>@3b",          5),
    TEST ("a<U0>@2b<U0>",      5),
    TEST ("a<U0>b<U0>@2c",     6),
    TEST ("a<U0>b<U0>c<U0>@2", 7),

#if TEST_RW_EXTENSIONS  // test case running too slowly
    TEST ("x@128",           128),
    TEST ("x@207",           207),
    TEST ("x@334",           334),
    TEST ("x@540",           540),
    TEST ("x@873",           873),
    TEST ("x@1412",         1412),
    TEST ("x@2284",         2284),
    TEST ("x@3695",         3695),
    TEST ("x@4096",         4096),
#else
    TEST ("x@31",             31),
    TEST ("x@32",             32),
    TEST ("x@33",             33),
    TEST ("x@63",             63),
    TEST ("x@64",             64),
    TEST ("x@65",             65),
    TEST ("x@127",           127),
    TEST ("x@128",           128),
    TEST ("x@129",           129),
#endif
    TEST ("last",              4)
};

/**************************************************************************/

// used to exercise
// max_size ()
static const ContainerTestCase
max_size_void_test_cases [] = {

#undef TEST
#define TEST(str) {                         \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        0, 0, -1                            \
    }

    //    +------------------------------ controlled sequence
    //    |                
    //    |                               
    //    |                               
    //    V                             
    TEST (0),     
    TEST (""),   

    TEST ("<U0>"),    
    TEST ("a"),       

    TEST ("test string"), 
    TEST ("a<U0>b<U0>c<U0>@2"),  
    TEST ("x@128"),     
    TEST ("x@207"),    
#if TEST_RW_EXTENSIONS  // these tests run too slowly and doesn't add any value
    TEST ("x@2284"),   
    TEST ("x@3695"), 
#endif

    TEST ("last")        
};

/**************************************************************************/

// used to exercise
// resize (size_type, value_type)
static const ContainerTestCase
resize_size_val_test_cases [] = {

#undef TEST
#define TEST(str, size, val, res, bthrow) {    \
        __LINE__, -1, size, -1, -1, val,       \
        str, sizeof str - 1, 0, 0,             \
        res, sizeof res - 1, bthrow            \
    }

    //    +------------------------------------------ controlled sequence
    //    |                      +------------------- new size
    //    |                      |     +------------- value_type argument
    //    |                      |     |    +-------- expected result sequence
    //    |                      |     |    |      +- exception info
    //    |                      |     |    |      |     0 - no exception
    //    |                      |     |    |      |     1 - length_error
    //    |                      |     |    |      |    
    //    |                      |     |    |      +-----------+
    //    V                      V     V    V                  V
    TEST ("",                    0,   'a',  "",                0),
    TEST ("",                    5,  '\0',  "<U0>@3<U0>@2",    0),
    TEST ("",                  334,   'x',  "x@334",           0),

    TEST ("<U0>",                0,   'a',  "",                0),
    TEST ("<U0>",                2,   'a',  "<U0>a",           0),
    TEST ("<U0>",                1,   'a',  "<U0>",            0),
    TEST ("<U0>",              128,   'a',  "<U0>a@127",       0),

    TEST ("a",                   0,   'a',  "",                0),
    TEST ("a",                   2,  '\0',  "a<U0>",           0),
    TEST ("a",                 540,   'a',  "a@540",           0),

    TEST ("a<U0>@3b",           10,   'a',  "a<U0>@3baaaaa",   0),
    TEST ("ab<U0>@3",           10,   'a',  "ab<U0>@3aaaaa",   0),
    TEST ("<U0>@3ab",           10,   'a',  "<U0>@3abaaaaa",   0),
    TEST ("a<U0>@3b",            7,  '\0',  "a<U0>@3b<U0>@2",  0),
    TEST ("ab<U0>@3",            7,  '\0',  "ab<U0>@3<U0>@2",  0),
    TEST ("<U0>@3ba",            7,  '\0',  "<U0>@3ba<U0>@2",  0),

    TEST ("a<U0>b<U0>c<U0>@2",   6,   'a',  "a<U0>b<U0>c<U0>", 0),
    TEST ("a<U0>b<U0>c<U0>@2",   5,  '\0',  "a<U0>b<U0>c",     0),
    TEST ("<U0>ba<U0>c<U0>@2",   1,  '\0',  "<U0>",            0),
    TEST ("<U0>ba<U0>c<U0>@2",   0,  '\0',  "",                0),
#if TEST_RW_EXTENSIONS  // these test runs too slowly
    TEST ("x@540",             127,   'a',  "x@127",           0),
    TEST ("x@873",             127,   'a',  "x@127",           0),
    TEST ("x@1412",            127,   'a',  "x@127",           0),
    TEST ("x@2284",            127,   'a',  "x@127",           0),

    TEST ("x@127",             128,   'a',  "x@127a",          0),
    TEST ("x@128",             207,   'a',  "x@128a@79",       0),
    TEST ("x@207",             334,   'a',  "x@207a@127",      0),
    TEST ("x@334",             540,   'a',  "x@334a@206",      0),
    TEST ("x@540",             873,   'a',  "x@540a@333",      0),
    TEST ("x@873",            1412,   'a',  "x@873a@539",      0),
    TEST ("x@1412",           2284,   'a',  "x@1412a@872",     0),
    TEST ("x@2284",           3695,   'a',  "x@2284a@1411",    0),
#else
    TEST ("x@63",              63,    'a',  "x@63",            0),
    TEST ("x@64",              63,    'a',  "x@63",            0),
    TEST ("x@80",              63,    'a',  "x@63",            0),

    TEST ("x@63",               64,   'a',  "x@63a",            0),
    TEST ("x@63",               80,   'a',  "x@63a@17",         0),

#endif
    TEST ("last",                4,   't',  "last",            0)
};

/**************************************************************************/

// used to exercise
// resize (size_type)
static const ContainerTestCase
resize_size_test_cases [] = {

#undef TEST
#define TEST(str, size, res, bthrow) {      \
        __LINE__, -1, size, -1, -1, -1,     \
        str, sizeof str - 1, 0, 0,          \
        res, sizeof res - 1, bthrow         \
    }

    //    +------------------------------------------ controlled sequence
    //    |                      +------------------- new size
    //    |                      |    +-------------- expected result sequence
    //    |                      |    |              +-- exception info
    //    |                      |    |              |      0 - no exception
    //    |                      |    |              |      1 - length_error
    //    |                      |    |              |
    //    V                      V    V              V    
    TEST ("",                    0,   "",            0),
    TEST ("",                    5,   "\0@5",        0),
    TEST ("",                  334,   "\0@334",      0),

    TEST ("<U0>",                0,   "",            0),
    TEST ("<U0>",                2,   "\0@2",        0),
    TEST ("<U0>",                1,   "\0",          0),
    TEST ("<U0>",              127,   "\0@127",      0),

    TEST ("a",                   0,   "",            0),
    TEST ("a",                   2,   "a\0",         0),
    TEST ("a",                 539,   "a\0@538",     0),

    TEST ("a<U0>@3b",           10,   "a<U0>@3b\0@5", 0),
    TEST ("ab<U0>@3",           10,   "ab<U0>@3\0@5", 0),
    TEST ("<U0>@3ab",           10,   "<U0>@3ab\0@5", 0),
    TEST ("a<U0>@3b",            7,   "a<U0>@3b\0@2", 0),
    TEST ("ab<U0>@3",            7,   "ab<U0>@3\0@2", 0),
    TEST ("<U0>@3ba",            7,   "<U0>@3ba\0@2", 0),

    TEST ("a<U0>b<U0>c<U0>@2",   5,   "a<U0>b<U0>c", 0),
    TEST ("a<U0>b<U0>c<U0>@2",   4,   "a<U0>b<U0>",  0),
    TEST ("<U0>ba<U0>c<U0>@2",   1,   "<U0>",        0),
    TEST ("<U0>ba<U0>c<U0>@2",   0,   "",            0),
#if TEST_RW_EXTENSIONS  // these tests run too slowly
    TEST ("x@540",             127,   "x@127",       0),
    TEST ("x@873",             127,   "x@127",       0),
    TEST ("x@1412",            127,   "x@127",       0),
    TEST ("x@2284",            127,   "x@127",       0),

    TEST ("x@127",             128,   "x@127\0",     0),
    TEST ("x@128",             207,   "x@128\0@79",  0),
    TEST ("x@207",             334,   "x@207\0@127", 0),
    TEST ("x@334",             540,   "x@334\0@206", 0),
    TEST ("x@540",             873,   "x@540\0@333", 0),
    TEST ("x@873",            1412,   "x@873\0@539", 0),
    TEST ("x@1412",           2284,   "x@1412\0@872", 0),
    TEST ("x@2284",           3695,   "x@2284\0@1411", 0),
#else
    TEST ("x@63",              63,    "x@63",            0),
    TEST ("x@64",              63,    "x@63",            0),
    TEST ("x@80",              63,    "x@63",            0),

    TEST ("x@63",              64,    "x@63\0",            0),
    TEST ("x@63",              80,    "x@63\0@17",         0),

#endif
    TEST ("last",                4,   "last",        0)
};

/**************************************************************************/

// used to exercise
// clear ()
static const ContainerTestCase
clear_void_test_cases [] = {

#undef TEST
#define TEST(str) {                         \
        __LINE__, -1, -1, -1, -1, -1,       \
        str, sizeof str - 1, 0, 0,          \
        "", 0, -1                           \
    }

    //    +------------------------------ controlled sequence
    //    |                
    //    |                               
    //    |                               
    //    V  
    TEST (0), 
    TEST (""),   

    TEST ("<U0>"),    
    TEST ("a"),       

    TEST ("test string"), 
    TEST ("a<U0>b<U0>c<U0>@2"),  
#if TEST_RW_EXTENSIONS
    TEST ("x@128"),     
    TEST ("x@207"),    
    TEST ("x@334"),
    TEST ("x@540"),
    TEST ("x@873"),
    TEST ("x@1412"),
    TEST ("x@2284"),   
    TEST ("x@3695"), 
    TEST ("x@4096"), 
#else
    TEST ("x@20"),
    TEST ("x@32"),
    TEST ("x@63"),
    TEST ("x@64"),
    TEST ("x@65"),
    TEST ("x@127"),
    TEST ("x@128"),
    TEST ("x@129"),
#endif

    TEST ("last") 
};

/**************************************************************************/

// used to exercise
// empty ()
static const ContainerTestCase
empty_void_test_cases [] = {

#undef TEST
#define TEST(str, res) {                 \
    __LINE__, -1, -1, -1, -1, -1,    \
    str, sizeof str - 1, 0, 0,       \
    0, res, -1                       \
}

    //    +------------------------------ controlled sequence
    //    |              +--------------- expected result              
    //    |              |                     
    //    |              |                     
    //    V              V
    TEST (0,             1), 
    TEST ("",            1),   

    TEST ("<U0>",        0),    
    TEST ("a",           0),       
    TEST ("<U0>ab<U0>c", 0),  

    TEST ("x@128",       0),     
#if TEST_RW_EXTENSIONS  // runs slowly and doesn't add any value
    TEST ("x@3695",      0), 
#endif
    TEST ("last",        0) 
};

/**************************************************************************/

template <class T, class Allocator>
void test_capacity (T*, Allocator*,
                    const ContainerTestCaseData<T> &tdata)
{
    typedef std::list<T, Allocator>       List;
    typedef typename List::iterator       ListIter;
    typedef typename List::const_iterator ListCIter;
    typedef ListState<List>               ListState;

    const ContainerFunc     &func  = tdata.func_;
    const ContainerTestCase &tcase = tdata.tcase_;

    // construct the list object to be modified and constant one
          List  lst (tdata.str_, tdata.str_ + tdata.strlen_, Allocator ());
    const List clst (tdata.str_, tdata.str_ + tdata.strlen_, Allocator ());

    // save the state of the list object before the call
    // to detect exception safety violations (changes to
    // the state of the object after an exception)
    const ListState lst_state (lst);

    const T arg_val = T::from_char (-1 != tcase.val ? char (tcase.val) : char ());

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

        if (tcase.bthrow) {
            return;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        const bool is_class = ListIds::UserClass == func.elem_id_;

        const std::size_t x_count_save = UserClass::count_;

        // start checking for memory leaks
        rw_check_leaks (lst.get_allocator ());

        std::size_t res = 0;

        try {

            // reset function call counters
            if (is_class)
                UserClass::reset_totals ();

            switch (func.which_) {

            case Size (void):
                res = clst.size ();
                break;

            case MaxSize (void):
                res = clst.max_size ();
                break;

            case Resize (size_val):
                 lst.resize (tcase.size, arg_val);
                break;

            case Resize (size):
                lst.resize (tcase.size);
                break;

            case Clear (void):
                lst.clear ();
                break;

            case Empty (void):
                res = clst.empty () ? 1 : 0;
                break;

            default:
                RW_ASSERT (!"test logic error: unknown capacity overload");
            }

            if (is_class
              &&
                (func.which_ == ListIds::clear_void
              || func.which_ == ListIds::resize_size
              || func.which_ == ListIds::resize_size_val)) {

                  std::size_t nctors = tdata.reslen_ > tdata.strlen_ ? 
                      tdata.reslen_ - tdata.strlen_ : 0;

                  std::size_t ndtors = tdata.reslen_ < tdata.strlen_ ? 
                      tdata.strlen_ - tdata.reslen_ : 0;

                  std::size_t new_count = x_count_save + nctors - ndtors;

                  std::size_t ndefctors = 0;

                  if (func.which_ == ListIds::resize_size_val)
                      ++nctors;
                  else if (func.which_ == ListIds::resize_size)
                      ++ndefctors;

                  bool success = UserClass::is_total (new_count, ndefctors,
                                                      nctors, 0, 0, 0);

#if DRQS  // Inconsistent count of copy constructor because strong exception
          // guarantee is not satisfied.
                  rw_assert (success, 0, tcase.line,
                             "line %d. %{$FUNCALL} called default/copy ctor "
                             "and operator=() %zu, %zu, and %zu times, "
                             "respectively, %zu, %zu, 0 expected",
                             __LINE__,
                             UserClass::n_total_def_ctor_,
                             UserClass::n_total_copy_ctor_,
                             UserClass::n_total_op_assign_,
                             ndefctors, nctors);
#endif
            }

            // verify the returned value
            if (func.which_ == ListIds::size_void 
             || func.which_ == ListIds::empty_void) {
                rw_assert (res == tcase.nres, 0, tcase.line,
                           "line %d. %{$FUNCALL} expected %zu, got %zu",
                           __LINE__, tcase.nres, res);
            }

            if (func.which_ == ListIds::max_size_void) {
                std::size_t cur_sz = clst.size ();

                rw_assert (cur_sz <= res, 0, tcase.line,
                           "line %d. %{$FUNCALL} == %zu, expected res > %zu",
                           __LINE__, res, cur_sz);
            }

            if (func.which_ == ListIds::resize_size_val 
             || func.which_ == ListIds::resize_size
             || func.which_ == ListIds::clear_void) {

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
                           "line %d. %{$FUNCALL} expected \"%{X=*}\" "
                           "with length %zu, got %{/*.*Gs} with length %zu",
                           __LINE__, cwidth, int (tdata.reslen_),
                           tdata.res_, tdata.reslen_,
                           1, int (got_size), got, got_size);

                if (tdata.reslen_ == got_size) {
                    // if the result length matches the expected length
                    // (and only then), also verify that the modified
                    // list matches the expected result
                    const std::size_t match = rw_match (tcase.res,
                                                        got, got_size);

                    rw_assert (match == tdata.reslen_, 0, tcase.line,
                               "line %d. %{$FUNCALL} expected \"%{X=*}\", "
                               "got %{/*.*Gs}, difference at offset %zu",
                               __LINE__,
                               cwidth, int (tdata.reslen_), tdata.res_,
                               1, int (got_size), got, match);
                }

                delete [] got;
            }
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
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object

#if DRQS  // 'list.resize' uses multi-element insert, which requires strong
          // exception guarantees.  Current implementation of list is not
          // doing this.
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

        break;
    }

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_capacity);

int main (int argc, char** argv)
{
    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(gsig, sig) {                                   \
    gsig, sig ## _test_cases,                               \
    sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
}

        TEST (ListIds::size_void, size_void),
        TEST (ListIds::max_size_void, max_size_void),
        TEST (ListIds::resize_size_val, resize_size_val),
        TEST (ListIds::resize_size, resize_size),
        TEST (ListIds::clear_void, clear_void),
        TEST (ListIds::empty_void, empty_void)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_cont_test (argc, argv, __FILE__,
                             "lib.list.capacity",
                             ContainerIds::List,
                             test_capacity_func_array, tests, test_count);
}
