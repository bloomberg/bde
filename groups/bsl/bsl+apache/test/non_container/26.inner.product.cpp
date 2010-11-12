/***************************************************************************
 *
 * 26.inner.product.cpp - test exercising 26.4.2 [lib.inner.product]
 *
 * $Id: 26.inner.product.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <numeric>      // for inner_product 
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

// plus-assign
template <class T>
struct plus_asgn: T
{
    plus_asgn& operator+= (const plus_asgn& rhs) {
        unused = rhs.unused;
        return *this;
    }
private:
    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};

#if !RW_BUG // standard requires a = a + b, but RW supports only a += b
template <class T>
plus_asgn<T> operator+(plus_asgn<T> a, plus_asgn<T> b)
{
    a += b;
    return a;
}
#endif // !RW_BUG

template <class T>
const plus_asgn<T>&
operator* (const plus_asgn<T> &lhs, const plus_asgn<T> &rhs)
{
    _RWSTD_UNUSED (rhs);

    return lhs;
}


_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template 
plus_asgn<assign<base<cpy_ctor> > >
inner_product (InputIter<plus_asgn<assign<base<cpy_ctor> > > >,
               InputIter<plus_asgn<assign<base<cpy_ctor> > > >,
               InputIter<plus_asgn<assign<base<cpy_ctor> > > >,
               plus_asgn<assign<base<cpy_ctor> > >);

template 
assign<base<cpy_ctor> > 
inner_product (InputIter<assign<base<cpy_ctor> > >,
               InputIter<assign<base<cpy_ctor> > >,
               InputIter<assign<base<cpy_ctor> > >,
               assign<base<cpy_ctor> >,
               binary_func<assign<base<cpy_ctor> > >,
               binary_func<assign<base<cpy_ctor> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

UserClass operator* (const UserClass& lhs, const UserClass& rhs)
{
    return UserClass (lhs) *= rhs;
}

#if !RW_BUG // standard requires a = a + b, but RW requires a += b
UserClass operator+(UserClass lhs, UserClass rhs)
{
    lhs += rhs;
    return lhs;
}
#endif // !RW_BUG

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


struct Multiplicator
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Multiplicator (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type convertible to UserClass
    conv_to_T<UserClass> operator() (const UserClass &x,
                                     const UserClass &y) /* non-const */ {
        ++funcalls_;
        UserClass res (x);
        res.data_.val_ *= y.data_.val_;
        return conv_to_T<UserClass>::make (res);
    }

private:
    void operator= (Multiplicator&);   // not assignable
};

std::size_t Multiplicator::funcalls_;

/**************************************************************************/

struct InnerProductBase
{
    virtual ~InnerProductBase() { /* no-op */ }

    const char* iter_names [2];

    // pure virtual
    virtual UserClass
    inner_product (const UserClass     *xsrc1, const UserClass   *xsrc1_end,
                   const UserClass     *xsrc2, const UserClass   *xsrc2_end,
                   const UserClass&     init,  const Accumulator *op1, 
                   const Multiplicator *op2) const = 0;
};

template <class InputIterator1, class InputIterator2>
struct InnerProduct : InnerProductBase
{
    InnerProduct () {
        iter_names [0] = type_name (InputIterator1 (0, 0, 0), (UserClass*)0);
        iter_names [1] = type_name (InputIterator2 (0, 0, 0), (UserClass*)0);
    }

    virtual UserClass
    inner_product (const UserClass     *xsrc1, const UserClass   *xsrc1_end,
                   const UserClass     *xsrc2, const UserClass   *xsrc2_end,
                   const UserClass&     init,  const Accumulator *op1, 
                   const Multiplicator *op2) const {

        const InputIterator1 first1 (xsrc1,     xsrc1, xsrc1_end);
        const InputIterator1 last1  (xsrc1_end, xsrc1, xsrc1_end);
        const InputIterator2 first2 (xsrc2,     xsrc2, xsrc2_end);

        const UserClass res = op1 ?
              std::inner_product (first1, last1, first2, init, *op1, *op2)
            : std::inner_product (first1, last1, first2, init);

        // silence EDG eccp 3.7 and prior remark #550-D:
        //   variable was set but never used
        _RWSTD_UNUSED (res);

        return res;
    }
};

/**************************************************************************/

