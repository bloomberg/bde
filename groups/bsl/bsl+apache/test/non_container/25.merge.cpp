/***************************************************************************
 *
 * 25.merge.cpp - test exercising 25.3.4 [lib.alg.merge]
 *
 * $Id: 25.merge.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for merge, 
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
OutputIter<lt_comp<assign<base<cpy_ctor> > > >
merge (InputIter<lt_comp<assign<base<cpy_ctor> > > >, 
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       OutputIter<lt_comp<assign<base<cpy_ctor> > > >);

template
OutputIter<lt_comp<assign<base<cpy_ctor> > > >
merge (InputIter<lt_comp<assign<base<cpy_ctor> > > >, 
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       OutputIter<lt_comp<assign<base<cpy_ctor> > > >,
       binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

template
void
inplace_merge (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
inplace_merge (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
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
struct MergeBase
{
    const char* iter_names [3];

    virtual ~MergeBase () { }

    // invokes inplace_merge with iterators initialized
    // to the specified arguments and (optionally) with
    // a predicate object
    virtual void
    inplace_merge (UserClass*, UserClass*, UserClass*, const Less*) const {
        RW_ASSERT (!"test logic error");
    }

    // invokes merge
    virtual UserClass*
    merge (const UserClass*, const UserClass*, const UserClass*,
           const UserClass*, UserClass*, UserClass*, const Less*) const {
        RW_ASSERT (!"test logic error");
        return 0;
    }
};

template <class InputIterator1, class InputIterator2, class OutputIterator>
struct Merge: MergeBase
{
    Merge () {
        iter_names [0] = type_name (InputIterator1 (0, 0, 0), (UserClass*)0);
        iter_names [1] = type_name (InputIterator2 (0, 0, 0), (UserClass*)0);
        iter_names [2] = type_name (OutputIterator (0, 0, 0), (UserClass*)0);
    }

    virtual UserClass*
    merge (const UserClass    *xsrc1, const UserClass *xsrc1_end,
           const UserClass    *xsrc2, const UserClass *xsrc2_end,
           UserClass          *xdst, UserClass *xdst_end,
           const Less *ppred) const {

        const InputIterator1 first1 (xsrc1,     xsrc1, xsrc1_end);
        const InputIterator1 last1  (xsrc1_end, xsrc1, xsrc1_end);
        const InputIterator2 first2 (xsrc2,     xsrc2, xsrc2_end);
        const InputIterator2 last2  (xsrc2_end, xsrc2, xsrc2_end);

        const OutputIterator result (xdst, xdst, xdst_end);

        const OutputIterator ret = ppred ?
              std::merge (first1, last1, first2, last2, result, *ppred)
            : std::merge (first1, last1, first2, last2, result);

        // silence EDG eccp 3.7 and prior remark #550-D:
        //   variable was set but never used
        _RWSTD_UNUSED (ret);

        return ret.cur_;
    }
};

template <class BidirectionalIterator>
struct InplaceMerge: MergeBase
{
    InplaceMerge () {
        iter_names [0] = type_name (BidirectionalIterator (0, 0, 0),
                                    (UserClass*)0);
        iter_names [1] = 0;
        iter_names [2] = 0;
    }

    virtual void
    inplace_merge (UserClass *xsrc, UserClass *xsrc_mid, UserClass *xsrc_end,
                   const Less *ppred) const {

        const BidirectionalIterator first (xsrc,     xsrc, xsrc_end);
        const BidirectionalIterator mid   (xsrc_mid, xsrc, xsrc_end);
        const BidirectionalIterator last  (xsrc_end, xsrc, xsrc_end);

        if (ppred)
            std::inplace_merge (first, mid, last, *ppred);
        else
            std::inplace_merge (first, mid, last);
    }
};

/**************************************************************************/

