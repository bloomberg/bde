/***************************************************************************
 *
 * 25.remove.cpp - test exercising 25.2.7 [lib.alg.remove]
 *
 * $Id: 25.remove.cpp 588637 2007-10-26 13:31:06Z faridz $
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

#include <algorithm>    // for remove(), remove_copy(), ...
#include <cstring>      // for strlen()

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
FwdIter<eq_comp<assign<base<> > > >
remove (FwdIter<eq_comp<assign<base<> > > >,
        FwdIter<eq_comp<assign<base<> > > >,
        const eq_comp<assign<base<> > >&);

template
FwdIter<eq_comp<assign<base<> > > >
remove_if (FwdIter<eq_comp<assign<base<> > > >,
           FwdIter<eq_comp<assign<base<> > > >,
           predicate<eq_comp<assign<base<> > > >);

template
OutputIter<eq_comp<assign<base<> > > >
remove_copy (InputIter<eq_comp<assign<base<> > > >,
             InputIter<eq_comp<assign<base<> > > >,
             OutputIter<eq_comp<assign<base<> > > >,
             const eq_comp<assign<base<> > >&);

template
OutputIter<eq_comp<assign<base<> > > >
remove_copy_if (InputIter<eq_comp<assign<base<> > > >,
                InputIter<eq_comp<assign<base<> > > >,
                OutputIter<eq_comp<assign<base<> > > >,
                predicate<eq_comp<assign<base<> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// names of tested functions
const char* const fnames[] = { "remove", "remove_copy" };
const char* const fnames_if[] = { "remove_if", "remove_copy_if" };

// tags to select remove or remove_copy at compile time
struct RemoveTag {
    enum { fname_inx = 0 };
    int use_predicate;
};

struct RemoveCopyTag {
    enum { fname_inx = 1 };
    int use_predicate;
};

// used as a dummy template argument to test functions exercising remove()
// and remove_if() (to differentiate from remove_copy() and remove_copy_if())
struct NoIterator { };
const char* type_name (NoIterator, const UserClass*)
{
    return 0;
}


typedef unsigned char UChar;


// predicate used as an argument to remove_if() and remove_copy_if()
template <class T>
struct EqualityPredicate
{
    static std::size_t funcalls_;

    EqualityPredicate (const T &val, int /* dummy */)
        : val_ (val) {
        funcalls_ = 0;
    }

    class ConvertibleToBool {
        bool result_;
    public:
        ConvertibleToBool (bool res): result_ (res) { /* empty */ }
        operator bool() const { return result_; }
    };

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    ConvertibleToBool operator() (const T &arg) {
        ++funcalls_;
        return ConvertibleToBool (arg == val_);
    }

private:
    void operator= (EqualityPredicate&);
    const T &val_;
};

template <class T>
std::size_t EqualityPredicate<T>::funcalls_;

/**************************************************************************/

