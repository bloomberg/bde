/***************************************************************************
 *
 * 26.accumulate.cpp - test exercising 26.4.1 [lib.accumulate]
 *
 * $Id: 26.accumulate.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <numeric>      // for accumulate 
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

#if RW_BUG // standard requires a = a + b, but RW supports only a += b
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
#else // !RW_BUG
template <class T>
struct plus_asgn: T
{
private:
    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};

template <class T>
inline plus_asgn<T> operator+(plus_asgn<T> a, plus_asgn<T> b)
    { return a; }
#endif // !RW_BUG

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template 
plus_asgn<assign<base<cpy_ctor> > >
accumulate (InputIter<plus_asgn<assign<base<cpy_ctor> > > >,
            InputIter<plus_asgn<assign<base<cpy_ctor> > > >,
            plus_asgn<assign<base<cpy_ctor> > >);

template 
assign<base<cpy_ctor> > 
accumulate (InputIter<assign<base<cpy_ctor> > >,
            InputIter<assign<base<cpy_ctor> > >,
            assign<base<cpy_ctor> >,
            binary_func<assign<base<cpy_ctor> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

struct Y: public UserClass
{
    // number of times the object's += operator has been invoked,
    // regardless of whether the operation threw an exception or not
    std::size_t n_op_plus_assign_;

    static std::size_t n_total_op_plus_assign_;   // ... += operators ...

    // class thrown from the respective functions
    struct OpPlusAssign: Exception { };

    // throw object's `id' wrapped in the appropriate struct when the
    // corresponding n_total_xxx_ counter reaches the value pointed to
    // by the respective pointer below
    static std::size_t* op_plus_assign_throw_ptr_;

    // objects to which the pointers above initally point
    static std::size_t op_plus_assign_throw_count_;

    Y (): UserClass () { /* empty */ }

    Y (const Y &rhs): UserClass (rhs) { /* empty */ }

    Y& operator+= (const Y& rhs) {

        // verify id validity and uniqueness
        RW_ASSERT (id_ && id_ <= id_gen_);
        RW_ASSERT (rhs.id_ && rhs.id_ <= id_gen_);
        RW_ASSERT (this == &rhs || id_ != rhs.id_);

        // increment the number of times each distinct object
        // has been used as the argument to operator+=
        // (do so even if the function throws an exception below)
        ++n_op_plus_assign_;

        if (this != &rhs)
            ++_RWSTD_CONST_CAST (Y*, &rhs)->n_op_plus_assign_;

        // increment the total number of invocations of the operator
        // (do so even if the function throws an exception below)
        ++n_total_op_plus_assign_;

#ifndef _RWSTD_NO_EXCEPTIONS

        // throw an exception if the number of calls
        // to operator== reaches the given value

        if (   op_plus_assign_throw_ptr_
            && n_total_op_plus_assign_ == *op_plus_assign_throw_ptr_) {
            OpPlusAssign ex;
            ex.id_ = id_;
            throw ex;
        }

#endif   // _RWSTD_NO_EXCEPTIONS

        data_.val_ += rhs.data_.val_;
        return *this;
    }
};

#if !RW_BUG // standard requires a = a + b, but RW supports only a += b
inline Y operator+(Y a, Y b) { a += b; return a; }
#endif

/* static */ std::size_t  Y::n_total_op_plus_assign_;
/* static */ std::size_t* Y::op_plus_assign_throw_ptr_ =
    &Y::op_plus_assign_throw_count_;
/* static */ std::size_t  Y::op_plus_assign_throw_count_ =
    std::size_t (-1);

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

    // return a type convertible to Y
    conv_to_T<Y> operator() (const Y &x, const Y &y) /* non-const */ {
        ++funcalls_;
        Y res (x);
        res.data_.val_ += y.data_.val_;
        return conv_to_T<Y>::make (res);
    }

private:
    void operator= (Accumulator&);   // not assignable
};

std::size_t Accumulator::funcalls_;

/**************************************************************************/

// exercises accumulate (26.4.1)
template <class T, class InputIterator, class BinaryOp>
void test_accumulate (const std::size_t    N,
                      const InputIterator &it,
                      const T*,
                      const BinaryOp      *op)
{
    const char* const itname = type_name (it, (T*)0);
    const char* const tname  = "Y";
    const char* const opname = "BinaryOperation";

    rw_info (0, 0, 0, "std::accumulate (%s, %1$s, %s%{?}, %s%{;})",
             itname, tname, 0 != op, opname);

    // construct initial T
    const T init;
    int sum = init.data_.val_;

    T::gen_ = gen_seq;

    T* const buf = new T [N];
    
    for (std::size_t i = 0; i != N; ++i) {

        T* const buf_end = buf + i;

        const InputIterator first (buf,     buf, buf_end);
        const InputIterator last  (buf_end, buf, buf_end);

        BinaryOp bin_op (0, 0);

        const T res = op ?
            std::accumulate (first, last, init, bin_op)
          : std::accumulate (first, last, init);

        // verify the result 26.4.1, p1
        bool success = sum == res.data_.val_;
        rw_assert (success, 0, __LINE__,
                   "step %zu: accumulate <%s, %s%{?}, %s%{;}> "
                   "= %d, expected %d",
                   i + 1, itname, tname, 0 != op, opname, res.data_.val_, sum);

        sum += buf [i].data_.val_;

        if (!success)
            break;
    }

    delete[] buf;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 256;           // --nloops
/* extern */ int rw_opt_no_binary_op;           // --no-binary_op
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class T, class BinaryOp>
void test_accumulate (const std::size_t  N,
                      const T*,
                      const BinaryOp    *op)
{
    static const InputIter<T>             input_iter (0, 0, 0);
    static const ConstFwdIter<T>          fwd_iter (0, 0, 0);
    static const ConstBidirIter<T>        bidir_iter (0, 0, 0);
    static const ConstRandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0,
             "template <class %s, class %s%{?}, class %s%{;}> "
             "%2$s std::accumulate (%1$s, %1$s, %2$s%{?}, %s%{;})",
             "InputIterator", "T", 0 != op, "BinaryOperation",
             0 != op, "BinaryOperation");

    if (rw_opt_no_input_iter) {
        rw_note (0, 0, __LINE__, "InputIterator test disabled");
    }
    else {
        test_accumulate (N, input_iter, (T*)0, op);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, 0, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_accumulate (N, fwd_iter, (T*)0, op);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, 0, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_accumulate (N, bidir_iter, (T*)0, op);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, 0, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_accumulate (N, rand_iter, (T*)0, op);
    }

}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    test_accumulate (N, (Y*)0, (Accumulator*)0);

    if (rw_opt_no_binary_op)
        rw_note (0, 0, 0, "accumulate with binary operation test disabled");
    else
        test_accumulate (N, (Y*)0, (Accumulator*)1);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.accumulate",
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
