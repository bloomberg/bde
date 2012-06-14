/***************************************************************************
 *
 * 25.swap.cpp - test exercising 25.2.2 [lib.alg.swap]
 *
 * $Id: 25.swap.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for swap, swap_ranges, iter_swap
#include <cstring>      // for size_t, strlen()

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()


_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void
swap (assign<base<cpy_ctor> >&, assign<base<cpy_ctor> >&);

template
FwdIter<assign<base<cpy_ctor> > >
swap_ranges (FwdIter<assign<base<cpy_ctor> > >,
             FwdIter<assign<base<cpy_ctor> > >,
             FwdIter<assign<base<cpy_ctor> > >);

template
void
iter_swap (FwdIter<assign<base<cpy_ctor> > >,
           FwdIter<assign<base<cpy_ctor> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T, class ForwardIterator1, class ForwardIterator2>
void test_iter_swap (int line,
                     const char*      seq,
                     ForwardIterator1 /* dummy */,
                     ForwardIterator2 /* dummy */,
                     const T*         /* dummy */,
                     bool             it_swap,
                     const char*      it1name,
                     const char*      it2name,
                     const char*      fname)
{
    // generate sequential values for each default constructed T
    const std::size_t nseq = std::strlen (seq);

    // construct a sequence of `nseq' elements to pass to swap
    T* const tseq = T::from_char (seq, nseq + 1);

    int a_val, b_val;
    bool success = true;
    std::size_t i = 1;
    for ( ; i < nseq; ++i) {

        const ForwardIterator1 it1 =
            make_iter (tseq, tseq, tseq + nseq, it1);

        const ForwardIterator2 it2 =
            make_iter (tseq + i, tseq, tseq + nseq, it2);

        a_val = (*it1).data_.val_;
        b_val = (*it2).data_.val_;

        it_swap ? std::iter_swap (it1, it2) : std::swap(*it1, *it2);

        // verify 25.2.2, p2, p7
        success = a_val == (*it2).data_.val_ && b_val == (*it1).data_.val_;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "%s<%s%{?}, %s%{;}>(%#c, %#c): got: { %#c, %#c } "
               "expected: { %5$#c, %4$#c } at step %zu",
               fname, it1name, it_swap, it2name, a_val, b_val,
               tseq->data_.val_, (tseq + i)->data_.val_, i);

    delete[] tseq;
}

template <class T, class ForwardIterator1, class ForwardIterator2>
void test_iter_swap (ForwardIterator1 it1, ForwardIterator2 it2,
                     const T*, bool it_swap)
{
    const char* const it1name = it_swap ? type_name (it1, (T*)0) : "UserClass";
    const char* const it2name = it_swap ? type_name (it2, (T*)0) : "UserClass";
    const char* const fname   = it_swap ? "iter_swap" : "swap";

    rw_info (0, 0, 0, "std::%s (%s, %s)", fname, it1name, it2name);

#undef TEST
#define TEST(seq)                                               \
    test_iter_swap (__LINE__, seq, it1, it2, (T*)0, it_swap,    \
                    it1name, it2name, fname)

    TEST("ab");
    TEST("abc");
    TEST("abcd");
    TEST("abcde");
    TEST("abcdef");
    TEST("abcdefg");
    TEST("abcdefgh");
    TEST("abcdefghi");
    TEST("abcdefghij");
    TEST("abcdefghijk");
}

/**************************************************************************/

template <class T, class ForwardIterator1, class ForwardIterator2>
void test_swap_ranges (int              line,
                       const char*      seq1,
                       const char*      seq2,
                       ForwardIterator1 it1,
                       ForwardIterator2 it2,
                       const T*         /* dummy */,
                       const char*      it1name,
                       const char*      it2name)
{
    const std::size_t nseq = std::strlen (seq1);

    // construct a sequence of `nseq' elements to pass to swap_ranges
    T* const tseq1 = T::from_char (seq1, nseq + 1);
    T* const tseq2 = T::from_char (seq2, nseq + 1);

    const ForwardIterator1 first1 =
        make_iter (tseq1, tseq1, tseq1 + nseq, it1);
    const ForwardIterator1 last1  =
        make_iter (tseq1 + nseq, tseq1, tseq1 + nseq, it1);
    const ForwardIterator2 first2 =
        make_iter (tseq2, tseq2, tseq2 + nseq, it2);
    const ForwardIterator2 last2 =
        make_iter (tseq2 + nseq, tseq2, tseq2 + nseq, it2);

    // silence bogus EDG eccp remark #550-D: variable was set
    // but never used
    _RWSTD_UNUSED (last2);

    T a, b;

    std::size_t last_n_op_assign = T::n_total_op_assign_;

    std::swap (a, b);

    std::size_t assigns_per_swap = T::n_total_op_assign_ - last_n_op_assign;

    last_n_op_assign = T::n_total_op_assign_;

    // exercise 25.2.3 - std::swap_ranges()
    const ForwardIterator2 res = std::swap_ranges(first1, last1, first2);

    // silence bogus EDG eccp remark #550-D: variable was set
    // but never used
    _RWSTD_UNUSED (res);

    // check the returned value, 25.2.2 p5
    bool success = res.cur_ == last2.cur_;
    rw_assert (success, 0, line,
               "swap_ranges<%s, %s>(\"%s\", \"%s\") == first + %td, "
               "got first + %td",
               it1name, it2name, seq1, seq2, res.cur_ - tseq2,
               last2.cur_ - tseq2);

    // check that the sequences were swapped, 25.2.2 p4
    std::size_t i = 0;
    for ( ; i < nseq; ++i) {
        success =    (tseq1 + i)->data_.val_ == seq2[i]
                  && (tseq2 + i)->data_.val_ == seq1[i];
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "swap_ranges<%s, %s>(\"%s\", \"%s\") mismatch at pos %zu "
               "got { %#c, %#c }, expected { %#c, %#c }",
               it1name, it2name, seq1, seq2, i, (tseq1 + i)->data_.val_,
               (tseq2 + i)->data_.val_, seq2[i], seq1[i]);

    // check the complexity, 25.2.2 p6
    std::size_t swaps_per_swap_ranges =
            (T::n_total_op_assign_ - last_n_op_assign) / assigns_per_swap;
    rw_assert (swaps_per_swap_ranges == nseq, 0, line,
               "swap_ranges<%s, %s>(\"%s\", \"%s\") complexity: "
               "%zu, expected %zu swaps",
               it1name, it2name, seq1, seq2, swaps_per_swap_ranges, nseq);

    delete[] tseq1;
    delete[] tseq2;
}

