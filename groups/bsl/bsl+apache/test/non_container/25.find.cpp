/***************************************************************************
 *
 * find.cpp - test exercising 25.1.2 [lib.alg.find]
 *
 * $Id: 25.find.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for find()
#include <cstring>      // for size_t, strlen()

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
InputIter<eq_comp<base<> > >
find (InputIter<eq_comp<base<> > >, InputIter<eq_comp<base<> > >,
      const eq_comp<base<> >&);

template
InputIter<eq_comp<base<> > >
find_if (InputIter<eq_comp<base<> > >, InputIter<eq_comp<base<> > >,
         predicate<eq_comp<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std


/**************************************************************************/

template <class T>
struct Predicate
{
    static std::size_t funcalls_;

    Predicate (const T &val, int /* dummy */)
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
    ConvertibleToBool operator() (T obj) /* not const */ {
        ++funcalls_;
        return ConvertibleToBool (obj == val_);
    }

private:
    void operator= (Predicate&);
    const T &val_;
};

template <class T>
std::size_t Predicate<T>::funcalls_;

/**************************************************************************/

template <class InputIterator>
void test_find (int           line,     // line number of test case
                const char   *src,      // source sequence
                std::size_t   findoff,  // offset of the element to find
                InputIterator dummy_iter,
                bool          test_pred)
{
    static const char* const itname = type_name (dummy_iter, (UserClass*)0);
    static const char* const pname  = test_pred ? "Predicate" : "operator==";

    const std::size_t nsrc = std::strlen (src);

    // normalize offset
    if (nsrc < findoff)
        findoff = nsrc;

    // create always at least 1 element (used to test failed searches)
    UserClass* const tsrc      = UserClass::from_char (src, nsrc + 1);
    UserClass* const src_begin = tsrc;
    UserClass* const src_end   = tsrc + nsrc;

    //                         current    [first,    last)
    const InputIterator first (src_begin, src_begin, src_end);
    const InputIterator last  (src_end,   src_begin, src_end);

    // get a reference to the object to find (when findoff == nsrc
    // the sought for element is outside the source range and won't
    // be found)
    const UserClass &to_find = tsrc [findoff];

    // construct a predicate object to use with find_if
    const Predicate<UserClass> pred (to_find, 0 /* dummy */);

    // reset the operator==() counter
    UserClass::n_total_op_eq_ = 0;

    // invoke find() or find_if(), depending on the predicate flag
    const InputIterator res = test_pred ?
        std::find_if (first, last, pred) : std::find (first, last, to_find);

    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
    _RWSTD_UNUSED (res);

    // verify 25.1.5 p2
    if (!rw_assert (res.cur_ == first.cur_ + findoff, 0, line,
                    "line %d: find%{?}_if%{;} (%s = \"%s\", ..., '%c')"
                    " == (it + %zu), got (it + %td)",
                    __LINE__, test_pred, itname, src, to_find.data_.val_,
                    findoff, res.cur_ - first.cur_)) {
        delete[] tsrc;
        return;
    }

    // verify 25.1.5 p3
    // Complexity: At most last - first applications
    //             of the corresponding predicate.
    // The complexity when find is successful is actually
    // (res - first) applications of the corresponding predicate.

    const std::size_t npreds =
        test_pred ? pred.funcalls_ : UserClass::n_total_op_eq_;

    rw_assert (npreds <= findoff + 1, 0, line,
               "line %d: find%{?}_if%{;} (%s = \"%s\", ..., '%c') "
               "invoked %s %zu times, expected no more than %zu",
               __LINE__, test_pred, itname, src, to_find.data_.val_,
               pname, npreds, findoff + 1);

    delete[] tsrc;
}

/**************************************************************************/

template <class InputIterator>
void test_find (InputIterator dummy_iter, bool test_pred)
{
    static const char* const itname = type_name (dummy_iter, (UserClass*)0);

    rw_info (0, 0, 0, "std::find%{?}_if%{;} (%s, %2$s, "
             "%{?}Predicate%{:}const UserClass&%{;})",
             test_pred, itname, test_pred);

#define TEST(src, off_find)                             \
    test_find (__LINE__, src, std::size_t (off_find),   \
               dummy_iter, test_pred)

    //    +------------------ subject sequence
    //    |               +--- offset of the value to find (-1 for none)
    //    |               |    also the offset of the returned iterator
    //    |               |
    //    |               |
    //    v               v
    TEST ("",            -1);
    TEST ("a",            0);
    TEST ("aa",           0);
    TEST ("ab",           1);
    TEST ("aaa",          0);
    TEST ("abb",          1);
    TEST ("abc",          2);
    TEST ("abcc",         2);
    TEST ("abcd",         3);
    TEST ("abcde",        4);
    TEST ("abcdef",       5);
    TEST ("abcdefg",      6);
    TEST ("abcdefgh",     7);
    TEST ("abcdefghi",    8);
    TEST ("abcdefghij",   9);
    TEST ("abcdefghijk", 10);
    TEST ("abcdefghijk", -1);
}

/**************************************************************************/

/* extern */ int rw_opt_no_input_iter;   // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator
/* extern */ int rw_opt_no_predicate;    // --no-Predicate

static void
test_find (bool test_pred)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s> "
             "%1$s std::find%{?}_if%{;} (%1$s, %1$s, "
             "%{?}%2$s%{:}const %2$s&%{;})",
             "InputIterator", test_pred ? "Predicate" : "T",
             test_pred);

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_find (InputIter<UserClass>(0, 0, 0), test_pred);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_find (ConstFwdIter<UserClass>(), test_pred);
        test_find (FwdIter<UserClass>(), test_pred);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_find (ConstBidirIter<UserClass>(), test_pred);
        test_find (BidirIter<UserClass>(), test_pred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_find (ConstRandomAccessIter<UserClass>(), test_pred);
        test_find (RandomAccessIter<UserClass>(), test_pred);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_find (false);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, "Predicate test disabled");
    }
    else {
        test_find (true);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.find",
                    0 /* no comment */, run_test,
                    "|-no-InputIterator#"
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#"
                    "|-no-Predicate#",
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
}