// exercises inner_product (26.4.2)
void test_inner_product (const std::size_t       N,
                         const InnerProductBase &alg,
                         bool                    binop)
{
    const char* const it1name = alg.iter_names [0];
    const char* const it2name = alg.iter_names [1];
    const char* const tname   = "UserClass";
    const char* const op1name = "Plus";
    const char* const op2name = "Multiple";

    rw_info (0, 0, 0, 
             "std::inner_product (%s, %1$s, %s, %s%{?}, %s, %s%{;})",
             it1name, it2name, tname, binop, op1name, op2name);

    // construct initial UserClass
    const UserClass init = UserClass ();
    int sum = init.data_.val_;

    UserClass::gen_ = gen_seq;

    UserClass* const buf1 = new UserClass [N];
    UserClass* const buf2 = new UserClass [N];
    
    for (std::size_t i = 0; i != N; ++i) {

        UserClass* const buf1_end = buf1 + i;
        UserClass* const buf2_end = buf2 + i;

        const Accumulator   acc  (0, 0);
        const Multiplicator mult (0, 0);

        const Accumulator* const   pbinop1 = binop ? &acc : 0;
        const Multiplicator* const pbinop2 = binop ? &mult : 0;

        const UserClass res = alg.inner_product (buf1, buf1_end, buf2, buf2_end, 
                                                 init, pbinop1, pbinop2);

        // verify the result 26.4.1, p1
        bool success = sum == res.data_.val_;
        rw_assert (success, 0, __LINE__,
                   "inner_product <%s, %s, %s%{?}, %s, %s%{;}>"
                   "({%{X=+*}}, {%{X=+*}}) == %d, got %d",
                   it1name, it2name, tname, binop, op1name, op2name,
                   int (i), buf1, int (i), buf2, sum, res.data_.val_);

        sum += (buf1 [i].data_.val_ * buf2 [i].data_.val_);

        if (!success)
            break;
    }

    delete[] buf1;
    delete[] buf2;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 64;            // --nloops
/* extern */ int rw_opt_no_binary_op;           // --no-binary_op
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class InputIterator1, class InputIterator2>
void gen_inner_product_test (const std::size_t N,
                             const InputIterator1&,
                             const InputIterator2&,
                             bool              binop)
{
    const InnerProduct<InputIterator1, InputIterator2> alg;

    test_inner_product (N, alg, binop);
}


template <class InputIterator1>
void gen_inner_product_test (const std::size_t     N,
                             const InputIterator1 &it1, 
                             bool                  binop)
{
    if (0 == rw_opt_no_input_iter)
        gen_inner_product_test (
            N, it1, InputIter<UserClass>(0, 0, 0), binop);
    if (0 == rw_opt_no_fwd_iter)
        gen_inner_product_test (
            N, it1, ConstFwdIter<UserClass>(0, 0, 0), binop);
    if (0 == rw_opt_no_bidir_iter)
        gen_inner_product_test (
            N, it1, ConstBidirIter<UserClass>(0, 0, 0), binop);
    if (0 == rw_opt_no_rnd_iter)
        gen_inner_product_test (
            N, it1, ConstRandomAccessIter<UserClass>(0, 0, 0), binop);
}

// generates a specialization of the inner_product test for each of the required
// iterator categopries
void gen_inner_product_test (const std::size_t N,
                             bool              binop)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s, class %s%{?}, class %s, "
             "class %s%{;}> %3$s inner_product (%1$s, %1$s, %2$s, "
             "%3$s%{?}, %s, %s%{;})", 
             "InputIterator1", "InputIterator2", "UserClass",
             binop, "BinaryOperation1", "BinaryOperation2", binop, 
             "BinaryOperation1", "BinaryOperation2");

    if (rw_opt_no_input_iter)
        rw_note (0, 0, 0, "InputIterator test disabled");
    else
        gen_inner_product_test (N, InputIter<UserClass>(0, 0, 0), binop);

    if (rw_opt_no_fwd_iter)
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    else
        gen_inner_product_test (N, ConstFwdIter<UserClass>(0, 0, 0), binop);

    if (rw_opt_no_bidir_iter)
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    else
        gen_inner_product_test (N, ConstBidirIter<UserClass>(0, 0, 0), binop);

    if (rw_opt_no_rnd_iter)
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    else
        gen_inner_product_test (N, ConstRandomAccessIter<UserClass>(0, 0, 0),
                                binop);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    gen_inner_product_test (N, false);

    if (rw_opt_no_binary_op)
        rw_note (0, 0, 0, 
                 "inner_product with binary operations test disabled");
    else
        gen_inner_product_test (N, true);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.inner.product",
                    0 /* no comment */, run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-binary_op#"
                    "|-no-InputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_nloops,
                    &rw_opt_no_binary_op,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
