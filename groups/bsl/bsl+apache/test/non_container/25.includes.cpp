/***************************************************************************
 *
 * 25.includes.cpp - test exercising 25.3.5.1 [lib.includes]
 *
 * $Id: 25.includes.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for includes
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template 
bool
includes (InputIter<lt_comp<base<no_ctor> > >,
          InputIter<lt_comp<base<no_ctor> > >,
          InputIter<lt_comp<base<no_ctor> > >,
          InputIter<lt_comp<base<no_ctor> > > );

template 
bool
includes (InputIter<lt_comp<base<no_ctor> > >,
          InputIter<lt_comp<base<no_ctor> > >,
          InputIter<lt_comp<base<no_ctor> > >,
          InputIter<lt_comp<base<no_ctor> > >,
          binary_predicate<lt_comp<base<no_ctor> > >);

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

template 
<class T, class InputIterator1, class InputIterator2, class Predicate>
void test_includes (int                     line,
                    const char             *src1,
                    const char             *src2,
                    const bool              res,
                    const InputIterator1   &it1,
                    const InputIterator2   &it2,
                    const T*,
                    const Predicate        *ppred)
{
    const char* const it1name  = type_name (it1, (T*)0);
    const char* const it2name  = type_name (it2, (T*)0);
    const char* const fname    = "includes";
    const char* const predname = Predicate::name();

    const std::size_t nsrc1 = std::strlen (src1);
    const std::size_t nsrc2 = std::strlen (src2);

    T* const xsrc1 = T::from_char (src1, nsrc1);
    T* const xsrc2 = T::from_char (src2, nsrc2);

    T* const xsrc1_end = xsrc1 + nsrc1;
    T* const xsrc2_end = xsrc2 + nsrc2;

    const InputIterator1 first1 = make_iter (xsrc1,     xsrc1, xsrc1_end, it1);
    const InputIterator1 last1  = make_iter (xsrc1_end, xsrc1, xsrc1_end, it1);
    const InputIterator2 first2 = make_iter (xsrc2,     xsrc2, xsrc2_end, it2);
    const InputIterator2 last2  = make_iter (xsrc2_end, xsrc2, xsrc2_end, it2);

    const Predicate pred (0, 0);

    const std::size_t last_n_op_lt = T::n_total_op_lt_;

    const bool result = ppred ?
        std::includes (first1, last1, first2, last2, pred)
      : std::includes (first1, last1, first2, last2);

    // check the returned value
    rw_assert (res == result, 0, line,
               "line %d: %s<%s, %s%{?}, %s%{;}>(\"%s\", \"%s\", ...) "
               "== %b, got %b",
               __LINE__, fname, it1name, it2name, ppred, predname, src1, src2,
               res, result);

    std::size_t n_ops_lt = ppred ? 
        Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;

    // check the complexity
    std::size_t n_exp_ops = nsrc1 + nsrc2 > 0 ? 2 * (nsrc1 + nsrc2) - 1 : 0;
    rw_assert (n_ops_lt <= n_exp_ops, 0, line,
               "line %d: %s<%s, %s%{?}, %s%{;}>(\"%s\", \"%s\", ...) "
               "complexity: got %zu, expected <= %zu",
               __LINE__, fname, it1name, it2name, ppred, predname, src1, src2,
               n_ops_lt, n_exp_ops);

    delete[] xsrc1;
    delete[] xsrc2;
}


/**************************************************************************/

template 
<class T, class InputIterator1, class InputIterator2, class Predicate>
void test_includes (const InputIterator1   &it1,
                    const InputIterator2   &it2,
                    const T*, 
                    const Predicate        *ppred)
{
    const char* const it1name  = type_name (it1, (T*)0);
    const char* const it2name  = type_name (it2, (T*)0);
    const char* const fname    = "includes";
    const char* const predname = Predicate::name ();

    rw_info (0, 0, 0,
             "std::%s(%s, %2$s, %s, %3$s%{?}, %s%{;})",
             fname, it1name, it2name, ppred, predname);

#define TEST(src1, src2, res)                                              \
    test_includes (__LINE__, src1, src2, res, it1, it2, (T*)0, ppred)  

    TEST ("", "", true);
    TEST ("a", "", true);
    TEST ("", "a", false);

    TEST ("a", "a", true);
    TEST ("a", "b", false);
    TEST ("b", "a", false);

    TEST ("aa", "aa", true);
    TEST ("aa", "ab", false);
    TEST ("ab", "aa", false);
    TEST ("ab", "ab", true);

    TEST ("abcdef", "b", true);
    TEST ("abcdef", "bd", true);
    TEST ("abcdef", "i", false);
    TEST ("abcdef", "ei", false);

    TEST ("abcde", "abcde", true);
    TEST ("abbde", "abbde", true);
    TEST ("abcce", "abcce", true);
    TEST ("abcdd", "abcdd", true);

    TEST ("ab", "aaabbbb", false);
    TEST ("ab", "aaabbbc", false);
    TEST ("aaabbbb", "ab", true);
    TEST ("aaabbbc", "ac", true);

    TEST ("a", "aaaaaaaa", false);
    TEST ("a", "aaaaaaab", false);

    TEST ("aaaaaaaa", "aaaaaaaa", true);
    TEST ("aaaaaaab", "aaaaaaab", true);
}

/**************************************************************************/

/* extern */ int rw_opt_no_predicate;           // --no-predicate
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class T, class InputIterator1, class Predicate>
void test_includes (const InputIterator1 &it1,
                    const T*,
                    const Predicate      *ppred)
{
    static const InputIter<T>        input_iter (0, 0, 0);
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_includes (it1, input_iter, (T*)0, ppred);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_includes (it1, fwd_iter, (T*)0, ppred);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_includes (it1, bidir_iter, (T*)0, ppred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_includes (it1, rand_iter, (T*)0, ppred);
    }
}

/**************************************************************************/

template <class T, class Predicate>
void test_includes (const T*,
                    const Predicate *ppred)
{
    static const InputIter<T>        input_iter (0, 0, 0);
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0,
        "template <class %s, class %s%{?}, class %s%{;}> "
             "bool std::includes (%1$s, %1$s, %2$s, %2$s%{?}, %s%{;})",
             "InputIterator1", "InputIterator2", ppred, "Compare", 
             ppred, "Compare");

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_includes (input_iter, (T*)0, ppred);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_includes (fwd_iter, (T*)0, ppred);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_includes (bidir_iter, (T*)0, ppred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_includes (rand_iter, (T*)0, ppred);
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_includes ((UserClass*)0, (Less<UserClass>*)0);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, 
                 "std::includes predicate test disabled");
    }
    else {
        test_includes ((UserClass*)0, (Less<UserClass>*)1);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.includes",
                    0 /* no comment */,
                    run_test,
                    "|-no-predicate#"
                    "|-no-InputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_predicate,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
