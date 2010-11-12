/***************************************************************************
 *
 * 25.nth.element.cpp - test exercising 25.3.2 [lib.alg.nth.element]
 *
 * $Id: 25.nth.element.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for nth_element, 
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void
nth_element (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >, 
             RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
             RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void 
nth_element (RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >, 
             RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
             RandomAccessIter<lt_comp<assign<base<cpy_ctor> > > >,
             binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T>
struct StrictWeakLess 
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    StrictWeakLess (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    conv_to_bool operator() (const T &x, const T &y) /* non-const */ {
        ++funcalls_;
        return conv_to_bool::make (x.data_.val_ < y.data_.val_);
    }

    static const char* name () { return "StrictWeakLess"; }

private:
    void operator= (StrictWeakLess&);   // not assignable
};

template<class T>
std::size_t StrictWeakLess<T>::funcalls_;

/**************************************************************************/

// exrcises nth_element
template <class T, class Predicate>
void test_nth_element (int                 line,
                       const char         *src,
                       const std::size_t   N,
                       const std::size_t   nth,
                       const T*,
                       const Predicate    *ppred)
{
    _RWSTD_UNUSED (ppred);

    typedef RandomAccessIter<T> RandIter;

    const RandIter it(0, 0, 0);

    const char* const itname = type_name (it, (T*)0);
    const char* const fname = "nth_element";
    const char* const funname = Predicate::name();

    const std::size_t nsrc = src ? std::strlen (src) : N;

    T::gen_ = gen_rnd;
    T* const xsrc = src ? T::from_char (src, nsrc) : new T[nsrc];

    T* const xsrc_end = xsrc + nsrc;

    //                                 current,    first, last
    const RandIter first  = make_iter (xsrc,       xsrc,  xsrc_end, it);
    const RandIter nth_it = make_iter (xsrc + nth, xsrc,  xsrc_end, it);
    const RandIter last   = make_iter (xsrc_end,   xsrc,  xsrc_end, it);

    Predicate pred (0, 0);

    const std::size_t last_n_op_lt = T::n_total_op_lt_; 

    if (ppred)
        std::nth_element (first, nth_it, last, pred);
    else
        std::nth_element (first, nth_it, last);

    // check the results, 25.3.2, p1
    bool success = true;
    std::size_t i = 0;
    std::size_t j = 0;

    for (i = 0; i < nth; i++) {
        for (j = nth; j < nsrc; j++) {
            success = xsrc[i].data_.val_ <= xsrc[j].data_.val_;
            if (!success)
                break;
        }

        if (!success)
            break;
    }

    // to avoid errors in --trace mode
    i = i < nth ? i : nth - 1;
    j = j < nsrc ? j : nsrc - 1;

    if (src) {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}>(\"%s\", %zu, ...) ==> "
                   "\"%{X=*.*}\", got %#c at %zu !< %#c at %zu",
                   __LINE__, fname, itname, ppred, funname, src, nth, 
                   int (nsrc), -1, xsrc, xsrc[i].data_.val_, i,
                   xsrc[j].data_.val_, j);
    }
    else {
        rw_assert (success, 0, line,
                   "line %d: %s<%s%{?}, %s%{;}>(%zu, %zu, ...) "
                   "got %d at %zu !< %d at %zu",
                   __LINE__, fname, itname, ppred, funname, N, nth, 
                   xsrc[i].data_.val_, i, xsrc[j].data_.val_, j);
    }

    // check the complexity, 25.3.2, p2
    // it should be linear, i.e. <= K * N, there K << N
    // after tests on random sequences use K == 8 as upper bound
    std::size_t n_ops = 
        ppred ? Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;
    std::size_t exp_ops = 8 * nsrc;

    success = n_ops <= exp_ops;

    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s%{;}>(%{?}\"%s\"%{;}%{?}%zu%{;}, "
               "%zu, ...): complexity: got %zu, expected no more than %zu",
               __LINE__, fname, itname, ppred, funname, 
               src, src, !src, N, nth, n_ops, exp_ops);

    delete[] xsrc;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 256;            // --nloops=#
