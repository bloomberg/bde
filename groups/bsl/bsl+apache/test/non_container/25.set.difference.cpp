/***************************************************************************
 *
 * 25.set.difference.cpp - test exercising lib.set.difference
 *
 * $Id: 25.set.difference.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for set_difference 
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template 
OutputIter<lt_comp<assign<base<cpy_ctor> > > >
set_difference (InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                OutputIter<lt_comp<assign<base<cpy_ctor> > > >);

template 
OutputIter<lt_comp<assign<base<cpy_ctor> > > >
set_difference (InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                InputIter<lt_comp<assign<base<cpy_ctor> > > >,
                OutputIter<lt_comp<assign<base<cpy_ctor> > > >,
                binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

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
    conv_to_bool operator() (const UserClass &x,
                             const UserClass &y) /* non-const */ {
        ++funcalls_;
        return conv_to_bool::make (x.data_.val_ < y.data_.val_);
    }

private:
    void operator= (Less&);   // not assignable
};

std::size_t Less::funcalls_;

/**************************************************************************/

// ordinary (non-template) base to minimize code bloat
struct SetDifferenceBase
{
    virtual ~SetDifferenceBase() {}

    const char* iter_names [3];

    // pure virtual
    virtual UserClass*
    set_difference (const UserClass*, const UserClass*, const UserClass*,
                    const UserClass*, UserClass*, UserClass*,
                    const Less*) const = 0;
};

template <class InputIterator1, class InputIterator2, class OutputIterator>
struct SetDifference: SetDifferenceBase
{
    SetDifference () {
        iter_names [0] = type_name (InputIterator1 (0, 0, 0), (UserClass*)0);
        iter_names [1] = type_name (InputIterator2 (0, 0, 0), (UserClass*)0);
        iter_names [2] = type_name (OutputIterator (0, 0, 0), (UserClass*)0);
    }

    virtual UserClass*
    set_difference (const UserClass    *xsrc1, const UserClass *xsrc1_end,
                    const UserClass    *xsrc2, const UserClass *xsrc2_end,
                    UserClass          *xdst,  UserClass       *xdst_end,
                    const Less *ppred) const {

        const InputIterator1 first1 (xsrc1,     xsrc1, xsrc1_end);
        const InputIterator1 last1  (xsrc1_end, xsrc1, xsrc1_end);
        const InputIterator2 first2 (xsrc2,     xsrc2, xsrc2_end);
        const InputIterator2 last2  (xsrc2_end, xsrc2, xsrc2_end);

        const OutputIterator result (xdst, xdst, xdst_end);

        const OutputIterator ret = ppred ?
            std::set_difference (first1, last1, first2, last2, result, *ppred)
          : std::set_difference (first1, last1, first2, last2, result);

        // silence EDG eccp 3.7 and prior remark #550-D:
        //   variable was set but never used
        _RWSTD_UNUSED (ret);

        return ret.cur_;
    }
};

/**************************************************************************/

