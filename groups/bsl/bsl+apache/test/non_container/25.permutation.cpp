/***************************************************************************
 *
 * 25.permutation.cpp - test exercising lib.alg.permutation.generators
 *
 * $Id: 25.permutation.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for prev_permutation, next_permutation
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
bool
prev_permutation (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
                  BidirIter<lt_comp<assign<base<cpy_ctor> > > >);

template
bool
prev_permutation (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
                  BidirIter<lt_comp<assign<base<cpy_ctor> > > >,          
                  binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

template
bool
next_permutation (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
                  BidirIter<lt_comp<assign<base<cpy_ctor> > > >);

template
bool
next_permutation (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
                  BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
                  binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION

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

// exercise prev_permutation and next_permutation 25.3.9
template <class T, class BidirectIterator, class Predicate>
void test_permutations (int                     line,
                        const char             *src,
                        const char             *dst,
                        bool                    res,
                        const BidirectIterator &it,
                        const T*,
                        const Predicate        *ppred,
                        bool                    prev)
{
    const char* const itname   = type_name (it, (T*)0);
    const char* const algname  = prev ? "prev_permutation" : "next_permutation";
    const char* const predname = Predicate::name ();

    const std::size_t nsrc = std::strlen (src);

    T* const xsrc = T::from_char (src, nsrc);
    T* const xsrc_end = xsrc + nsrc;

    const BidirectIterator first = make_iter (xsrc,     xsrc, xsrc_end, it);
    const BidirectIterator last  = make_iter (xsrc_end, xsrc, xsrc_end, it);

    const Predicate pred (0, 0);

    const std::size_t last_n_op_assign = T::n_total_op_assign_;

    bool result = false;

    if (ppred) {
        result = prev ? 
            std::prev_permutation (first, last, pred) 
          : std::next_permutation (first, last, pred);
    }
    else {
        result = prev ? 
            std::prev_permutation (first, last) 
          : std::next_permutation (first, last);
    }

    std::size_t n_ops_assign = T::n_total_op_assign_ - last_n_op_assign;

    // verify the returned value: 25.3.9, p1 and p4
    bool success = result == res;
    rw_assert (success, 0, line,
               "line %d. %s <%s%{?}, %s%{;}> (\"%s\", ...) == %b, got %b",
               __LINE__, algname, itname, 0 != ppred, predname, 
               src, result, res);

    // verify the permutation result
    std::size_t i = 0;
    for ( ; i < nsrc; i++) {
        success = xsrc [i].data_.val_ == dst [i];
        if (!success)
            break;
    }

    // to avoid errors in --trace mode
    i = i < nsrc ? i : nsrc - 1;

    rw_assert (success, 0, line,
               "line %d. %s <%s%{?}, %s%{;}> (\"%s\", ...) "
               "==> got \"%{X=*.*}\", expected \"%s\"",
               __LINE__, algname, itname, 0 != ppred, predname, src,
               int (nsrc), int (i), xsrc, dst);

    // verify the complexity: 25.3.9 p2 and p5.
    // (last - first) / 2 swaps, 2 assign per swap
    success = n_ops_assign <= nsrc;
    rw_assert (success, 0, line,
               "line %d. %s <%s%{?}, %s%{;}> (\"%s\", ...) "
               "complexity: got %zu assigns, expected no more than %zu",
               __LINE__, algname, itname, 0 != ppred, predname, src, 
               n_ops_assign, nsrc);

    delete[] xsrc;
}

/**************************************************************************/

