/***************************************************************************
 *
 * 25.adjacent.find.cpp - test exercising lib.alg.adjacent.find
 *
 * $Id: 25.adjacent.find.cpp 510071 2007-02-21 15:58:53Z faridz $
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
 * Copyright 2004-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <algorithm>   // for adjacent_find()
#include <cstring>     // for size_t, strlen()

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>

/**************************************************************************/

// used to initialize an array of objects of type UserClass
static const char *xinit_begin;

int xinit ()
{
    typedef unsigned char UChar;

    static const char *cur = 0;

    if (!cur || !*cur)
        cur = xinit_begin;

    return UChar (*cur++);
}

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

// exercises std::adjacent_find()
template <class ForwardIterator, class T>
void do_test (int             line,     // line number of test case
              const char     *src,      // source sequence
              std::size_t     resoff,   // offset of result
              ForwardIterator dummy_iter,
              const T*,
              const char*     predname)
{
    static const char* const itname = type_name (dummy_iter, (T*)0);

    const std::size_t nsrc = std::strlen (src);

    if (std::size_t (-1) == resoff)
        resoff = nsrc;

    // have the UserClass default ctor initialize objects from `src'
    xinit_begin = src;
    UserClass::gen_     = xinit;

    UserClass* const xsrc = new UserClass [nsrc];

    const ForwardIterator first =
        make_iter (xsrc, xsrc, xsrc + nsrc, dummy_iter);

    const ForwardIterator last =
        make_iter (xsrc + nsrc, xsrc, xsrc + nsrc, dummy_iter);

    // reset predicate counters
    UserClass::n_total_op_eq_                  = 0;
    EqualityPredicate<T, T>::funcalls_ = 0;

    // construct a predicate object
    const EqualityPredicate<T, T> pred (0, 0);

    const ForwardIterator res = predname ?
          std::adjacent_find (first, last, pred)
        : std::adjacent_find (first, last);

    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
    _RWSTD_UNUSED (res);

    const std::size_t n_total_pred = predname ?
          EqualityPredicate<T, T>::funcalls_
        : UserClass::n_total_op_eq_;

    // verify that the returned iterator is set as expected
    int success = res.cur_ == first.cur_ + resoff;
    rw_assert (success, 0, line, 
               "line %d: adjacent_find<%s>(it = \"%s\", ...)"
               " == (it + %zu), got (it + %td)",
               __LINE__, itname, src,
               resoff, res.cur_ - first.cur_);

    // verify the number of applications of the predicate (lwg issue 240):
    // Complexity: For a nonempty range, exactly
    //             min((i - first) + 1, (last - first) - 1)
    // applications of the corresponding predicate, where i is
    // adjacent_find's return value.

    // compute the expected number of invocations of the predicate
    std::size_t n_expect_pred = 0;

    if (nsrc) {
        // test iterators are guaranteed to be in range
        _RWSTD_ASSERT (first.cur_ <= res.cur_ && res.cur_ <= last.cur_);

        n_expect_pred = std::size_t (res.cur_ - first.cur_) + 1;
        const std::size_t tmp = std::size_t (last.cur_ - first.cur_) - 1;
        if (tmp < n_expect_pred)
            n_expect_pred = tmp;
    }

    success = std::size_t (n_expect_pred) == n_total_pred;
    rw_assert (success, 0, line, 
               "line %d: adjacent_find<%s>(\"%s\", ...) "
               "invoked %s %zu times, expected %td",
               __LINE__, itname, src,
               predname ? predname : "operator==()", 
               n_total_pred, n_expect_pred);
}

/**************************************************************************/

template <class ForwardIterator, class T>
void run_tests (ForwardIterator dummy_iter, const T*, const char* predname)
{
    static const char* const itname = type_name (dummy_iter, (T*)0);

    rw_info (0, 0, 0, "std::adjacent_find (%s, %1$s%{?}, %s%{;})", 
             itname, 0 != predname, predname);
    
#define TEST(src, off) \
    do_test (__LINE__, src, std::size_t (off), dummy_iter, \
             (UserClass*)0, predname)

    //    +------------------ subject sequence
    //    |               +-- offset of returned iterator,
    //    |               |   -1 denotes the end of sequence
    //    v               v
    TEST ("",            -1);
    TEST ("a",           -1);
    TEST ("ab",          -1);
    TEST ("abc",         -1);
    TEST ("abcd",        -1);
    TEST ("abcde",       -1);
    TEST ("abcdef",      -1);
    TEST ("abcdefg",     -1);
    TEST ("abcdefgh",    -1);
    TEST ("abcdefghi",   -1);
    TEST ("abcdefghij",  -1);
    TEST ("abcdefghijk", -1);

    TEST ("aabcdefghij",  0);
    TEST ("abbcdefghij",  1);
    TEST ("abccdefghij",  2);
    TEST ("abcddefghij",  3);
    TEST ("abcdeefghij",  4);
    TEST ("abcdeffghij",  5);
    TEST ("abcdefgghij",  6);
    TEST ("abcdefghhij",  7);
    TEST ("abcdefghiij",  8);
    TEST ("abcdefghijj",  9);

    TEST ("aaabcdefghi",  0);
    TEST ("abbbcdefghi",  1);
    TEST ("abcccdefghi",  2);
    TEST ("abcdddefghi",  3);
    TEST ("abcdeeefghi",  4);
    TEST ("abcdefffghi",  5);
    TEST ("abcdefggghi",  6);
    TEST ("abcdefghhhi",  7);
    TEST ("abcdefghiii",  8);
    TEST ("abcdefghijjj", 9);

}

/**************************************************************************/

/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator
/* extern */ int rw_opt_no_predicate;    // --no-Predicate

static 
void test_adjacent_find (const char* predname)
{
    rw_info (0, 0, 0, 
             "template <class %s%{?}, class %s%{;}> "
             "%1$s std::adjacent_find (%1$s, %1$s%{?}, %3$s%{;})",
             "ForwardIterator", 0 != predname, "BinaryPredicate",
             0 != predname);

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        run_tests (FwdIter<UserClass>(), (UserClass*)0, predname);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        run_tests (BidirIter<UserClass>(), (UserClass*)0, predname);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        run_tests (RandomAccessIter<UserClass>(), (UserClass*)0, predname);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_adjacent_find (0);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, "Predicate test disabled");
    }
    else {
        test_adjacent_find ("EqualityPredicate<UserClass, UserClass>");
    }
                
    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.adjacent.find",
                    0 /* no comment */, run_test,
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#"
                    "|-no-Predicate#",
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
}
