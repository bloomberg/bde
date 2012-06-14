/***************************************************************************
 *
 * 23.list.special.cpp - test exercising [lib.list.special]
 *
 * $Id: 23.list.special.cpp 522951 2007-03-27 15:16:23Z faridz $
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
#include <cstddef>          // size_t

#include <23.list.h>        // for ListMembers
#include <driver.h>         // for rw_assert()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_new.h>         // for bad_alloc, replacement operator new

/**************************************************************************/

// for convenience and brevity
#define Swap(sig)   ListIds::swap_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception"
};

/**************************************************************************/

// exercises:
// swap (list&)
static const ContainerTestCase
cont_cont_test_cases [] = {

#undef TEST
#define TEST(str, arg)                                \
    { __LINE__, -1, -1, -1, -1, -1,                   \
    str, sizeof str - 1, arg, sizeof arg - 1,         \
    0, 0, 0                                           \
    }

    //    +------------------------- controlled "destination" sequence
    //    |                +-------- controlled "source" sequence
    //    |                |
    //    V                V
    TEST ("",              ""),
};

// exercises:
// swap (list&)
static const ContainerTestCase
cont_test_cases [] = {

    //    +------------------------- controlled "destination" sequence
    //    |                +-------- controlled "source" sequence
    //    |                |
    //    V                V
    TEST ("",              ""),
    TEST ("",              "a"),
    TEST ("a",             ""),
    TEST ("",              "<U0>"),
    TEST ("<U0>",          ""),

    TEST ("a",             "b"),
    TEST ("a",             "bc"),
    TEST ("ab",            "c"),

    TEST ("a<U0>b<U0>@2c", "<U0>b<U0>@2c"),
    TEST ("<U0>b<U0>@2c",  "a<U0>b<U0>@2c"),

    TEST ("a<U0>b<U0>@2c", "<U0>@2"),
    TEST ("<U0>@2",        "a<U0>b<U0>@2c"),

#if TEST_RW_EXTENSIONS  // these tests run too slowly and doesn't add value
    TEST ("x@4096",        ""),
    TEST ("",              "x@4096"),
    TEST ("x@4096",        "<U0>@3"),
    TEST ("<U0>@3",        "x@4096"),
    TEST ("x@4096",        "x@4096"),

    TEST ("",              "x@128"),
    TEST ("x@207",         "x@128"),
    TEST ("x@128",         "x@334"),
    TEST ("x@873",         "x@334"),
    TEST ("x@1412",        "x@540"),
    TEST ("x@540",         "x@2284"),
    TEST ("x@3695",        "x@2284"),
    TEST ("x@3695",        "x@128"),
#else
    TEST ("x@64",          ""),
    TEST ("",              "x@64"),
    TEST ("x@64",          "<U0>@3"),
    TEST ("<U0>@3",        "x@64"),
    TEST ("x@64",          "x@64"),

    TEST ("x@64",          "x@128"),
    TEST ("x@128",         "x@64"),
#endif

    TEST ("",              0),
    TEST ("<U0>",          0),
    TEST ("abc",           0),
    TEST ("a<U0>b<U0>@2c", 0),

#if TEST_RW_EXTENSIONS  // these tests run too slowly and doesn't add value
    TEST ("x@4096",        0),
    TEST (0,               ""),
    TEST (0,               "<U0>"),
    TEST (0,               "abc@1024"),
    TEST (0,               "a<U0>b<U0>@2c"),
    TEST (0,               "x@4096"),
#else
    TEST ("x@64",          0),
    TEST (0,               ""),
    TEST (0,               "<U0>"),
    TEST (0,               "abc@32"),
    TEST (0,               "a<U0>b<U0>@2c"),
    TEST (0,               "x@64"),
#endif

    TEST ("last",          "test")
};


/**************************************************************************/

struct ListValueType { };

typedef std::allocator<ListValueType>           ListAllocator;
typedef bsl::list<ListValueType, ListAllocator> ListType;

static int list_swap_called;

//_RWSTD_NAMESPACE (std) {
#undef std

namespace bsl {

    // define an explicit specialization of the list::swap() member
    // to verify tha the non-member swap function calls the member

#if !defined(BSLS_PLATFORM__CMP_CLANG)

_RWSTD_SPECIALIZED_FUNCTION
void ListType::swap (ListType&)
{
    ++list_swap_called;
}

#endif

}   // namespace std
#define std bsl
/**************************************************************************/

void test_std_swap ()
{
    static bool tested = false;

    if (tested)
        return;

    tested = true;

    rw_info (0, 0, 0,
             "Testing std::swap (std::list&, std::list&) "
             "calls std::list::swap");

    // verify the signature of the function specialization
#if DRQS
    // need to investigate
    void (*pswap)(ListType&, ListType&) =
        &std::swap<ListValueType, ListAllocator>;

    _RWSTD_UNUSED (pswap);
#endif
    // verify that std::swap() calls std::list::swap()
    ListType lst;

    std::swap (lst, lst);

    rw_assert (1 == list_swap_called, 0, __LINE__,
               "std::swap (std::list<T, A>&, std::list<T, A>&) called "
               "std::list<T, A>::swap (std::list<T, A>&) exactly once; "
               "got %d times", list_swap_called);
}

/**************************************************************************/

