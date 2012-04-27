/***************************************************************************
 *
 * unique.cpp - test exercising 25.2.8 [lib.alg.unique]
 *
 * $Id: 25.unique.cpp 510071 2007-02-21 15:58:53Z faridz $
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
 * Copyright 2000-2006 Rogue Wave Software.
 *
 **************************************************************************/

#include <algorithm>    // for unique, unique_copy
#include <cstring>      // for strlen

#include <alg_test.h>   // for BinaryPredicate
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

// also exercise LWG issue 241:
// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#241

template
FwdIter<eq_comp<assign<base<> > > >
unique (FwdIter<eq_comp<assign<base<> > > >,
        FwdIter<eq_comp<assign<base<> > > >);

template
FwdIter<eq_comp<assign<base<> > > >
unique (FwdIter<eq_comp<assign<base<> > > >,
        FwdIter<eq_comp<assign<base<> > > >,
        binary_predicate<eq_comp<assign<base<> > > >);

template
OutputIter<eq_comp<assign<base<cpy_ctor> > > >
unique_copy (InputIter<eq_comp<assign<base<cpy_ctor> > > >,
             InputIter<eq_comp<assign<base<cpy_ctor> > > >,
             OutputIter<eq_comp<assign<base<cpy_ctor> > > >);

#if TEST_RW_EXTENSIONS
template
OutputIter<eq_comp<assign<base<cpy_ctor> > > >
unique_copy (InputIter<eq_comp<assign<base<cpy_ctor> > > >,
             InputIter<eq_comp<assign<base<cpy_ctor> > > >,
             OutputIter<eq_comp<assign<base<cpy_ctor> > > >,
             binary_predicate<eq_comp<assign<base<cpy_ctor> > > >);
#endif // TEST_RW_EXTENSIONS

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// exercises std::unique and std::unique_copy()
template <class FwdIterator, class FwdCopyIterator,
          class OutIterator, class T, class Predicate>
void test_unique (int                    line,
                  const char            *src,
                  const char            *dst,
                  const FwdIterator     &it,
                  const FwdCopyIterator &itc,
                  const OutIterator     &out,
                  const T*,
                  const Predicate       *ppred,
                  bool                   use_copy)
{
    const char* const outname = type_name (out, (T*)0);
    const char* const itname =
        use_copy ? type_name (itc, (T*)0) : type_name (it, (T*)0);
    const char* const fname =
        use_copy ? "unique_copy" : "unique";
    const char* const funname =
        ppred ? "BinaryPredicate" : "operator==()";

    const std::size_t nsrc = std::strlen (src);
    const std::size_t ndst = std::strlen (dst);

    T* const xsrc = T::from_char (src, nsrc);
    T*       xdst = use_copy ? T::from_char (dst, ndst) : xsrc;

    T* const xsrc_end = xsrc + nsrc;
    T* const xdst_end = xdst + ndst;

    const FwdIterator first = make_iter (xsrc,     xsrc, xsrc_end, it);
    const FwdIterator last =  make_iter (xsrc_end, xsrc, xsrc_end, it);

    const FwdCopyIterator first_c = make_iter (xsrc,     xsrc, xsrc_end, itc);
    const FwdCopyIterator last_c  = make_iter (xsrc_end, xsrc, xsrc_end, itc);

    const OutIterator result =  make_iter (xdst, xdst, xdst_end, out);

    // compute the actual number of invocations of operator==() or predicate
    std::size_t n_total_ops =
        ppred ? Predicate::n_total_op_fcall_ : T::n_total_op_eq_;

    OutIterator res_c (0, 0, 0);
    FwdIterator res (0, 0, 0);

    if (use_copy)
        res_c = ppred ?
              std::unique_copy (first_c, last_c, result, *ppred)
            : std::unique_copy (first_c, last_c, result);
    else
        res = ppred ?
              std::unique (first, last, *ppred)
            : std::unique (first, last);

    const T* const f_cur = use_copy ? result.cur_ : first.cur_;
    const T* const r_cur = use_copy ? res_c.cur_ : res.cur_;

    bool success = r_cur == f_cur + ndst;
    rw_assert (success, 0, line,
               "line %d: std::%s <%s%{?}, %s%{;}%{?}, %s%{;}>(\"%s\", ...) "
               "== (res + %zu), got (res + %td)",
               __LINE__, fname, itname, use_copy, outname, 0 != ppred,
               funname, src, ndst, r_cur - f_cur);

    // verify that the copied sequence matches the expected range
    const T* const mismatch = T::mismatch (xdst, dst, ndst);

    rw_assert (0 == mismatch, 0, line,
               "line %d: std::%s <%s%{?}, %s%{;}%{?}, %s%{;}>(\"%s\", ...) "
               "==> \"%s\", got \"%{X=*.@}\"",
               __LINE__, fname, itname, use_copy, outname, 0 != ppred,
               funname, src, dst, int (ndst), mismatch, xdst);

    if (ppred)
        n_total_ops = ppred->n_total_op_fcall_ - n_total_ops;
    else
        n_total_ops = T::n_total_op_eq_ - n_total_ops;

    // compute the expected number of invocations of operator==() or predicate
    const std::size_t n_expect_ops = nsrc ? nsrc - 1 : 0;

    // verify the number of applications of operator==() or predicate
    rw_assert (n_total_ops <= n_expect_ops, 0, line,
               "line %d: std::%s <%s%{?}, %s%{;}%{?}, %s%{;}>(\"%s\", ...) "
               "%s called %zu times, %zu expected",
               __LINE__, fname, itname, use_copy, outname, 0 != ppred,
               funname, src, funname, n_total_ops, n_expect_ops);
}

