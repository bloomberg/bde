/***************************************************************************
 *
 * 25.reverse.cpp - test exercising 25.2.9 [lib.alg.reverse]
 *
 * $Id: 25.reverse.cpp 510970 2007-02-23 14:57:45Z faridz $
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

#include <algorithm>    // for reverse, reverse_copy
#include <cstring>      // for size_t, strlen()

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void reverse (BidirIter<assign<base<cpy_ctor> > >,
              BidirIter<assign<base<cpy_ctor> > >);

template
OutputIter<eq_comp<assign<base<> > > >
reverse_copy (BidirIter<eq_comp<assign<base<> > > >,
              BidirIter<eq_comp<assign<base<> > > >,
              OutputIter<eq_comp<assign<base<> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// tags to select reverse or reverse_copy at compile time
struct ReverseTag {
    enum { fname_inx = 0 };
    int dummy;
};

struct ReverseCopyTag {
    enum { fname_inx = 1 };
    int dummy;
};

// used as a dummy template argument to test functions exercising reverse()
// and reverse_if() (to differentiate from reverse_copy() and reverse_copy_if())
struct NoIterator { };
const char* type_name (NoIterator, const UserClass*)
{
    return 0;
}

/**************************************************************************/

unsigned iter_swap_calls;

_RWSTD_NAMESPACE (std) {

_RWSTD_SPECIALIZED_FUNCTION
void iter_swap (BidirIter<UserClass> i, BidirIter<UserClass> j)
{
    ++iter_swap_calls;

    const int tmp = (*i).data_.val_;

    (*i).data_.val_ = (*j).data_.val_;
    (*j).data_.val_ = tmp;
}

_RWSTD_SPECIALIZED_FUNCTION
void iter_swap (RandomAccessIter<UserClass> i, RandomAccessIter<UserClass> j)
{
    ++iter_swap_calls;

    const int tmp = (*i).data_.val_;

    (*i).data_.val_ = (*j).data_.val_;
    (*j).data_.val_ = tmp;
}

}   // namespace std


/**************************************************************************/

// exercises std::reverse()
template <class Iterator, class T>
void test_reverse (int line,
                   const char *src,
                   Iterator dummy, NoIterator, const T*, ReverseTag)
{
    static const char* const itname = type_name (dummy, (T*)0);
    const char* const fname = "reverse";

    // compute the length of the source sequence
    const std::size_t nsrc = std::strlen (src);

    // construct a sequence of `nsrc' elements to pass to reverse
    T* const xsrc = T::from_char (src, nsrc);

    // construct iterators pointing to the beginning and end
    // of the source sequence
    const Iterator first = make_iter (xsrc, xsrc, xsrc + nsrc, dummy);
    const Iterator last  = make_iter (xsrc + nsrc, xsrc, xsrc + nsrc, dummy);

    // zero out the iter_swap() call counter
    iter_swap_calls = 0;

    // call reverse()
    std::reverse (first, last);

    bool success = true;
    std::size_t i = 0;

    // verify that the sequence was correctly reversed and that it was
    // done by swapping elements i.e., not simply by copying them over
    for ( ; i != nsrc; ++i) {

        typedef unsigned char UChar;

        success = UChar (src [i]) == xsrc [nsrc - i - 1].data_.val_;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ...) ==> "
               "\"%{X=*.*}\"; unexpected element value %#c at offset %zu",
               __LINE__, fname, itname, src,
               int (nsrc), int (i), xsrc,
               xsrc [nsrc - i - 1].data_.val_, i);

    const std::size_t iter_swap_expect = nsrc / 2;
    success = iter_swap_calls == iter_swap_expect;
    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ...); called iter_swap() "
               "%zu times, %zu expected",
               __LINE__, fname, itname, src,
               iter_swap_calls, iter_swap_expect);

    delete[] xsrc;
}

/**************************************************************************/

// exercises std::reverse_copy()
template <class Iterator, class OutputIterator, class T>
void test_reverse (int line,
                   const char *src,
                   Iterator& it, OutputIterator& ,
                   const T*, ReverseCopyTag)
{
    static const char* const itname = type_name (it, (T*)0);
    const char* const fname = "reverse_copy";

    const std::size_t nsrc = std::strlen (src);

    T* const xsrc = T::from_char (src, nsrc);
    T* const xdst = T::from_char (src, nsrc);

    const Iterator first (xsrc, xsrc, xsrc + nsrc);
    const Iterator last  (xsrc + nsrc, xsrc, xsrc + nsrc);

    const OutputIterator result (xdst, xdst, xdst + nsrc);

    std::size_t last_n_op_assign = T::n_total_op_assign_;

    const OutputIterator end = std::reverse_copy (first, last, result);

    // verify that the returned iterator is set as expected
    bool success = end.cur_ == result.cur_ + nsrc;
    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ...) == result + %zu, got %td",
               __LINE__, fname, itname, src, nsrc, end.cur_ - xdst);

    // verify that the sequence was correctly reversed
    std::size_t i = 0;
    for ( ; i != nsrc; ++i) {

        typedef unsigned char UChar;

        success = UChar (src [i]) == xdst [nsrc - i - 1].data_.val_;
        if (!success)
            break;
    }

    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ...) ==> "
               "\"%{X=*.*}\"; unexpected element value %#c at offset %zu",
               __LINE__, fname, itname, src,
               int (nsrc), int (i), xsrc,
               xdst [nsrc - i - 1].data_.val_, i);

    success = T::n_total_op_assign_ - last_n_op_assign == nsrc;
    rw_assert (success, 0, line,
               "line %d: %s<%s>(\"%s\", ...); called operator=() "
               "%zu times, %zu expected",
               __LINE__, fname, itname, src,
               T::n_total_op_assign_ - last_n_op_assign, nsrc);

    delete[] xsrc;
    delete[] xdst;
}

