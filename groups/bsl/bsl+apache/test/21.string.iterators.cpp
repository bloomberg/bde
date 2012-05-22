/***************************************************************************
 *
 * 21.string.iterators.cpp - string test exercising lib.string.iterators
 *
 * $Id: 21.string.iterators.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <string>         // for string
#include <stdexcept>      // for out_of_range, length_error
#include <cstddef>        // for size_t

#include <21.strings.h>   // for StringMembers
#include <driver.h>       // for rw_test()
#include <rw_allocator.h> // for UserAlloc
#include <rw_char.h>      // for rw_expand()
#include <rw_new.h>       // for bad_alloc, replacement operator new

/**************************************************************************/

static const char* const exceptions[] = {
    "unknown exception", "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise
// begin () and rend ()
static const StringTestCase
begin_void_test_cases [] = {

#undef TEST
#define TEST(str, res) {                    \
    __LINE__, -1, -1, -1, -1, -1,           \
    str, sizeof (str) - 1,                  \
    0, 0, 0, res, 0                         \
}

    //    +--------------------------------- controlled sequence
    //    |                 +--------------- expected result
    //    |                 |
    //    V                 V
    TEST ("a",             'a' ),
    TEST ("<U0>",          '\0'),
    TEST ("abc",           'a' ),
    TEST ("<U0>ab<U0>@2c", '\0'),
    TEST ("a<U0>b<U0>@2c", 'a' ),
    TEST ("a<U0>bc<U0>@2", 'a' ),
    TEST ("x@4096",        'x' ),
    TEST ("last",          'l' )
};

/**************************************************************************/

// used to exercise
// end () and rbegin ()
static const StringTestCase
end_void_test_cases [] = {

#undef TEST
#define TEST(str, res) {                    \
    __LINE__, -1, -1, -1, -1, -1,           \
    str, sizeof (str) - 1,                  \
    0, 0, 0, res, 0                         \
}

    //    +--------------------------------- controlled sequence
    //    |                 +--------------- expected result
    //    |                 |
    //    V                 V
    TEST ("a",             'a' ),
    TEST ("<U0>",          '\0'),
    TEST ("abc",           'c' ),
    TEST ("<U0>ab<U0>@2c", 'c' ),
    TEST ("a<U0>b<U0>@2c", 'c' ),
    TEST ("a<U0>bc<U0>@2", '\0'),
    TEST ("x@4096",        'x' ),
    TEST ("last",          't' )
};

/**************************************************************************/

// exercises:
// c_str() and data ()
static const StringTestCase
c_str_void_test_cases [] = {

#undef TEST
#define TEST(str) {                   \
    __LINE__, -1, -1, -1, -1, -1,     \
    str, sizeof (str) - 1, 0, 0,      \
    str, sizeof (str) - 1, 0          \
}

    //    +-------------------------------- controlled sequence
    //    |
    //    |
    //    |
    //    |
    //    |
    //    |
    //    |
    //    V
    TEST ("ab"              ),

    TEST (""                ),

    TEST ("<U0>"              ),

    TEST ("abcdefghij"      ),
    TEST ("abcdefghi"       ),

    TEST ("abbdefghij"      ),
    TEST ("abcdeeghij"      ),
    TEST ("abcdefghii"      ),

    TEST ("bbcdefghij"      ),
    TEST ("eeeeeeghij"      ),
    TEST ("a"               ),
    TEST ("aeeee"           ),

    TEST ("bbcdefghij"      ),
    TEST ("abcdffghij"      ),
    TEST ("abcdefghjj"      ),

    TEST ("a<U0>b<U0>@2c"   ),
    TEST ("abc<U0>@3"       ),
    TEST ("<U0>ab<U0>@2c"   ),

    TEST ("a<U0>b"          ),
    TEST ("ab<U0>"          ),
    TEST ("<U0>ab"          ),

    TEST ("x@4096"          ),
    TEST ("xx"              ),

    TEST ("a@2048b@2048"    ),
    TEST ("a@2048cb@2047"   ),

    TEST ("last"            )
};

/**************************************************************************/

// exercises:
// get_allocator ()
static const StringTestCase
get_allocator_void_test_cases [] = {

#undef TEST
#define TEST(str) {                   \
    __LINE__, -1, -1, -1, -1, -1,     \
    str, sizeof (str) - 1,            \
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
    TEST ("ab"            ),
    TEST (""              ),
    TEST ("<U0>"          ),
    TEST ("x@4096"        ),
    TEST ("a<U0>b"        ),
    TEST ("a@2048cb@2047" ),
    TEST ("last"          )
};


