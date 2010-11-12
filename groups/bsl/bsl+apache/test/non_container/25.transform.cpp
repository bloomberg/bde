/***************************************************************************
 *
 * 25.transform.cpp - test exercising 25.2.3 [lib.alg.transform]
 *
 * $Id: 25.transform.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for transform
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()


_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
OutputIter<assign<base<> > >
transform (InputIter<assign<base<> > >,
           InputIter<assign<base<> > >,
           OutputIter<assign<base<> > >,
           func<assign<base<> > >);

template
OutputIter<assign<base<> > >
transform (InputIter<assign<base<> > >,
           InputIter<assign<base<> > >,
           InputIter<assign<base<> > >,
           OutputIter<assign<base<> > >,
           binary_func<assign<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T>
struct incT
{
    // not a binary function
    enum { binary = 0 };

    // function name
    static const char* name () { return "unary_function"; }

    // dummy arguments to prevent the class from being default
    // constructible or construtible by conversion from an int
    incT (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // non-const in order to detect unwarranted assumptions
    // in the algorithm(s)
    T operator() (const T &x) /* non-const */ {
        ++funcalls_;
        T y (x);
        y.data_.val_ += 1;
        return y;
    }

    static std::size_t funcalls_;
};


template <class T>
struct plusT
{
    // binary function
    enum { binary = 1 };

    // function name
    static const char* name () { return "binary_function"; }

    // dummy arguments to prevent the class from being default
    // constructible or construtible by conversion from an int
    plusT (int, int) {
        funcalls_ = 0;
    }

    // non-const in order to detect unwarranted assumptions
    // in the algorithm(s)
    T operator() (const T &a, const T &b) /* non-const */ {
        ++funcalls_;
        T x (a);
        x.data_.val_ = a.data_.val_ + b.data_.val_;
        return x;
    }

    static std::size_t funcalls_;
};

template <class T> std::size_t incT<T>::funcalls_;
template <class T> std::size_t plusT<T>::funcalls_;

/**************************************************************************/

/* extern */ int rw_opt_nloops = 32;          // --nloops=#
/* extern */ int rw_opt_no_unary_function;    // --no-unary_function
/* extern */ int rw_opt_no_binary_function;   // --no-binary_function
/* extern */ int rw_opt_no_input_iter;        // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;       // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;          // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;        // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;          // --no-RandomAccessIterator

/**************************************************************************/