/* extern */ int rw_opt_no_predicate;            // --no-predicate
/* extern */ int rw_opt_no_complexity;           // --no-complexity

/**************************************************************************/

template <class T, class Predicate>
void test_nth_element (const T*,
                       const Predicate *ppred)
{
    typedef RandomAccessIter<T> RandIter;

    const char* const itname  = type_name (RandIter (0, 0, 0), (T*)0);
    const char* const fname   = "nth_element";
    const char* const funname = Predicate::name();

    rw_info (0, 0, 0,
             "std::%s (%s, %2$s, %2$s%{?}, %s%{;})",
             fname, itname, ppred, funname);

#define TEST(src, nth)                                                  \
    test_nth_element (__LINE__, src, 0, nth, (T*)0, ppred)

    TEST ("a", 0);
    TEST ("aa", 1);
    TEST ("ab", 0);

    TEST ("jihgfedcba", 0);
    TEST ("jihgfedcba", 1);
    TEST ("jihgfedcba", 2);
    TEST ("jihgfedcba", 3);
    TEST ("jihgfedcba", 4);
    TEST ("jihgfedcba", 5);
    TEST ("jihgfedcba", 6);
    TEST ("jihgfedcba", 7);
    TEST ("jihgfedcba", 8);
    TEST ("jihgfedcba", 9);

    TEST ("abcdefghij", 0);
    TEST ("abcdefghij", 1);
    TEST ("abcdefghij", 2);
    TEST ("abcdefghij", 3);
    TEST ("abcdefghij", 4);
    TEST ("abcdefghij", 5);
    TEST ("abcdefghij", 6);
    TEST ("abcdefghij", 7);
    TEST ("abcdefghij", 8);
    TEST ("abcdefghij", 9);

    TEST ("jijgfgdcda", 5);
    TEST ("jihifefcbc", 5);
    TEST ("jiigffdcca", 5);
    TEST ("jihhfeecbb", 5);

    TEST ("cbbbbbbbba", 5);
    TEST ("ccbbbbbbaa", 5);

    TEST ("abadedghgj", 5);
    TEST ("abcbefehih", 5);
    TEST ("abbdeeghhj", 5);
    TEST ("abcceffhii", 5);

    TEST ("abbbbbbbbc", 5);
    TEST ("aabbbbbbcc", 5);

#undef TEST

    if (rw_opt_no_complexity) {
        rw_note (0, 0, 0,
                 "std::%s (%s, %2$s, %2$s%{?}, %s%{;}): "
                 "complexity test disabled",
                 fname, itname, ppred, funname);
    }
    else {
        rw_info (0, 0, 0,
                 "std::%s (%s, %2$s, %2$s%{?}, %s%{;}): complexity test",
                 fname, itname, ppred, funname);

        const std::size_t N = std::size_t (rw_opt_nloops);
        for (std::size_t i = 1; i < N; i++)
            test_nth_element (__LINE__, 0, i, i / 2, (T*)0, ppred);
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    rw_info (0, 0, 0,
             "template <class %s> "
             "void std::nth_element (%1$s, %1$s, %1$s)",
             "RandomAccessIterator");

    test_nth_element ((UserClass*)0, (StrictWeakLess<UserClass>*)0);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, 
                 "std::nth_element predicate test disabled");
    }
    else {
        rw_info (0, 0, 0,
                 "template <class %s, class %s> "
                 "void std::nth_element (%1$s, %1$s, %1$s, %2$s)",
                 "RandomAccessIterator", "StrictWeakLess");

        const StrictWeakLess<UserClass> pred(0, 0);
        test_nth_element ((UserClass*)0, &pred);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.nth_element",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-predicate#"
                    "|-no-complexity",
                    &rw_opt_nloops,
                    &rw_opt_no_predicate,
                    &rw_opt_no_complexity);
}
