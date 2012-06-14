/***************************************************************************
 *
 * 25.sort.cpp - test exercising lib.sort and lib.stable.sort
 *
 * $Id: 25.sort.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for sort, stable_sort
#include <cstring>      // for strlen, size_t
#include <cstddef>      // for ptrdiff_t

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
sort (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
      RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
sort (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
      RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
      binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

template
void
stable_sort (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
             RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
stable_sort (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
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

template <class T, class Predicate>
void test_sort (int                line,
                const char        *src,
                const std::size_t  N,
                const T*,
                const Predicate   *ppred,
                bool               stable,
                bool               alloc)
{
    typedef RandomAccessIter<T> RandIter;
    const RandIter it(0, 0, 0);

    const char* const itname  = "RandomAccessIterator";
    const char* const fname   = stable ? "stable_sort" : "sort";
    const char* const funname = ppred ? Predicate::name() : "operator<()";

    // generate random values for each default constructed T
    T::gen_ = gen_rnd;

    const std::size_t nsrc = src ? std::strlen (src) : N;

    T* const xsrc = src ? T::from_char (src, nsrc) : new T[nsrc];

    T* const xsrc_end = xsrc + nsrc;

    RandIter first = make_iter (xsrc, xsrc, xsrc_end, it);
    RandIter last  = make_iter (xsrc_end, xsrc, xsrc_end, it);

    const Predicate pred(0, 0);

    std::size_t last_n_op_lt  = T::n_total_op_lt_;
    std::size_t last_n_op_cpy = T::n_total_op_assign_ + T::n_total_copy_ctor_;

    _RWSTD_UNUSED (last_n_op_cpy);

    if (stable) {
        std::pair<UserClass*, std::ptrdiff_t> dummy;
        if (alloc) {
            dummy = std::_GET_TEMP_BUFFER (T, nsrc + 1);
            rw_assert (0 != dummy.first, 0, 0,
                       "line %d: %s<%s%{?}, %s%{;}> (): "
                       "memory allocation failed for %zu elements",
                       __LINE__, fname, itname, ppred, funname, nsrc);
        }

        if (ppred)
            std::stable_sort (first, last, pred);
        else
            std::stable_sort (first, last);

        if (alloc && dummy.first)
            std::return_temporary_buffer (dummy.first);
    }
    else {
        if (ppred)
            std::sort (first, last, pred);
        else
            std::sort (first, last);
    }

    // some tracing goes here
    /*
    if (! stable) {
        // number of comparison operations
        std::size_t ops = std::size_t (T::n_total_op_lt_ - last_n_op_lt);
        // number of copy ctor and assignmen operator calls
        std::size_t cpy =
            std::size_t (T::n_total_op_assign_ + T::n_total_copy_ctor_)
            - std::size_t (last_n_op_cpy);

        // expected complexity (number opf comparisons)
        std::size_t cmplx = (nsrc + 1) * ilog2 (nsrc + 2);
        double      x     = double (ops) / cmplx;

        // max and min T
        static double x_max = 0.0;
        static double x_min = 1.0;

        if (x > x_max)
            x_max = x;

        if (nsrc > 16 && x < x_min)
            x_min = x;

        // complexity: UserClass * N * log (N),
        // ideally with UserClass approaching 1

        if (!(nsrc % 20)) {
            rw_info (0, 0, 0,
                     "\n+------+------+------+------+------+------+------+\n"
                     "|   N  | COMP | COPY |N lg N|   X  | max X| min X|\n"
                     "+======+======+======+======+======+======+======+\n");

            // # | comp | assign | exp. complexity | X | max X | min X
            rw_info (0, 0, 0, "\n|%6d|%6d|%6d|%6d|%6.2f|%6.2f|%6.2f|\n",
                     nsrc + 1, ops, cpy, cmplx, x, x_max, x_min);
        }
    }
    */

    // check that the array is sorted
    bool success = is_sorted_lt (xsrc, xsrc_end);
    if (src) {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}> (\"%s\", ...) ==> "
                   "\"%{X=*.*}\" not sorted",
                   __LINE__, fname, itname, ppred, funname, src,
                   int (nsrc), -1, xsrc);
    }
    else {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}> (%zu, ...): "
                   "not sorted",
                   __LINE__, fname, itname, ppred, funname, nsrc);
    }

    // verify 25.3.1.1, p2 and 25.3.1.2, p3
    // the complexity of our implementation is no worse than
    // 3.33 * N * log (N) (hence the magic 7 and 2)
    const std::size_t n_ops =
        ppred ? Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;

    const std::size_t exp_ops = 7 * nsrc * ::ilog2 (nsrc);
    success = 2 * n_ops <= exp_ops;
    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s%{;}> (): complexity for "
               "length %zu is %zu, expected no more than %zu",
               __LINE__, fname, itname, ppred, funname, nsrc,
               n_ops, exp_ops / 2);

    // verify 25.3.1.2 p2
    if (stable) {
        std::size_t j = 1;
        for ( ; j < N; j++) {
            if (xsrc[j - 1].data_.val_ == xsrc[j].data_.val_)
                success = xsrc[j - 1].origin_ < xsrc[j].origin_;

            if (!success)
                break;
        }

        // to avoid errors in --trace mode
        j = j < nsrc ? j : nsrc - 1;

        if (src) {
            rw_assert (success, 0, line,
                       "line %d: %s<%s%{?}, %s%{;}> (\"%s\", ...) ==> "
                       "\"%{X=*.*}\" relative order is broken at %zu: "
                       "got ids %zu and %zu for values %#c and %#c",
                       __LINE__, fname, itname, ppred, funname, src,
                       int (nsrc), -1, xsrc, j, xsrc[j - 1].origin_,
                       xsrc[j].origin_, xsrc[j - 1].data_.val_,
                       xsrc[j].data_.val_);
        }
        else {
            rw_assert (success, 0, line,
                       "line %d: %s<%s%{?}, %s%{;}> (): relative order "
                       "is broken for %zu at %zu: got ids %zu and %zu "
                       "for values %d and %d",
                       __LINE__, fname, itname, ppred, funname,
                       nsrc, j, xsrc[j - 1].origin_, xsrc[j].origin_,
                       xsrc[j - 1].data_.val_, xsrc[j].data_.val_);
        }
    }

    delete[] xsrc;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 256;            // --nloops=#
