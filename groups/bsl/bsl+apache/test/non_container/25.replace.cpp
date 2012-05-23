/***************************************************************************
 *
 * 25.replace.cpp - test exercising 25.2.4 [lib.alg.replace]
 *
 * $Id: 25.replace.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>     // for replace(), replace_copy()
#include <cstring>       // for size_t, strlen()

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>      // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void replace (FwdIter<eq_comp<assign<base<> > > >,
              FwdIter<eq_comp<assign<base<> > > >,
              const eq_comp<assign<base<> > >&,
              const eq_comp<assign<base<> > >&);

template
void replace_if (FwdIter<assign<base<> > > ,
                 FwdIter<assign<base<> > > ,
                 predicate<assign<base<> > > ,
                 const assign<base<> > &);

template
OutputIter<eq_comp<assign<base<> > > >
replace_copy (InputIter<eq_comp<assign<base<> > > >,
              InputIter<eq_comp<assign<base<> > > >,
              OutputIter<eq_comp<assign<base<> > > >,
              const eq_comp<assign<base<> > >&,
              const eq_comp<assign<base<> > >&);

template
OutputIter<assign<base<> > >
replace_copy_if (InputIter<assign<base<> > > ,
                 InputIter<assign<base<> > > ,
                 OutputIter<assign<base<> > > ,
                 predicate<assign<base<> > > ,
                 const assign<base<> > &);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// names of tested functions
const char* const fnames[] = { "replace", "replace_copy" };
const char* const fnames_if[] = { "replace_if", "replace_copy_if" };

// tags to select replace or replace_copy at compile time
struct ReplaceTag {
    enum { fname_inx = 0 };
    int use_predicate;
};

struct ReplaceCopyTag {
    enum { fname_inx = 1 };
    int use_predicate;
};

// used as a dummy template argument to test functions exercising replace()
// and replace_if() (to differentiate from replace_copy() and replace_copy_if())
struct NoIterator { };
const char* type_name (NoIterator, const UserClass*)
{
    return 0;
}

/**************************************************************************/

// predicate used as an argument to replace_if() and replace_copy_if()
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

