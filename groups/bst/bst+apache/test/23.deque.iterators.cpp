/***************************************************************************
 *
 * 23.deque.iterators.cpp - test exercising lib.deque.iterators
 *
 * $Id: 23.deque.iterators.cpp 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <cstddef>    // for size_t
#include <deque>      // for deque

#include <driver.h>   // for rw_test()
#include <alg_test.h>

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
   // disabled for compilers that can't reliably replace the operators

   // replace operators new and delete with versions that invalidate
   // storage to detect problems due to deque iterators accessing
   // uninitialized pointers
#  include <rw_new.h>
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE

/**************************************************************************/

std::size_t new_capacity;

typedef std::deque<int> IntDeque;

#if TEST_RW_EXTENSIONS
namespace __rw {

_RWSTD_SPECIALIZED_FUNCTION
inline IntDeque::size_type
__rw_new_capacity<IntDeque>(IntDeque::size_type n, const IntDeque*)
{
    if (n) {
        // non-zero size argument indicates a request for an increase
        // in the capacity of a deque object's dynamically sizable
        // vector of nodes
        return n * 2;
    }

    // zero size argument is a request for the initial size of a deque
    // object's dynamically sizable vector of nodes or for the size of
    // the objects's fixed-size buffer for elements
    return new_capacity;
}

}
#endif // TEST_RW_EXTENSIONS

/**************************************************************************/