/**************************************************************************/

// exercises all four function templates
template <class Iterator1, class Iterator2, class T, class Tag>
void test_reverse (Iterator1 it1, Iterator2 it2, const T*, Tag tag)
{
    static const char* const it1name = type_name (it1, (T*)0);
    static const char* const it2name = type_name (it2, (T*)0);

    if (tag.fname_inx) {  // reverse_copy()
        rw_info (0, 0, 0, "std::reverse_copy (%s, %1$s, %s)", it1name, it2name);
    }
    else {   // reverse()
        rw_info (0, 0, 0,  "std::reverse (%s, %1$s)", it1name);
    }


#define TEST(src) \
    test_reverse (__LINE__, src, it1, it2, (T*)0, tag)

    TEST ("");
    TEST ("a");
    TEST ("ab");
    TEST ("abc");
    TEST ("abcd");
    TEST ("abcde");
    TEST ("abcdef");
    TEST ("abcdefg");
    TEST ("abcdefgh");
    TEST ("abcdefghi");
    TEST ("abcdefghij");
    TEST ("abcdefghijk");
    TEST ("abcdefghijkl");
    TEST ("abcdefghijklm");
    TEST ("abcdefghijklmn");
    TEST ("abcdefghijklmno");
    TEST ("abcdefghijklmnop");
    TEST ("abcdefghijklmnopq");
    TEST ("abcdefghijklmnopqr");
    TEST ("abcdefghijklmnopqrs");
    TEST ("abcdefghijklmnopqrst");
    TEST ("abcdefghijklmnopqrstu");
    TEST ("abcdefghijklmnopqrstuv");
    TEST ("abcdefghijklmnopqrstuvw");
    TEST ("abcdefghijklmnopqrstuvwx");
    TEST ("abcdefghijklmnopqrstuvwxy");
    TEST ("abcdefghijklmnopqrstuvwxyz");
}

/**************************************************************************/

/* extern */ int rw_opt_no_reverse;            // --no-reverse
/* extern */ int rw_opt_no_reverse_copy;       // --no-reverse_copy
/* extern */ int rw_opt_no_output_iter;        // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;           // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;         // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;           // --no-RandomAccessIterator


template <class T, class Tag>
void test_reverse(const T*, Tag tag)
{
    rw_info (0, 0, 0,
             "template <class %s> %1$s std::reverse (%1$s, %1$s)",
             "BidirectionalIterator");

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_reverse (BidirIter<T>(), NoIterator (), (T*)0, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_reverse (RandomAccessIter<T>(), NoIterator (), (T*)0, tag);
    }
}

/**************************************************************************/

template <class ReverseIterator, class T, class Tag>
void test_reverse_copy (ReverseIterator it, const T*, Tag tag)
{
    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_reverse (it, OutputIter<T>(0, 0, 0), (T*)0, tag);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_reverse (it, FwdIter<T>(), (T*)0, tag);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_reverse (it, BidirIter<T>(), (T*)0, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_reverse (it, RandomAccessIter<T>(), (T*)0, tag);
    }
}

/**************************************************************************/

template <class T, class Tag>
void test_reverse_copy (const T*, Tag tag)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s> %2$s "
             "std::reverse_copy (%1$s, %1$s, %2$s)",
             "BidirectionalIterator", "OutputIterator");

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_reverse_copy (ConstBidirIter<T>(), (T*)0, tag);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_reverse_copy (ConstRandomAccessIter<T>(), (T*)0, tag);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const ReverseTag reverse_tag = { false };
    const ReverseCopyTag reverse_copy_tag = { true };

    if (rw_opt_no_reverse) {
        rw_note (0, __FILE__, __LINE__, "std::reverse test disabled");
    }
    else {
        test_reverse((UserClass*)0, reverse_tag);
    }

    if (rw_opt_no_reverse_copy) {
        rw_note (0, __FILE__, __LINE__, "std::reverse_copy test disabled");
    }
    else {
        test_reverse_copy((UserClass*)0, reverse_copy_tag);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.reverse",
                    0 /* no comment */, run_test,
                    "|-no-reverse# "
                    "|-no-reverse_copy# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_reverse,
                    &rw_opt_no_reverse_copy,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
