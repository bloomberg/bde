/***************************************************************************
 *
 * 25.equal.range.cpp - test exercising 25.3.3.3 [lib.equal.range]
 *
 * $Id: 25.equal.range.cpp 510970 2007-02-23 14:57:45Z faridz $
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
 * Copyright 2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
  // disable implicit inclusion to work around a limitation
  // of IBM VisualAge 5.0 (see PR #26959)
#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif

#include <algorithm>    // for equal_range()
#include <cstring>      // for strlen()

#include <alg_test.h>   // for iterators
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test(), ...

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
pair <FwdIter<lt_comp<assign<base<cpy_ctor> > > >, 
      FwdIter<lt_comp<assign<base<cpy_ctor> > > > >
equal_range (FwdIter<lt_comp<assign<base<cpy_ctor> > > >, 
             FwdIter<lt_comp<assign<base<cpy_ctor> > > >,
             const lt_comp<assign<base<cpy_ctor> > >&);

template
pair <FwdIter<lt_comp<assign<base<cpy_ctor> > > >, 
      FwdIter<lt_comp<assign<base<cpy_ctor> > > > >
equal_range (FwdIter<lt_comp<assign<base<cpy_ctor> > > >, 
             FwdIter<lt_comp<assign<base<cpy_ctor> > > >,
             const lt_comp<assign<base<cpy_ctor> > >&,
             binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// distinct and not Less-Than-Comparable with class T (except as
// defined below) to detect unwarranted assumptions made by the
// implementation of the algorithms
struct Y
{
    UserClass xval_;

    // not Default-Constructible
    Y (char val, int /*dummy */): xval_ () {
        typedef unsigned char UChar;
        xval_.data_.val_ = int (UChar (val));
    }

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

template <class T, class ForwardIterator>
void test_equal_range (int                    line,
                       const char            *src_str,
                       char                   val_char,
                       std::size_t            result_loff,
                       std::size_t            result_uoff,
                       std::size_t            ncomp,
                       const ForwardIterator &it,
                       const T*,
                       bool                   predicate)
{
    RW_ASSERT (0 != src_str);

    const char* const tname   = "UserClass";
    const char* const itname  = type_name (it, (T*)0);
    const char* const funname = predicate ? "LessThan" : "operator<()";

    const std::size_t nsrc = std::strlen (src_str);
    T* const xsrc = T::from_char (src_str, nsrc + 1);
    T* const xsrc_end = xsrc + nsrc;

    if (nsrc < result_uoff)
        result_uoff = nsrc;

    // construct the source range to pass to the algorithm
    const ForwardIterator first = make_iter (xsrc,     xsrc, xsrc_end, it);
    const ForwardIterator last  = make_iter (xsrc_end, xsrc, xsrc_end, it);

    // construct the object to pass to the algorithm
    // the type of the object is distinct from the iterator's value_type
    // to detect unwarranted assumptions made by the implementation of
    // the algorithm
    const Y value (val_char, 0 /* dummy */);

    // construct the Compare function object to pass to the algorithm
    // when `predicate' is true
    const LessThan comp (0, 0 /* dummy arguments */);

    // reset the counter of invocations of T::operator<()
    T::n_total_op_lt_ = 0;

    // invoke the appropriate form of the algorithm, storing
    // the resturned value
    const std::pair <ForwardIterator, ForwardIterator> result = predicate
        ? std::equal_range (first, last, value, comp)
#if DRQS // stlport's equal_range assumes that ForwardIterator::value_type is the same as T.  Also, __Less_2 takes a by-value second arg (should be const ref).
        : std::equal_range (first, last, value);
#else
        : std::equal_range (first, last, value.xval_);
#endif

    // silence bogus EDG eccp 3.6 remark #550-D:
    // variable was set but never used
    _RWSTD_UNUSED (result);

    // verify correctness
    const std::size_t loff = std::size_t (result.first.cur_ - xsrc);
    const std::size_t uoff = std::size_t (result.second.cur_ - xsrc);

    rw_assert (loff == result_loff && uoff == result_uoff, 0, line,
               "equal_range <%s, const %s&%{?}, %s%{;}> (\"%s\", %#c, ...) "
               "== first + { %zu, %zu }, got first + { %zu, %zu }",
               itname, tname, predicate, funname, src_str, val_char,
               loff, uoff, result_loff, result_uoff);

    // verify complexity
    const std::size_t n_ops = predicate
        ? LessThan::funcalls_ : T::n_total_op_lt_;

    rw_assert (n_ops <= ncomp, 0, line,
               "equal_range <%s, const %s&%{?}, %s%{;}> (\"%s\", %#c, ...) "
               "complexity: invoked %4$s at most %zu times, got %zu",
               itname, tname, predicate, funname, src_str, val_char,
               ncomp, n_ops);

    delete[] xsrc;
}

