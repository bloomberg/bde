/***************************************************************************
 *
 * 25.partitions.cpp - test exercising 25.2.12 [lib.alg.partitions]
 *
 * $Id: 25.partitions.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for partition, stable_partition
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
BidirIter<assign<base<cpy_ctor> > >
partition (BidirIter<assign<base<cpy_ctor> > >,
           BidirIter<assign<base<cpy_ctor> > >,
           predicate<assign<base<cpy_ctor> > >);

template
BidirIter<assign<base<cpy_ctor> > >
stable_partition (BidirIter<assign<base<cpy_ctor> > >,
                  BidirIter<assign<base<cpy_ctor> > >,
                  predicate<assign<base<cpy_ctor> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T>
struct GreaterThanPredicate 
{
    static std::size_t funcalls_;

    // dummy argument provide to prevent implicit conversions
    GreaterThanPredicate (int value, int /* dummy */)
        : value_(value) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    conv_to_bool operator() (const T &arg) {
        ++funcalls_;
        return conv_to_bool::make (value_ < arg.data_.val_);
    }

    static const char* name () { return "GreaterThanPredicate"; }

private:
    const int value_;
    void operator= (GreaterThanPredicate&);   // not assignable
};

template<class T> std::size_t GreaterThanPredicate<T>::funcalls_;

/**************************************************************************/