/* extern */ int rw_opt_no_sort;                 // --no-sort
/* extern */ int rw_opt_no_stable_sort;          // --no-stable_sort
/* extern */ int rw_opt_no_predicate;            // --no-predicate
/* extern */ int rw_opt_no_complexity;           // --no-complexity

/**************************************************************************/

template <class T, class Predicate>
void test_sort (const std::size_t  N,
                const T*,
                const Predicate   *ppred,
                bool               stable,
                bool               alloc)
{
    rw_info (0, 0, 0,
             "template <class %s%{?}, class %s%{;}> "
             "void std::%{?}stable_%{;}sort (%1$s, %1$s%{?}, %3$s%{;})"
             "%{?} with memory allocation%{;}",
             "RandomAccessIterator", ppred, "StrictWeakComp",
             stable, ppred, stable && alloc);

    const char* const itname  = "RandomAccessIterator";
    const char* const fname   = stable ? "stable_sort" : "sort";
    const char* const funname = ppred ? Predicate::name() : "operator<()";

    rw_info (0, 0, 0,
             "std::%s (%s, %2$s%{?}, %s%{;})",
             fname, itname, ppred, funname);

#define TEST(src)                                              \
    test_sort (__LINE__, src, 0, (T*)0, ppred, stable, alloc)

    TEST ("a");

    TEST ("ba");
    TEST ("cba");
    TEST ("dcba");
    TEST ("edcba");
    TEST ("fedcba");
    TEST ("gfedcba");
    TEST ("hgfedcba");
    TEST ("ihgfedcba");
    TEST ("jihgfedcba");

    TEST ("ab");
    TEST ("abc");
    TEST ("abcd");
    TEST ("abcde");
    TEST ("abcdef");
    TEST ("abcdefg");
    TEST ("abcdefgh");
    TEST ("abcdefghi");
    TEST ("abcdefghij");

    TEST ("aa");
    TEST ("aabb");
    TEST ("bbccaa");
    TEST ("ddbbccaa");
    TEST ("ddeebbccaa");

    TEST ("aaaaaaaaaa");
    TEST ("ababababab");
    TEST ("bababababa");

#undef TEST

    if (rw_opt_no_complexity) {
        rw_note (0, 0, 0,
                 "std::%s (%s, %2$s%{?}, %s%{;}) complexity test disabled",
                 fname, itname, ppred, funname);
    }
    else {
        rw_info (0, 0, 0,
                 "std::%s (%s, %2$s%{?}, %s%{;}): complexity test",
                 fname, itname, ppred, funname);

        for (std::size_t i = 1; i < N; i++)
            test_sort (__LINE__, 0, i, (T*)0, ppred, stable, alloc);
    }
}

/**************************************************************************/

template <class T>
void test_sort (const std::size_t N,
                const T*,
                bool              stable,
                bool              alloc)
{
    test_sort (N, (T*)0, (Less<T>*)0, stable, alloc);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "std::%{?}stable_%{;}sort predicate test disabled",
                 stable);
    }
    else {
        const Less<T> pred(0, 0);
        test_sort (N, (T*)0, &pred, stable, alloc);
    }
}

/**************************************************************************/


static int run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    if (rw_opt_no_sort) {
        rw_note (0, __FILE__, __LINE__, "std::sort test disabled");
    }
    else {
        test_sort (N, (UserClass*)0, false, false);
    }

    if (rw_opt_no_stable_sort) {
        rw_note (0, __FILE__, __LINE__, "std::stable_sort test disabled");
    }
    else {
        test_sort (N, (UserClass*)0, true, false);
        // test with memory reallocation
        test_sort (N, (UserClass*)0, true, true);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.sort",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-sort# "
                    "|-no-stable_sort# "
                    "|-no-predicate",
                    &rw_opt_nloops,
                    &rw_opt_no_sort,
                    &rw_opt_no_stable_sort,
                    &rw_opt_no_predicate,
                    &rw_opt_no_complexity);
}
