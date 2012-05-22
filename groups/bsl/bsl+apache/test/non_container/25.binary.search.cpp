/***************************************************************************
 *
 * 25.binary.search.cpp - test exercising 25.3.4 [lib.binary.search]
 *
 * $Id: 25.binary.search.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for binary_search()
#include <cstring>      // for strlen()

#include <alg_test.h>   // for iterators
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test(), ...

/**************************************************************************/

// meets requirements listed at 25.3, p2
template <class T>
struct compare {
    bool operator() (const T &a, const T &b) const {
        _RWSTD_UNUSED (a);
        _RWSTD_UNUSED (b);
        return true;
    }
};

_RWSTD_NAMESPACE (std) {

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
bool
binary_search (FwdIter<lt_comp<assign<base<cpy_ctor> > > >,
               FwdIter<lt_comp<assign<base<cpy_ctor> > > >,
               const lt_comp<assign<base<cpy_ctor> > >&);

template
bool
binary_search (FwdIter<lt_comp<assign<base<cpy_ctor> > > >,
               FwdIter<lt_comp<assign<base<cpy_ctor> > > >,
               const lt_comp<assign<base<cpy_ctor> > >&,
               compare<lt_comp<assign<base<cpy_ctor> > > >);

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
    Y (int /* dummy */, int /*dummy */): xval_ () { }

    // CopyConstructible
    Y (const Y &rhs): xval_ (rhs.xval_) { }

private:

    void operator=(Y&);   // not Assignable
};


inline bool
operator< (const UserClass &lhs, const Y &rhs)
{
    return lhs < rhs.xval_;
}

inline bool
operator< (const Y &lhs, const UserClass &rhs)
{
    return lhs.xval_ < rhs;
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
    bool operator() (const UserClass &lhs, const Y &rhs) {
        ++funcalls_;
        return lhs < rhs.xval_;
    }

    bool operator() (const Y &lhs, const UserClass &rhs) {
        ++funcalls_;
        return lhs.xval_ < rhs;
    }

private:
    void operator= (LessThan&);   // not assignable
};

std::size_t LessThan::funcalls_;

/**************************************************************************/

template <class T, class ForwardIterator>
void test_binary_search (int                    line,
                         const char            *src_str,
                         char                   val_char,
                         bool                   exp_res,
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
    T* const xsrc     = T::from_char (src_str, nsrc);
    T* const xsrc_end = xsrc + nsrc;

    // construct the source range to pass to the algorithm
    const ForwardIterator first = make_iter (xsrc, xsrc, xsrc_end, it);
    const ForwardIterator last  = make_iter (xsrc_end, xsrc, xsrc_end, it);

    // construct the object to pass to the algorithm
    // the type of the object is distinct from the iterator's value_type
    // to detect unwarranted assumptions made by the implementation
    Y value (0, 0 /* dummy arguments */);
    value.xval_.data_.val_ = val_char;

    // construct the Compare function object to pass to the algorithm
    // when `predicate' is true
    const LessThan comp (0, 0 /* dummy arguments */);

    // reset the counter of invocations of T::operator<()
    T::n_total_op_lt_ = 0;

    // invoke the appropriate form of the algorithm, storing
    // the resturned value
    const bool result = predicate
        ? std::binary_search (first, last, value, comp)
        : std::binary_search (first, last, value);

    // silence bogus EDG eccp 3.6 remark #550-D:
    // variable was set but never used
    _RWSTD_UNUSED (result);

    rw_assert (result == exp_res, 0, line,
               "binary_search <%s, const %s&%{?}, %s%{;}> (\"%s\", %#c, ...) "
               "expected %s, got %s",
               itname, tname, predicate, funname, src_str, val_char,
               exp_res ? "true" : "false", result ? "true" : "false");

    // verify complexity
    const std::size_t n_ops = predicate
        ? LessThan::funcalls_ : T::n_total_op_lt_;

    rw_assert (n_ops <= ncomp, 0, line,
               "binary_search <%s, const %s&%{?}, %s%{;}> (\"%s\", %#c, ...) "
               "complexity: invoked %4$s at most %zu times, got %zu",
               itname, tname, predicate, funname, src_str, val_char,
               ncomp, n_ops);

    delete[] xsrc;
}

/**************************************************************************/

template <class T, class ForwardIterator>
void test_binary_search (const ForwardIterator &it,
                         const T*,
                         bool                   predicate)
{
    const char* const itname = type_name (it, (T*)0);
    const char* const funname = predicate ? "LessThan" : "operator<()";

    rw_info (0, 0, 0,
             "std::binary_search (%s, %1$s, const UserClass&%{?}, %s%{;})",
             itname, predicate, funname);

#define TEST(str, val, res, comp) \
    test_binary_search (__LINE__, str, val, res, std::size_t (comp),       \
                        it, (T*)0, predicate)

    //    +--------------- source sequence
    //    |     +--------- value argument
    //    |     |   +----- expected result: true/false
    //    |     |   |      +-- complexity: at most (log(last - first) + 2)
    //    |     |   |      |   comparisons (or applications of the predicate)
    //    |     |   |      |
    //    V     V   V      V
    TEST ("",  'a', false, 0);
    TEST ("a", 'a', true,  2);
    TEST ("a", 'b', false, 2);
    TEST ("b", 'a', false, 2);

    TEST ("aa", 'a', true,  3);
    TEST ("ab", 'a', true,  3);
    TEST ("ab", 'b', true,  3);
    TEST ("bb", 'a', false, 3);

    TEST ("abcdefghij", 'a', true,  6);
    TEST ("abcdefghij", 'c', true,  6);
    TEST ("abcdefghij", 'f', true,  6);
    TEST ("abcdefghij", 'h', true,  6);
    TEST ("abcdefghij", 'j', true,  6);
    TEST ("aacceeggii", 'c', true,  6);
    TEST ("aacceeggii", 'g', true,  6);
    TEST ("aacceeggii", 'b', false, 6);
    TEST ("aacceeggii", 'f', false, 6);
    TEST ("aacceeggii", 'j', false, 6);
    TEST ("cccceeggii", 'a', false, 6);
}

/**************************************************************************/

/* extern */ int rw_opt_no_predicate;            // --no-predicate
/* extern */ int rw_opt_no_fwd_iter;             // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;           // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;             // --no-RandomAccessIterator

/**************************************************************************/

template <class T>
void test_binary_search (const T*,
                         bool predicate)
{
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0, "template <class %s, class %s%{?}, class %s%{;}> "
             "bool std::binary_search (%1$s, %1$s, const %2$s&%{?}, %s%{;})",
             "ForwardIterator", "UserClass", predicate, "Compare", predicate,
             "Compare");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    }
    else {
        test_binary_search (fwd_iter, (T*)0, predicate);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    }
    else {
        test_binary_search (bidir_iter, (T*)0, predicate);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    }
    else {
        test_binary_search (rand_iter, (T*)0, predicate);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_binary_search ((UserClass*)0, false);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__,
                 "test of the Predicate form of std::binary_search disabled");
    }
    else {
        test_binary_search ((UserClass*)0, true);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.binary.search",
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