template <class T, class BidirectIterator, class Predicate>
void test_permutations (const BidirectIterator &it,
                        const T*,
                        const Predicate        *ppred,
                        bool                    prev)
{
    const char* const itname   = type_name (it, (T*)0);
    const char* const algname  = prev ? "prev_permutation" : "next_permutation";
    const char* const predname = Predicate::name();

    rw_info (0, 0, 0,
             "std::%s(%s, %2$s%{?}, %s%{;})",
             algname, itname, 0 != ppred, predname);

#define TEST(src, dst, res)                                                 \
    test_permutations (__LINE__, src, dst, res, it, (T*)0, ppred, prev) 

    if (prev) {
        //     +------------- initial (source) sequence
        //     |     +------- final (destination) sequence
        //     |     |    +-- expected return value
        //     |     |    |
        //     V     V    V
        TEST ("a",  "a",  false);
        TEST ("aa", "aa", false);
        TEST ("ab", "ba", false);
        TEST ("ba", "ab", true);

        TEST ("bcaefigjhd", "bcaefigjdh", true);
        TEST ("bcaefihdgj", "bcaefigjhd", true);

        TEST ("hefdiacjbg", "hefdiacgjb", true);
        TEST ("hefdiacjgb", "hefdiacjbg", true);

        TEST ("aaaaaaaaaa", "aaaaaaaaaa", false);
        TEST ("aaaaabbbbb", "bbbbbaaaaa", false);
        TEST ("ababababab", "abababaabb", true);
        TEST ("bbbbbaaaaa", "bbbbabaaaa", true);

        TEST ("abcdefghij", "jihgfedcba", false);
        TEST ("jihgfedcba", "jihgfedcab", true);
    }
    else {
        TEST ("a", "a", false);
        TEST ("aa", "aa", false);
        TEST ("ab", "ba", true);
        TEST ("ba", "ab", false);

        TEST ("bcaefigjhd", "bcaefihdgj", true);
        TEST ("bcaefigjdh", "bcaefigjhd", true);

        TEST ("hefdiacjbg", "hefdiacjgb", true);
        TEST ("hefdiacgjb", "hefdiacjbg", true);

        TEST ("aaaaaaaaaa", "aaaaaaaaaa", false);
        TEST ("aaaaabbbbb", "aaaababbbb", true);
        TEST ("ababababab", "ababababba", true);
        TEST ("bbbbbaaaaa", "aaaaabbbbb", false);

        TEST ("abcdefghij", "abcdefghji", true);
        TEST ("jihgfedcba", "abcdefghij", false);
    }                        
}

/**************************************************************************/

/* extern */ int rw_opt_no_prev_permutation;    // --no-prev_permutation
/* extern */ int rw_opt_no_next_permutation;    // --no-next_permutation
/* extern */ int rw_opt_no_predicate;           // --no-predicate
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class T, class Predicate>
void test_permutations (const T*,
                        const Predicate* ppred,
                        bool             prev)
{
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0,
             "template <class %s%{?}, class %s%{;}> "
             "bool %s (%1$s, %1$s%{?}, %s%{;})",
             "BidirectionalIterator", 0 != ppred, "Compare", 
             prev ? "prev_permutation" : "next_permutation", 
             0 != ppred, "Compare");

    if (rw_opt_no_bidir_iter) {
        rw_note (0, 0, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_permutations (bidir_iter, (T*)0, ppred, prev);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, 0, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_permutations (rand_iter, (T*)0, ppred, prev);
    }
}

/**************************************************************************/

template <class T, class Predicate>
void test_permutations (const T*,
                        const Predicate* ppred)
{
    if (rw_opt_no_prev_permutation) {
        rw_note (0, 0, __LINE__, "std::prev_permutation test disabled");
    }
    else {
        test_permutations ((UserClass*)0, ppred, true); 
    }

    if (rw_opt_no_next_permutation) {
        rw_note (0, 0, __LINE__, "std::next_permutation test disabled");
    }
    else {
        test_permutations ((UserClass*)0, ppred, false); 
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_permutations ((UserClass*)0, (Less<UserClass>*)0); 

    if (rw_opt_no_predicate) {
        rw_note (0, 0, __LINE__, "predicate test disabled");
    }
    else {
        test_permutations ((UserClass*)0, (Less<UserClass>*)1); 
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.permutation.generators",
                    0 /* no comment */,
                    run_test,
                    "|-no-prev_permutation# "
                    "|-no-next_permutation# "
                    "|-no-predicate# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_prev_permutation,
                    &rw_opt_no_next_permutation,
                    &rw_opt_no_predicate,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
