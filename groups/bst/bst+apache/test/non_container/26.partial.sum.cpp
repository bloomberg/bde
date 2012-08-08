/***************************************************************************
 *
 * 26.partial.sum.cpp - test exercising 26.4.3 [lib.partial.sum]
 *
 * $Id: 26.partial.sum.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <numeric>      // for partial_sum
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

// has_plus
template <class T>
struct has_plus: T
{
private:
    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};


template <class T>
const has_plus<T>& operator+ (const has_plus<T>& lhs, const has_plus<T>& rhs)
{
    _RWSTD_UNUSED (rhs);
    return lhs;
}

_RWSTD_NAMESPACE (std) {

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
OutputIter<has_plus<assign<base<cpy_ctor> > > >
partial_sum (InputIter<has_plus<assign<base<cpy_ctor> > > >,
             InputIter<has_plus<assign<base<cpy_ctor> > > >,
             OutputIter<has_plus<assign<base<cpy_ctor> > > >);

template
OutputIter<assign<base<cpy_ctor> > >
partial_sum (InputIter<assign<base<cpy_ctor> > >,
             InputIter<assign<base<cpy_ctor> > >,
             OutputIter<assign<base<cpy_ctor> > >,
             binary_func<assign<base<cpy_ctor> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

UserClass operator+ (const UserClass &lhs, const UserClass &rhs)
{
    return UserClass (lhs)+= rhs;
}

/**************************************************************************/

template <class T>
struct conv_to_T
{
    static conv_to_T make (T val) {
        return conv_to_T (val);
    }

    // strictly convertible to a T value
    operator T () const {
        return val_;
    }

private:
    // not (publicly) Default-Constructible
    conv_to_T (T val): val_ (val) { }

    void operator= (conv_to_T);   // not Assignable
    void operator!() const;       // not defined

    T val_;
};

/**************************************************************************/

struct Accumulator
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Accumulator (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type convertible to UserClass
    conv_to_T<UserClass> operator() (const UserClass &x,
                                     const UserClass &y) /* non-const */ {
        ++funcalls_;
        UserClass res (x);
        res.data_.val_ += y.data_.val_;
        return conv_to_T<UserClass>::make (res);
    }

private:
    void operator= (Accumulator&);   // not assignable
};

std::size_t Accumulator::funcalls_;

/**************************************************************************/

struct PartialSumBase
{
    virtual ~PartialSumBase() { /* no-op */ }

    const char* iter_names [2];

    // pure virtual
    virtual UserClass*
    partial_sum (const UserClass *xsrc, const UserClass *xsrc_end,
                 UserClass *xdst, const UserClass *xdst_end,
                 const Accumulator *op) const = 0;
};


template <class InputIterator, class OutputIterator>
struct PartialSum: PartialSumBase
{
    PartialSum () {
        iter_names [0] = type_name (InputIterator (0, 0, 0), (UserClass*)0);
        iter_names [1] = type_name (OutputIterator (0, 0, 0), (UserClass*)0);
    }

    virtual UserClass*
    partial_sum (const UserClass *xsrc, const UserClass *xsrc_end,
                 UserClass *xdst, const UserClass *xdst_end,
                 const Accumulator *op) const {

        const InputIterator  first (xsrc,     xsrc, xsrc_end);
        const InputIterator  last  (xsrc_end, xsrc, xsrc_end);
        const OutputIterator result (xdst,    xdst, xdst_end);

        const OutputIterator res = op ?
              std::partial_sum (first, last, result, *op)
            : std::partial_sum (first, last, result);

        // silence EDG eccp 3.7 and prior remark #550-D:
        //   variable was set but never used
        _RWSTD_UNUSED (res);

        return res.cur_;
    }
};

/**************************************************************************/

