/***************************************************************************
 *
 * 25.partial.sort.cpp - test exercising [lib.partial.sort]
 *
 * $Id: 25.partial.sort.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for partial_sort, partial_sort_copy
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void
partial_sort (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
              RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
              RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
partial_sort (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
              RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
              RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
              binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

template
RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >
partial_sort_copy (InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                   InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                   RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
                   RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >
partial_sort_copy (InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                   InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                   RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
                   RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
                   binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T>
struct Less
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Less (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    conv_to_bool operator() (const T &x, const T &y) /* non-const */ {
        ++funcalls_;
        return conv_to_bool::make (x.data_.val_ < y.data_.val_);
    }

    static const char* name () { return "Less"; }

private:
    void operator= (Less&);   // not assignable
};

template<class T> std::size_t Less<T>::funcalls_;

/**************************************************************************/

template <class Iterator, class CopyIterator, class T, class Predicate>
void test_partial_sort (int                  line,
                        const char          *src,
                        const std::size_t    N,
                        const std::size_t    mid,
                        const Iterator      &it,
                        const CopyIterator  &itc,
                        const T* ,
                        const Predicate     *ppred,
                        bool                 copy)
{
    typedef RandomAccessIter<T> RandIter;
    const RandIter rand_it(0, 0, 0);

    const char* const itname  =
        copy ? type_name (itc, (T*)0) : type_name (it, (T*)0);
    const char* const outname = "RandomAccessIterator";
    const char* const fname   = copy ? "partial_sort_copy" : "partial_sort";
    const char* const funname = ppred ? Predicate::name() : "operator<()";

    // generate random values for each default constructed T
    T::gen_ = gen_rnd;

    const std::size_t nsrc = src ? std::strlen (src) : N;

    T* const xsrc = src ? T::from_char (src, nsrc) : new T[nsrc];
    T* const xdst = new T [mid];
    T* res_x = copy ? xdst : xsrc;

    T* const xsrc_end = xsrc + nsrc;
    T* const xdst_end = xdst + mid;

    const Iterator first  = make_iter (xsrc, xsrc, xsrc_end, it);
    const Iterator middle = make_iter (xsrc + mid, xsrc, xsrc_end, it);
    const Iterator last   = make_iter (xsrc_end, xsrc, xsrc_end, it);

    CopyIterator first_c  = make_iter (xsrc, xsrc, xsrc_end, itc);
    CopyIterator last_c   = make_iter (xsrc_end, xsrc, xsrc_end, itc);

    RandIter res_first = make_iter (xdst, xdst, xdst_end, rand_it);
    RandIter res_last  = make_iter (xdst_end, xdst, xdst_end, rand_it);

    const Predicate pred (0, 0);
    RandIter result (0, 0, 0);

    std::size_t last_n_op_lt  = T::n_total_op_lt_;

    if (copy) {
        if (ppred)
            result = std::partial_sort_copy (
                first_c, last_c, res_first, res_last, pred);
        else
            result = std::partial_sort_copy (
                first_c, last_c, res_first, res_last);
    }
    else {
        if (ppred)
            std::partial_sort (first, middle, last, pred);
        else
            std::partial_sort (first, middle, last);
    }

    bool success = true;
    // check the returned iterator for copy version
    if (copy) {
        success = result.cur_ == res_first.cur_ + mid;
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}%{?}, %s%{;}> (): "
                   "returned iterator it is invalid: got result + %td, "
                   "expected result + %zu",
                   __LINE__, fname, itname, copy, outname, ppred, funname,
                   result.cur_ - res_first.cur_, mid);
    }

    // check that the array is sorted
    success = is_sorted_lt (res_x, res_x + mid);
    if (src) {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}%{?}, %s%{;}> "
                   "(\"%s\", %zu, ...): ==> \"%{X=*.*}\" not sorted",
                    __LINE__, fname, itname, copy, outname, ppred, funname,
                    src, mid, int (mid), -1, res_x);
    }
    else {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}%{?}, %s%{;}> "
                   "(%zu, %zu, ...): not sorted",
                   __LINE__, fname, itname, copy, outname, ppred, funname,
                   nsrc, mid);
    }

    // check that any element in the sorted range <= that any element
    // in the rest part of the array
    int max_el = res_x[0].data_.val_;
    std::size_t j = 1;
    for ( ; j < mid; j++)
        max_el = max_el < res_x[j].data_.val_ ? res_x[j].data_.val_ : max_el;

    if (copy) {
        std::size_t tmp = 0;
        for (j = 0; j < nsrc; j++)
            if (max_el > xsrc[j].data_.val_)
                tmp++;

        success = tmp <= mid;
    }
    else {
        for (j = mid; j < nsrc; j++) {
            success = max_el <= xsrc[j].data_.val_;
            if (! success)
                break;
        }
    }

    // to avoid error in trace mode
    j = j < nsrc ? j : nsrc - 1;

    if (src) {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}%{?}, %s%{;}> "
                   "(\"%s\", %zu, ...): ==> \"%{X=*.*}\" got less element "
                   "%{?}%#c%{;} in the unsorted part",
                   __LINE__, fname, itname, copy, outname, ppred, funname,
                   src, mid, int (copy ? mid : nsrc), -1, res_x,
                   !copy, xsrc[j].data_.val_);
    }
    else {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}%{?}, %s%{;}> "
                   "(%zu, %zu, ...): got less element in the unsorted part",
                   __LINE__, fname, itname, copy, outname, ppred, funname,
                   nsrc, mid);
    }

    // verify 25.3.1.1, p2 and 25.3.1.2, p3
    // the complexity of our implementation is no worse than
    // 3.33 * N * log (N) (hence the magic 7 and 2)
    std::size_t n_ops =
       ppred ? Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;
    std::size_t exp_ops = 7 * nsrc * ::ilog2 (mid > 1 ? mid : 2);
    success = 2 * n_ops <= exp_ops;
    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s%{;}%{?}, %s%{;}> (): complexity "
               "for length %zu is %zu, expected no more than %zu",
               __LINE__, fname, itname, copy, outname, ppred, funname,
               nsrc, n_ops, exp_ops / 2);

    delete[] xsrc;
    delete[] xdst;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 256;            // --nloops=#
