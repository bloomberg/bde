/***************************************************************************
 *
 * specialized.cpp - test exercising 20.4.4 [lib.specialized.algorithms]
 *
 * $Id: 20.specialized.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2001-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <memory>
#include <alg_test.h>
#include <driver.h>

/**************************************************************************/

#ifndef _RWSTD_NO_EXCEPTIONS

struct Y
{
    static int count_;
    static int ctor_;
    static int dtor_;
    static int throw_at_;

    int val_;

    Y (const Y &rhs) : val_ (rhs.val_) {
        ++ctor_;
        if (count_ + 1 == throw_at_)
            throw 0;
        ++count_;
    }

    Y (int val) : val_ (val) {
        ++ctor_;
        if (count_ + 1 == throw_at_)
            throw 0;
        ++count_;
    }

    ~Y () {
        ++dtor_;
        val_ = -2;
        --count_;
    }

private:
    void operator= (const Y&);   // not Assignable
};


int Y::count_;
int Y::ctor_;
int Y::dtor_;
int Y::throw_at_;

/**************************************************************************/

template <class T>
const char* type_name (volatile T*, T*) { return "volatile T*"; }

template <class T>
const char* type_name (const volatile T*, T*) { return "const volatile T*"; }

// defined as ordinary functions (as opposed to templates)
// to avoid tripping up broken compilers on argument deduction
inline const volatile int*
make_iter (const int *beg, const int*, const int*, const volatile int*)
{
    return beg;
}

inline volatile Y*
make_iter (Y *beg, Y*, Y*, volatile Y*)
{
    return beg;
}

/**************************************************************************/

// exercises uninitialized_copy [lib.uninitialized.copy]
// with emphasis on exception safety - the function is to have no effect
// if an exception is thrown
template <class InputIterator, class ForwardIterator>
void test_uninitialized_copy (const InputIterator &dummy,
                              const ForwardIterator*)
{
    const char* const i1name = type_name (dummy, (int*)0);
    const char* const i2name = type_name (ForwardIterator (), (Y*)0);

    rw_info (0, 0, __LINE__, "std::uninitialized_copy(%s, %1$s, %s)",
             i1name, i2name);

    static const int a[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };

    const unsigned N = sizeof a / sizeof *a;

    // allocate an uninitialized buffer
    Y *buf = _RWSTD_STATIC_CAST (Y*, ::operator new (N * sizeof (Y)));

    for (unsigned i = 0; i != N; ++i) {

        Y::count_ = Y::ctor_ = Y::dtor_ = 0;

        // throw during the copy construction of the i-th elem
        Y::throw_at_ = i;

        try {
            // constructs i elements, the last ctor throws
            const InputIterator first =
                make_iter (a, a, a + i, dummy);
            const InputIterator last =
                make_iter (a + i, a, a + i, first);

            const ForwardIterator result =
                make_iter (buf, buf, buf + i, ForwardIterator ());

            std::uninitialized_copy (first, last, result);

            rw_assert (i == 0, 0, __LINE__,
                       "%zu. expected exception not thrown", i);
        }
        catch (int) {
            // ctor throws an int, this is what's expected
        }
        catch (...) {
            rw_assert (0, 0, __LINE__,
                       "%zu. exception of an unexpected type thrown", i);
        }

        // verify that all constructed elements were destroyed
        rw_assert (Y::count_ == 0, 0, __LINE__,
                   "%zu. %d elements not destroyed", i, Y::count_);

        // verify that the expected number of ctor and dtor calls were made
        rw_assert (Y::ctor_ == Y::throw_at_, 0, __LINE__,
                   "%zu. %d ctor calls expected, got %d",
                   i, Y::throw_at_, Y::ctor_);
        rw_assert (i ? Y::dtor_ + 1 == Y::throw_at_ : true, 0, __LINE__,
                   "%zu. %d dtor calls expected, got %d",
                   i, Y::throw_at_ - 1, Y::dtor_);
    }

    ::operator delete (buf);
}

/**************************************************************************/