/**************************************************************************/

template <class T, class ForwardIterator>
void test_equal_range (const ForwardIterator &it,
                       const T*,
                       bool                   predicate)
{
    const char* const itname = type_name (it, (T*)0);
    const char* const funname = predicate ? "LessThan" : "operator<()";

    rw_info (0, 0, 0,
             "std::equal_range (%s, %1$s, const UserClass&%{?}, %s%{;})",
             itname, predicate, funname);

#define TEST(str, val, loff, uoff, comp) \
    test_equal_range (__LINE__, str, val, std::size_t (loff),           \
                      std::size_t (uoff), std::size_t (comp),           \
                      it, (T*)0, predicate)

    //    +------------------ source sequence
    //    |     +------------ value argument
    //    |     |   +-------- lower offset of the returned pair
    //    |     |   |  +----- upper offset of the returned pair
    //    |     |   |  |  +-- complexity: at most (2 * log(last - first) + 1)
    //    |     |   |  |  |   comparisons (or applications of the predicate)
    //    |     |   |  |  |
    //    V     V   V  V  V
    TEST ("",  'a', 0, 0, 0);
    TEST ("a", 'a', 0, 1, 2);
    TEST ("a", 'b', 1, 1, 1);
    TEST ("b", 'a', 0, 0, 2);

    TEST ("aa", 'a', 0, 2, 4);
    TEST ("ab", 'a', 0, 1, 4);
    TEST ("ab", 'b', 1, 2, 3);
    TEST ("bb", 'a', 0, 0, 4);

    TEST ("ace", 'a', 0, 1, 5);
    TEST ("ace", 'b', 1, 1, 4);
    TEST ("ace", 'c', 1, 2, 4);
    TEST ("ace", 'd', 2, 2, 4);
    TEST ("ace", 'e', 2, 3, 4);
    TEST ("ace", 'f', 3, 3, 4);

    TEST ("aceg", 'a', 0, 1, 6);
    TEST ("aceg", 'b', 1, 1, 5);
    TEST ("aceg", 'c', 1, 2, 5);
    TEST ("aceg", 'd', 2, 2, 5);
    TEST ("aceg", 'e', 2, 3, 5);
    TEST ("aceg", 'f', 3, 3, 5);
    TEST ("aceg", 'g', 3, 4, 5);
    TEST ("aceg", 'h', 4, 4, 5);
}

/**************************************************************************/

static int rw_opt_no_fwd_iter;
static int rw_opt_no_bidir_iter;
static int rw_opt_no_rnd_iter;
static int rw_opt_no_predicate;


template <class T>
void test_equal_range (const T*,
                       bool predicate)
{
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0, "template <class %s, class %s%{?}, class %s%{;}> "
             "std::pair<%1$s, %1$s> std::equal_range "
             "(%1$s, %1$s, const %2$s&%{?}, %s%{;})",
             "ForwardIterator", "UserClass", predicate, "Compare", 
             predicate, "Compare");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    }
    else {
        test_equal_range (fwd_iter, (T*)0, predicate);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    }
    else {
        test_equal_range (bidir_iter, (T*)0, predicate);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    }
    else {
        test_equal_range (rand_iter, (T*)0, predicate);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_equal_range ((UserClass*)0, false);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "test of the Predicate form of std::equal_range disabled");
    }
    else {
        test_equal_range ((UserClass*)0, true);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.equal.range",
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