// exercises set_difference: 25.3.5.4
void test_set_difference (int                      line,
                          const char              *src1,
                          std::size_t              nsrc1,
                          const char              *src2,
                          std::size_t              nsrc2,
                          const char              *res,
                          std::size_t              ndst,
                          bool                     predicate,
                          const SetDifferenceBase &alg)
{
    const char* const it1name = alg.iter_names [0];
    const char* const it2name = alg.iter_names [1];
    const char* const outname = alg.iter_names [2];
    const char* const fname   = "set_difference";
    const char* const funname = predicate ? "Less" : 0;

    UserClass* const xsrc1 = UserClass::from_char (src1, nsrc1,
                                                   true /* must be sorted */);
    UserClass* const xsrc2 = UserClass::from_char (src2, nsrc2,
                                                   true /* must be sorted */);

    // assert that the sequences have been successfully created
    RW_ASSERT (0 == nsrc1 || xsrc1);
    RW_ASSERT (0 == nsrc2 || xsrc2);

    UserClass* const xdst = new UserClass [ndst];

    const int max1_id = nsrc1 > 0 ? xsrc1[nsrc1 - 1].id_ : -1;

    UserClass* const xsrc1_end = xsrc1 + nsrc1;
    UserClass* const xsrc2_end = xsrc2 + nsrc2;
    UserClass* const xdst_end  = xdst + ndst;

    const std::size_t last_n_op_lt = UserClass::n_total_op_lt_;

    const Less pred (0, 0);
    const Less* const ppred = predicate ? &pred : 0;

    UserClass* xdst_res = alg.set_difference (xsrc1, xsrc1_end,
                                              xsrc2, xsrc2_end, 
                                              xdst, xdst_end, ppred);

    // check the returned value
    bool success = xdst_res == xdst_end;
    rw_assert (success, 0, line,
               "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...) "
               "got res + %td, expected res + %zu", 
               __LINE__, fname, it1name, it2name, outname, predicate, 
               funname, src1, src2, xdst_res - xdst, ndst);

    // quit here to avoid the running out of the array boundaries
    if (!success) {    
        delete[] xsrc1;
        delete[] xsrc2;
        delete[] xdst;
        return;
    }

    const std::size_t n_ops_lt = ppred ? 
        Less::funcalls_ : UserClass::n_total_op_lt_ - last_n_op_lt;

    // verify the algorithm correctness
    std::size_t i = 0;
    std::size_t n1 = 0;
    std::size_t n2 = 0;
    for ( ; i < ndst; ++i) {
        xdst[i].origin_ <= max1_id ? n1++ : n2++;
        success = xdst[i].data_.val_ == res[i];
        if (!success)
            break;
    }

    // to avoid errors in --trace mode
    i = i < ndst ? i : ndst - 1;

    rw_assert (success, 0, line,
               "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...) "
               " ==> \"%{X=*.*}\", expected \"%s\"", 
               __LINE__, fname, it1name, it2name, outname, predicate, 
               funname, src1, src2, int (ndst), i, xdst, res);

    // verfiy that only elements from first sequence sequence were taken
    success = n1 == ndst;
    rw_assert (success, 0, line,
               "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...) "
               " ==> \"%{X=*.*}\" got %zu elements from first sequence and "
               "%zu from second, expected %zu from first and 0 from second",
               __LINE__, fname, it1name, it2name, outname, predicate, funname, 
               src1, src2, int (ndst), -1, xdst, n1, n2, ndst);

    // verify that the operation is stable : two equal elements
    // should go in the same order
    for (i = 1; i < ndst; i++) {
        success = xdst[i - 1].origin_ < xdst[i].origin_;
        if (!success)
            break;
    }

    // to avoid errors in --trace mode
    if (ndst > 1) {
        i = i < ndst ? i : ndst - 1;

        rw_assert (success, 0, line,
                   "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...)"
                   " ==> \"%{X=*.*}\" not stable : elements %#c==%#c have "
                   "ids %d >= %d ",
                   __LINE__, fname, it1name, it2name, outname, predicate, 
                   funname, src1, src2, int (ndst), i, xdst,
                   xdst[i - 1].data_.val_, xdst[i].data_.val_,
                   xdst[i - 1].origin_, xdst[i].origin_);
    }

    // verify the complexity
    const std::size_t n_exp_ops =
        nsrc1 + nsrc2 > 0 ? 2 * (nsrc1 + nsrc2) - 1 : 0;

    rw_assert (n_ops_lt <= n_exp_ops, 0, line,
               "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...) "
               "complexity: got %zu, expected no more than %zu",
               __LINE__, fname, it1name, it2name, outname, predicate, funname, 
               src1, src2, n_ops_lt, n_exp_ops);

    delete[] xsrc1;
    delete[] xsrc2;
    delete[] xdst;
}

/**************************************************************************/