// exercises 20.4.4.2 and 3 - uninitialized_fill [lib.uninitialized.fill]
// and uninitialized_fill_n [lib.uninitialized.fill_n] with emphasis on
// exception safety - the function is to have no effect if an exception
// is thrown
template <class ForwardIterator>
void test_uninitialized_fill (const ForwardIterator*, bool test_count)
{
    const char* const itname = type_name (ForwardIterator (), (Y*)0);

    rw_info (0, 0, __LINE__,
             "std::uninitialized_fill%{?}_n%{;}(%s, %{?}size_t%{:}%2$s%{;}, "
             "const int&)", test_count, itname, test_count);

    const unsigned N = 32;

    // allocate an uninitialized buffer
    Y *buf = _RWSTD_STATIC_CAST (Y*, ::operator new (N * sizeof (Y)));

    for (unsigned i = 0; i != N; ++i) {

        // prevent ctor below from throwing
        Y::throw_at_ = -1;

        // create a temporary and reset counters (in that order)
        Y val (i);

        Y::count_ = Y::ctor_ = Y::dtor_ = 0;

        // throw during the copy construction of the i-th elem
        Y::throw_at_ = i;

        try {
            // copy-constructs i elements, the last copy ctor throws
            const ForwardIterator first =
                make_iter (buf, buf, buf + i, ForwardIterator ());

            if (test_count) {
                std::uninitialized_fill_n (first, i, val);
            }
            else {
                const ForwardIterator last =
                    make_iter (buf + i, buf, buf + i, first);

                std::uninitialized_fill (first, last, val);
            }

            rw_assert (i == 0, 0, __LINE__,
                       "%zu. expected exception not thrown", i);
        }
        catch (int) {
            // ctor throws an int, this is what's expected
        }
        catch (...) {
            rw_assert (0, 0, __LINE__,
                       "%zu. exception of an unexpected type thrown", i);
        }

        // verify that all constructed elements were destroyed
        rw_assert (Y::count_ == 0, 0, __LINE__,
                   "%zu. %d elements not destroyed", i, Y::count_);

        // verify that the expected number of ctor and dtor calls were made
        rw_assert (Y::ctor_ == Y::throw_at_, 0, __LINE__,
                   "%zu. %d ctor calls expected, got %d",
                   i, Y::throw_at_, Y::ctor_);
        rw_assert (i ? Y::dtor_ + 1 == Y::throw_at_ : true, 0, __LINE__,
                   "%d. %d dtor calls expected, got %d",
                   i, Y::throw_at_ - 1, Y::dtor_);
    }

    ::operator delete (buf);
}

/**************************************************************************/

int opt_copy;
int opt_fill;
int opt_fill_n;

int opt_input_iter;
int opt_fwd_iter;
int opt_bidir_iter;
int opt_rnd_iter;
int opt_volatile_ptr;


template <class InputIterator>
void test_uninitialized_copy (const InputIterator &dummy)
{
    if (-1 < opt_fwd_iter)
        test_uninitialized_copy (dummy, (FwdIter<Y>*)0);
    else
        rw_note (-1 > opt_fwd_iter--, 0, __LINE__,
                 "ForwardIterator tests disabled");

    if (-1 < opt_bidir_iter)
        test_uninitialized_copy (dummy, (BidirIter<Y>*)0);
    else
        rw_note (-1 > opt_bidir_iter--, 0, __LINE__,
                 "BidirectionalIterator tests disabled");

    if (-1 < opt_rnd_iter)
        test_uninitialized_copy (dummy, (RandomAccessIter<Y>*)0);
    else
        rw_note (-1 > opt_rnd_iter--, 0, __LINE__,
                 "RandomAccessIterator tests disabled");

    if (-1 < opt_volatile_ptr) {
#if TEST_RW_EXTENSION // operator new(size_t, volatile void*)
        typedef volatile Y* VolatilePointer;
        test_uninitialized_copy (dummy, (VolatilePointer*)0);
#endif // TEST_RW_EXTENSION
    }
    else
        rw_note (-1 > opt_volatile_ptr--, 0, __LINE__,
                 "volatile T* tests disabled");
}

/**************************************************************************/

