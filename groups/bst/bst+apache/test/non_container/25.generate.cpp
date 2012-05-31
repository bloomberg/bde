/***************************************************************************
 *
 * 25.generate.cpp - test exercising 25.2.6 [lib.alg.generate]
 *
 * $Id: 25.generate.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for generate
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

template <class T>
struct Generator
{
    // return a const reference to avoid requiring
    // that T be copy-constructible
    const T& operator() () const {
        static const union {
            void*         align_;
            unsigned char buf_ [sizeof (T)];
        } u = { 0 };
        return *(const T*)(const void*)&u;
    }
};


_RWSTD_SPECIALIZED_CLASS
struct Generator<UserClass>
{
    // dummy arguments provided to prevent Generator
    // from being DefaultConstructible
    Generator (int, int /* dummy */) {
        // use the generator of sequential values
        UserClass::gen_ = gen_seq;
    }

    UserClass operator() () /* non-const */ {
        // return a default-constructed UserClass intialized
        // to the next sequential value
        return UserClass ();
    }
};

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void generate (FwdIter<assign<base<> > >, FwdIter<assign<base<> > >,
               Generator<assign<base<> > >);

#if TEST_RW_PEDANTIC // generate_n is supposed to return void, not OutputIter
template
void generate_n (OutputIter<assign<base<> > >, std::size_t,
                 Generator<assign<base<> > >);
#else // !TEST_RW_PEDANTIC
template
OutputIter<assign<base<> > >
     generate_n (OutputIter<assign<base<> > >, std::size_t,
                 Generator<assign<base<> > >);
#endif // !TEST_RW_PEDANTIC

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// exercises std::generate()
template <class ForwardIterator, class T>
void test_generate (std::size_t N,
                    const ForwardIterator& gen_iter,
                    const T* )
{
    static const char* const itname  = type_name (gen_iter, (T*) 0);
    static const char* const genname = "Generator";

    rw_info (0, 0, 0,
             "void std::generate (%s, %1$s, %s)", itname, genname);

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    T *buf = new UserClass [N];

    for (std::size_t i = 0; i < N; ++i) {

        // exercise 25.2.6, std::generate<> ()
        std::size_t last_n_op_assign = T::n_total_op_assign_;

        T* const buf_end = buf + i + 1;

        const ForwardIterator begin =
            make_iter (buf, buf, buf_end, gen_iter);

        const ForwardIterator end =
            make_iter (buf_end, buf_end, buf_end, gen_iter);

        const Generator<T> gen (0, 0);

        // store the value of the next element
        const int last_val = Generator<T>(0, 0)().data_.val_;

        std::generate (begin, end, gen);

        bool success = true;

        // verify 25.2.6, p2
        std::size_t j = 0;
        for ( ; j != i; ++j) {
            success = (begin.cur_ + j)->data_.val_ == int (last_val + j + 1);
            if (!success)
                break;
        }

        rw_assert (success, 0, __LINE__,
                   "%zu. generate (): buf[%zu]: %d != %d",
                   i + 1, j, last_val + j + 1, (begin.cur_ + j)->data_.val_ );

        if (!success)
            break;

        // verify 25.2.6, p3
        success = T::n_total_op_assign_ - last_n_op_assign == i + 1;
        rw_assert (success, 0, __LINE__,
                   "%zu. generate (): complexity: %zu != %zu",
                   i + 1, T::n_total_op_assign_ - last_n_op_assign, i + 1);

        if (!success)
            break;
    }

    delete[] (buf);
}

/**************************************************************************/

// exercises std::generate_n()
template <class ForwardIterator, class Size, class T>
void test_generate_n (std::size_t            N,
                      const ForwardIterator &gen_iter,
                      const Size*,
                      const T*)
{
    static const char* const itname  = type_name (gen_iter, (T*) 0);
    static const char* const szname  = "Size<int>";
    static const char* const genname = "Generator";

    rw_info (0, 0, 0,
             "void std::generate_n (%s, %s, %s)", itname, szname, genname);

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    T *buf = new UserClass [N];

    for (std::size_t i = 0; i <= N; ++i) {

        std::size_t last_n_op_assign = T::n_total_op_assign_;

        T* const buf_end = buf + i + 1;

        const ForwardIterator begin =
            make_iter (buf, buf, buf_end, gen_iter);

        const Size n (i, 0);
        const Generator<T> gen (0, 0);

        // store the value of the next element
        const int last_val = Generator<T>(0, 0)().data_.val_;

        std::generate_n (begin, n, gen);

        bool success = true;

        // verify 25.2.6, p2
        std::size_t j = 0;
        for ( ; j != i; ++j) {
            success = (begin.cur_ + j)->data_.val_ == int (last_val + j + 1);
            if (!success)
                break;
        }

        rw_assert (success, 0, __LINE__,
                   "%zu. generate_n (): buf[%zu]: %d != %d",
                   i + 1, j, last_val + j + 1, (begin.cur_ + j)->data_.val_ );

        if (!success)
            break;

        // verify 25.2.6, p3
        success = T::n_total_op_assign_ - last_n_op_assign == i;
        rw_assert (success, 0, __LINE__,
                   "%zu. generate_n (): complexity: %zu != %zu",
                   i + 1, T::n_total_op_assign_ - last_n_op_assign, i);

        if (!success)
            break;
    }

    delete[] (buf);
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 32;     // --nloops
/* extern */ int rw_opt_no_generate;     // --no-generate
/* extern */ int rw_opt_no_generate_n;   // --no-generate_n
/* extern */ int rw_opt_no_output_iter;  // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator

static void
test_generate (const std::size_t N)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s> "
             "void std::generate (%1$s, %1$s, %2$s&)",
             "ForwardIterator", "Generator");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_generate (N, FwdIter<UserClass>(), (UserClass*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_generate (N, BidirIter<UserClass>(), (UserClass*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_generate (N, RandomAccessIter<UserClass>(), (UserClass*)0);
    }
}

/**************************************************************************/

static void
test_generate_n (const std::size_t N)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s, class %s> "
             "void std::generate_n (%1$s, %2$s, const %3$s&)",
             "OutputIterator", "Size", "Generator");

    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_generate_n (N, OutputIter<UserClass>(0, 0, 0), (Size<int>*)0,
                         (UserClass*)0);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_generate_n (N, FwdIter<UserClass>(), (Size<int>*)0, (UserClass*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_generate_n (N, BidirIter<UserClass>(), (Size<int>*)0,
                         (UserClass*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_generate_n (N, RandomAccessIter<UserClass>(), (Size<int>*)0,
                         (UserClass*)0);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    RW_ASSERT (0 <= rw_opt_nloops);

    const std::size_t N = std::size_t (rw_opt_nloops);

    if (rw_opt_no_generate) {
        rw_note (0, __FILE__, __LINE__, "std::generate test disabled");
    }
    else {
        test_generate (N);
    }

    if (rw_opt_no_generate_n) {
        rw_note (0, __FILE__, __LINE__, "std::generate_n test disabled");
    }
    else {
        test_generate_n (N);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.generate",
                    0 /* no comment */, run_test,
                    "|-nloops#0 "   // argument must be non-negative
                    "|-no-generate# "
                    "|-no-generate_n# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_nloops,
                    &rw_opt_no_generate,
                    &rw_opt_no_generate_n,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