/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_iterators (charT*, Traits*, Allocator*,
                     const StringTestCaseData<charT> &tdata)
{
    typedef std::basic_string <charT, Traits, Allocator> String;
    typedef typename String::iterator                    StringIter;
    typedef typename String::const_iterator              StringCIter;
    typedef typename String::reverse_iterator            StringRIter;
    typedef typename String::const_reverse_iterator      StringCRIter;

    const StringFunc     &func  = tdata.func_;
    const StringTestCase &tcase = tdata.tcase_;

    const bool test_iters = func.which_ == StringIds::begin_void
        || func.which_ == StringIds::begin_const_void
        || func.which_ == StringIds::end_void
        || func.which_ == StringIds::end_const_void
        || func.which_ == StringIds::rbegin_void
        || func.which_ == StringIds::rbegin_const_void
        || func.which_ == StringIds::rend_void
        || func.which_ == StringIds::rend_const_void;

    const bool test_const_iters = func.which_ == StringIds::begin_const_void
        || func.which_ == StringIds::end_const_void
        || func.which_ == StringIds::rbegin_const_void
        || func.which_ == StringIds::rend_const_void;

    const bool test_end_iters = func.which_ == StringIds::end_void
        || func.which_ == StringIds::end_const_void
        || func.which_ == StringIds::rend_void
        || func.which_ == StringIds::rend_const_void;

    // allocator object for test get_allocator
    Allocator alloc;

    // construct the string object
    String str (tdata.str_, tdata.strlen_, alloc);
    // construct the constant string object
    const String cstr (tdata.str_, tdata.strlen_, alloc);

    const std::size_t s_size = test_const_iters ? cstr.size () : str.size ();

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (str));

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

        // by default excercise the exception safety of the function
        // by iteratively inducing an exception at each call to operator
        // new or Allocator::allocate() until the call succeeds
        expected = exceptions [1];      // bad_alloc
        *pst->throw_at_calls_ [0] = pst->new_calls_ [0] + throw_count + 1;
        pal->throw_at_calls_ [pal->m_allocate] =
            pal->throw_at_calls_ [pal->m_allocate] + throw_count + 1;