template <class T, class Allocator>
void test_swap (T*,
                Allocator                      &a1,
                Allocator                      &a2,
                const ContainerTestCaseData<T> &tdata)
{
    typedef std::list <T, Allocator> List;
    typedef ListState<List>          ListState;

    const ContainerTestCase &tcase = tdata.tcase_;
    const ContainerFunc     &func  = tdata.func_;

    // construct the list object to be modified
    // and the argument list
    List src_list (tdata.str_, tdata.str_ + tdata.strlen_, a1);
    List dst_list (tdata.arg_, tdata.arg_ + tdata.arglen_, a2);

    List& arg_list            = tcase.arg ? dst_list : src_list;

    // save the state of the list object before the call
    // to detect exception safety violations (changes to
    // the state of the object after an exception)
    const ListState src_state (src_list);
    const ListState arg_state (arg_list);

    rwt_free_store* const pst = rwt_get_free_store (0);
    SharedAlloc*    const pal = SharedAlloc::instance ();

    // iterate for`throw_count' starting at the next call to operator new,
    // forcing each call to throw an exception, until the function finally
    // succeeds (i.e, no exception is thrown)
    std::size_t throw_count;
    for (throw_count = 0; ; ++throw_count) {
        const char* expected = 0;
        const char* caught = 0;

#ifndef _RWSTD_NO_EXCEPTIONS

        // no exceptions expected
        if (0 == tcase.bthrow && a1 == a2) {
            // by default exercise the exception safety of the function
            // by iteratively inducing an exception at each call to operator
            // new or Allocator::allocate() until the call succeeds
            expected = exceptions [1];      // bad_alloc
            *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_count + 1;
            pal->throw_at_calls_ [pal->m_allocate] =
                pal->throw_at_calls_ [pal->m_allocate] + throw_count + 1;
        }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

        if (tcase.bthrow)
            return;

#endif   // _RWSTD_NO_EXCEPTIONS

        // start checking for memory leaks
        rw_check_leaks (src_list.get_allocator ());
        rw_check_leaks (arg_list.get_allocator ());

        try {
            const bool is_class = ListIds::UserClass == func.elem_id_;

            // reset function call counters
            if (is_class)
                UserClass::reset_totals ();

            src_list.swap (arg_list);

            if (is_class && a1 == a2) {

                bool success =  0 == (UserClass::n_total_def_ctor_
                                    | UserClass::n_total_copy_ctor_
                                    | UserClass::n_total_op_assign_);

                rw_assert (success, 0, tcase.line,
                           "line %d. %{$FUNCALL}: complexity: %zu def ctors, "
                           "%zu copy ctors, %zu assigns", __LINE__,
                           UserClass::n_total_def_ctor_,
                           UserClass::n_total_copy_ctor_,
                           UserClass::n_total_op_assign_);
            }

            if (0 == tcase.str) {

                rw_assert (0 == arg_list.size (), 0, tcase.line,
                           "line %d. %{$FUNCALL}: expected 0 size, "
                           "got %zu",  __LINE__, arg_list.size ());
            }

            if (a1 == a2) {

                src_state.assert_equal (ListState (arg_list),
                    __LINE__, tcase.line, "swap");

                arg_state.assert_equal (ListState (src_list),
                    __LINE__, tcase.line, "swap");
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
        rw_check_leaks (src_list.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        rw_check_leaks (arg_list.get_allocator (), tcase.line,
                        std::size_t (-1), std::size_t (-1));

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object
            src_state.assert_equal (ListState (src_list),
                                    __LINE__, tcase.line, caught);

            arg_state.assert_equal (ListState (arg_list),
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

    // no exception expected
    const std::size_t expect_throws = 0;
#if TEST_RW_EXTENSIONS  // need to make copy (which throws exceptions) if
                        // allocators are different

    rw_assert (expect_throws == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected exactly 0 %s exception "
               "while the swap, got %zu",
               __LINE__, exceptions [1], throw_count);
#else
    if (a1 == a2) {
        rw_assert (expect_throws == throw_count, 0, tcase.line,
                   "line %d: %{$FUNCALL}: expected exactly 0 %s exception "
                   "while the swap, got %zu",
                   __LINE__, exceptions [1], throw_count);
    }
#endif
    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

template <class T>
std::allocator<T>
make_alloc (SharedAlloc&, std::allocator<T>*) {
    return std::allocator<T>();
}

template <class T, class Types>
UserAlloc<T, Types>
make_alloc (SharedAlloc &shal, UserAlloc<T, Types>*) {
    return UserAlloc<T, Types>(&shal);
}

/**************************************************************************/

template <class T, class Allocator>
void test_swap (T*, Allocator*,
                const ContainerTestCaseData<T> &tdata)
{
    if (Swap (cont_cont) == tdata.func_.which_) {

        test_std_swap ();

        return;
    }

    SharedAlloc sa1;
    Allocator a1 = make_alloc(sa1, (Allocator*)0);

    // test swap using the same allocator objects
    test_swap ((T*)0, a1, a1, tdata);

    SharedAlloc sa2;
    Allocator a2 = make_alloc(sa2, (Allocator*)0);

    if (a1 != a2) {
        // test swap using different allocator objects
        test_swap ((T*)0, a1, a2, tdata);
    }
}

/**************************************************************************/

DEFINE_CONTAINER_TEST_FUNCTIONS (test_swap);

int main (int argc, char** argv)
{
    static const ContainerTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        Swap (sig), sig ## _test_cases,                         \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases, \
    }

        TEST (cont),
        TEST (cont_cont)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    const int status =
        rw_run_cont_test (argc, argv, __FILE__,
                          "lib.list.special",
                          ContainerIds::List,
                          test_swap_func_array, tests, test_count);

    return status;
}