// ordinary (non-template) function to minimize code bloat
void test_merge (int                line,
                 const char        *src1,
                 const char        *src2,
                 const std::size_t  midinx,
                 bool               predicate,
                 const MergeBase   &alg)
{
    const char* const it1name  = alg.iter_names [0];
    const char* const it2name  = alg.iter_names [1];
    const char* const outname  = alg.iter_names [2];
    const char* const predname = predicate ? "Less" : 0;
    const bool        inplace  = 0 == it2name;
    const char* const algname  =  inplace ? "inplace_merge" : "merge";

    const std::size_t nsrc1 = std::strlen (src1);
    const std::size_t nsrc2 = std::strlen (src2);

    UserClass* const xsrc1 = UserClass::from_char (src1, nsrc1);
    UserClass* const xsrc2 = UserClass::from_char (src2, nsrc2);

    const std::size_t ndst = nsrc1 + nsrc2;
    UserClass* const xdst = inplace ? xsrc1 : new UserClass [ndst];

    UserClass* const xsrc1_end = xsrc1 + nsrc1;
    UserClass* const xsrc2_end = xsrc2 + nsrc2;
    UserClass* const xsrc_mid  = xsrc1 + midinx;
    UserClass* const xdst_end  = xdst + ndst;

    const std::size_t last_n_op_lt = UserClass::n_total_op_lt_;

    const Less pred (0, 0);
    const Less* const ppred = predicate ? &pred : 0;

    UserClass* xdst_res = 0;

    if (inplace) {  // inplace_merge
        alg.inplace_merge (xsrc1, xsrc_mid, xsrc1_end, ppred);
    }
    else {
        xdst_res = alg.merge (xsrc1, xsrc1_end,
                              xsrc2, xsrc2_end, xdst, xdst_end, ppred);
    }

    const std::size_t n_ops_lt = ppred ? 
        Less::funcalls_ : UserClass::n_total_op_lt_ - last_n_op_lt;

    bool success = true;

    // check output iterator for merge
    if (!inplace) {
        success = xdst_res == xdst_end;
        rw_assert (success, 0, line,
                   "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...)"
                   " == first + %zu, got first + %td",
                   __LINE__, algname, it1name, it2name, outname, predicate,
                   predname, src1, src2, ndst, xdst_res - xdst);
    }

    // check that the sequence is sorted
    success = is_sorted_lt (xdst, xdst_end);
    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s, %s%{;}%{?}, %s%{;}> "
               "(\"%s\", %{?}\"%s\"%{;}%{?}%zu%{;}, ...) ==> \"%{X=*.*}\" "
               "not sorted",
               __LINE__, algname, it1name, !inplace, it2name, outname, 
               predicate, predname, src1, !inplace, src2, inplace, midinx,
               int (ndst), -1, xdst);

    // check that the algorithm is stable
    std::size_t i = 1;
    for ( ; i < ndst; i++) {
        if (xdst [i - 1].data_.val_ == xdst [i].data_.val_) {
            success = xdst [i - 1].origin_ < xdst [i].origin_;
            if (!success)
                break;
        }
    }

    // to avoid errors in --trace mode
    i = i < ndst ? i : ndst - 1;

    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s, %s%{;}%{?}, %s%{;}> "
               "(\"%s\", %{?}\"%s\"%{;}%{?}%zu%{;}, ...) ==> \"%{X=*.*}\" "
               "not stable at %zu: got ids %d != %d for values %#c == %#c",
               __LINE__, algname, it1name, !inplace, it2name, outname, 
               predicate, predname, src1, !inplace, src2, inplace, midinx,
               int (ndst), i, xdst, i, xdst [i - 1].origin_, 
               xdst [i].origin_, xdst [i - 1].data_.val_, xdst [i].data_.val_);

    // check the complexity
    success = n_ops_lt <= ndst - 1;
    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s, %s%{;}%{?}, %s%{;}> "
               "(\"%s\", %{?}\"%s\"%{;}%{?}%zu%{;}, ...) ==> \"%{X=*.*}\" "
               "complexity: got %zu, expected <= %zu",
               __LINE__, algname, it1name, !inplace, it2name, outname, 
               predicate, predname, src1, !inplace, src2, inplace, midinx,
               int (ndst), -1, xdst, n_ops_lt, ndst - 1);

    delete[] xsrc1;
    delete[] xsrc2;

    if (!inplace)
        delete[] xdst;
}

/**************************************************************************/