static int
run_test (int, char**)
{
    typedef const volatile int* ConstVolatilePointer;
    typedef /* */ volatile Y*   VolatilePointer;

    if (-1 < opt_copy) {
        if (-1 < opt_input_iter)
            test_uninitialized_copy (InputIter<int>(0, 0, 0));
        else
            rw_note (-1 > opt_input_iter--, 0, __LINE__,
                     "InputIterator tests disabled");

        if (-1 < opt_fwd_iter)
            test_uninitialized_copy (ConstFwdIter<int>());
        else
            rw_note (-1 > opt_fwd_iter--, 0, __LINE__,
                     "ForwardIterator tests disabled");

        if (-1 < opt_bidir_iter)
            test_uninitialized_copy (ConstBidirIter<int>());
        else
            rw_note (-1 > opt_bidir_iter--, 0, __LINE__,
                     "BidirectionalIterator tests disabled");

        if (-1 < opt_rnd_iter)
            test_uninitialized_copy (ConstRandomAccessIter<int>());
        else
            rw_note (-1 > opt_rnd_iter--, 0, __LINE__,
                     "RandomAccessIterator tests disabled");

        if (-1 < opt_volatile_ptr)
            test_uninitialized_copy (ConstVolatilePointer ());
        else
            rw_note (-1 > opt_volatile_ptr--, 0, __LINE__,
                     "volatile T* tests disabled");
    }
    else
        rw_note (0, 0, 0, "tests of std::uninitialized_copy disabled");

    //////////////////////////////////////////////////////////////////

    if (-1 < opt_fill) {
        if (-1 < opt_fwd_iter)
            test_uninitialized_fill ((FwdIter<Y>*)0, false);
        else
            rw_note (-1 > opt_fwd_iter--, 0, __LINE__,
                     "ForwardIterator tests disabled");

        if (-1 < opt_bidir_iter)
            test_uninitialized_fill ((BidirIter<Y>*)0, false);
        else
            rw_note (-1 > opt_bidir_iter--, 0, __LINE__,
                     "BidirectionalIterator tests disabled");

        if (-1 < opt_bidir_iter)
            test_uninitialized_fill ((RandomAccessIter<Y>*)0, false);
        else
            rw_note (-1 > opt_bidir_iter--, 0, __LINE__,
                     "RandomAccessIterator tests disabled");

        if (-1 < opt_volatile_ptr) {
#if TEST_RW_EXTENSION // operator new(size_t, volatile void*)
            test_uninitialized_fill ((VolatilePointer*)0, false);
#endif // TEST_RW_EXTENSION
        }
        else
            rw_note (-1 > opt_volatile_ptr++,
                     0, 0, "volatile T* tests disabled");
    }
    else
        rw_note (0, 0, 0, "tests of std::uninitialized_fill disabled");

    //////////////////////////////////////////////////////////////////
    if (-1 < opt_fill) {
        if (-1 < opt_fwd_iter)
            test_uninitialized_fill ((FwdIter<Y>*)0, true);
        else
            rw_note (-1 > opt_fwd_iter--, 0, __LINE__,
                     "ForwardIterator tests disabled");

        if (-1 < opt_bidir_iter)
            test_uninitialized_fill ((BidirIter<Y>*)0, true);
        else
            rw_note (-1 > opt_bidir_iter--, 0, __LINE__,
                     "BidirectionalIterator tests disabled");

        if (-1 < opt_bidir_iter)
            test_uninitialized_fill ((RandomAccessIter<Y>*)0, true);
        else
            rw_note (-1 > opt_bidir_iter--, 0, __LINE__,
                     "RandomAccessIterator tests disabled");

        if (-1 < opt_volatile_ptr) {
#if TEST_RW_EXTENSION // operator new(size_t, volatile void*)
            test_uninitialized_fill ((VolatilePointer*)0, true);
#endif // TEST_RW_EXTENSION
        }
        else
            rw_note (-1 > opt_volatile_ptr--, 0, __LINE__,
                     "volatile T* tests disabled");
    }
    else
        rw_note (0, 0, 0, "tests of std::uninitialized_fill disabled");

    return 0;
}

#else   // _RWSTD_NO_EXCEPTIONS

static int
run_test (int, char**)
{
    rw_note (0, 0, 0,
             "exceptions disabled (_RWSTD_NO_EXCEPTIONS #defined), "
             "cannot test");

    return 0;
}

#endif   // _RWSTD_NO_EXCEPTIONS

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.specialized.algorithms",
                    0 /* no comment */,
                    run_test,
                    "|-uninitialized_copy~ "
                    "|-uninitialized_fill~ "
                    "|-uninitialized_fill_n~ "
                    "|-InputIterator~ "
                    "|-ForwardIterator~ "
                    "|-BidirectionalIterator~ "
                    "|-RandomAccessIterator~ "
                    "|-volatile-pointer",
                    &opt_copy,
                    &opt_fill,
                    &opt_fill_n,
                    &opt_input_iter,
                    &opt_fwd_iter,
                    &opt_bidir_iter,
                    &opt_rnd_iter,
                    &opt_volatile_ptr);
}
