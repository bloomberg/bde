/***************************************************************************
 *
 * 25.upper.bound.cpp - test exercising 25.3.3.2 [lib.upper.bound]
 *
 * $Id: 25.upper.bound.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
  // disable implicit inclusion to work around a limitation
  // of IBM VisualAge 5.0 (see PR #26959)
#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <algorithm>    // for upper_bound()
#include <cstring>      // for strlen()

#include <alg_test.h>   // for iterators
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test(), ...

/**************************************************************************/

// distinct and not Less-Than-Comparable with class UserClass (except as
// defined below) to detect unwarranted assumptions made by the
// implementation of the algorithms
struct Y
{
    UserClass xval_;

    // not Default-Constructible
    Y (int /* dummy */, int /*dummy */): xval_ () { }

    // CopyConstructible
    Y (const Y &rhs): xval_ (rhs.xval_) { }

private:

    void operator=(Y&);   // not Assignable
};


inline conv_to_bool
operator< (const UserClass &lhs, const Y &rhs)
{
    return conv_to_bool::make (lhs < rhs.xval_);
}

inline conv_to_bool
operator< (const Y &lhs, const UserClass &rhs)
{
    return conv_to_bool::make (lhs.xval_ < rhs);
}

// predicate used as the Compare argument to upper_bound()
struct LessThan
{
    static std::size_t funcalls_;

    LessThan (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    conv_to_bool operator() (const UserClass &lhs, const Y &rhs) {
        ++funcalls_;
        return conv_to_bool::make (lhs < rhs.xval_);
    }

    conv_to_bool operator() (const Y &lhs, const UserClass &rhs) {
        ++funcalls_;
        return conv_to_bool::make (lhs.xval_ < rhs);
    }

private:
    void operator= (LessThan&);   // not assignable
};

std::size_t LessThan::funcalls_;

/**************************************************************************/

template <class ForwardIterator>
void test_upper_bound (int                    line,
                       const char            *src_str,
                       char                   val_char,
                       std::size_t            result_off,
                       std::size_t            ncomp,
                       const ForwardIterator*,
                       const char            *itname,
                       bool                   predicate)
{
    RW_ASSERT (0 != src_str);

    const std::size_t nsrc = std::strlen (src_str);
    UserClass* const xsrc = UserClass::from_char (src_str, nsrc);

    if (nsrc < result_off)
        result_off = nsrc;

    // construct the source range to pass to the algorithm
    const ForwardIterator first (xsrc,        xsrc,        xsrc + nsrc);
    const ForwardIterator last  (xsrc + nsrc, xsrc + nsrc, xsrc + nsrc);

    // construct the object to pass to the algorithm
    // the type of the object is distinct from the iterator's value_type
    // to detect unwarranted assumptions made by the implementation
    Y value (0, 0 /* dummy arguments */);
    value.xval_.data_.val_ = val_char;

    // construct the Compare function object to pass to the algorithm
    // when `predicate' is true
    const LessThan comp (0, 0 /* dummy arguments */);

    // reset the counter of invocations of UserClass::operator<()
    UserClass::n_total_op_lt_ = 0;

    // invoke the appropriate form of the algorithm, storing
    // the resturned value
    const ForwardIterator result = predicate
        ? std::upper_bound (first, last, value, comp)
#if DRQS // stlport's upper_bound assumes that ForwardIterator::value_type is the same as T.  Also, __Less_2 takes a by-value second arg (should be const ref).
        : std::upper_bound (first, last, value);
#else
        : std::upper_bound (first, last, value.xval_);
#endif

    // silence bogus EDG eccp 3.6 remark #550-D:
    // variable was set but never used
    _RWSTD_UNUSED (result);

    // verify correctness
    const std::size_t off = std::size_t (result.cur_ - xsrc);

    rw_assert (off == result_off, 0, line,
               "upper_bound(%s = \"%s\", ...%{?}%#c%{;}) ==> "
               "first + %zu, got %zu",
               itname, src_str, !predicate, val_char,
               off, result_off);

    // verify complexity
    const std::size_t funcalls = predicate
        ? LessThan::funcalls_ : UserClass::n_total_op_lt_;

    rw_assert (funcalls <= ncomp, 0, line,
               "upper_bound(%s = \"%s\", ...%{?}%#c%{;}) complexity: "
               "invoked predicate at most %zu times, got %zu",
               itname, src_str, !predicate, val_char,
               ncomp, funcalls);

    delete[] xsrc;
}

/**************************************************************************/

template <class ForwardIterator>
void test_upper_bound (const ForwardIterator*,
                       const char           *itname,
                       bool                  predicate)
{
#define TEST(str, val, off, comp) \
    test_upper_bound (__LINE__, str, val, std::size_t (off), \
                      std::size_t (comp), (ForwardIterator*)0, \
                      itname, predicate)

    rw_info (0, 0, 0,
             "std::upper_bound (%s, %1$s, const UserClass&%{?}, %s%{;})",
             itname, predicate, "LessThan");

    //    +--------------- source sequence
    //    |     +--------- value argument
    //    |     |   +----- offset of the resturned iterator, -1 for end
    //    |     |   |  +-- complexity: at most (log(last - first) + 1)
    //    |     |   |  |   comparisons (or applications of the predicate)
    //    |     |   |  |
    //    V     V   V  V
    TEST ("",  'a', 0, 0);
    TEST ("a", 'a', 1, 1);
    TEST ("a", 'b', 1, 1);
    TEST ("b", 'a', 0, 1);

    TEST ("aa", 'a', 2, 2);
    TEST ("ab", 'a', 1, 2);
    TEST ("ab", 'b', 2, 2);
    TEST ("bb", 'a', 0, 2);

    TEST ("ace", 'a', 1, 3);
    TEST ("ace", 'b', 1, 3);
    TEST ("ace", 'c', 2, 3);
    TEST ("ace", 'd', 2, 3);
    TEST ("ace", 'e', 3, 3);
    TEST ("ace", 'f', 3, 3);

    TEST ("aceg", 'a', 1, 3);
    TEST ("aceg", 'b', 1, 3);
    TEST ("aceg", 'c', 2, 3);
    TEST ("aceg", 'd', 2, 3);
    TEST ("aceg", 'e', 3, 3);
    TEST ("aceg", 'f', 3, 3);
    TEST ("aceg", 'g', 4, 3);
    TEST ("aceg", 'h', 4, 3);
}

/**************************************************************************/

static int rw_opt_no_fwd_iter;
static int rw_opt_no_bidir_iter;
static int rw_opt_no_rnd_iter;
static int rw_opt_no_predicate;


static void
test_upper_bound (bool predicate)
{
    rw_info (0, 0, 0, "template <class %s, class %s%{?}, class %s%{;}> "
             "std::upper_bound (%1$s, %1$s, const UserClass&%{?}, %4$s%{;})",
             "ForwardIterator", "UserClass", predicate, "Compare", predicate);

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    }
    else {
        test_upper_bound ((FwdIter<UserClass>*)0, "ForwardIterator", predicate);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    }
    else {
        test_upper_bound ((BidirIter<UserClass>*)0, "BidirectionalIterator",
                          predicate);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    }
    else {
        test_upper_bound ((RandomAccessIter<UserClass>*)0,
                          "RandomAccessIterator",
                          predicate);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_upper_bound (false);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "test of the Predicate form of std::upper_bound disabled");
    }
    else {
        test_upper_bound (true);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.upper.bound",
                    0 /* no comment */,
                    run_test,
                    "|-no-Predicate# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_predicate,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