/* extern */ int rw_opt_no_partial_sort;         // --no-partial_sort
/* extern */ int rw_opt_no_partial_sort_copy;    // --no-partial_sort_copy
/* extern */ int rw_opt_no_predicate;            // --no-predicate
/* extern */ int rw_opt_no_complexity;           // --no-complexity
/* extern */ int rw_opt_no_input_iter;           // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;             // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;           // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;             // --no-RandomAccessIterator

/**************************************************************************/

template <class Iterator, class CopyIterator, class T, class Predicate>
void test_partial_sort (const std::size_t    N,
                        const Iterator      &it,
                        const CopyIterator  &itc,
                        const T* ,
                        const Predicate     *ppred,
                        bool                 copy)
{
    const char* const itname  =
        copy ? type_name (itc, (T*)0) : type_name (it, (T*)0);
    const char* const outname = "RandomAccessIterator";
    const char* const fname   = copy ? "partial_sort_copy" : "partial_sort";
    const char* const funname = ppred ? Predicate::name() : "operator<()";

    rw_info (0, 0, 0,
             "%{?}%s %{;}std::%s (%s, %4$s, %s%{?}, %2$s%{;}%{?}, %s%{;})",
             copy, outname, fname, itname, copy ? outname : itname,
             copy, ppred, funname);

#define TEST(src, mid)                                                      \
    test_partial_sort (__LINE__, src, 0, mid, it, itc, (T*)0, ppred, copy)

    TEST ("a", 1);

    TEST ("ba",         1);
    TEST ("cba",        1);
    TEST ("dcba",       2);
    TEST ("edcba",      2);
    TEST ("fedcba",     3);
    TEST ("gfedcba",    3);
    TEST ("hgfedcba",   4);
    TEST ("ihgfedcba",  4);
    TEST ("jihgfedcba", 5);

    TEST ("ab",         1);
    TEST ("abc",        1);
    TEST ("abcd",       2);
    TEST ("abcde",      2);
    TEST ("abcdef",     3);
    TEST ("abcdefg",    3);
    TEST ("abcdefgh",   4);
    TEST ("abcdefghi",  4);
    TEST ("abcdefghij", 5);

    TEST ("aa",         1);
    TEST ("aabb",       2);
    TEST ("bbccaa",     3);
    TEST ("ddbbccaa",   4);
    TEST ("ddeebbccaa", 5);

    TEST ("aa",          2);
    TEST ("aabb",        4);
    TEST ("bbccaa",      6);
    TEST ("ddbbccaa",    8);
    TEST ("ddeebbccaa", 10);

    TEST ("aaaaaaaaaa", 5);
    TEST ("ababababab", 4);
    TEST ("bababababa", 4);

#undef TEST

    if (rw_opt_no_complexity) {
        rw_note (0, 0, 0,
                 "%{?}%s %{;}std::%s (%s, %4$s, %s%{?}, %2$s%{;}%{?}, %s%{;})"
                 ": complexity test disabled",
                 copy, outname, fname, itname, copy ? outname : itname,
                 copy, ppred, funname);
    }
    else {

    rw_info (0, 0, 0,
            "%{?}%s %{;}std::%s (%s, %4$s, %s%{?}, %2$s%{;}%{?}, %s%{;})"
            ": complexity test",
             copy, outname, fname, itname, copy ? outname : itname,
             copy, ppred, funname);

        for (std::size_t i = 1; i < N; i++)
            test_partial_sort (__LINE__, 0, i, i > 1 ? i / 2 : 1, it, itc,
                              (T*)0, ppred, copy);
    }
}