// exercises std::partition() and std::stable_partition()
template <class T, class Iterator, class Predicate>
void test_partitions (int                   line, 
                      const char           *src,
                      const char           *dst,
                      const int             value,
                      const std::size_t     offset,
                      const Iterator       &it, 
                      const Predicate      *,
                      const T*, 
                      bool                  stable)
{
    const char* const itname  = type_name (it, (T*)0);
    const char* const fname   = stable ? "stable_partition" : "partition";
    const char* const funname = Predicate::name();

    const std::size_t nsrc = std::strlen (src);
    const std::size_t ndst = std::strlen (dst);

    T* const xsrc = T::from_char (src, nsrc);
    T* const xdst = T::from_char (dst, ndst);

    T* const xsrc_end = xsrc + nsrc;

    const Iterator first = make_iter (xsrc,     xsrc, xsrc_end, it);
    const Iterator last  = make_iter (xsrc_end, xsrc, xsrc_end, it);

    const std::size_t last_n_op_assign = T::n_total_op_assign_;
    const std::size_t t_n_ctors = T::n_total_def_ctor_ + T::n_total_copy_ctor_;
    const std::size_t t_n_dtors = T::n_total_dtor_;

    const Predicate pred (value, 0 /* dummy */);

    const Iterator res = 
            stable ? std::stable_partition (first, last, pred) 
                   : std::partition (first, last, pred);

    // check that there is no leaked objects
     const std::size_t t_n_objs_diff =
           T::n_total_def_ctor_ + T::n_total_copy_ctor_
         - t_n_ctors - T::n_total_dtor_ + t_n_dtors;

    bool success = 0 == t_n_objs_diff;
    rw_assert (success, 0, line,
               "line %d: std::%s <%s, %s>(\"%s\", ...) ==> \"%{X=*.*}\", "
               "number of objects changed on %d: %s detected",
               __LINE__, fname, itname, funname, src, 
               int (nsrc), -1, xsrc, t_n_objs_diff, 
               t_n_objs_diff > 0 ? "memory leak" : "unexpected dtor call");

    // check that the returned iterator points to the expected element
    success = res.cur_ == first.cur_ + offset;
    rw_assert (success, 0, line,
               "line %d: std::%s <%s, %s>(\"%s\", ...) ==> \"%{X=*.*}\", "
               "returned iterator it = first + %td, expected first + %zu",
               __LINE__, fname, itname, funname, src, int (nsrc), 
               -1, xsrc, res.cur_ - first.cur_, offset);

    // check 25.2.12, p2 & p5
    // "left" part of the array there the predicate should be true
    std::size_t i = 0;
    for ( ; i < offset; i++) {
        success = xsrc[i].data_.val_ > value;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: std::%s <%s, %s>(\"%s\", ...) "
               "==> \"%{X=*.*}\", at %zu got: %#c !> %#c",
               __LINE__, fname, itname, funname, src, int (nsrc), 
               -1, xsrc, i + 1, xsrc[i].data_.val_, value);


    // "right" part of the array there the predicate should be false
    for ( ; i < nsrc; i++) {
        success = xsrc[i].data_.val_ <= value;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: std::%s <%s, %s>(\"%s\", ...) "
               "==> \"%{X=*.*}\", at %zu got: %#c !<= %#c",
               __LINE__, fname, itname, funname, src, int (nsrc), 
               -1, xsrc, i + 1, xsrc[i].data_.val_, value);


    // check the complexity, 25.2.12 p3 & p6
    // first check number of swaps, 2 assignments per swap

    // add 1 in case of stable_partition to avoid the assertion failing
    // when i == 0 (only one element presents in the sequence)
    // is this one extra assignment an algorithm bug??
    // the standard talks about swaps only, not about assignments
    // and there are 0 swaps (1 swap equals 2 assignments) in this case...
    std::size_t exp_assigns =  stable ? 2 * nsrc * ::ilog2 (nsrc) + 1 : nsrc;
    std::size_t op_assigns  = T::n_total_op_assign_ - last_n_op_assign;

    success = op_assigns <= exp_assigns;
    rw_assert (success, 0, line,
               "line %d: std::%s <%s, %s>(\"%s\", ...): complexity: "
               "got %zu assignments, expected no more than %zu",
                __LINE__, fname, itname, funname, src,
                op_assigns, exp_assigns);

    // second check the number of the predicate calls
    success = Predicate::funcalls_ == nsrc;
    rw_assert (success, 0, line,
               "line %d: std::%s <%s, %s>(\"%s\", ...): complexity: "
               "got %zu applications of the predicate, "
               "expected no more than %zu",
                __LINE__, fname, itname, funname, src,
                Predicate::funcalls_, nsrc);

    if (stable) {

        // check the stable_partition is really stable 25.2.12, p5
        for (i = 0; i < nsrc; i++) {
            success = xsrc[i].data_.val_ == xdst[i].data_.val_;
            if (!success)
                break;
        }

        rw_assert (success, 0, line,
                   "line %d: std::%s <%s, %s>(\"%s\", ...) ==> \"%{X=*.*}\", "
                   "expected \"%{X=*.*}\", realtive order broken at %zu, "
                   "%#c != %#c",
                   __LINE__, fname, itname, funname, src, 
                   int (nsrc), int (i), xsrc, int (ndst), int (i), xdst,
                   i, xsrc[i].data_.val_, xdst[i].data_.val_);
    }

    delete[] xsrc;
    delete[] xdst;
}

/**************************************************************************/

template <class T, class Iterator>
void test_partitions (const Iterator &it, 
                      const T* , 
                      bool            stable)
{
    const char* const itname = type_name (it, (T*)0);
    const char* const fname = 
        stable ? "stable_partition" : "partition";
    const char* const funname = GreaterThanPredicate<T>::name();

    rw_info (0, 0, 0,
             "%s %s (%1$s, %1$s, %s)",
             itname, fname, funname);

#define TEST(src, dest, mid, offet)                                \
    test_partitions (__LINE__, src, dest, mid, offet, it,          \
                     (GreaterThanPredicate<T>*)0, (T*)0, stable)

    // stable_partition fails this test
    TEST ("abcdefghij", "abcdefghij",  0,  10); 

    TEST ("abcdefghij", "bcdefghija", 'a',  9);
    TEST ("abcdefghij", "cdefghijab", 'b',  8);
    TEST ("abcdefghij", "defghijabc", 'c',  7);
    TEST ("abcdefghij", "efghijabcd", 'd',  6);
    TEST ("abcdefghij", "fghijabcde", 'e',  5);
    TEST ("abcdefghij", "ghijabcdef", 'f',  4);
    TEST ("abcdefghij", "hijabcdefg", 'g',  3);
    TEST ("abcdefghij", "ijabcdefgh", 'h',  2);
    TEST ("abcdefghij", "jabcdefghi", 'i',  1);
    TEST ("abcdefghij", "abcdefghij", 'j',  0);

    // stable_partition fails this test
    TEST ("jihgfedcba", "jihgfedcba",  0,  10);

    TEST ("jihgfedcba", "jihgfedcba", 'a',  9);
    TEST ("jihgfedcba", "jihgfedcba", 'b',  8);
    TEST ("jihgfedcba", "jihgfedcba", 'c',  7);
    TEST ("jihgfedcba", "jihgfedcba", 'd',  6);
    TEST ("jihgfedcba", "jihgfedcba", 'e',  5);
    TEST ("jihgfedcba", "jihgfedcba", 'f',  4);
    TEST ("jihgfedcba", "jihgfedcba", 'g',  3);
    TEST ("jihgfedcba", "jihgfedcba", 'h',  2);
    TEST ("jihgfedcba", "jihgfedcba", 'i',  1);
    TEST ("jihgfedcba", "jihgfedcba", 'j',  0);

    // stable_partition fails this test
    TEST ("a", "a", 0, 1);  

    TEST ("a", "a", 'a', 0);
}

/**************************************************************************/

/* extern */ int rw_opt_no_partition;           // --no-partition
/* extern */ int rw_opt_no_stable_partition;    // --no-stable_partition
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator


template <class T>
void test_partitions (const T*, bool stable)
{
    rw_info (0, 0, 0, 
             "template <class %s, class %s> %1$s %s (%1$s, %1$s, %2$s)",
             "BidirectionalIterator", "Predicate", 
             stable ? "stable_partition" : "partition");

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_partitions (BidirIter<T>(), (T*)0, stable); 
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_partitions (RandomAccessIter<T>(), (T*)0, stable);  
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    if (rw_opt_no_partition) {
        rw_note (0, __FILE__, __LINE__, "std::partition test disabled");
    }
    else {
        test_partitions ((UserClass*)0, false);
    }

    if (rw_opt_no_stable_partition) {
        rw_note (0, __FILE__, __LINE__, 
                 "std::stable_partition test disabled");
    }
    else {
        test_partitions ((UserClass*)0, true);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.partitions",
                    0 /* no comment */, run_test,
                    "|-no-partition# "
                    "|-no-stable_partition# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator", 
                    &rw_opt_no_partition,
                    &rw_opt_no_stable_partition,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