void test_merge (const MergeBase &alg, bool predicate)
{
    const char* const it1name  = alg.iter_names [0];
    const char* const it2name  = alg.iter_names [1];
    const char* const outname  = alg.iter_names [2];
    const char* const predname = predicate ? "Less" : 0;
    const char* const algname  = it2name ? "merge" : "inplace_merge";

    rw_info (0, 0, 0,
             "%s std::%s(%s, %3$s, %s, %4$s, %1$s%{?}, %s%{;})",
             outname, algname, it1name, it2name, predicate, predname);

#define TEST(src1, src2)                                            \
    test_merge (__LINE__, src1, src2, 0, predicate, alg)

    TEST ("ab", "");
    TEST ("", "ab");

    TEST ("a", "b");
    TEST ("b", "a");
    TEST ("aa", "bb");
    TEST ("bb", "aa");

    TEST ("acegi", "bdfhj");
    TEST ("aaegi", "bdfjj");
    TEST ("accgi", "bdhhj");
    TEST ("aceei", "bffhj");
    TEST ("acegg", "ddfhj");
    TEST ("aceii", "bbfhj");

    TEST ("bdfhj", "acegi");
    TEST ("bdfjj", "aaegi");
    TEST ("bdhhj", "accgi");
    TEST ("bffhj", "aceei");
    TEST ("ddfhj", "acegg");
    TEST ("bbfhj", "aceii");

    TEST ("abcde", "abcde");
    TEST ("aacde", "aacde");
    TEST ("abbde", "abbde");
    TEST ("abcce", "abcce");
    TEST ("abcdd", "abcdd");
    TEST ("abcee", "abcee");

    TEST ("aaegi", "bdfjj");
    TEST ("accgi", "bdhhj");
    TEST ("aceei", "bffhj");
    TEST ("acegg", "ddfhj");
    TEST ("aceii", "bbfhj");

    TEST ("a","abcdefghij");
    TEST ("ac","abcdefghi");
    TEST ("ace","abcdefgh");
    TEST ("aceg","abcdefg");

    TEST ("j","abcdefghjj");
    TEST ("gj","abcdeggjj");
    TEST ("egj","abeeggjj");

    TEST ("abcdefghij","a");
    TEST ("abcdefghi","ac");
    TEST ("abcdefgh","ace");
    TEST ("abcdefg","aceg");

    TEST ("abcdefghjj","j");
    TEST ("abcdeggjj","gj");
    TEST ("abeeggjj","egj");

    TEST ("aaaaa", "aaaaa");
    TEST ("bbbbb", "bbbbb");
}

/**************************************************************************/

void test_inplace_merge (const MergeBase &alg, bool predicate)
{
    const char* const itname   = alg.iter_names [0];
    const char* const predname = predicate ? "Less" : 0;
    const char* const algname  = alg.iter_names [1] ?
        "merge" : "inplace_merge";

    rw_info (0, 0, 0, "std::%s(%s, %2$s, %2$s%{?}, %s%{;})",
             algname, itname, predicate, predname);

#undef TEST
#define TEST(src, mid)                                            \
    test_merge (__LINE__, src, "", mid, predicate, alg)     

    TEST ("a",  0);
    TEST ("aa", 0);
    TEST ("a",  1);
    TEST ("aa", 2);

    TEST ("aa", 1);
    TEST ("ab", 1);
    TEST ("ba", 1);

    TEST ("abcdefghij", 5);
    TEST ("aacdeffhij", 5);
    TEST ("abbdefggij", 5);
    TEST ("abccefghhj", 5);
    TEST ("abcddfghii", 5);
    TEST ("abceefghjj", 5);

    TEST ("abcdeabcde", 5);
    TEST ("aacdeaacde", 5);
    TEST ("abbdeabbde", 5);
    TEST ("abcceabcce", 5);
    TEST ("abcddabcdd", 5);
    TEST ("abceeabcee", 5);

    TEST ("aabcdefghi", 1);
    TEST ("aaacdefghi", 1);
    TEST ("babcdefghi", 1);
    TEST ("babbdefghi", 1);

    TEST ("abcdefghia", 9);
    TEST ("aacdefghia", 9);
    TEST ("abcdefghib", 9);
    TEST ("abbdefghib", 9);
}

/**************************************************************************/