// exercises std::remove() and std::remove_if()
template <class Iterator, class T, class Predicate>
void test_remove (int line,
                  const char *src, const char val, std::size_t nrem,
                  Iterator, NoIterator, const T*,
                  const Predicate*, RemoveTag tag)
{
    static const char* const itname = type_name (Iterator (), (T*)0);
    static const char* const fname = tag.use_predicate ?
        fnames_if [tag.fname_inx] : fnames [tag.fname_inx];

    // compute the length of the source sequence
    const std::size_t nsrc = std::strlen (src);

    // construct a sequence of `nsrc' elements to pass to remove
    T* const xsrc = T::from_char (src, nsrc + 1);

    // construct an element to remove
    T to_remove;
    to_remove.data_.val_ = val;

    // construct a predicate object (used with remove_if() only)
    const Predicate pred (to_remove, 0);

    // construct iterators pointing to the beginning and end
    // of the source sequence
    const Iterator first =
        make_iter (xsrc, xsrc, xsrc + nsrc, Iterator ());

    const Iterator last =
        make_iter (xsrc + nsrc, xsrc, xsrc + nsrc, Iterator ());

    // zero out predicate counters
    T::n_total_op_eq_ = 0;

    // call remove() or remove_if()
    const Iterator end = tag.use_predicate
        ? std::remove_if (first, last, pred)
        : std::remove (first, last, to_remove);

    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
    _RWSTD_UNUSED (end);

    // verify that the returned iterator is set as expected
    bool success = end.cur_ == first.cur_ + (nsrc - nrem);
    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c) == first + %zu, got %zd",
               __LINE__, fname, itname, src, val,
               nsrc - nrem, end.cur_ - xsrc);

    // verify that the value to be removed does not appear
    // anywhere in the range [first, end) : p 25.2.7.2
    success = true;
    for (std::size_t i = 0; i != nsrc - nrem && success; ++i) {
        success = UChar (val) != xsrc [i].data_.val_;
        rw_assert (success, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) ==> "
                   "\"%{X=*.*}\"; expected element value %#c",
                   __LINE__, fname, itname, src, val,
                   int (nsrc), int (i), xsrc, src [i]);
    }


    // verify that the algorithm is stable: the relative order of the elements
    // that are not removed remains unchanged : p 25.2.7.4
    success = true;
    for (std::size_t i = 1; i < nsrc - nrem && success; ++i) {
        success = xsrc [i - 1].id_ < xsrc [i].id_;
        rw_assert (success, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) ==> \"%{X=#*.*}\"; "
                   "unstable at offset %zu element ids: %d and %d",
                   __LINE__, fname, itname, src, val,
                   int (nsrc), int (i - 1), xsrc,
                   i - 1, xsrc [i - 1].id_, xsrc [i].id_);
    }


    // verify that the values of elements in the range [end, last)
    // are unchanged
    success = true;
    for (std::size_t i = nsrc - nrem; i != nsrc && success; ++i) {
        success = src [i] == xsrc [i].data_.val_;
        rw_assert (success, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) ==> "
                   "\"%{X=*.*}\"; expected element value %#c",
                   __LINE__, fname, itname, src, val,
                   int (nsrc), int (i), xsrc, val);
    }


    // verify the number of applications of the predicate: p 25.2.7.5
    if (tag.use_predicate) {
        rw_assert (pred.funcalls_ == nsrc, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) called "
                   "Predicate::operator() %zu times, %zu expected",
                   __LINE__, fname, itname, src, val,
                   pred.funcalls_, nsrc);
    }
    else {
        rw_assert (T::n_total_op_eq_ == nsrc, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) called "
                   "T::operator< %zu times, %zu expected",
                   __LINE__, fname, itname, src, val,
                   T::n_total_op_eq_, nsrc);
    }

    delete[] xsrc;
}

/**************************************************************************/

