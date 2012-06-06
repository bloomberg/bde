/***************************************************************************
 *
 * 25.search.cpp - test exercising 25.1.9 [lib.alg.search]
 *
 * $Id: 25.search.cpp 510970 2007-02-23 14:57:45Z faridz $
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
 * Copyright 1994-2005 Rogue Wave Software.
 *
 **************************************************************************/

#include <algorithm>    // for search, search_n
#include <cstring>      // for strlen

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

// instantiate each algorithm on all kinds of iterators
// it is required to work with (the algorithm may delegate
// to different implementations specialized for each kind
// of iterator)
template
FwdIter<eq_comp<base<> > >
search (FwdIter<eq_comp<base<> > >,
        FwdIter<eq_comp<base<> > >,
        FwdIter<eq_comp<base<> > >,
        FwdIter<eq_comp<base<> > >);

template
BidirIter<eq_comp<base<> > >
search (BidirIter<eq_comp<base<> > >,
        BidirIter<eq_comp<base<> > >,
        BidirIter<eq_comp<base<> > >,
        BidirIter<eq_comp<base<> > >);

template
RandomAccessIter<eq_comp<base<> > >
search (RandomAccessIter<eq_comp<base<> > >,
        RandomAccessIter<eq_comp<base<> > >,
        RandomAccessIter<eq_comp<base<> > >,
        RandomAccessIter<eq_comp<base<> > >);

template
FwdIter<eq_comp<base<> > >
search (FwdIter<eq_comp<base<> > >,
        FwdIter<eq_comp<base<> > >,
        FwdIter<eq_comp<base<> > >,
        FwdIter<eq_comp<base<> > >,
        binary_predicate<eq_comp<base<> > >);

template
BidirIter<eq_comp<base<> > >
search (BidirIter<eq_comp<base<> > >,
        BidirIter<eq_comp<base<> > >,
        BidirIter<eq_comp<base<> > >,
        BidirIter<eq_comp<base<> > >,
        binary_predicate<eq_comp<base<> > >);

template
RandomAccessIter<eq_comp<base<> > >
search (RandomAccessIter<eq_comp<base<> > >,
        RandomAccessIter<eq_comp<base<> > >,
        RandomAccessIter<eq_comp<base<> > >,
        RandomAccessIter<eq_comp<base<> > >,
        binary_predicate<eq_comp<base<> > >);

template
FwdIter<eq_comp<base<> > >
search_n (FwdIter<eq_comp<base<> > >,
          FwdIter<eq_comp<base<> > >,
          Size<int>, const eq_comp<base<> >&);

template
BidirIter<eq_comp<base<> > >
search_n (BidirIter<eq_comp<base<> > >,
          BidirIter<eq_comp<base<> > >,
          Size<int>, const eq_comp<base<> >&);

template
RandomAccessIter<eq_comp<base<> > >
search_n (RandomAccessIter<eq_comp<base<> > >,
          RandomAccessIter<eq_comp<base<> > >,
          Size<int>, const eq_comp<base<> >&);

template
FwdIter<eq_comp<base<> > >
search_n (FwdIter<eq_comp<base<> > >,
          FwdIter<eq_comp<base<> > >,
          Size<int>, const eq_comp<base<> >&,
          binary_predicate<eq_comp<base<> > >);

template
BidirIter<eq_comp<base<> > >
search_n (BidirIter<eq_comp<base<> > >,
          BidirIter<eq_comp<base<> > >,
          Size<int>, const eq_comp<base<> >&,
          binary_predicate<eq_comp<base<> > >);