/* extern */ int rw_opt_no_merge;           // --no-merge
/* extern */ int rw_opt_no_inplace_merge;   // --no-inplace_merge
/* extern */ int rw_opt_no_predicate;       // --no-predicate
/* extern */ int rw_opt_no_input_iter;      // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;     // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;        // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;      // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;        // --no-RandomAccessIterator

/**************************************************************************/

template <class InputIterator1, class InputIterator2, class OutputIterator>
void gen_merge_test (const InputIterator1&,
                     const InputIterator2&,
                     const OutputIterator&,
                     bool predicate)
{
    const Merge<InputIterator1, InputIterator2, OutputIterator> alg;

    test_merge (alg, predicate);
}

template <class InputIterator1, class InputIterator2>
void gen_merge_test (const InputIterator1 &it1,
                     const InputIterator2 &it2,
                     bool predicate)
{
    if (0 == rw_opt_no_output_iter)
        gen_merge_test (it1, it2, OutputIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_fwd_iter)
        gen_merge_test (it1, it2, FwdIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_bidir_iter)
        gen_merge_test (it1, it2, BidirIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_rnd_iter)
        gen_merge_test (it1, it2, RandomAccessIter<UserClass>(0, 0, 0),
                        predicate);
}

template <class InputIterator1>
void gen_merge_test (const InputIterator1 &it1, bool predicate)
{
    if (0 == rw_opt_no_input_iter)
        gen_merge_test (it1, InputIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_fwd_iter)
        gen_merge_test (it1, ConstFwdIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_bidir_iter)
        gen_merge_test (it1, ConstBidirIter<UserClass>(0, 0, 0), predicate);
    if (0 == rw_opt_no_rnd_iter)
        gen_merge_test (it1, ConstRandomAccessIter<UserClass>(0, 0, 0),
                        predicate);
}

// generates a specialization of the merge test for each of the required
// iterator categopries
void gen_merge_test (bool predicate)
{
    if (rw_opt_no_input_iter)
        rw_note (0, 0, 0, "InputIterator test disabled");
    else
        gen_merge_test (InputIter<UserClass>(0, 0, 0), predicate);

    if (rw_opt_no_fwd_iter)
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    else
        gen_merge_test (ConstFwdIter<UserClass>(0, 0, 0), predicate);

    if (rw_opt_no_bidir_iter)
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    else
        gen_merge_test (ConstBidirIter<UserClass>(0, 0, 0), predicate);

    if (rw_opt_no_rnd_iter)
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    else
        gen_merge_test (ConstRandomAccessIter<UserClass>(0, 0, 0), predicate);
}

/**************************************************************************/

template <class BidirectionalIterator>
void gen_inplace_merge_test (const BidirectionalIterator&, bool predicate)
{
    const InplaceMerge<BidirectionalIterator> alg;

    test_inplace_merge (alg, predicate);
}

/**************************************************************************/

static int run_test (int, char*[])
{
    if (rw_opt_no_predicate)
        rw_note (0, 0, 0, "predicate test disabled");

    const int niters = rw_opt_no_predicate ? 1 : 2;

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_merge)
        rw_note (0, 0, 0, "merge test disabled");
    else {
        for (int i = 0; i != niters; ++i) {
            gen_merge_test (1 == i);
        }
    }

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_inplace_merge) {
        rw_note (0, 0, 0, "inplace_merge test disabled");
    }
    else {
        for (int i = 0; i != niters; ++i) {

            if (rw_opt_no_bidir_iter)
                rw_note (0, 0, 0, "BidirectionalIterator test disabled");
            else
                gen_inplace_merge_test (BidirIter<UserClass> (0, 0, 0), 1 == i);

            if (rw_opt_no_rnd_iter)
                rw_note (0, 0, 0, "RandomAccessIterator test disabled");
            else
                gen_inplace_merge_test (RandomAccessIter<UserClass> (0, 0, 0),
                                        1 == i);
        }
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.merge",
                    0 /* no comment */,
                    run_test,
                    "|-no-merge# "
                    "|-no-inplace_merge# "
                    "|-no-predicate#"
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_merge,
                    &rw_opt_no_inplace_merge,
                    &rw_opt_no_predicate,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