/**************************************************************************/

template <class FwdIterator, class FwdCopyIterator,
          class OutIterator, class T, class Predicate>
void test_unique (const FwdIterator     &it,
                  const FwdCopyIterator &itc,
                  const OutIterator     &out,
                  const T*,
                  const Predicate       *ppred,
                  bool                   use_copy)
{
    const char* const outname = type_name (out, (T*)0);
    const char* const itname  = use_copy ?
        type_name (itc, (T*)0) : type_name (it, (T*)0);
    const char* const fname   =  use_copy ? "unique_copy" : "unique";
    const char* const funname = ppred ? "BinaryPredicate" : "operator==()";

    rw_info (0, 0, 0,
             "std::%s (%s, %2$s%{?}, %s%{;}%{?}, %s%{;})",
             fname, itname, use_copy, outname, ppred, funname);

#define TEST(src, dst)                                          \
    test_unique (__LINE__, src, dst, it, itc, out, (T*)0,       \
                 ppred, use_copy)

    //    +---------------- source sequence
    //    |              +- destination sequence
    //    |              |
    //    v              v
    TEST ("",            "");
    TEST ("a",           "a");
    TEST ("ab",          "ab");
    TEST ("abc",         "abc");
    TEST ("abcd",        "abcd");
    TEST ("abcde",       "abcde");
    TEST ("abcdef",      "abcdef");
    TEST ("abcdefg",     "abcdefg");
    TEST ("abcdefgh",    "abcdefgh");
    TEST ("abcdefghi",   "abcdefghi");
    TEST ("abcdefghij",  "abcdefghij");
    TEST ("abcdefghijk", "abcdefghijk");

    TEST ("abcabc",      "abcabc");
    TEST ("abcabcabc",   "abcabcabc");
    TEST ("abcdabcd",    "abcdabcd");
    TEST ("abcdabcdabcd","abcdabcdabcd");

    TEST ("aa",          "a");
    TEST ("aaa",         "a");
    TEST ("aab",         "ab");
    TEST ("aaa",         "a");
    TEST ("aaaa",        "a");
    TEST ("aaabb",       "ab");
    TEST ("aaabbb",      "ab");
    TEST ("abba",        "aba");
    TEST ("abbba",       "aba");
    TEST ("aaabbba",     "aba");
    TEST ("aaabbbaa",    "aba");
    TEST ("aaabbbaaa",   "aba");
    TEST ("aaabbbaaab",  "abab");
    TEST ("aaabbbaaabb", "abab");
    TEST ("abbbbbaaabb", "abab");
    TEST ("abaaaaaaabb", "abab");
    TEST ("ababbbbbbbb", "abab");
}

