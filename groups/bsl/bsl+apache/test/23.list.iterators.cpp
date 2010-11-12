/***************************************************************************
 *
 * 23.list.iterators.cpp - test exercising [lib.list.iterators]
 *
 * $Id: 23.list.iterators.cpp 523692 2007-03-29 13:01:55Z faridz $
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
#include <stdexcept>        // for out_of_range, length_error
#include <cstddef>          // for size_t

#include <23.list.h>        // for ListMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// begin () and rend () and front ()
static const ContainerTestCase 
begin_void_test_cases [] = {

#undef TEST
#define TEST(lst, res) {                    \
    __LINE__, -1, -1, -1, -1, -1,           \
    lst, sizeof (lst) - 1,                  \
    0, 0, 0, res, 0                         \
}

    //    +--------------------------------- controlled sequence
    //    |                +---------------- expected result 
    //    |                |     
    //    V                V     
    TEST ("",              _RWSTD_SIZE_MAX), 
    TEST ("a",             'a' ), 
    TEST ("<U0>",          '\0'),
    TEST ("abc",           'a' ),  
    TEST ("<U0>ab<U0>@2c", '\0'), 
    TEST ("a<U0>b<U0>@2c", 'a' ),  
    TEST ("a<U0>bc<U0>@2", 'a' ), 
#if TEST_RW_EXTENSIONS  // test runs slow
    TEST ("x@4096",        'x' ),  
#else
    TEST ("x@64",          'x' ),  
#endif
    TEST ("last",          'l' )  
};

/**************************************************************************/

// used to exercise
// end () and rbegin () and back ()
static const ContainerTestCase 
end_void_test_cases [] = {

#undef TEST
#define TEST(lst, res) {                    \
    __LINE__, -1, -1, -1, -1, -1,           \
    lst, sizeof (lst) - 1,                  \
    0, 0, 0, res, 0                         \
}

    //    +--------------------------------- controlled sequence
    //    |                +---------------- expected result 
    //    |                |     
    //    V                V     
    TEST ("",              _RWSTD_SIZE_MAX), 
    TEST ("a",             'a' ), 
    TEST ("<U0>",          '\0'),
    TEST ("abc",           'c' ),  
    TEST ("<U0>ab<U0>@2c", 'c' ), 
    TEST ("a<U0>b<U0>@2c", 'c' ),  
    TEST ("a<U0>bc<U0>@2", '\0'), 
#if TEST_RW_EXTENSIONS  // test runs slow
    TEST ("x@4096",        'x' ),  
#else
    TEST ("x@64",          'x' ),  
#endif
    TEST ("last",          't' )  
};

/**************************************************************************/

// exercises:
// get_allocator ()
static const ContainerTestCase
get_allocator_void_test_cases [] = {

#undef TEST
#define TEST(lst) {                   \
    __LINE__, -1, -1, -1, -1, -1,     \
    lst, sizeof (lst) - 1,            \
    0, 0, 0, 0, 0                     \
}

    //    +------------------------------------------ controlled sequence
    //    | 
    //    | 
    //    |    
    //    |    
    //    | 
    //    | 
    //    | 
    //    V 
    TEST (""              ),
    TEST ("ab"            ),
    TEST ("<U0>"          ),
#if TEST_RW_EXTENSIONS
    TEST ("x@4096"        ),
    TEST ("a@2048cb@2047" ),
#else
    TEST ("a@64cb@64"     ),
    TEST ("x@64"          ),
#endif
    TEST ("a<U0>b"        ),
    TEST ("last"          )
};


/**************************************************************************/