/**************************************************************************/

template <class T, class Predicate>
void test_partial_sort (const std::size_t  N,
                        const T* ,
                        const Predicate   *ppred,
                        bool               copy)
{
    rw_info (0, 0, 0,
             "template <class %s%{?}, class %s%{;}%{?}, class %s%{;}> "
             "%s std::partial_sort%{?}_copy%{;} (%1$s%{?}, %1$s%{;}, %1$s"
             "%{?}, %3$s, %3$s%{;}%{?}, %5$s%{;})",
             copy ? "InputIterator" : "RandomAccessIterator",
             copy, "RandomAccessIterator", ppred, "StrictWeakComp",
             copy ? "RandomAccessIterator" : "void",
             copy, !copy, copy, ppred);

    static const InputIter<T>        input_iter (0, 0, 0);
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    if (! copy) {
        test_partial_sort (N, rand_iter, rand_iter,
                          (T*)0, ppred, false);
        return;
    }

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_partial_sort (N, rand_iter, input_iter,
                          (T*)0, ppred, true);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_partial_sort (N, rand_iter, fwd_iter,
                          (T*)0, ppred, true);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_partial_sort (N, rand_iter, bidir_iter,
                          (T*)0, ppred, true);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_partial_sort (N, rand_iter, rand_iter,
                          (T*)0, ppred, true);
    }
}

/**************************************************************************/

template <class T>
void test_partial_sort (const std::size_t N,
                        const T* ,
                        bool        copy)
{
    test_partial_sort (N, (T*)0, (Less<T>*)0, copy);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "std::partial_sort%{?}_copy%{;} predicate test disabled",
                 copy);
    }
    else {
        const Less<T> pred(0, 0);
        test_partial_sort (N, (T*)0, &pred, copy);
    }
}

/**************************************************************************/


static int run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    if (rw_opt_no_partial_sort) {
        rw_note (0, __FILE__, __LINE__,
                 "std::partial_sort test disabled");
    }
    else {
        test_partial_sort (N, (UserClass*)0, false);
    }

    if (rw_opt_no_partial_sort_copy) {
        rw_note (0, __FILE__, __LINE__,
                 "std::partial_sort_copy test disabled");
    }
    else {
        test_partial_sort (N, (UserClass*)0, true);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.partial.sort",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-partial_sort# "
                    "|-no-partial_sort_copy# "
                    "|-no-InputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator# "
                    "|-no-predicate",
                    &rw_opt_nloops,
                    &rw_opt_no_partial_sort,
                    &rw_opt_no_partial_sort_copy,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate,
                    &rw_opt_no_complexity);
}
