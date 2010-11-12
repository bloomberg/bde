/***************************************************************************
 *
 * 25.min.max.cpp - test exercising 25.3.7 [lib.alg.min.max]
 *
 * $Id: 25.min.max.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for min, max, min_element, max_element
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
const lt_comp<base<cpy_ctor> >& 
min (const lt_comp<base<cpy_ctor> >&,
     const lt_comp<base<cpy_ctor> >&);

template
const lt_comp<base<cpy_ctor> >& 
min (const lt_comp<base<cpy_ctor> >&,
     const lt_comp<base<cpy_ctor> >&,
     binary_predicate<lt_comp<base<cpy_ctor> > >);

template
const lt_comp<base<cpy_ctor> >& 
max (const lt_comp<base<cpy_ctor> >&,
     const lt_comp<base<cpy_ctor> >&);

template
const lt_comp<base<cpy_ctor> >& 
max (const lt_comp<base<cpy_ctor> >&,
     const lt_comp<base<cpy_ctor> >&,
     binary_predicate<lt_comp<base<cpy_ctor> > >);

template
FwdIter<lt_comp<base<cpy_ctor> > >
min_element (FwdIter<lt_comp<base<cpy_ctor> > >,
             FwdIter<lt_comp<base<cpy_ctor> > >);

template
FwdIter<lt_comp<base<cpy_ctor> > >
min_element (FwdIter<lt_comp<base<cpy_ctor> > >,
             FwdIter<lt_comp<base<cpy_ctor> > >,
             binary_predicate<lt_comp<base<cpy_ctor> > >);

template
FwdIter<lt_comp<base<cpy_ctor> > >
max_element (FwdIter<lt_comp<base<cpy_ctor> > >,
             FwdIter<lt_comp<base<cpy_ctor> > >);

template
FwdIter<lt_comp<base<cpy_ctor> > >
max_element (FwdIter<lt_comp<base<cpy_ctor> > >,
             FwdIter<lt_comp<base<cpy_ctor> > >,
             binary_predicate<lt_comp<base<cpy_ctor> > >);

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

// exercises min, max: 25.3.7
template <class T, class Predicate>
void test_min_max (int              line,
                   const char       a,
                   const char       b,
                   const T*,
                   const Predicate *ppred,
                   bool             test_min)
{
    const char* const tname   = "UserClass";
    const char* const fname   = test_min ? "min" : "max";
    const char* const funname = Predicate::name();

    T ta, tb;
    ta.data_.val_ = a;
    tb.data_.val_ = b;

    const int exp_id =
        test_min ? b < a ? tb.id_ : ta.id_ : b > a ? tb.id_ : ta.id_;

    const char exp_res = test_min ? b < a ? b : a : b > a ? b : a;

    const Predicate pred (0, 0);   // dummy arguments

    const T& res = ppred ? 
        test_min ? std::min (ta, tb, pred) : std::max (ta, tb, pred)
      : test_min ? std::min (ta, tb) : std::max (ta, tb);

    // verify 25.3.7 p2, p3, p5, p6
    rw_assert (exp_res == res.data_.val_ && exp_id == res.origin_, 0, line,
               "line %d %s <%s%{?}, %s%{;}> (%#c, %#c) returned "
               "value %#c, id %d; expected value %#c, id %d",
               __LINE__, fname, tname, 0 != ppred, funname, a, b,
               res.data_.val_, res.origin_, exp_res, exp_id);
}

/**************************************************************************/

// exrcises min_element, max_element: 25.3.7 
template <class T, class ForwardIterator, class Predicate>
void test_min_max_element (int                     line,
                           const char             *src,                    
                           const std::size_t       min_off,
                           const std::size_t       max_off,
                           const ForwardIterator  &it,
                           const T*,
                           const Predicate        *ppred,
                           bool                    test_min)
{
    const char* const itname  = type_name (it, (T*)0);
    const char* const fname   = test_min ? "min_element" : "max_element";
    const char* const funname = Predicate::name();

    const std::size_t nsrc = std::strlen (src);

    T* const xsrc     = T::from_char (src, nsrc);
    T* const xsrc_end = xsrc + nsrc;

    const ForwardIterator first = make_iter (xsrc,     xsrc, xsrc_end, it);
    const ForwardIterator last  = make_iter (xsrc_end, xsrc, xsrc_end, it);

    Predicate pred (0, 0);

    const std::size_t last_n_op_lt = T::n_total_op_lt_;

    ForwardIterator res (0, 0, 0);

    if (ppred) {
        res = test_min ? 
            std::min_element (first, last, pred) 
          : std::max_element (first, last, pred);
    }
    else {
        res = test_min ? 
            std::min_element (first, last) 
          : std::max_element (first, last);
    }

    // verify the returned value: 25.3.7 p7, p9
    const std::size_t off = test_min ? min_off : max_off;
    rw_assert (res.cur_ == xsrc + off, 0, line,
               "line %d: %s<%s%{?}, %s%{;}> (\"%s\", %zu, ...) "
               "got first + %td, expected first + %zu", 
               __LINE__, fname, itname, 0 != ppred, funname, src, off,
               res.cur_ - xsrc, off);

    const std::size_t n_ops_lt = ppred ? 
        Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;

    // verify the complexity: 25.3.7 p8, p10
    std::size_t n_exp_ops = nsrc > 0 ? nsrc - 1 : 0;
    rw_assert (n_ops_lt == n_exp_ops, 0, line,
               "line %d: %s<%s%{?}, %s%{;}> (\"%s\", %zu, ...) "
               "complexity: got %zu, expected %zu",
               __LINE__, fname, itname, 0 != ppred, funname, src, off,
               n_ops_lt, n_exp_ops);

    delete[] xsrc;
}


