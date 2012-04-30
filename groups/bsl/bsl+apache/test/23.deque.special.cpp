/***************************************************************************
 *
 * 23.deque.special.cpp - test exercising [lib.deque.special]
 *
 * $Id: 23.deque.special.cpp 510071 2007-02-21 15:58:53Z faridz $
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

#include <deque>        // for deque

#include <cstddef>      // for size_t

#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test(), ...

/**************************************************************************/

struct DequeValueType { };

typedef std::allocator<DequeValueType>             DequeAllocator;
typedef std::deque<DequeValueType, DequeAllocator> DequeType;


int deque_swap_called;

#if !DRQS // std::deque should forward all functions, not just inherit from bslstl_Deque
namespace bsl {

template <>
    void deque<DequeValueType, DequeAllocator>::swap(
    deque<DequeValueType, DequeAllocator>&)
#else
_RWSTD_NAMESPACE (std) {

// define an explicit specialization of the deque::swap() member
// to verify tha the non-member swap function calls the member

_RWSTD_SPECIALIZED_FUNCTION
void DequeType::swap (DequeType&)
#endif
{
    ++deque_swap_called;
}

}   // namespace std

/**************************************************************************/

void test_std_swap ()
{
    rw_info (0, 0, 0,
             "Testing std::swap (std::deque&, std::deque&) "
             "calls std::deque::swap");

    // verify the signature of the function specialization
    void (*pswap)(DequeType&, DequeType&) =
        &std::swap<DequeValueType, DequeAllocator>;

    _RWSTD_UNUSED (pswap);

    // verify that std::swap() calls std::deque::swap()
    DequeType d;

    std::swap (d, d);

    rw_assert (1 == deque_swap_called, 0, __LINE__,
               "std::swap (std::deque<T, A>&, std::deque<T, A>&) called "
               "std::deque<T, A>::swap (std::deque<T, A>&) exactly once; "
               "got %d times", deque_swap_called);
}

/**************************************************************************/

typedef std::deque<UserClass, std::allocator<UserClass> > Deque;

Deque::size_type new_capacity;

#if TEST_RW_EXTENSIONS
namespace __rw {

_RWSTD_SPECIALIZED_FUNCTION
inline Deque::size_type
__rw_new_capacity<Deque>(Deque::size_type n, const Deque*)
{
    if (n) {
        // non-zero size argument indicates a request for an increase
        // in the capacity of a deque object's dynamically sizable
        // vector of nodes
        return n * 2;
    }

    // zero size argument is a request for the initial size of a deque
    // object's dynamically sizable vector of nodes or for the size of
    // the objects's fixed-size buffer for elements
    return new_capacity;
}

}
#endif // TEST_RW_EXTENSIONS

/**************************************************************************/

template <class T, class Allocator>
void test_swap (const T *lhs_seq, std::size_t lhs_seq_len,
                const T *rhs_seq, std::size_t rhs_seq_len,
                std::deque<T, Allocator>*,
                const char *tname)
{
    typedef std::deque<T, Allocator>  Deque;
    typedef typename Deque::iterator  Iterator;
    typedef typename Deque::size_type SizeType;

    // create two containers from the provided sequences
    Deque lhs (lhs_seq, lhs_seq + lhs_seq_len);
    Deque rhs (rhs_seq, rhs_seq + rhs_seq_len);

    // save the begin and and iterators and the size
    // of each container before swapping the objects
    const Iterator lhs_begin_0 = lhs.begin ();
    const Iterator lhs_end_0   = lhs.end ();
    const SizeType lhs_size_0  = lhs.size ();

    const Iterator rhs_begin_0 = rhs.begin ();
    const Iterator rhs_end_0   = rhs.end ();
    const SizeType rhs_size_0  = rhs.size ();

    // swap the two containers
    lhs.swap (rhs);

    // compute the begin and and iterators and the size
    // of each container after swapping the objects
    const Iterator lhs_begin_1 = lhs.begin ();
    const Iterator lhs_end_1   = lhs.end ();
    const SizeType lhs_size_1  = lhs.size ();

    const Iterator rhs_begin_1 = rhs.begin ();
    const Iterator rhs_end_1   = rhs.end ();
    const SizeType rhs_size_1  = rhs.size ();

    // verify that the iterators and sizes
    // of the two objects were swapped
    rw_assert (lhs_begin_0 == rhs_begin_1 && lhs_begin_1 == rhs_begin_0,
               0, __LINE__,
               "begin() not swapped for \"%{X=*.*}\" and \"%{X=*.*}\"",
               int (lhs_seq_len), -1, lhs_seq,
               int (rhs_seq_len), -1, rhs_seq);

    rw_assert (lhs_end_0 == rhs_end_1 && lhs_end_1 == rhs_end_0,
               0, __LINE__,
               "end() not swapped for \"%{X=*.*}\" and \"%{X=*.*}\"",
               int (lhs_seq_len), -1, lhs_seq,
               int (rhs_seq_len), -1, rhs_seq);

    rw_assert (lhs_size_0 == rhs_size_1 && lhs_size_1 == rhs_size_0,
               0, __LINE__,
               "size() not swapped for \"%{X=*.*}\" and \"%{X=*.*}\"",
               int (lhs_seq_len), -1, lhs_seq,
               int (rhs_seq_len), -1, rhs_seq);

    // swap one of the containers with an empty unnamed temporary
    // container and verify that the object is empty
    { Deque ().swap (lhs); }

    const Iterator lhs_begin_2 = lhs.begin ();
    const Iterator lhs_end_2   = lhs.end ();
    const SizeType lhs_size_2  = lhs.size ();

    rw_assert (lhs_begin_2 == lhs_end_2, 0, __LINE__,
               "deque<%s>().begin() not swapped for \"%{X=*.*}\"",
               tname, int (rhs_seq_len), -1, rhs_seq);

    rw_assert (0 == lhs_size_2, 0, __LINE__,
               "deque<%s>().size() not swapped for \"%{X=*.*}\"",
               tname, int (rhs_seq_len), -1, rhs_seq);
}


template <class T>
void test_swap (const T*, const char* tname)
{
    rw_info (0, 0, 0,
             "std::deque<%s>::swap(deque<%1$s>&)", tname);

    typedef std::deque<T, std::allocator<T> > MyDeque;
    typedef typename MyDeque::iterator        Iterator;

    // create two empty deque objects
    MyDeque empty [2];

    // save their begin and end iterators before calling swap
    const Iterator before [2][2] = {
        { empty [0].begin (), empty [0].end () },
        { empty [1].begin (), empty [1].end () }
    };

    // swap the two containers
    empty [0].swap (empty [1]);

    // get the new begin and end iterators
    const Iterator after [2][2] = {
        { empty [0].begin (), empty [0].end () },
        { empty [1].begin (), empty [1].end () }
    };

    // verify that the iterators have not been invalidated
    rw_assert (   before [0][0] == after [1][0]
               && before [1][0] == after [0][0], 0, __LINE__,
               "deque<%s>().begin() not swapped", tname);

    rw_assert (   before [0][1] == after [1][1]
               && before [1][1] == after [0][1], 0, __LINE__,
               "deque<%s>().end() not swapped", tname);

    // static to zero-initialize if T is a POD type
    static T seq [32];

    const std::size_t seq_len = sizeof seq / sizeof *seq;

    for (std::size_t i = 0; i != seq_len; ++i) {
        for (std::size_t j = 0; j != seq_len; ++j) {
            test_swap (seq, i, seq, j, (MyDeque*)0, tname);
        }
    }
}

/**************************************************************************/

void test_swap ()
{
    test_swap ((int*)0, "int");
    test_swap ((UserClass*)0, "UserClass");
}

/**************************************************************************/

int run_test (int, char**)
{
    // Test std::swap calling std::deque::swap
    test_std_swap ();

    static const Deque::size_type caps[] = {
        2, 3, 4, 5, 16, 32
    };

    for (std::size_t i = 0; i != sizeof caps / sizeof *caps; ++i) {

#if TEST_RW_EXTENSIONS
        new_capacity = caps [i];

        rw_info (0, 0, 0,
                 "__rw::__rw_new_capacity<std::deque<UserClass> >(0) = %u",
                 _RW::__rw_new_capacity (0, (Deque*)0));
#endif // TEST_RW_EXTENSIONS

        test_swap ();
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.deque.special",
                    0 /* no comment */,
                    run_test,
                    0 /* co command line options */);
}