/**************************************************************************/

template <class T, class ForwardIterator1, class ForwardIterator2>
void test_swap_ranges (ForwardIterator1 it1,
                       ForwardIterator2 it2,
                       const T*         /* dummy */)
{
    static const char* const it1name = type_name (it1, (T*)0);
    static const char* const it2name = type_name (it2, (T*)0);

    rw_info (0, 0, 0,
             "std::swap_ranges (%s, %1$s, %s)",  it1name, it2name);

#undef TEST
#define TEST(seq1, seq2)                                       \
    test_swap_ranges (__LINE__, seq1, seq2, it1, it2, (T*)0,   \
                      it1name, it2name)

    TEST("", "");
    TEST("a", "z");
    TEST("ab", "zy");
    TEST("abc", "zyx");
    TEST("abcd", "zyxw");
    TEST("abcde", "zyxwv");
    TEST("abcdef", "zyxwvu");
    TEST("abcdefg", "zyxwvut");
    TEST("abcdefgh", "zyxwvuts");
    TEST("abcdefghi", "zyxwvutsr");
    TEST("abcdefghij", "zyxwvutsrq");
    TEST("abcdefghijk", "zyxwvutsrqp");
    TEST("abcdefghijkl", "zyxwvutsrqpo");
    TEST("abcdefghijklm", "zyxwvutsrqpon");
}

/**************************************************************************/

/* extern */ int rw_opt_no_swap;             // --no-swap
/* extern */ int rw_opt_no_swap_ranges;      // --no-swap_ranges
/* extern */ int rw_opt_no_iter_swap;        // --no-iter_swap
/* extern */ int rw_opt_no_fwd_iter;         // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;       // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;         // --no-RandomAccessIterator

template <class T, class ForwardIterator1>
void test_swap (ForwardIterator1 it1, const T*, bool test_ranges)
{
    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        if (test_ranges)
            test_swap_ranges (it1, FwdIter<T>(), (T*)0);
        else
            test_iter_swap (it1, FwdIter<T>(), (T*)0, true);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        if (test_ranges)
            test_swap_ranges (it1, BidirIter<T>(), (T*)0);
        else
            test_iter_swap (it1, BidirIter<T>(), (T*)0, true);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        if (test_ranges)
            test_swap_ranges (it1, RandomAccessIter<T>(), (T*)0);
        else
            test_iter_swap (it1, RandomAccessIter<T>(), (T*)0, true);
    }
}

/**************************************************************************/

template <class T>
void test_swap (const T*, bool test_ranges)
{
    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_swap (FwdIter<T>(), (T*)0, test_ranges);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_swap (BidirIter<T>(), (T*)0, test_ranges);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_swap (RandomAccessIter<T>(), (T*)0, test_ranges);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if (rw_opt_no_swap) {
        rw_note (0, __FILE__, __LINE__,  "std::swap test disabled");
    }
    else {
        rw_info (0, 0, 0, "template <class T> void swap (T&, T&)");

        test_iter_swap (FwdIter<UserClass>(), FwdIter<UserClass>(),
                        (UserClass*)0, false);
    }

    if (rw_opt_no_swap_ranges) {
        rw_note (0, __FILE__, __LINE__,  "std::swap_ranges test disabled");
    }
    else {
        rw_info (0, 0, 0,
                 "template <class %s, class %s> %2$s "
                 "swap_ranges (%1$s, %1$s, %2$s)",
                 "ForwardIterator1", "ForwardIterator2");

        test_swap ((UserClass*)0, true);
    }

    if (rw_opt_no_iter_swap) {
        rw_note (0, __FILE__, __LINE__,  "std::iter_swap test disabled");
    }
    else {
        rw_info (0, 0, 0,
                 "template <class %s, class %s> void iter_swap (%1$s, %2$s)",
                 "ForwardIterator1", "ForwardIterator2");

        test_swap ((UserClass*)0, false);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.swap",
                    0 /* no comment */, run_test,
                    "|-no-swap# "
                    "|-no-swap_ranges# "
                    "|-no-iter_swap# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator",
                    &rw_opt_no_swap,
                    &rw_opt_no_swap_ranges,
                    &rw_opt_no_iter_swap,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