/**************************************************************************/

template <class T, class Predicate>
void test_min_max (const T*,
                   const Predicate *ppred,
                   bool             test_min)
{
    const char* const tname = "UserClass";
    const char* const fname = test_min ? "min" : "max";
    const char* const funname = Predicate::name();

    rw_info (0, 0, 0,
             "const %s& std::%s(const %1$s&, const %1$s&%{?}, %s%{;})",
             tname, fname, 0 != ppred, funname);

#define TEST(a, b)                                          \
    test_min_max (__LINE__, a, b, (T*)0, ppred, test_min)

    TEST ('a', 'a');
    TEST ('a', 'b');
    TEST ('b', 'a');
    TEST ('b', 'b');

    TEST ('a', 'e');
    TEST ('e', 'a');

#undef TEST
}

/**************************************************************************/

template <class T, class ForwardIterator, class Predicate>
void test_min_max_element (const ForwardIterator   &it,
                           const T*, 
                           const Predicate         *ppred,
                           bool                     test_min)
{
    const char* const itname  = type_name (it, (T*)0);
    const char* const fname   = test_min ? "min_element" : "max_element";
    const char* const funname = Predicate::name();

    rw_info (0, 0, 0,
             "%s std::%s(%1$s, %1$s%{?}, %s%{;})",
             itname, fname, 0 != ppred, funname);

#define TEST(src, min_off, max_off)                                  \
    test_min_max_element (__LINE__, src, min_off, max_off, it,       \
                         (T*)0, ppred, test_min)  

    TEST ("", 0, 0);
    TEST ("a", 0, 0);
    TEST ("ab", 0, 1);
    TEST ("aa", 0, 0);
    TEST ("ba", 1, 0);

    TEST ("abcdefghij", 0, 9);
    TEST ("bacdefghji", 1, 8);
    TEST ("bcadefgjhi", 2, 7);
    TEST ("bcdaefjghi", 3, 6);
    TEST ("bcdeajfghi", 4, 5);
    TEST ("bcdejafghi", 5, 4);
    TEST ("bcdjefaghi", 6, 3);
    TEST ("bcjdefgahi", 7, 2);
    TEST ("bjcdefghai", 8, 1);
    TEST ("jbcdefghia", 9, 0);

    TEST ("jihgfedcba", 9, 0);

    TEST ("aaaaaaaaaa", 0, 0);
    TEST ("ababababab", 0, 1);
    TEST ("bababababa", 1, 0);
    TEST ("aaaaabaaaa", 0, 5);
    TEST ("bbbbbabbbb", 5, 0);

#undef TEST
}

/**************************************************************************/

/* extern */ int rw_opt_no_min;                 // --no-min
/* extern */ int rw_opt_no_max;                 // --no-max
/* extern */ int rw_opt_no_min_element;         // --no-min_element
/* extern */ int rw_opt_no_max_element;         // --no-max_element
/* extern */ int rw_opt_no_predicate;           // --no-predicate
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class T, class Predicate>
void test_min_max_element (const T*,
                           const Predicate *ppred,
                           bool             test_min)
{
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0,
             "template <class %s%{?}, class %s%{;}> "
             "%1$s std::%s (%1$s, %1$s%{?}, %s%{;})",
             "ForwardIterator", ppred, "Compare",
             test_min ? "min_element" : "max_element", ppred, "Compare");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_min_max_element (fwd_iter, (T*)0, ppred, test_min);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, 0, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_min_max_element (bidir_iter, (T*)0, ppred, test_min);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, 0, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_min_max_element (rand_iter, (T*)0, ppred, test_min);
    }
}

/**************************************************************************/

template <class T, class Predicate>
void test_alg_min_max (const T*,
                       const Predicate *ppred)
{
    if (rw_opt_no_min) {
        rw_note (0, 0, __LINE__,  "std::min test disabled");
    }
    else {
        test_min_max ((T*)0, ppred, true);
    }

    if (rw_opt_no_max) {
        rw_note (0, 0, __LINE__, "std::max test disabled");
    }
    else {
        test_min_max ((T*)0, ppred, false);
    }

    if (rw_opt_no_min_element) {
        rw_note (0, 0, __LINE__, "std::min_element test disabled");
    }
    else {
        test_min_max_element ((T*)0, ppred, true);
    }

    if (rw_opt_no_max_element) {
        rw_note (0, 0, __LINE__, "std::max_element test disabled");
    }
    else {
        test_min_max_element ((T*)0, ppred, false);
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_alg_min_max ((UserClass*)0, (Less<UserClass>*)0);

    if (rw_opt_no_predicate) {
        rw_note (0, 0, __LINE__, "std.alg.min.max predicate tests disabled");
    }
    else {
        test_alg_min_max ((UserClass*)0, (Less<UserClass>*)1);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.min.max",
                    0 /* no comment */, run_test,
                    "|-no-min#"
                    "|-no-max#"
                    "|-no-min_element#"
                    "|-no-max_element#"
                    "|-no-predicate#"
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator",
                    &rw_opt_no_min,
                    &rw_opt_no_max,
                    &rw_opt_no_min_element,
                    &rw_opt_no_max_element,
                    &rw_opt_no_predicate,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