void test_set_difference (const SetDifferenceBase &alg, 
                          bool                     predicate)
{
    const char* const it1name = alg.iter_names [0];
    const char* const it2name = alg.iter_names [1];
    const char* const outname = alg.iter_names [2];
    const char* const fname   = "set_difference";
    const char* const funname = predicate ? "Less" : 0;

    rw_info (0, 0, 0,
             "%s std::%s(%s, %3$s, %s, %4$s, %1$s%{?}, %s%{;})",
             outname, fname, it1name, it2name, predicate, funname);

#define TEST(src1, src2, res)                                           \
    test_set_difference (__LINE__, src1, sizeof src1 - 1,               \
                         src2, sizeof src2 - 1, res, sizeof res - 1,    \
                         predicate, alg)  

    //    +-------------------- first set
    //    |        +----------- second set
    //    |        |        +-- difference
    //    |        |        |
    //    V        V        V
    TEST ("a",     "",      "a");
    TEST ("abcde", "",      "abcde");
    TEST ("",      "a",     "");
    TEST ("",      "abcde", "");
    TEST ("a",     "b",     "a");
    TEST ("b",     "a",     "b");
    TEST ("aa",    "aa",    "");
    TEST ("ab",    "ab",    "");
    TEST ("aa",    "ab",    "a");
    TEST ("aa",    "bb",    "aa");
    TEST ("ab",    "bb",    "a");
    TEST ("ac",    "bb",    "ac");
    TEST ("ace",   "bdf",   "ace");
    TEST ("acf",   "bdf",   "ac");
    TEST ("ade",   "bdf",   "ae");
    TEST ("bce",   "bdf",   "ce");
    TEST ("aacee", "aacee", "");
    TEST ("aacee", "aaace", "e");
    TEST ("aacee", "aaaae", "ce");
    TEST ("aacee", "aaaaa", "cee");
    TEST ("aacee", "aaabd", "cee");
    TEST ("aacee", "aabee", "c");
    TEST ("aaaaa", "aaaaa", "");
    TEST ("aaaaa", "aaaa",  "a");
    TEST ("aaaaa", "aaa",   "aa");
    TEST ("aaaaa", "aa",    "aaa");
    TEST ("aaaaa", "a",     "aaaa");
    TEST ("acegi", "bdfhj", "acegi");
    TEST ("bdfhj", "acegi", "bdfhj");
}

/**************************************************************************/

/* extern */ int rw_opt_no_predicate;           // --no-predicate
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;         // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class InputIterator1, class InputIterator2, class OutputIterator>
void gen_set_difference_test (const InputIterator1&,
                              const InputIterator2&,
                              const OutputIterator&,
                              bool predicate)
{
    const SetDifference<InputIterator1, InputIterator2, OutputIterator> alg;

    test_set_difference (alg, predicate);
}

/**************************************************************************/

template <class InputIterator1, class InputIterator2>
void gen_set_difference_test (const InputIterator1 &it1,
                              const InputIterator2 &it2,
                              bool predicate)
{
    if (0 == rw_opt_no_output_iter)
        gen_set_difference_test (
            it1, it2, OutputIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_fwd_iter)
        gen_set_difference_test (
            it1, it2, FwdIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_bidir_iter)
        gen_set_difference_test (
            it1, it2, BidirIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_rnd_iter)
        gen_set_difference_test (
            it1, it2, RandomAccessIter<UserClass>(0, 0, 0), predicate);
}

template <class InputIterator1>
void gen_set_difference_test (const InputIterator1 &it1, 
                              bool                  predicate)
{
    if (0 == rw_opt_no_input_iter)
        gen_set_difference_test (
            it1, InputIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_fwd_iter)
        gen_set_difference_test (
            it1, ConstFwdIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_bidir_iter)
        gen_set_difference_test (
            it1, ConstBidirIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_rnd_iter)
        gen_set_difference_test (
            it1, ConstRandomAccessIter<UserClass>(0, 0, 0), predicate);
}

// generates a specialization of the set_union test for each of the required
// iterator categopries
void gen_set_difference_test (bool predicate)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s, class %s%{?}, class %s%{;}> %3$s "
             "set_difference (%1$s, %1$s, %2$s, %2$s, %3$s%{?}, %s%{;})",
             "InputIterator1", "InputIterator2", "OutputIterator",
             predicate, "Compare", predicate, "Compare");

    if (rw_opt_no_output_iter)
        rw_note (0, 0, 0, "OutputIterator test disabled");

    if (rw_opt_no_input_iter)
        rw_note (0, 0, 0, "InputIterator test disabled");
    else
        gen_set_difference_test (InputIter<UserClass>(0, 0, 0), predicate);

    if (rw_opt_no_fwd_iter)
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    else
        gen_set_difference_test (ConstFwdIter<UserClass>(0, 0, 0), predicate);

    if (rw_opt_no_bidir_iter)
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    else
        gen_set_difference_test (ConstBidirIter<UserClass>(0, 0, 0), predicate);

    if (rw_opt_no_rnd_iter)
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    else
        gen_set_difference_test (
            ConstRandomAccessIter<UserClass>(0, 0, 0), predicate);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if (rw_opt_no_predicate)
        rw_note (0, 0, 0, "predicate test disabled");

    const int niters = rw_opt_no_predicate ? 1 : 2;

    //////////////////////////////////////////////////////////////////
    for (int i = 0; i != niters; ++i) {
        gen_set_difference_test (1 == i);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.set.difference",
                    0 /* no comment */, run_test,
                    "|-no-predicate#"
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_predicate,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