// exercises std::replace() and std::replace_if()
template <class Iterator, class T, class Predicate>
void test_replace (int line,
                   const char *src,       // source sequence
                   char        val,       // value to replace
                   char        new_val,   // value to replace it with
                   Iterator    dummy,     // not used
                   NoIterator,
                   const T*,
                   const Predicate*,
                   ReplaceTag  tag)       // replace() or replace_if()
{
    static const char* const itname = type_name (Iterator (), (T*)0);
    static const char* const fname = tag.use_predicate ?
        fnames_if [tag.fname_inx] : fnames [tag.fname_inx];

    // compute the length of the source sequence
    const std::size_t nsrc = std::strlen (src);

    // construct a sequence of `nsrc' elements to pass to replace
    // (make sure there's always at least 1 element, even if the
    // sequence is otherwise empty, to avoid undefined behavior
    // when de-referencing xsrc[0] in inactive diagnostic messages)
    T* const xsrc     = T::from_char (src, nsrc + 1);
    T* const xsrc_end = xsrc + nsrc;

    // construct an element to replace
    T to_replace;
    to_replace.data_.val_ = val;

    // construct an element to replace it with
    T replace_with;
    replace_with.data_.val_ = new_val;

    // construct a predicate object (used with replace_if() only)
    const Predicate pred (to_replace, 0);

    // construct iterators pointing to the beginning and end
    // of the source sequence
    const Iterator first = make_iter (xsrc,     xsrc,     xsrc_end, dummy);
    const Iterator last  = make_iter (xsrc_end, xsrc_end, xsrc_end, dummy);

    // zero out predicate counters
    T::n_total_op_eq_ = 0;

    // call replace() or replace_if()
    if (tag.use_predicate)
        std::replace_if (first, last, pred, replace_with);
    else
        std::replace (first, last, to_replace, replace_with);

    // verify that the value to be replaced has been replaced
    // with the replacement value
    bool success = true;
    std::size_t i = 0;
    for ( ; i != nsrc; ++i) {
        success = to_replace.data_.val_ != xsrc [i].data_.val_;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c, %#c) ==> "
               "\"%{X=*.*}\"; unexpected element value %#c",
               __LINE__, fname, itname, src, val, new_val,
               int (nsrc), int (i), xsrc, src [i]);

    // check the id (not just the value) of the matching element
    // to make sure it has really been replaced
    for (i = 0; i != nsrc; ++i) {

        success =
               val != src [i]
            || val == src [i]
            && replace_with.origin_ == xsrc [i].origin_;

        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c, %#c) ==> "
               "\"%{X=#*.*}\"; failed to replace element %zu: "
               "origin %d (%d, %d), expected %d (%d)",
               __LINE__, fname, itname, src, val, new_val,
               int (nsrc), int (i), xsrc,
               i, xsrc [i].origin_, xsrc [i].id_, xsrc [i].src_id_,
               replace_with.origin_, replace_with.id_);

    // verify the number of applications of the predicate: p 25.2.4.3
    if (tag.use_predicate) {
        rw_assert (pred.funcalls_ == nsrc, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) called  "
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

// exercises std::replace_copy()
template <class Iterator, class OutputIterator, class T, class Predicate>
void test_replace (int line,
                   const char    *src,       // source sequence
                   char           val,       // value to replace
                   char           new_val,   // value to replace it with
                   Iterator       it,
                   OutputIterator dummy,
                   const T*,
                   const Predicate*,
                   ReplaceCopyTag tag)       // replace_copy or replace_copy_if
{
    static const char* const itname = type_name (it, (T*)0);
    static const char* const fname = tag.use_predicate ?
        fnames_if [tag.fname_inx] : fnames [tag.fname_inx];

    const std::size_t nsrc = std::strlen (src);

    // construct a sequence of `nsrc' elements to pass to replace
    // (make sure there's always at least 1 element, even if the
    // sequence is otherwise empty, to avoid undefined behavior
    // when de-referencing xsrc[0] in inactive diagnostic messages)
    T* const xsrc = T::from_char (src, nsrc + 1);
    T* const xdst = T::from_char (src, nsrc + 1);

    T* const xsrc_end = xsrc + nsrc;
    T* const xdst_end = xdst + nsrc;

    T to_replace;
    to_replace.data_.val_ = val;

    T replace_with;
    replace_with.data_.val_ = new_val;

    const Predicate pred (to_replace, 0);

    const Iterator first = make_iter (xsrc,     xsrc,     xsrc_end, it);
    const Iterator last  = make_iter (xsrc_end, xsrc_end, xsrc_end, it);

    const OutputIterator result = make_iter (xdst, xdst, xdst_end, dummy);

    // zero out predicate counters
    T::n_total_op_eq_ = 0;

    const OutputIterator end = tag.use_predicate
        ? std::replace_copy_if (first, last, result, pred, replace_with)
        : std::replace_copy (first, last, result, to_replace, replace_with);

    // verify that the returned iterator is set as expected
    rw_assert (end.cur_ == result.cur_ + nsrc, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c) == result + %zu, got %td",
               __LINE__, fname, itname, src, val, nsrc, end.cur_ - xdst);

    // verify that the value to be replaced does not appear anywhere
    // in the range [result, end)
    bool success = true;
    std::size_t i = 0;
    for ( ; i != nsrc; ++i) {
        typedef unsigned char UChar;
        success = UChar (val) != xdst [i].data_.val_;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c) ==> "
               "\"%{X=*.*}\"; unexpected element value %#c",
               __LINE__, fname, itname, src, val,
               int (nsrc), int (i), xdst, src [i]);

    // check the id (not just the value) of the matching element
    // to make sure it has really been copied
    for (i = 0; i != nsrc; ++i) {
        success =
               val != src [i]
            || val == src [i]
            && replace_with.origin_ == xdst [i].origin_;

        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ..., %#c, %#c) ==> "
               "\"%{X=*.*}\"; failed to copy and replace element %zu: "
               "origin %d (%d, %d), expected %d (%d)",
               __LINE__, fname, itname, src, val, new_val,
               int (nsrc), int (i), xdst,
               i, xdst [i].origin_, xdst [i].id_, xdst [i].src_id_,
               replace_with.origin_, replace_with.id_);

    // verify the number of applications of the predicate: p 25.2.4.7
    if (tag.use_predicate) {
        rw_assert (pred.funcalls_ == nsrc, 0, line,
                   "line %d: %s<%s>(\"%s\", ..., %#c) called  "
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
void test_replace (Iterator1 it1, Iterator2 it2, const T*,
                   const Predicate* pred, Tag tag)
{
    static const char* const it1name = type_name (it1, (T*)0);
    static const char* const it2name = type_name (it2, (T*)0);

    if (tag.fname_inx) {  // replace_copy(), replace_copy_if()
         rw_info (0, 0, 0,
                  "std::%s (%s, %s, %s, %s, const T&)",
                  tag.use_predicate ? "replace_copy_if" : "replace_copy",
                  it1name, it1name, it2name,
                  tag.use_predicate ? "UnaryPredicate" : "const T&");
    }
    else {   // replace, replace_if()
         rw_info (0, 0, 0,
                  "std::%s (%s, %s, %s, const T&)",
                  tag.use_predicate ? "replace_if" : "replace",
                  it1name, it1name,
                  tag.use_predicate ? "UnaryPredicate" : "const T&");
    }


#define TEST(src, val, new_val) \
    test_replace (__LINE__, src, val, new_val, it1, it2, (T*)0, pred, tag)

    //    +----------------------- original sequence
    //    |               +------- value to replace
    //    |               |     +- value to replace it with
    //    |               |     |
    //    v               v     v
    TEST ("",            'a',  'b');
    TEST ("a",           'a',  'b');
    TEST ("aa",          'a',  'b');
    TEST ("aaa",         'a',  'b');
    TEST ("aaaa",        'a',  'b');
    TEST ("aaaaa",       'a',  'b');
    TEST ("aaaaaa",      'a',  'b');
    TEST ("aaaaaaa",     'a',  'b');
    TEST ("aaaaaaaa",    'a',  'b');
    TEST ("aaaaaaaaa",   'a',  'b');
    TEST ("aaaaaaaaaa",  'a',  'b');

    TEST ("b",           'a',  'b');
    TEST ("bb",          'a',  'b');
    TEST ("ba",          'a',  'b');
    TEST ("ab",          'a',  'b');
    TEST ("abc",         'a',  'b');
    TEST ("bac",         'a',  'b');
    TEST ("bca",         'a',  'b');
    TEST ("aba",         'a',  'b');
    TEST ("abab",        'a',  'b');
    TEST ("ababa",       'a',  'b');
    TEST ("bababa",      'a',  'b');
    TEST ("bababab",     'a',  'b');
    TEST ("babababa",    'a',  'b');
    TEST ("bbbbbbbbb",   'a',  'b');
}

/**************************************************************************/

/* extern */ int rw_opt_no_replace;            // --no-replace
/* extern */ int rw_opt_no_replace_if;         // --no-replace_if
/* extern */ int rw_opt_no_replace_copy;       // --no-replace_copy
/* extern */ int rw_opt_no_replace_copy_if;    // --no-replace_copy_if
/* extern */ int rw_opt_no_input_iter;         // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;        // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;           // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;         // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;           // --no-RandomAccessIterator

template <class T, class Predicate, class Tag>
void test_replace (const T* , const Predicate* pred, Tag tag)
{
    rw_info (0, 0, 0,
            "template <class ForwardIterator, class T> "
            "std::%s (ForwardIterator, ForwardIterator, "
            "%s, const T&)",
            tag.use_predicate ? "replace_if" : "replace",
            tag.use_predicate ? "Predicate" : "const T&");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_replace (FwdIter<T>(), NoIterator (), (T*)0, pred, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_replace (BidirIter<T>(), NoIterator (), (T*)0, pred, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_replace (RandomAccessIter<T>(), NoIterator (), (T*)0, pred, tag);
    }
}

/**************************************************************************/

template <class T>
void test_replace (const T* )
{
    const ReplaceTag replace_tag    = { false };
    const ReplaceTag replace_if_tag = { true };

    if (rw_opt_no_replace) {
        rw_note (0, __FILE__, __LINE__,  "std::replace test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 0;
        test_replace((T*)0, pred, replace_tag);
    }

    if (rw_opt_no_replace_if) {
        rw_note (0, __FILE__, __LINE__,  "std::replace_if test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 1;
        test_replace((T*)0, pred, replace_if_tag);
    }
}

/**************************************************************************/

template <class InputIterator, class T, class Predicate, class Tag>
void test_replace_copy (const InputIterator& iter, const T*,
                        const Predicate* pred, Tag tag)
{
    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_replace (iter, OutputIter<T>(0, 0, 0), (T*)0, pred, tag);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_replace (iter, FwdIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_replace (iter, BidirIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_replace (iter, RandomAccessIter<T>(), (T*)0, pred, tag);
    }
}

/**************************************************************************/

template <class T, class Predicate, class Tag>
void test_replace_copy (const T*, const Predicate* pred, Tag tag)
{
    rw_info (0, 0, 0,
             "template "
             "<class InputIterator, class OutputIterator, class T> "
             "std::%s (InputIterator, InputIterator, "
             "OutputIterator, %s, const T&)",
             tag.use_predicate ? "replace_copy_if" : "replace_copy",
             tag.use_predicate ? "Predicate" : "const T&");

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_replace_copy (InputIter<T>(0, 0, 0), (T*)0, pred, tag);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_replace_copy (FwdIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_replace_copy (BidirIter<T>(), (T*)0, pred, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_replace_copy (RandomAccessIter<T>(), (T*)0, pred, tag);
    }
}

/**************************************************************************/

template <class T>
void test_replace_copy (const T* )
{
    const ReplaceCopyTag replace_copy_tag    = { false };
    const ReplaceCopyTag replace_copy_if_tag = { true };

    if (rw_opt_no_replace_copy) {
        rw_note (0, __FILE__, __LINE__, "std::replace_copy test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*) 0;
        test_replace_copy ((T*)0, pred, replace_copy_tag);
    }

    if (rw_opt_no_replace_copy_if) {
        rw_note (0, __FILE__, __LINE__, "std::replace_copy_if test disabled");
    }
    else {
        const EqualityPredicate<T>* const pred = (EqualityPredicate<T>*)1;
        test_replace_copy ((T*)0, pred, replace_copy_if_tag);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_replace ((UserClass*)0);

    test_replace_copy ((UserClass*)0);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.replace",
                    0 /* no comment */,
                    run_test,
                    "|-no-replace# "
                    "|-no-replace_if# "
                    "|-no-replace_copy# "
                    "|-no-replace_copy_if# "
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_replace,
                    &rw_opt_no_replace_if,
                    &rw_opt_no_replace_copy,
                    &rw_opt_no_replace_copy_if,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
