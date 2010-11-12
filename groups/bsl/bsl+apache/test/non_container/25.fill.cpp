/***************************************************************************
 *
 * 25.fill.cpp - test exercising 25.2.5 [lib.alg.fill]
 *
 * $Id: 25.fill.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for fill
#include <cstddef>      // for size_t

#include <alg_test.h>   
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void
fill (FwdIter<assign<base<> > >, FwdIter<assign<base<> > >,
      const assign<base<> >&);

template
#if DRQS  // fill_n is supposed to return void, not OutputIterator
void
#else
OutputIter<assign<base<> > >
#endif
fill_n (OutputIter<assign<base<> > >, Size<int>,
        const assign<base<> >&);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// exercises std::fill()
template <class ForwardIterator, class T>
void test_fill (std::size_t            N,
                const ForwardIterator &dummy_iter,
                const T*)
{
    static const char* const itname = type_name (dummy_iter, (T*)0);
    static const char* const tname  = "UserClass";

    rw_info (0, 0, 0, "void std::fill (%s, %1$s, const %s&)", itname, tname);

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    T* const buf = new T [N];

    for (std::size_t i = 0; i < N; ++i) {

        T* const buf_end = buf + i;

        const ForwardIterator begin =
            make_iter (buf, buf, buf_end, dummy_iter);

        const ForwardIterator end =
            make_iter (buf_end, buf_end, buf_end, dummy_iter);

        const T value;

        // the number of invocations of the assignment operator
        std::size_t n_op_assign  = T::n_total_op_assign_;

        std::fill (begin, end, value);

        // compute the number of invocations of the assignment operator
        // by the algorithm
        n_op_assign  = T::n_total_op_assign_ - n_op_assign;

        bool success = true;

        // verify 25.2.5, p2
        std::size_t j = 0;
        for ( ; success && j != i; ++j) {
            success = buf [j].data_.val_ == value.data_.val_;
        }

        if (!rw_assert (success, 0, __LINE__, 
                        "%zu. fill (%s, %2$s, const %s&): buf [%zu]: %d != %d",
                        i, itname, tname, j, buf [j].data_.val_,
                        value.data_.val_))
            break;

        // verify the complexity requirements:
        //   Exactly last - first assignments.
        if (!rw_assert (n_op_assign == i, 0, __LINE__,
                        "%zu. fill (%s, %2$s, const %s&) expected "
                        "%zu invocations of %2$s::operator=(); got %zu",
                        i, itname, tname, i, n_op_assign))
            break;
    }

    delete[] buf;
}

/**************************************************************************/

// exercises std::fill_n()
template <class OutputIterator, class Size, class T>
void test_fill_n (std::size_t           N,
                  const OutputIterator &dummy_iter,
                  const Size*,
                  const T*)
{
    static const char* const itname = type_name (dummy_iter, (T*)0);
    static const char* const szname = "Size<int>";
    static const char* const tname  = "UserClass";

    rw_info (0, 0, 0, "void std::fill_n (%s, %s, const %s&)",
             itname, szname, tname);

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    T* const buf = new T [N];

    for (std::size_t i = 0; i < N; ++i) {

        T* const buf_end = buf + i;

        const OutputIterator begin =
            make_iter (buf, buf, buf_end, dummy_iter);

        const Size n (i, 0 /* dummy */);
        const T    value;

        // the number of invocations of the assignment operator
        std::size_t n_op_assign  = T::n_total_op_assign_;

        std::fill_n (begin, n, value);

        // compute the number of invocations of the assignment operator
        n_op_assign = T::n_total_op_assign_ - n_op_assign;

        bool success = true;

        // verify 25.2.5, p2
        std::size_t j = 0;
        for ( ; success && j != i; ++j) {
            success = buf [j].data_.val_ == value.data_.val_;
        }

        rw_assert (success, 0, __LINE__, 
                   "%zu. fill_n (%s, %s, const %s&): buf [%zu]: %d != %d",
                   i, itname, szname, tname, j, buf [j].data_.val_,
                   value.data_.val_);

        if (!success)
            break;

        success = n_op_assign == i;
        rw_assert (success, 0, __LINE__,
                   "%zu. fill_n (%s, %s, const %s&) complexity: "
                   "%zu != %zu", i, itname, szname, tname,
                   n_op_assign, i);

        if (!success)
            break;
    }

    delete[] buf;
}


/**************************************************************************/

/* extern */ int rw_opt_nloops = 32;     // --nloops
/* extern */ int rw_opt_no_fill;         // --no-fill
/* extern */ int rw_opt_no_fill_n;       // --no-fill_n
/* extern */ int rw_opt_no_output_iter;  // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator

static void
test_fill (const std::size_t N)
{
    rw_info (0, 0, 0,
            "template <class %s, class %s> "
            "void std::fill (%1$s, %1$s, const %2$s&)",
            "ForwardIterator", "T");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_fill (N, FwdIter<UserClass>(), (UserClass*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_fill (N, BidirIter<UserClass>(), (UserClass*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_fill (N, RandomAccessIter<UserClass>(), (UserClass*)0);
    }
}

/**************************************************************************/

static void
test_fill_n (const std::size_t N)
{
    rw_info (0, 0, 0,
            "template <class %s, class %s, class %s> "
            "void std::fill_n (%1$s, %2$s, const %3$s&)",
            "OutputIterator", "Size", "T");

    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_fill_n (N, OutputIter<UserClass>(0, 0, 0), (Size<int>*)0,
                     (UserClass*)0);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_fill_n (N, FwdIter<UserClass>(), (Size<int>*)0, (UserClass*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_fill_n (N, BidirIter<UserClass>(), (Size<int>*)0, (UserClass*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_fill_n (N, RandomAccessIter<UserClass>(), (Size<int>*)0,
                     (UserClass*)0);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    // check that the number of loops is non-negative
    rw_fatal (-1 < rw_opt_nloops, 0, 0,
              "number of loops must be non-negative, got %d",
              rw_opt_nloops);

    const std::size_t N = std::size_t (rw_opt_nloops);

    if (rw_opt_no_fill) {
        rw_note (0, __FILE__, __LINE__, "std::fill test disabled");
    }
    else {
        test_fill (N);
    }

    if (rw_opt_no_fill_n) {
        rw_note (0, __FILE__, __LINE__, "std::fill_n test disabled");
    }
    else {
        test_fill_n (N);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.fill",
                    0 /* no comment */, run_test,
                    "|-nloops# "
                    "|-no-fill# "
                    "|-no-fill_n# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_nloops,
                    &rw_opt_no_fill,
                    &rw_opt_no_fill_n,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