template <class T, class Allocator>
void test_iterators (T*, Allocator*,
                     const ContainerTestCaseData<T> &tdata)
{
    typedef std::list <T, Allocator>                   List;
    typedef typename List::iterator                    ListIter;
    typedef typename List::const_iterator              ListCIter;
    typedef typename List::reverse_iterator            ListRIter;
    typedef typename List::const_reverse_iterator      ListCRIter;
    typedef ListState<List>                            ListState;

    const ContainerFunc     &func  = tdata.func_;
    const ContainerTestCase &tcase = tdata.tcase_;

    const bool test_iters =
           func.which_ == ListIds::begin_void 
        || func.which_ == ListIds::begin_const_void 
        || func.which_ == ListIds::end_void
        || func.which_ == ListIds::end_const_void
        || func.which_ == ListIds::rbegin_void
        || func.which_ == ListIds::rbegin_const_void
        || func.which_ == ListIds::rend_void
        || func.which_ == ListIds::rend_const_void
        || func.which_ == ListIds::front_void
        || func.which_ == ListIds::front_const_void
        || func.which_ == ListIds::back_void
        || func.which_ == ListIds::back_const_void;

    const bool test_const_list =
           func.which_ == ListIds::begin_const_void 
        || func.which_ == ListIds::end_const_void
        || func.which_ == ListIds::rbegin_const_void
        || func.which_ == ListIds::rend_const_void
        || func.which_ == ListIds::front_const_void
        || func.which_ == ListIds::back_const_void;

    const bool test_end_iters =
           func.which_ == ListIds::end_void 
        || func.which_ == ListIds::end_const_void
        || func.which_ == ListIds::rend_void
        || func.which_ == ListIds::rend_const_void;

    // allocator object for test get_allocator
    Allocator alloc;

    // construct the list object 
    List lst (tdata.str_, tdata.str_ + tdata.strlen_, alloc);
    // construct the constant list object
    const List clst (tdata.str_, tdata.str_ + tdata.strlen_, alloc);

    const std::size_t s_size = test_const_list ? clst.size () : lst.size ();

    // save the state of the list object before the call
    // to detect exception safety violations (changes to
    // the state of the object after an exception)
    const ListState lst_state (lst);

    // start checking for memory leaks
    rw_check_leaks (lst.get_allocator ());

    try {

        // methods result
        const T* res_ptr = 0;

        // for begin () and end ()
        ListIter it;
        // for begin () const and end () const
        ListCIter cit;
        // for rbegin () and rend ()
        ListRIter rit;
        // for rbegin () const and rend () const
        ListCRIter crit;

        // for get_allocator ()
        Allocator resalloc;

        bool it_relations = true;

        switch (func.which_) {

        case ListIds::begin_void:
            it = lst.begin();
            it_relations = it == lst.rend ().base ();
            if (s_size) res_ptr = &*it;
            break;

        case ListIds::begin_const_void:
            cit = clst.begin();
            it_relations = cit == clst.rend ().base ();
            if (s_size) res_ptr = &*cit;
            break;

        case ListIds::end_void:
            it = lst.end();
            it_relations = it == lst.rbegin ().base ();
            if (s_size) res_ptr = &*--it;
            break;

        case ListIds::end_const_void:
            cit = clst.end();
            it_relations = cit == clst.rbegin ().base ();
            if (s_size) res_ptr = &*--cit;
            break;

        case ListIds::rbegin_void:
            rit = lst.rbegin();
            it_relations = rit.base () == lst.end ();
            if (s_size) res_ptr = &*rit;
            break;

        case ListIds::rbegin_const_void:
            crit = clst.rbegin();
            it_relations = crit.base () == clst.end ();
            if (s_size) res_ptr = &*crit;
            break;

        case ListIds::rend_void:
            rit = lst.rend();
            it_relations = rit.base () == lst.begin ();
            if (s_size) res_ptr = &*--rit;
            break;

        case ListIds::rend_const_void:
            crit = clst.rend();
            it_relations = crit.base () == clst.begin ();
            if (s_size) res_ptr = &*--crit;
            break;

        case ListIds::front_void:
            if (s_size) {
                res_ptr = &lst.front ();
                it_relations = res_ptr == &*lst.begin ();
            }
            break;

        case ListIds::front_const_void:
            if (s_size) {
                res_ptr = &clst.front ();
                it_relations = res_ptr == &*clst.begin ();
            }
            break;

        case ListIds::back_void:
            if (s_size) {
                res_ptr = &lst.back ();
                it_relations = res_ptr == &*--lst.end ();
            }
            break;

        case ListIds::back_const_void:
            if (s_size) {
                res_ptr = &clst.back ();
                it_relations = res_ptr == &*--clst.end ();
            }
            break;

        case ListIds::get_allocator_void:
#if TEST_RW_PEDANTIC || DRQS // std::allocator is assignable in standard
	    resalloc = clst.get_allocator ();
#else
	    // Hack to make any allocator assignable
	    resalloc.~Allocator();
	    new ((void*) &resalloc) Allocator(clst.get_allocator ());
#endif
            break;

        default:
            RW_ASSERT (!"test logic error: unknown iterators overload");
            return;
        }

        if (res_ptr) {

            const char exp_res = 
                (_RWSTD_SIZE_MAX != tcase.nres ? char (tcase.nres) : char ());

            const bool success = 0 == T::mismatch (res_ptr, &exp_res, 1);

            rw_assert (success, 0, tcase.line,
                       "line %d. %{$FUNCALL}%{?} - 1%{;} expected "
                       "%{#c}, got %{#c}", __LINE__, 
                       test_end_iters, exp_res, char (res_ptr->data_.val_));
        }

        if (test_iters) {

            if (0 == s_size) {

                bool success = true;

                if (   func.which_ == ListIds::begin_void 
                    || func.which_ == ListIds::end_void) 
                    success = it == lst.begin () && it == lst.end ();

                if (   func.which_ == ListIds::begin_const_void 
                    || func.which_ == ListIds::end_const_void) 
                    success = cit == clst.begin () && cit == clst.end ();

                if (   func.which_ == ListIds::rbegin_void 
                    || func.which_ == ListIds::rend_void) 
                    success = rit == lst.rbegin () && rit == lst.rend ();

                if (   func.which_ == ListIds::rbegin_const_void 
                    || func.which_ == ListIds::rend_const_void) 
                    success = crit == clst.rbegin () && crit == clst.rend ();

                // check the begin () == end (), rbegin () == rend ()
                rw_assert(success, 0, tcase.line,
                          "line %d. %{$FUNCALL} returned iterator is not "
                          "equal to begin and end for an empty list", 
                          __LINE__);
            }

            // check the iterators relationship
            rw_assert(it_relations, 0, tcase.line,
                      "line %d. %{$FUNCALL} iterators "
                      "relationship is broken", __LINE__);
        } 

        if (func.which_ == ListIds::get_allocator_void) {

            // verify that the allocator returned from the function
            // equal to allocator passed to the ctor
            const bool success = (alloc == resalloc);

            rw_assert(success, 0, tcase.line,
                      "line %d. %{$FUNCALL} expected equal to allocator "
                      "passed to the ctor", __LINE__);
        }
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::bad_alloc &ex) {
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} unexpectedly caught std::%s(%#s)",
                   __LINE__, exceptions [1], ex.what ());
    }
    catch (const std::exception &ex) {
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} unexpectedly caught std::%s(%#s)",
                   __LINE__, exceptions [2], ex.what ());
    }
    catch (...) {
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} unexpectedly caught %s",
                   __LINE__, exceptions [0]);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // FIXME: verify the number of blocks the function call
    // is expected to allocate and detect any memory leaks
    rw_check_leaks (lst.get_allocator (), tcase.line,
        std::size_t (-1), std::size_t (-1));

    // verify that state of the object didn't changed
    lst_state.assert_equal (ListState (lst),
        __LINE__, tcase.line, "call");
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_iterators);

int main (int argc, char** argv)
{
    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(gsig, sig) {                                       \
    gsig, sig ## _test_cases,                                   \
    sizeof sig ## _test_cases / sizeof *sig ## _test_cases      \
}

        TEST (ListIds::begin_void, begin_void),
        TEST (ListIds::begin_const_void, begin_void),
        TEST (ListIds::end_void, end_void),
        TEST (ListIds::end_const_void, end_void),
        TEST (ListIds::rbegin_void, end_void),
        TEST (ListIds::rbegin_const_void, end_void),
        TEST (ListIds::rend_void, begin_void),
        TEST (ListIds::rend_const_void, begin_void),
        TEST (ListIds::front_void, begin_void),
        TEST (ListIds::front_const_void, begin_void),
        TEST (ListIds::back_void, end_void),
        TEST (ListIds::back_const_void, end_void),
        TEST (ListIds::get_allocator_void, get_allocator_void)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_cont_test (argc, argv, __FILE__,
                             "lib.list.iterators", ContainerIds::List,
                             test_iterators_func_array, tests, test_count);
}