// exercises partial_sum (26.4.3)
void test_partial_sum (const std::size_t     N,
                       const PartialSumBase &alg,
                       bool                  binop,
                       bool                  same_seq)
{
    const char* const itname =  alg.iter_names [0];
    const char* const outname = alg.iter_names [1];
    const char* const opname = "Plus";

    rw_info (0, 0, 0,
             "std::partial_sum (%s, %1$s, %s%{?}, %s%{;})%{?}, %s%{;}",
             itname, outname, binop, opname, same_seq, "first == result");

    UserClass::gen_ = gen_seq;

    UserClass* const src = new UserClass [N];
    UserClass* dst = same_seq ? src : new UserClass [N];

    for (std::size_t i = 0; i != N; ++i) {

        UserClass* const src_end = src + i;
        UserClass* const dst_end = dst + i;

        std::size_t last_n_op_plus  = UserClass::n_total_op_plus_assign_;

        const Accumulator acc  (0, 0);
        const Accumulator* const pbinop = binop ? &acc : 0;

        std::size_t k = 0;
        int* const tmp_val = new int [i];
        for (; k < i; ++k)
            tmp_val [k] = src [k].data_.val_;

        const UserClass* const res =
            alg.partial_sum (src, src_end, dst, dst_end, pbinop);

        const std::size_t plus_ops = binop ? Accumulator::funcalls_ :
            UserClass::n_total_op_plus_assign_ - last_n_op_plus;

        // verify the returned iterator 26.4.3, p2
        bool success = res == dst_end;
        rw_assert (success, 0, __LINE__,
                   "partial_sum<%s, %s%{?}, %s%{;}>"
                   "({%{X=+*}}, ...) == result + %d, got result %td",
                   itname, outname, binop, opname,
                   int (i), src, dst_end - dst, res - dst_end);

        int sum = 0;
        for (k = 0; k < i; ++k) {
            sum += tmp_val [k];
            success = dst [k].data_.val_ == sum;
            if (!success)
                break;
        }

        // verify the result 26.4.3, p1
        if (i > 0) {
            // to avoid errors in --trace mode
            k = k < i ? k : i - 1;

            rw_assert (success, 0, __LINE__,
                       "partial_sum<%s, %s%{?}, %s%{;}>"
                       "({%{X=+*}}, ...) ==> {%{X=+*.*}}, expected %d",
                       itname, outname, binop, opname,
                       int (i), src, int (i), int (k), dst, sum);
        }

        delete[] tmp_val;

        if (!success)
            break;

        // verify the complexity, 26.4.3, p3
        const std::size_t exp_plus_ops = i > 0 ? i - 1 : 0;
        success = plus_ops == exp_plus_ops;
        rw_assert (success, 0, __LINE__,
                   "partial_sum <%s, %s%{?}, %s%{;}>"
                   "({%{X=+*}}, ...) complexity: got %zu invocations "
                   "of %s, expected %zu",
                   itname, outname, binop, opname,
                   int (i), src, plus_ops,
                   binop ? "BinaryPlus" : "operator+", exp_plus_ops);

        if (!success)
            break;
    }

    delete[] src;

    if (!same_seq)
        delete[] dst;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 64;            // --nloops
/* extern */ int rw_opt_no_binary_op;           // --no-binary_op
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;         // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class InputIterator, class OutputIterator>
void gen_partial_sum_test (const std::size_t N,
                           const InputIterator&,
                           const OutputIterator&,
                           bool              binop)
{
    const PartialSum<InputIterator, OutputIterator> alg;

    // test the algorithm than input and output arrays differ
    test_partial_sum (N, alg, binop, false);

    // test the algorithm than input and output arrays are the same
    test_partial_sum (N, alg, binop, true);
}


template <class InputIterator>
void gen_partial_sum_test (const std::size_t     N,
                           const InputIterator  &it,
                           bool                  binop)
{
    if (0 == rw_opt_no_output_iter)
        gen_partial_sum_test (
            N, it, OutputIter<UserClass>(0, 0, 0), binop);
    if (0 == rw_opt_no_fwd_iter)
        gen_partial_sum_test (
            N, it, FwdIter<UserClass>(0, 0, 0), binop);
    if (0 == rw_opt_no_bidir_iter)
        gen_partial_sum_test (
            N, it, BidirIter<UserClass>(0, 0, 0), binop);
    if (0 == rw_opt_no_rnd_iter)
        gen_partial_sum_test (
            N, it, RandomAccessIter<UserClass>(0, 0, 0), binop);
}

// generates a specialization of the partial_sum test for each of the required
// iterator categopries
void gen_partial_sum_test (const std::size_t N,
                           bool              binop)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s%{?}, class %s%{;}> "
             "%2$s partial_sum (%1$s, %1$s, %2$s%{?}, %s%{;})",
             "InputIterator", "OutputIterator", binop, "BinaryOperation",
             binop, "BinaryOperation");

    if (rw_opt_no_output_iter)
        rw_note (0, 0, 0, "OutputIterator test disabled");

    if (rw_opt_no_input_iter)
        rw_note (0, 0, 0, "InputIterator test disabled");
    else
        gen_partial_sum_test (N, InputIter<UserClass>(0, 0, 0), binop);

    if (rw_opt_no_fwd_iter)
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    else
        gen_partial_sum_test (N, ConstFwdIter<UserClass>(0, 0, 0), binop);

    if (rw_opt_no_bidir_iter)
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    else
        gen_partial_sum_test (N, ConstBidirIter<UserClass>(0, 0, 0), binop);

    if (rw_opt_no_rnd_iter)
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    else
        gen_partial_sum_test (N, ConstRandomAccessIter<UserClass>(0, 0, 0),
                              binop);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    gen_partial_sum_test (N, false);

    if (rw_opt_no_binary_op)
        rw_note (0, 0, 0,
                 "partial_sum with binary operation test disabled");
    else
        gen_partial_sum_test (N, true);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.partial.sum",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-binary_op#"
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_nloops,
                    &rw_opt_no_binary_op,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