template <class T>
void test_iterators (std::size_t N, int dir, T*)
{
    typedef std::deque<T>                   Deque;
    typedef typename Deque::difference_type Difference;
    typedef typename Deque::size_type       Size;
    typedef typename Deque::value_type      Value;
    typedef typename Deque::iterator        Iterator;
    typedef typename Deque::const_iterator  ConstIterator;

    Deque d;

    // fill `d' with the range of values [1, N)
    if (dir < 0) {
        for (std::size_t i = 0; i != N; ++i)
            d.push_front (Value (N - i));
    }
    else if (dir > 0) {
        for (std::size_t i = 0; i != N; ++i)
            d.push_back (Value (i + 1));
    }
    else {
        d = Deque (Size (N), Value ());
        for (std::size_t i = 0; i != N; ++i)
            d [i] = Value (i + 1);
    }

    const Iterator begin = d.begin ();
    const Iterator end   = d.end ();

    Iterator i0 = begin;
    Iterator i1 = end;

    // convert N to Difference to avoid any potential issues
    // due to integer promotion in expressions below (e.g.,
    // on LP64)
    const Difference N_diff = Difference (N);

    for (std::size_t i = 0; i != N; ++i, ++i0, --i1) {

        // convert i to Difference for the same reason as N above
        const Difference i_diff = Difference (i);

        // exercise iterator::operator*()
        //      and iterator::operator[](difference_type)
        rw_assert (Value (i + 1) == *i0, 0, __LINE__,
                   "Expected iterator::operator*() value to be %d; got %d.",
                   Value (i + 1), *i0);

        rw_assert (Value (N - i) == i0 [N_diff - 2 * i_diff - 1], 0, __LINE__,
                   "Expected value of iterator::operator[](%d) to be %d; "
                   "got %d.", N_diff - 2 * i_diff - 1, Value (N - i),
                   i0 [N_diff - 2 * i_diff - 1]);

        rw_assert (Value (N - i) == i1 [Difference (-1)], 0, __LINE__,
                   "Expected value of iterator::operator[](%d) to be %d; "
                   "got %d.", Difference (-1), Value (N - i),
                   i1 [Difference (-1)]);

        rw_assert (Value (i + 1) == i1 [2 * i_diff - N_diff], 0, __LINE__,
                   "Expected value of iterator::operator[](%d) to be %d; "
                   "got %d.", 2 * i_diff - N_diff, Value (i + 1),
                   i1 [2 * i_diff - N_diff]);

        // exercise operator-(deque::iterator, deque::iterator)
        Difference d0_begin = i0 - begin;
        Difference d1_begin = i1 - begin;
        Difference d0_end   = end - i0;
        Difference d1_end   = end - i1;

        rw_assert (d0_begin == Difference (i), 0, __LINE__,
                   "Expected value of operator-(deque::iterator, "
                   "deque::iterator) to be %d; got %d.",
                   Difference (i), d0_begin);

        rw_assert (d1_begin == N_diff - i_diff, 0, __LINE__,
                   "Expected value of operator-(deque::iterator, "
                   "deque::iterator) to be %d; got %d.",
                   N_diff - i_diff, d1_begin);

        rw_assert (d0_end == N_diff - i_diff, 0, __LINE__,
                   "Expected value of operator-(deque::iterator, "
                   "deque::iterator) to be %d; got %d.",
                   N_diff - i_diff, d0_end);

        rw_assert (d1_end == i_diff, 0, __LINE__,
                   "Expected value of operator-(deque::iterator, "
                   "deque::iterator) to be %d; got %d.",
                   i_diff, d1_end);

        // exercise operator+(deque::iterator, deque::difference_type),
        //          operator-(deque::iterator, deque::difference_type),
        //          operator==(deque::iterator, deque::iterator), and
        //          operator< (deque::iterator, deque::iterator)
        const Iterator i_begin = begin + i_diff;
        const Iterator i_end   = end - (N_diff - i_diff);

        rw_assert (i_begin == i0, 0, __LINE__,
                   "Expected operator==(deque::iterator, "
                   "deque::iterator) to evaluate true.");

        rw_assert (i_end == i0, 0, __LINE__,
                   "Expected operator==(deque::iterator, "
                   "deque::iterator) to evaluate true.");

        rw_assert (!(i_begin < i0), 0, __LINE__,
                   "Expected operator<(deque::iterator, "
                   "deque::iterator) to evaluate true.");

        rw_assert (!(i_end < i0), 0, __LINE__,
                   "Expected operator<(deque::iterator, "
                   "deque::iterator) to evaluate true.");

        d0_begin = i_begin - begin;
        d0_end   = end - i_end;

        rw_assert (d0_begin == i_diff, 0, __LINE__,
                   "Expected operator==(deque::iterator, "
                   "deque::iterator) to evaluate true.");

        rw_assert (d0_end == N_diff - i_diff, 0, __LINE__,
                   "Expected operator==(deque::iterator, "
                   "deque::iterator) to evaluate true.");

        // exercise iterator::operator++() and iterator::operator--()
        Iterator i2 = i0;
        Iterator i3 = i0;

        if (i && i < N - 1) {
            ++i2;
            rw_assert (i2 == i0 + 1, 0, __LINE__,
                       "Expected ++iterator(it) == it + 1.");

            rw_assert (i0 < i2, 0, __LINE__,
                       "Expected it < ++iterator(it).");

            rw_assert (!(i2 < i0), 0, __LINE__,
                       "Expected !(++iterator (it) < it) to yield true.");

            rw_assert (i2 - i0 == Difference (+1), 0, __LINE__,
                       "Expected ++iterator(it) - it == 1.");

            rw_assert (i0 - i2 == Difference (-1), 0, __LINE__,
                       "Expected it - (++iterator(it)) == -1");

            i2 -= 1;

            rw_assert (i2 - i0 == 0, 0, __LINE__,
                       "Expected (++iterator(it) - 1) - it == 0.");

            rw_assert (i0 - i2 == 0, 0, __LINE__,
                       "Expected it - (++iterator(it) - 1) == 0.");

            i3 -= 1;
            rw_assert (i3 == i0 - 1, 0, __LINE__,
                       "Expected --iterator (it) == it - 1.");
            rw_assert (i3 < i0, 0, __LINE__,
                       "Expected --iterator (it) < it.");
            rw_assert (!(i0 < i3), 0, __LINE__,
                       "Expected !(it < --iterator (it)) to yield true.");

            rw_assert (i3 - i0 == Difference (-1), 0, __LINE__,
                       "Expected (--iterator(it) - it) == -1.");

            rw_assert (i0 - i3 == Difference (+1), 0, __LINE__,
                       "Expected (it - (--iterator(it))) == 1.");

            ++i3;

            rw_assert (i3 - i0 == 0, 0, __LINE__,
                       "Expected --iterator (it) + 1 - it == 0.");
            rw_assert (i0 - i3 == 0, 0, __LINE__,
                       "Expected it - (--iterator (it) + 1) == 0.");
        }

        rw_assert (i2 == i0, 0, __LINE__,
                   "Expected iterators to be equal.");
        rw_assert (i3 == i0, 0, __LINE__,
                   "Expected iterators to be equal.");
        rw_assert (i2 == i3, 0, __LINE__,
                   "Expected iterators to be equal.");

        rw_assert (!(i2 < i0), 0, __LINE__,
                   "Expected iterators to be equal.");
        rw_assert (!(i3 < i0), 0, __LINE__,
                   "Expected iterators to be equal.");
        rw_assert (!(i2 < i3), 0, __LINE__,
                   "Expected iterators to be equal.");
    }
}