/**************************************************************************/

/* extern */ int rw_opt_no_unique;             // --no-unique
/* extern */ int rw_opt_no_unique_copy;        // --no-unique_copy
/* extern */ int rw_opt_no_predicate;          // --no-predicate
/* extern */ int rw_opt_no_input_iter;         // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;        // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;           // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;         // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;           // --no-RandomAccessIterator


template <class T, class Predicate, class FwdIterator>
void test_unique (const FwdIterator &it,
                  const T*,
                  const Predicate   *ppred)
{
    static const InputIter<T>        input_iter (0, 0, 0);
    static const OutputIter<T>       output_iter (0, 0, 0);
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_unique (fwd_iter, it, fwd_iter, (T*)0, ppred, true);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_unique (bidir_iter, it, bidir_iter, (T*)0, ppred, true);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_unique (rand_iter, it, rand_iter, (T*)0, ppred, true);
    }

    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_unique (fwd_iter, it, output_iter, (T*)0, ppred, true);
    }
}

/**************************************************************************/

template <class T, class Predicate>
void test_unique (const T*,
                  const Predicate *ppred,
                  bool             use_copy)
{
    static const InputIter<T>        input_iter (0, 0, 0);
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0,
             "template <class %s%{?}, class %s%{;}%{?}, class %s%{;}> "
             "%s std::%s (%1$s, %1$s%{?}, %3$s%{;}%{?}, %5$s%{;})",
             use_copy ? "InputIterator" : "ForwardIterator",
             use_copy, "OutputIterator", 0 != ppred, "Predicate",
             use_copy ? "OutputIterator" : "ForwardIterator",
             use_copy ? "unique_copy" : "unique", use_copy, 0 != ppred);

    if (use_copy)
    {
        if (rw_opt_no_input_iter) {
            rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
        }
        else {
            test_unique (input_iter, (T*)0, ppred);
        }
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        if (use_copy)
            test_unique (fwd_iter, (T*)0, ppred);
        else
            test_unique (fwd_iter, fwd_iter, fwd_iter, (T*)0, ppred, false);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        if (use_copy)
            test_unique (bidir_iter, (T*)0, ppred);
        else
            test_unique (bidir_iter, bidir_iter, bidir_iter,
                        (T*)0, ppred, false);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        if (use_copy)
            test_unique (rand_iter, (T*)0, ppred);
        else
            test_unique (rand_iter, rand_iter, rand_iter,
                         (T*)0, ppred, false);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const BinaryPredicate pred (BinaryPredicate::op_equals);

    if (rw_opt_no_unique) {
        rw_note (0, __FILE__, __LINE__,  "std::unique test disabled");
    }
    else {
        test_unique ((UserClass*)0, (BinaryPredicate*)0, false);

        if (rw_opt_no_predicate) {
           rw_note (0, __FILE__, __LINE__,
                    "std::unique predicate test disabled");
        }
        else {
            test_unique ((UserClass*)0, &pred, false);
        }
    }

    if (rw_opt_no_unique_copy) {
        rw_note (0, __FILE__, __LINE__,  "std::unique_copy test disabled");
    }
    else {
        test_unique ((UserClass*) 0, (BinaryPredicate*)0, true);

        if (rw_opt_no_predicate) {
           rw_note (0, __FILE__, __LINE__,
                    "std::unique_copy predicate test disabled");
        }
        else {
            test_unique ((UserClass*)0, &pred, true);
        }
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.unique",
                    0 /* no comment */, run_test,
                    "|-no-unique# "
                    "|-no-unique_copy# "
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator# "
                    "|-no-predicate",
                    &rw_opt_no_unique,
                    &rw_opt_no_unique_copy,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
}