#endif   // _RWSTD_NO_EXCEPTIONS

        // start checking for memory leaks
        rw_check_leaks (str.get_allocator ());

        // for data () and c_str ()
        const charT* ret_ptr = 0;

        // methods result
        charT res;

        // for begin () and end ()
        StringIter it;
        // for begin () const and end () const
        StringCIter cit;
        // for rbegin () and rend ()
        StringRIter rit;
        // for rbegin () const and rend () const
        StringCRIter crit;

        // for get_allocator ()
        Allocator resalloc;

        try {

            bool it_relations = true;

            switch (func.which_) {

            case StringIds::begin_void:
                it = str.begin();
                res = s_size ? *it : charT ();
                it_relations = it == str.rend ().base ();
                break;

            case StringIds::begin_const_void:
                cit = cstr.begin();
                res = s_size ? *cit : charT ();
                it_relations = cit == cstr.rend ().base ();
                break;

            case StringIds::end_void:
                it = str.end();
                res = s_size ? *(it - 1) : charT ();
                it_relations = it == str.rbegin ().base ();
                break;

            case StringIds::end_const_void:
                cit = cstr.end();
                res = s_size ? *(cit - 1) : charT ();
                it_relations = cit == cstr.rbegin ().base ();
                break;

            case StringIds::rbegin_void:
                rit = str.rbegin();
                res = s_size ? *rit : charT ();
                it_relations = rit.base () == str.end ();
                break;

            case StringIds::rbegin_const_void:
                crit = cstr.rbegin();
                res = s_size ? *crit : charT ();
                it_relations = crit.base () == cstr.end ();
                break;

            case StringIds::rend_void:
                rit = str.rend();
                res = s_size ? *(rit - 1) : charT ();
                it_relations = rit.base () == str.begin ();
                break;

            case StringIds::rend_const_void:
                crit = cstr.rend();
                res = s_size ? *(crit - 1) : charT ();
                it_relations = crit.base () == cstr.begin ();
                break;

            case StringIds::c_str_void:
                ret_ptr = cstr.c_str ();
                break;

            case StringIds::data_void:
                ret_ptr = cstr.data ();
                break;

            case StringIds::get_allocator_void:
#if TEST_RW_PEDANTIC || DRQS // std::allocator is assignable in standard
                resalloc = cstr.get_allocator ();
#else
        // Hack to make any allocator assignable
        resalloc.~Allocator();
        new ((void*) &resalloc) Allocator(cstr.get_allocator ());
#endif
                break;

            default:
                RW_ASSERT ("test logic error: unknown iterators overload");
                return;
            }

            // for convenience
            static const int cwidth = sizeof (charT);

            if (   func.which_ == StringIds::data_void
                || func.which_ == StringIds::c_str_void) {

                // 21.3.6
                // the member returns a non-null pointer
                rw_assert(0 != ret_ptr, 0, tcase.line,
                          "line %d. %{$FUNCALL} expected non null, got null",
                          __LINE__);

                if (ret_ptr) {
                    // 21.3.6.1
                    // if size() is nonzero, the member returns a pointer to the
                    // initial element of an array whose first size() elements
                    // equal the corresponding elements of the string controlled
                    // by *this
                    // 21.3.6.3
                    // if size() is nonzero, the member returns a pointer to the
                    // initial element of an array whose first size() elements
                    // equal the corresponding elements of the string controlled
                    // by *this
                    const std::size_t match =
                        rw_match (tcase.res, ret_ptr, tdata.reslen_);

                    rw_assert (match == tdata.reslen_, 0, tcase.line,
                               "line %d. %{$FUNCALL} expected %{#*s}, "
                               "got %{/*.*Gs}, differ at pos %zu",
                               __LINE__, int (tdata.reslen_), tdata.res_,
                               cwidth, int (s_size), ret_ptr, match);

                    if (func.which_ == StringIds::c_str_void) {

                        // check the last element is equal to char ()
                        const char null = char ();
                        const bool success =
                            (1 == rw_match (&null, &ret_ptr[s_size], 1));

                        rw_assert(success, 0, tcase.line,
                                  "line %d. %{$FUNCALL} expected last element "
                                  "is a null character %{#c}, got %{#c}",
                                  __LINE__, null, ret_ptr[s_size]);
                    }
                }
            }

            if (test_iters) {

                if (s_size) {

                    const char exp_res =
                        (NPOS != tcase.nres ? char (tcase.nres) : char ());

                    const bool success = (1 == rw_match (&exp_res, &res, 1));

                    rw_assert (success, 0, tcase.line,
                               "line %d. %{$FUNCALL}%{?} - 1%{;} expected "
                               "%{#c}, got %{#c}", __LINE__,
                               test_end_iters, exp_res, res);
                }
                else {
                    bool success = true;

                    if (   func.which_ == StringIds::begin_void
                        || func.which_ == StringIds::end_void)
                        success = it == str.begin () && it == str.end ();

                    if (   func.which_ == StringIds::begin_const_void
                        || func.which_ == StringIds::end_const_void)
                        success = cit == cstr.begin () && cit == cstr.end ();

                    if (   func.which_ == StringIds::rbegin_void
                        || func.which_ == StringIds::rend_void)
                        success = rit == str.rbegin () && rit == str.rend ();

                    if (   func.which_ == StringIds::rbegin_const_void
                        || func.which_ == StringIds::rend_const_void)
                        success =
                            crit == cstr.rbegin () && crit == cstr.rend ();

                    // check the begin () == end (), rbegin () == rend ()
                    rw_assert(success, 0, tcase.line,
                              "line %d. %{$FUNCALL} returned iterator is not "
                              "equal to begin and end for an empty string",
                              __LINE__);
                }

                // check the iterators relationship
                rw_assert(it_relations, 0, tcase.line,
                          "line %d. %{$FUNCALL} iterators "
                          "relationship is broken", __LINE__);
            }

            if (func.which_ == StringIds::get_allocator_void) {

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
        rw_check_leaks (str.get_allocator (), tcase.line,
            std::size_t (-1), std::size_t (-1));

        if (caught) {
            // verify that an exception thrown during allocation
            // didn't cause a change in the state of the object
            str_state.assert_equal (rw_get_string_state (str),
                __LINE__, tcase.line, caught);

            // allow this call to operator new to succeed and try
            // to make the next one to fail during the next call
            // to the same function again
            continue;
        }

        break;
    }

    rw_assert (0 == throw_count, 0, tcase.line,
               "line %d: %{$FUNCALL}: expected 0 %s exceptions, got %zu",
               __LINE__, exceptions [1], throw_count);

    // disable bad_alloc exceptions
    *pst->throw_at_calls_ [0] = 0;
    pal->throw_at_calls_ [pal->m_allocate] = 0;
}

/**************************************************************************/

DEFINE_STRING_TEST_FUNCTIONS (test_iterators);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(gsig, sig) {                                       \
    gsig, sig ## _test_cases,                                   \
    sizeof sig ## _test_cases / sizeof *sig ## _test_cases      \
}

        TEST (StringIds::begin_void, begin_void),
        TEST (StringIds::begin_const_void, begin_void),
        TEST (StringIds::end_void, end_void),
        TEST (StringIds::end_const_void, end_void),
        TEST (StringIds::rbegin_void, end_void),
        TEST (StringIds::rbegin_const_void, end_void),
        TEST (StringIds::rend_void, begin_void),
        TEST (StringIds::rend_const_void, begin_void),
        TEST (StringIds::c_str_void, c_str_void),
        TEST (StringIds::data_void, c_str_void),
        TEST (StringIds::get_allocator_void, get_allocator_void)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.iterators",
                               test_iterators_func_array, tests, test_count);
}