// exercises std::remove_copy()
template <class Iterator, class OutputIterator, class Predicate, class T>
void test_remove (int line,
                  const char *src, const char val, std::size_t nrem,
                  Iterator it, OutputIterator dummy, const T*,
                  const Predicate*, RemoveCopyTag tag)
{
    static const char* const itname = type_name (it, (T*)0);
    static const char* const fname = tag.use_predicate ?
        fnames_if [tag.fname_inx] : fnames [tag.fname_inx];

    const std::size_t nsrc = std::strlen (src);

    T* const xsrc = T::from_char (src, nsrc);
    T* const xdst = T::from_char (src, nsrc);

    T to_remove;
    to_remove.data_.val_ = val;

    const Predicate pred (to_remove, 0);

    const Iterator first = make_iter (xsrc, xsrc, xsrc + nsrc, it);
    const Iterator last = make_iter (xsrc + nsrc, xsrc, xsrc + nsrc, it);
    const OutputIterator result = make_iter (xdst, xdst, xdst + nsrc, dummy);

    // zero out predicate counters
    T::n_total_op_eq_ = 0;

    const OutputIterator end = tag.use_predicate
        ? std::remove_copy_if (first, last, result, pred)
        : std::remove_copy (first, last, result, to_remove);

    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
    _RWSTD_UNUSED (end);

    // verify that the returned iterator is set as expected p 25.2.7.8
    bool success = end.cur_ == result.cur_ + (nsrc - nrem);
    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c) == first + %zu, got %zd",
               __LINE__, fname, itname, src, val, nsrc - nrem, end.cur_ - xsrc);

    // verify that the value to be removed does not appear anywhere
    // in the range [result, end)
    success = true;
    for (std::size_t i = 0; i != nsrc - nrem && success; ++i) {
        success = UChar (val) != xdst [i].data_.val_;
        rw_assert (success, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) ==> "
                   "\"%{X=*.*}\"; expected element value %#c",
                   __LINE__, fname, itname, src, val,
                   int (nsrc - nrem), int (i), xdst, src [i]);
    }

    // verify that the algorithm is stable: the relative order of the elements
    // that are not removed remains unchanged : p 25.2.7.10
    success = true;
    for (std::size_t i = 1; i < nsrc - nrem && success; ++i) {
        success = xdst [i - 1].id_ < xdst [i].id_;
        rw_assert (success, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) ==> \"%{X=#*.*}\"; "
                   "unstable at offset %zu: element ids: %d and %d",
                   __LINE__, fname, itname, src, val,
                   int (nsrc - nrem), int (i - 1), xdst,
                   i - 1, xdst [i - 1].id_, xdst [i].id_);
    }

    // verify the number of applications of the predicate p 25.2.7.9
    if (tag.use_predicate) {
        rw_assert (pred.funcalls_ == nsrc, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) called "
                   "Predicate::operator() %zu times, %zu expected",
                   __LINE__, fname, itname, src, val,
                   pred.funcalls_, nsrc);
    }
    else {
        rw_assert (T::n_total_op_eq_ == nsrc, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) called "
                   "T::operator< %zu times, %zu expected",
                   __LINE__, fname, itname, src, val,
                   T::n_total_op_eq_, nsrc);
    }

    delete[] xsrc;
    delete[] xdst;
}

/**************************************************************************/

// exercises all four function templates
template <class Iterator1, class Iterator2, class T,
          class Predicate, class Tag>
void test_remove (Iterator1 it1, Iterator2 it2,
                  const T*, const Predicate* pred, Tag tag)
{
    static const char* const it1name = type_name (it1, (T*)0);
    static const char* const it2name = type_name (it2, (T*)0);

    if (tag.fname_inx) {  // remove_copy(), remove_copy_if()
         rw_info (0, 0, 0,
                  "std::%s (%s, %2$s, %s, %s)",
                  tag.use_predicate ? "remove_copy_if" : "remove_copy",
                  it1name, it2name,
                  tag.use_predicate ? "UnaryPredicate" : "const T&");
    }
    else {   // remove, remove_if()
         rw_info (0, 0, 0,
                  "std::%s (%s, %2$s, %s)",
                  tag.use_predicate ? "remove_if" : "remove",
                  it1name,
                  tag.use_predicate ? "UnaryPredicate" : "const T&");
    }

#define TEST(src, val, nremoved) \
    test_remove (__LINE__, src, val, nremoved, it1, it2, (T*)0, pred, tag)

    //    +---------------------- original sequence
    //    |               +------ element to remove
    //    |               |    +- number of removals
    //    |               |    |
    //    v               v    v
    TEST ("",            'a',  0);
    TEST ("a",           'a',  1);
    TEST ("aa",          'a',  2);
    TEST ("aaa",         'a',  3);
    TEST ("aaaa",        'a',  4);
    TEST ("aaaaa",       'a',  5);
    TEST ("aaaaaa",      'a',  6);
    TEST ("aaaaaaa",     'a',  7);
    TEST ("aaaaaaaa",    'a',  8);
    TEST ("aaaaaaaaa",   'a',  9);
    TEST ("aaaaaaaaaa",  'a', 10);

    TEST ("b",           'a',  0);
    TEST ("bb",          'a',  0);
    TEST ("ba",          'a',  1);
    TEST ("ab",          'a',  1);
    TEST ("abc",         'a',  1);
    TEST ("bac",         'a',  1);
    TEST ("bca",         'a',  1);
    TEST ("aba",         'a',  2);
    TEST ("abab",        'a',  2);
    TEST ("ababa",       'a',  3);
    TEST ("bababa",      'a',  3);
    TEST ("bababab",     'a',  3);
    TEST ("babababa",    'a',  4);
    TEST ("baacaadaaeaa",'a',  8);
}