/**************************************************************************/

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
// replacement operator new is expensive, avoid long runtimes
unsigned rw_opt_nloops = 6;      // for --nloops
#else
unsigned rw_opt_nloops = 10;     // for --nloops
#endif

int      rw_opt_no_fill;         // for --no-fill
int      rw_opt_no_push_back;    // for --no-push_back
int      rw_opt_no_push_front;   // for --no-push_front


template <class T>
void test_iterators (std::size_t N, T*)
{
    // avoid repeatedly noting that test has been disabled
    static int push_back_noted;
    static int push_front_noted;
    static int fill_noted;

    // fill deque by repeatedly calling either push_front() or push_back()

    if (rw_opt_no_push_back)
        rw_note (push_back_noted++, 0, 0, "push_back test disabled");
    else
        test_iterators (N, -1, (T*)0);

    if (rw_opt_no_fill)
        rw_note (fill_noted++, 0, 0, "fill test disabled");
    else
        test_iterators (N, 0, (T*)0);

    if (rw_opt_no_push_front)
        rw_note (push_front_noted++, 0, 0, "push_front test disabled");
    else
        test_iterators (N, 1, (T*)0);
}

/**************************************************************************/

int run_test (int, char**)
{
#ifdef _RWSTD_NO_DEBUG_ITER

    // verify the size of deque<int>::iterator (space optimization
    // available only when debugging iterators are not enabled)

    const std::size_t itsize = sizeof (IntDeque::iterator);

    rw_assert (itsize == 2 * sizeof (IntDeque::pointer), 0, __LINE__,
               "sizeof(deque<int>::iterator) == %zu, got %zu "
               "[space optimization]",
               2 * sizeof (IntDeque::pointer), itsize);

#endif   // _RWSTD_NO_DEBUG_ITER

    // capacity must be at least 2
    for (std::size_t i = 2; i < rw_opt_nloops; ++i) {

        // set the result of __rw::__rw_new_capacity() to override
        // the container's allocation policy and force an early
        // reallocation

        new_capacity = i;

#if TEST_RW_EXTENSIONS
        const IntDeque::size_type cap =
            __rw::__rw_new_capacity (IntDeque::size_type (),
                                     (const IntDeque*)0);

        rw_info (0, 0, 0,
                 "__rw::__rw_new_capacity(std::deque<int>::"
                 "size_type, std::deque<int>*) = %zu", cap);
#endif // TEST_RW_EXTENSIONS

        const std::size_t max_size = i * i * 10;

        for (std::size_t j = 0; j != max_size; ++j) {

            if (j % (max_size / 10) == 0)
                rw_info (0, 0, 0, "std::deque<int>(%u, ...)", j);

            test_iterators (j, (int*)0);
        }
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.deque.iterators",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#2 "   // argument must be greater than 1
                    "|-no-push_back# "
                    "|-no-push_front# "
                    "|-no-fill#",
                    &rw_opt_nloops,
                    &rw_opt_no_push_back,
                    &rw_opt_no_push_front,
                    &rw_opt_no_fill);
}