template
RandomAccessIter<eq_comp<base<> > >
search_n (RandomAccessIter<eq_comp<base<> > >,
          RandomAccessIter<eq_comp<base<> > >,
          Size<int>, const eq_comp<base<> >&,
          binary_predicate<eq_comp<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T, class U>
struct EqualityPredicate
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class
    // from being default constructible
    EqualityPredicate (T* /* dummy */, U* /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    class ConvertibleToBool {
        bool result_;
    public:
        ConvertibleToBool (bool res): result_ (res) { /* empty */ }
        operator bool() const { return result_; }
    };

    ConvertibleToBool operator() (const T &x, const U &y) /* non-const */ {
        ++funcalls_;
        return x == y;
    }
};

template <class T, class U>
std::size_t EqualityPredicate<T, U>::funcalls_;

/**************************************************************************/

// tags to select predicate or non-predicate versions
template <class T>
struct PredicateTag {
    enum { pred_inx = 1 };
    int dummy;

    typedef EqualityPredicate<T, T> Predicate;
};

template <class T>
struct NonPredicateTag {
    enum { pred_inx = 0 };
    int dummy;

    // dummy predicate to be used for non-predicate
    // versions as a placeholder
    struct DummyPredicate {
        DummyPredicate(T*, T*) {}

        bool operator() (const T& , const T& ) {
            return false;
        }

        static std::size_t funcalls_;
    };

    typedef DummyPredicate Predicate;
};

template <class T>
std::size_t NonPredicateTag<T>::DummyPredicate::funcalls_;


#if defined (_MSC_VER) && _MSC_VER <= 1300
// to prevent MSVC 7.0 error LNK2001: unresolved external symbol "public:
// static unsigned int
// NonPredicateTag<struct UserClass>::DummyPredicate::funcalls_"
std::size_t NonPredicateTag<UserClass>::DummyPredicate::funcalls_;
#endif

/**************************************************************************/

// exercises std::search()
template
<class ForwardIterator1, class ForwardIterator2, class T, class PredTag>
void test_search (int line,
                  const char *seq1, const char *seq2,
                  std::size_t off,
                  ForwardIterator1 it1, ForwardIterator2 it2,
                  const T* , PredTag pred_tag)
{
    const char* const fname = "search";
    static const char* const it1name  = type_name (it1, (T*)0);
    static const char* const it2name  = type_name (it2, (T*)0);
    static const char* const predname =
        pred_tag.pred_inx ? "BinaryPredicate" : "operator <()";

    const std::size_t nseq1 = std::strlen (seq1);
    const std::size_t nseq2 = std::strlen (seq2);

    // construct a sequence of `nsrc' elements to pass to search
    T* const xseq1 = T::from_char (seq1, nseq1);
    T* const xseq2 = T::from_char (seq2, nseq2);

    // construct iterators pointing to the beginning and end
    // of the source sequences
    const ForwardIterator1 first1 =
        make_iter (xseq1, xseq1, xseq1 + nseq1, it1);
    const ForwardIterator1 last1  =
        make_iter (xseq1 + nseq1, xseq1, xseq1 + nseq1, it1);

    const ForwardIterator1 expected = _RWSTD_SIZE_MAX == off ?
          last1
        : make_iter (xseq1 + off, xseq1, xseq1 + off, it1);

    _RWSTD_UNUSED (expected);

    const ForwardIterator2 first2 =
        make_iter (xseq2, xseq2, xseq2 + nseq2, it2);
    const ForwardIterator2 last2  =
        make_iter (xseq2 + nseq2, xseq2, xseq2 + nseq2, it2);

    T::n_total_op_eq_ = 0;
    PredTag::Predicate::funcalls_ = 0;

    typename PredTag::Predicate pred(0, 0);

    const ForwardIterator1 result = pred_tag.pred_inx ?
        std::search (first1, last1, first2, last2, pred)
      : std::search (first1, last1, first2, last2);

    _RWSTD_UNUSED (result);

    // 25.1.9, p2:
    // check the returned iterator
    rw_assert (result.cur_ == expected.cur_, 0, line,
               "std::%s<%s, %s%{?}, %s%{;}> (\"%s\", ..., \"%s\") "
               "found subsequence at %td, expected at %{?}end%{;}%zu%{;}",
               fname, it1name, it2name, pred_tag.pred_inx, predname,
               seq1, seq2, result.cur_ - first1.cur_,
               _RWSTD_SIZE_MAX == off, off);

    // 25.1.9, p3:
    // Complexity: At most (last1 ­ first1) * (last2 ­ first2)
    // applications of the corresponding predicate or operator
    const std::size_t max_op_eq = nseq1 * nseq2;
    const std::size_t op_called = pred_tag.pred_inx ?
        PredTag::Predicate::funcalls_
      : T::n_total_op_eq_;

    rw_assert (op_called <= max_op_eq, 0, line,
               "std::%s<%s, %s%{?}, %s%{;}> (\"%s\", ..., \"%s\") "
               "called %s %zu times, expected no more than %zu",
               fname, it1name, it2name, pred_tag.pred_inx, predname,
               seq1, seq2, predname, op_called, max_op_eq);

    delete[] xseq1;
    delete[] xseq2;
}

template
<class ForwardIterator1, class ForwardIterator2, class T, class PredTag>
void test_search (ForwardIterator1 it1, ForwardIterator2 it2,
                  const T* , PredTag pred_tag)
{
    static const char* const it1name  = type_name (it1, (T*)0);
    static const char* const it2name  = type_name (it2, (T*)0);
    static const char* const predname = "EqualityPredicate";

    if (pred_tag.pred_inx) {  // use predicate
        rw_info (0, 0, 0,
                 "std::search (%s, %1$s, %s, %2$s, %s)",
                 it1name, it2name, predname);
    }
    else {   // not use predicate
        rw_info (0, 0, 0,
                 "std::search (%s, %1$s, %s, %2$s)",
                 it1name, it2name);
    }

#define TEST(seq1, seq2, off)                                   \
    test_search (__LINE__, seq1, seq2, std::size_t (off),       \
                 it1, it2, (T*)0, pred_tag)

    //
    //    +--------------- sequence to search through
    //    |    +---------- subsequence to search for
    //    |    |      +--- match found at offset
    //    v    v      v
    TEST ("", "",     0);
    TEST ("", "a",   -1);
    TEST ("", "ab",  -1);
    TEST ("", "abc", -1);

    TEST ("a",  "ab",   -1);
    TEST ("a",  "a",     0);
    TEST ("ab",  "b",    1);
    TEST ("ab",  "bb",  -1);

    TEST ("abc", "a",  0);
    TEST ("abc", "b",  1);
    TEST ("abc", "c",  2);

    TEST ("abc", "ac", -1);
    TEST ("abc", "ab",  0);
    TEST ("abc", "bc",  1);
    TEST ("abc", "cd", -1);
    TEST ("abc", "abc", 0);

    TEST ("abcd", "ab",   0);
    TEST ("abcd", "bc",   1);
    TEST ("abcd", "cd",   2);
    TEST ("abcd", "abc",  0);
    TEST ("abcd", "bcd",  1);
    TEST ("abcd", "bce", -1);

    TEST ("abcde", "",  0);
    TEST ("abcde", "a", 0);
    TEST ("abcde", "b", 1);
    TEST ("abcde", "c", 2);
    TEST ("abcde", "d", 3);
    TEST ("abcde", "e", 4);

    TEST ("abcde", "ab",  0);
    TEST ("abcde", "bc",  1);
    TEST ("abcde", "cd",  2);
    TEST ("abcde", "de",  3);
    TEST ("abcde", "ef", -1);

    TEST ("abcde", "abc",  0);
    TEST ("abcde", "bcd",  1);
    TEST ("abcde", "cde",  2);
    TEST ("abcde", "def", -1);

    TEST ("abcde", "abcd",  0);
    TEST ("abcde", "bcde",  1);
    TEST ("abcde", "cdef", -1);

    TEST ("abcde", "abcde",  0);
    TEST ("abcde", "bcdef", -1);

    TEST ("abcabfg", "ab",  0);
    TEST ("abcbcfg", "bc",  1);
    TEST ("abcdecd", "cd",  2);
    TEST ("abcdede", "de",  3);
}


/**************************************************************************/

// exercises std::search_n()
template <class ForwardIterator, class T, class Size, class PredTag>
void test_search_n (int line, const char *seq, Size cnt, const char val,
                    std::size_t off, ForwardIterator it,
                    const T* , PredTag pred_tag)
{
    const char* const fname = "search_n";
    static const char* const itname  = type_name (it, (T*)0);
    static const char* const szname  = "Size";
    static const char* const tname   = "UserClass";
    static const char* const predname =
        pred_tag.pred_inx ? "BinaryPredicate" : "operator <()";

    const std::size_t nseq = std::strlen (seq);

    // construct a sequence of `nsrc' elements to pass to search_n
    T* const xseq = T::from_char (seq, nseq);

    // construct iterators pointing to the beginning and end
    // of the source sequence
    const ForwardIterator first =
        make_iter (xseq, xseq, xseq + nseq, it);
    const ForwardIterator last  =
        make_iter (xseq + nseq, xseq, xseq + nseq, it);

    const ForwardIterator expected = _RWSTD_SIZE_MAX == off ?
          last
        : make_iter (xseq + off, xseq, xseq + off, it);

    _RWSTD_UNUSED (expected);

    T value;
    value.data_.val_ = val;

    T::n_total_op_eq_ = 0;
    PredTag::Predicate::funcalls_ = 0;

    typename PredTag::Predicate pred(0, 0);

    const ForwardIterator result = pred_tag.pred_inx ?
        std::search_n (first, last, cnt, value, pred)
      : std::search_n (first, last, cnt, value);

    _RWSTD_UNUSED (result);

    // 25.1.9, p6:
    // check the returned iterator
    rw_assert (result.cur_ == expected.cur_, 0, line,
               "std::%s<%s, %s, %s%{?}, %s%{;}> (\"%s\", ..., %d, '%c') "
               "found subsequence at %td, expected at %{?}end%{:}%zu%{;}",
               fname, itname, szname, tname, pred_tag.pred_inx, predname,
               seq, int (cnt), val, result.cur_ - first.cur_,
               _RWSTD_SIZE_MAX == off, off);

    // 25.1.9, p7:
    // Complexity: At most (last ­ first) * count
    // applications of the corresponding predicate or operator.
    // In real cases the complexity should be at most (last ­ first)
    const std::size_t max_op_eq = nseq * cnt;
    const std::size_t op_called = pred_tag.pred_inx ?
        PredTag::Predicate::funcalls_
      : T::n_total_op_eq_;

    rw_assert (op_called <= max_op_eq, 0, line,
               "std::%s<%s, %s, %s%{?}, %s%{;}> (\"%s\", ..., %d, '%c') "
               "called %s %zu times, expected no more than %zu",
               fname, itname, szname, tname, pred_tag.pred_inx, predname,
               seq, int (cnt), val, predname, op_called, max_op_eq);

    delete[] xseq;
}

template <class ForwardIterator, class T, class Size, class PredTag>
void test_search_n (ForwardIterator it, const T* ,
                    const Size* , PredTag pred_tag)
{
    static const char* const itname   = type_name (it, (T*)0);
    static const char* const szname   = "Size";
    static const char* const tname    = "UserClass";
    static const char* const predname = "EqualityPredicate";

    if (pred_tag.pred_inx) {  // use predicate
        rw_info (0, 0, 0,
                 "std::search_n (%s, %1$s, %s, const %s&, %s)",
                 itname, szname, tname, predname);
    }
    else {   // not use predicate
        rw_info (0, 0, 0,
                 "std::search_n (%s, %1$s, %s, const %s&)",
                 itname, szname, tname);
    }

#undef TEST
#define TEST(seq, n, val, off)                                        \
    test_search_n (__LINE__, seq, Size(n, 0), val, std::size_t (off), \
                   it, (T*)0, pred_tag)

    //
    //    +----------------- sequence to search through
    //    |    +------------ number of occurrences
    //    |    |   +-------- element value to match
    //    |    |   |    +--- match found at offset
    //    v    v   v    v
    TEST ("",  0, 'x',  0);
    TEST ("",  1, 'x', -1);
    TEST ("",  2, 'x', -1);
    TEST ("", -1, 'x', -1);

    TEST ("A", 0, 'B',  0);
    TEST ("A", 0, 'A',  0);
    TEST ("A", 1, 'B', -1);
    TEST ("A", 1, 'A',  0);

    TEST ("AB", 1, 'A',  0);
    TEST ("AB", 2, 'A', -1);
    TEST ("AB", 1, 'B',  1);
    TEST ("AB", 2, 'B', -1);

    TEST ("ABC", 0, 'A', 0);
    TEST ("ABC", 0, 'B', 0);
    TEST ("ABC", 0, 'C', 0);
    TEST ("ABC", 0, 'D', 0);

    TEST ("ABC", 1, 'A',  0);
    TEST ("ABC", 1, 'B',  1);
    TEST ("ABC", 1, 'C',  2);
    TEST ("ABC", 1, 'D', -1);

    TEST ("ABC", 2, 'A', -1);
    TEST ("ABC", 2, 'B', -1);
    TEST ("ABC", 2, 'C', -1);
    TEST ("ABC", 2, 'D', -1);

    TEST ("ABC", -1, 'A', -1);
    TEST ("ABC", -2, 'B', -1);
    TEST ("ABC", -3, 'C', -1);
    TEST ("ABC", -4, 'D', -1);

    TEST ("ABAD", 1, 'A', 0);
    TEST ("ABCB", 1, 'B', 1);
    TEST ("ABCC", 1, 'C', 2);

    TEST ("ABAACAAAD", 2, 'A', 2);
    TEST ("ABCBBDBBB", 2, 'B', 3);

    TEST ("ABAACAADE", 3, 'A', -1);
    TEST ("ABCBBDBBE", 3, 'B', -1);
    TEST ("ABAACAAAD", 3, 'A',  5);
    TEST ("ABCBBDBBB", 3, 'B',  6);
}

/**************************************************************************/

/* extern */ int rw_opt_no_search;             // --no-search
/* extern */ int rw_opt_no_search_n;           // --no-search_n
/* extern */ int rw_opt_no_fwd_iter;           // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;         // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;           // --no-RandomAccessIterator
/* extern */ int rw_opt_no_predicate;          // --no-Predicate


template <class ForwardIterator, class T, class PredTag>
void test_search (ForwardIterator it, const T*, PredTag pred_tag)
{
    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_search (it, FwdIter<T>(), (T*)0, pred_tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_search (it, BidirIter<T>(), (T*)0, pred_tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_search (it, RandomAccessIter<T>(), (T*)0, pred_tag);
    }
}

/**************************************************************************/

template <class T, class PredTag>
void test_search (const T*, PredTag pred_tag)
{
    rw_info (0, 0, 0,
             "template <class %s, class %1$s%{?}, class %s%{;}> "
             "std::search (%1$s, %1$s, %1$s, %1$s%{?}, %3$s%{;})",
             "ForwardIterator", pred_tag.pred_inx, "BinaryPredicate",
             pred_tag.pred_inx);

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_search (FwdIter<T>(), (T*)0, pred_tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_search (BidirIter<T>(), (T*)0, pred_tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_search (RandomAccessIter<T>(), (T*)0, pred_tag);
    }
}

/**************************************************************************/

template <class T, class Size, class PredTag>
void test_search_n(const T* , Size* , PredTag pred_tag)
{
//    rw_info (0, 0, 0,
//             "template <class %s, class %s, class %s%{?}, class %s%{;}> "
//             "std::search_n (%1$s, %1$s, %2$s, %s%{?}, %4$s%{;})",
//             "ForwardIterator", "Size", "T",
//             pred_tag.pred_inx, "BinaryPredicate", "const T&",
//             pred_tag.pred_inx);

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_search_n (FwdIter<T>(), (T*)0, (Size*)0, pred_tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_search_n (BidirIter<T>(), (T*)0, (Size*)0, pred_tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_search_n (RandomAccessIter<T>(), (T*)0, (Size*)0, pred_tag);
    }
}

/**************************************************************************/

template <class T>
void test_search (const T*)
{
    NonPredicateTag<T> non_predicate_tag = { false };
    PredicateTag<T> predicate_tag = { true };

    test_search ((T*)0, non_predicate_tag);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, "std::search predicate test disabled");
    }
    else {
        test_search ((T*)0, predicate_tag);
    }
}

/**************************************************************************/

template <class T>
void test_search_n (const T*)
{
    NonPredicateTag<T> non_predicate_tag = { false };
    PredicateTag<T> predicate_tag = { true };

    test_search_n ((T*)0, (Size<int>*)0, non_predicate_tag);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "std::search_n predicate test disabled");
    }
    else {
        test_search_n ((T*)0, (Size<int>*)0, predicate_tag);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if (rw_opt_no_search) {
        rw_note (0, __FILE__, __LINE__,  "std::search test disabled");
    }
    else {
        test_search ((UserClass*)0);
    }

    if (rw_opt_no_search_n) {
        rw_note (0, __FILE__, __LINE__,  "std::search_n test disabled");
    }
    else {
        test_search_n ((UserClass*)0);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.search",
                    0 /* no comment */, run_test,
                    "|-no-search# "
                    "|-no-search_n# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator# "
                    "|-no-Predicate",
                    &rw_opt_no_search,
                    &rw_opt_no_search_n,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
}