/**************************************************************************/

/* extern */ int rw_opt_no_remove;            // --no-remove
/* extern */ int rw_opt_no_remove_if;         // --no-remove_if
/* extern */ int rw_opt_no_remove_copy;       // --no-remove_copy
/* extern */ int rw_opt_no_remove_copy_if;    // --no-remove_copy_if
/* extern */ int rw_opt_no_input_iter;        // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;       // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;          // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;        // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;          // --no-RandomAccessIterator

/**************************************************************************/

template <class T, class Predicate, class Tag>
void test_remove (const T*, const Predicate* pred, Tag tag)
{
    rw_info (0, 0, 0,
             "template <class %s, class T> "
             "std::%s (%1$s, %1$s, %s)",
             "ForwardIterator",
             tag.use_predicate ? "remove_if" : "remove",
             tag.use_predicate ? "Predicate" : "const T&");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_remove (FwdIter<T>(), NoIterator (), (T*)0, pred, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_remove (BidirIter<T>(), NoIterator (), (T*)0, pred, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_remove (RandomAccessIter<T>(), NoIterator (), (T*)0, pred, tag);
    }
}

/**************************************************************************/

template <class T>
void test_remove (const T*)
{
    const RemoveTag remove_tag    = { false };
    const RemoveTag remove_if_tag = { true };

    if (rw_opt_no_remove) {
        rw_note (0, __FILE__, __LINE__,  "std::remove test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 0;
        test_remove((T*)0, pred, remove_tag);
    }

    if (rw_opt_no_remove_if) {
        rw_note (0, __FILE__, __LINE__,  "std::remove_if test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 1;
        test_remove((T*)0, pred, remove_if_tag);
    }
}

/**************************************************************************/

template <class InputIterator, class T, class Predicate, class Tag>
void test_remove_copy (const InputIterator& iter, const T*,
                       const Predicate* pred, Tag tag)
{
    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_remove (iter, OutputIter<T>(0, 0, 0), (T*)0, pred, tag);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_remove (iter, FwdIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_remove (iter, BidirIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_remove (iter, RandomAccessIter<T>(), (T*)0, pred, tag);
    }
}

/**************************************************************************/

template <class T, class Predicate, class Tag>
void test_remove_copy (const T*, const Predicate* pred, Tag tag)
{
    rw_info (0, 0, 0,
             "template "
             "<class InputIterator, class OutputIterator, class T> "
             "std::%s (InputIterator, InputIterator, "
             "OutputIterator, %s)",
             tag.use_predicate ? "remove_copy_if" : "remove_copy",
             tag.use_predicate ? "Predicate" : "const T&");

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_remove_copy (InputIter<T>(0, 0, 0), (T*)0, pred, tag);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_remove_copy (FwdIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_remove_copy (BidirIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_remove_copy (RandomAccessIter<T>(), (T*)0, pred, tag);
    }
}

/**************************************************************************/

template <class T>
void test_remove_copy (const T* )
{
    const RemoveCopyTag remove_copy_tag    = { false };
    const RemoveCopyTag remove_copy_if_tag = { true };

    if (rw_opt_no_remove_copy) {
        rw_note (0, __FILE__, __LINE__, "std::remove_copy test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 0;
        test_remove_copy((T*)0, pred, remove_copy_tag);
    }

    if (rw_opt_no_remove_copy_if) {
        rw_note (0, __FILE__, __LINE__, "std::remove_copy_if test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 1;
        test_remove_copy((T*)0, pred, remove_copy_if_tag);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_remove ((UserClass*)0);

    test_remove_copy ((UserClass*)0);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.remove",
                    0 /* no comment */, run_test,
                    "|-no-remove# "
                    "|-no-remove_if# "
                    "|-no-remove_copy# "
                    "|-no-remove_copy_if# "
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_remove,
                    &rw_opt_no_remove_if,
                    &rw_opt_no_remove_copy,
                    &rw_opt_no_remove_copy_if,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