template <class InputIterator, class Unused, class OutputIterator>
OutputIterator
invoke_transform (InputIterator first, InputIterator last,
                  Unused /* for compatibility with the overload below */,
                  OutputIterator dest, incT<UserClass> fun)
{
    return std::transform (first, last, dest, fun);
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator
invoke_transform (InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2,
                  OutputIterator dest, plusT<UserClass> fun)
{
    return std::transform (first1, last1, first2, dest, fun);
}


template <class T, class InputIterator1, class InputIterator2,
          class OutputIterator, class Function>
void test_transform (const T*              ptr,
                     const InputIterator1 &it1,
                     const InputIterator2 &it2,
                     const OutputIterator &out,
                     const Function*,
                     int                   same_seq)
{
    static const char* const it1name = type_name (it1, ptr);
    static const char* const it2name = type_name (it2, ptr);
    static const char* const outname = type_name (out, ptr);
    static const char* const algname = "transform";
    static const char* const funname = Function::name ();
    static const bool        binary  = Function::binary;

    const std::size_t nloops = std::size_t (rw_opt_nloops);

    rw_info (0, 0, 0,
             "std::%s (%s, %2$s%{?}, %s%{;}, %s, %s)%{?}, %s%d == %s%{;}",
             algname, it1name, binary, it2name, outname, funname,
             same_seq, "first", same_seq, "dest");

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    // make sure buf{1,2}[0] is dereferenceable even when (nloops == 0)
    T* const buf1 = new T [nloops + 1];
    T* const buf2 = new T [nloops + 1];
    T* const buf3 = same_seq ? (same_seq == 1 ? buf1 : buf2) : new T [nloops];

    const int start1_val =
        same_seq == 2 ? buf2 [0].data_.val_ : buf1 [0].data_.val_;
    const int start2_val =
        same_seq == 2 ? buf1 [0].data_.val_ : buf2 [0].data_.val_;

    for (std::size_t i = 0; i < nloops; ++i) {

              T* const buf1_end = buf1 + i;
        const T* const buf2_end = buf2 + i;
        const T* const buf3_end = buf3 + i;

        const InputIterator1 first1 = make_iter (buf1, buf1, buf1_end, it1);
        const InputIterator1 last1  = make_iter (buf1_end, buf1, buf1_end, it1);
        const InputIterator2 first2 = make_iter (buf2, buf2, buf2_end, it2);
        const OutputIterator dest   = make_iter (buf3, buf3, buf3_end, out);

        const Function fun (0, 0);   // dummy arguments

        // invoke the overload of std::transform() appropriate
        // for this Function
        const OutputIterator result =
            invoke_transform  (first1, last1, first2, dest, fun);

        // check the returned iterator
        bool success = result.cur_ == buf3_end;
        rw_assert (success, 0, __LINE__,
                   "%zu. %s (%s, %2$s%{?}, %s%{;}, %s, %s)%{?}, "
                   "%s%d == %s%{;} : return val: "
                   "dest + %td, expected dest + %zu",
                   i, algname, it1name, binary, it2name, outname, funname,
                   same_seq, "first", same_seq, "dest",
                   result.cur_ - buf3, i);

        if (!success)
            break;

        // check the transformation results
        int exp_val = 0;
        std::size_t j = 0;
        for ( ; j < i; j++) {
            if (0 == same_seq) {
                exp_val = binary ?
                    start1_val + int (j) + start2_val + int (j)
                  : start1_val + int (j) + 1;
            }
            else {
                exp_val = binary ?
                      start1_val + int (j)
                    + (start2_val + int (j)) * int (i - j)
                  : start1_val + int (i);
            }

            success = buf3 [j].data_.val_ == exp_val;
            if (!success)
                break;
        }

        rw_assert (success, 0, __LINE__,
                   "%zu. %s (%s, %2$s%{?}, %s%{;}, %s, %s)%{?}, "
                   "%s%d == %s%{;} : error: incorrect value %d, "
                   "expected %d, position %zu",
                   i, algname, it1name, binary, it2name, outname, funname,
                   same_seq, "first", same_seq, "dest",
                   buf3 [j].data_.val_, exp_val, j + 1);

        if (!success)
            break;

        // check compexity, 25.2.3 p4
        rw_assert (Function::funcalls_ == i, 0, __LINE__,
                   "%zu. %s (%s, %2$s%{?}, %s%{;}, %s, %s)%{?}, "
                   "%s%d == %s%{;} : complexity : %zu "
                   "applications of %5$s, expected %zu",
                   i, algname, it1name, binary, it2name, outname, funname,
                   same_seq, "first", same_seq, "dest",
                   Function::funcalls_, i);
    }

    delete[] buf1;
    delete[] buf2;

    if (0 == same_seq)
        delete[] buf3;
}

/**************************************************************************/

template <class T, class InputIterator1, class InputIterator2,
          class OutputIterator, class Function>
void gen_test (const T*              ptr,
               const InputIterator1 &it1,
               const InputIterator2 &it2,
               const OutputIterator &out,
               const Function*       pfun,
               int                   tag1,
               int                   tag2,
               int                   tag3,
               int                   same_seq)
{
    const InputIter<UserClass>        input_iter (0, 0, 0);
    const OutputIter<T>       output_iter (0, 0, 0);
    const FwdIter<T>          fwd_iter (0, 0, 0);
    const BidirIter<T>        bidir_iter (0, 0, 0);
    const RandomAccessIter<UserClass> rand_iter (0, 0, 0);

    // tag1, tag2 and tag3 indicates that an iterator needs to be generated
    // at the corresponding position by a recursive call to gen_test
    // and other positions there the iterator will be used
    // for all tags:
    // 0 means that no iterator is needed here
    //         (maybe it was already generated or just not needed)
    // 1 means that the iterator is needed at this position
    //         (first for tag1, second for tag2, etc)
    // 2 means that the iterator is needed at this position and the
    //         same type of iterator should be used at third position

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_input_iter) {
        if (1 == tag1 || 0 == tag1 && 1 == tag2)
            rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        if (1 == tag1) {
            gen_test (ptr, input_iter, it2, out,
                      pfun, 0, tag2, tag3, same_seq);
        }
        else if (0 == tag1 && 1 == tag2) {
            gen_test (ptr, it1, input_iter, out,
                      pfun, tag1, 0, tag3, same_seq);
        }
    }

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_fwd_iter) {
        if (tag1 || !tag1 && tag2 || !tag1 && !tag2 && tag3)
            rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        if (1 == tag1) {
            gen_test (ptr, fwd_iter, it2, out,
                      pfun, 0, tag2, tag3, same_seq);
        }
        else if (2 == tag1) {
            gen_test (ptr, fwd_iter, it2, fwd_iter,
                      pfun, 0, tag2, 0, same_seq);
        }
        else if (0 == tag1 && 1 == tag2) {
            gen_test (ptr, it1, fwd_iter, out,
                      pfun, tag1, 0, tag3, same_seq);
        }
        else if (0 == tag1 && 2 == tag2) {
            gen_test (ptr, it1, fwd_iter, fwd_iter,
                      pfun, tag1, 0, 0, same_seq);
        }
        else if (0 == tag1 && 0 == tag2 && 1 == tag3) {
            gen_test (ptr, it1, it2, fwd_iter,
                      pfun, tag1, tag2, 0, same_seq);
        }
    }

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_bidir_iter) {
        if (tag1 || !tag1 && tag2 || !tag1 && !tag2 && tag3)
            rw_note (0, __FILE__, __LINE__,
                     "BidirectionalIterator test disabled");
    }
    else {
        if (1 == tag1) {
            gen_test (ptr, bidir_iter, it2, out,
                      pfun, 0, tag2, tag3, same_seq);
        }
        else if (2 == tag1) {
            gen_test (ptr, bidir_iter, it2, bidir_iter,
                      pfun, 0, tag2, 0, same_seq);
        }
        else if (0 == tag1 && 1 == tag2) {
            gen_test (ptr, it1, bidir_iter, out,
                      pfun, tag1, 0, tag3, same_seq);
        }
        else if (0 == tag1 && 2 == tag2) {
            gen_test (ptr, it1, bidir_iter, bidir_iter,
                      pfun, tag1, 0, 0, same_seq);
        }
        else if (0 == tag1 && 0 == tag2 && 1 == tag3) {
            gen_test (ptr, it1, it2, bidir_iter,
                      pfun, tag1, tag2, 0, same_seq);
        }
    }

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_rnd_iter) {
        if (tag1 || !tag1 && tag2 || !tag1 && !tag2 && tag3)
            rw_note (0, __FILE__, __LINE__,
                     "RandomAccessIterator test disabled");
    }
    else {
        if (1 == tag1) {
            gen_test (ptr, rand_iter, it2, out,
                      pfun, 0, tag2, tag3, same_seq);
        }
        else if (2 == tag1) {
            gen_test (ptr, rand_iter, it2, rand_iter,
                      pfun, 0, tag2, 0, same_seq);
        }
        else if (0 == tag1 && 1 == tag2) {
            gen_test (ptr, it1, rand_iter, out,
                      pfun, tag1, 0, tag3, same_seq);
        }
        else if (0 == tag1 && 2 == tag2) {
            gen_test (ptr, it1, rand_iter, rand_iter,
                      pfun, tag1, 0, 0, same_seq);
        }
        else if (0 == tag1 && 0 == tag2 && 1 == tag3) {
            gen_test (ptr, it1, it2, rand_iter,
                      pfun, tag1, tag2, 0, same_seq);
        }
    }

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_output_iter) {
        if (0 == tag1 && 0 == tag2 && 1 == tag3)
            rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        if (0 == tag1 && 0 == tag2 && 1 == tag3) {
            gen_test (ptr, it1, it2, output_iter,
                      pfun, tag1, tag2, 0, same_seq);
        }
    }

    if (0 == tag1 && 0 == tag2 && 0 == tag3) {
        test_transform (ptr, it1, it2, out, pfun, same_seq);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const InputIter<UserClass>        input_iter (0, 0, 0);
    const RandomAccessIter<UserClass> rand_iter;
    const UserClass* const            ptr = 0;

    // test transform with a unary function
    // according to 25.2.3 p5 'result may be equal to first'
    // so it is neccessary to test this case separately
    if (rw_opt_no_unary_function) {
        rw_note (0, __FILE__, __LINE__,
                 "std::transform unary function test disabled");
    }
    else {
        const incT<UserClass>* const pfun = 0;

        gen_test (ptr, input_iter, rand_iter, rand_iter,
                  pfun, 1, 0, 1, 0 /* result distinct from first */);

        // test 25.2.3 p5 - result is equal to first
        // set tag1 = 2 and tag3 = 0 to test all iterators at the first
        // position and avoid generating iterators at the third pos
        gen_test (ptr, input_iter, rand_iter, rand_iter,
                  pfun, 2, 0, 0, 1 /* result same as first */);
    }

    // test transform with a binary function
    // according to 25.2.3 p5 'result may be equal to first1 or first2'
    // so it is neccessary to test these cases separately
    if (rw_opt_no_binary_function) {
        rw_note (0, __FILE__, __LINE__,
                 "std::transform binary function test disabled");
    }
    else {
        const plusT<UserClass>* const pfun = 0;

        gen_test (ptr, input_iter, rand_iter, rand_iter,
                  pfun, 1, 1, 1, 0 /* result distinct from first{1,2} */);

        // test 25.2.3 p5 - result is equal to first2
        // set tag2 = 2 and tag3 = 0 to test all iterators at the second
        // position and avoid generating iterators at the third pos
        gen_test (ptr, input_iter, rand_iter, rand_iter,
                  pfun, 1, 2, 0, 2 /* result same as first2 */);

        // test 25.2.3 p5 - result is equal to first1
        // set tag1 = 2 and tag3 = 0 to test all iterators at the first
        // position and avoid generating iterators at the third pos
        // set same_seq to 1 to indicate that first1 and dest should be
        // the same iterators
        gen_test (ptr, input_iter, rand_iter, rand_iter,
                  pfun, 2, 1, 0, 1 /* result same as first1 */);
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.transform",
                    0 /* no comment */, run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-unary_function# "
                    "|-no-binary_function# "
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator",
                    &rw_opt_nloops,
                    &rw_opt_no_unary_function,
                    &rw_opt_no_binary_function,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
