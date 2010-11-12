/***************************************************************************
 *
 * 25.random.shuffle.cpp - test exercising 25.2.11 [lib.alg.random.shuffle]
 *
 * $Id: 25.random.shuffle.cpp 650944 2008-04-23 17:27:06Z sebor $
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <algorithm>    // for random_shuffle
#include <cstring>      // for memset(), size_t

#include <alg_test.h>
#include <rw_value.h>   // for UserClass
#include <driver.h>     // for rw_test()


_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
void
random_shuffle (RandomAccessIter<assign<base<cpy_ctor> > >, 
                RandomAccessIter<assign<base<cpy_ctor> > >);

template
void
random_shuffle (RandomAccessIter<assign<base<cpy_ctor> > >, 
                RandomAccessIter<assign<base<cpy_ctor> > >,
                func<const long>&);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

// 25.2.11, p3 requires that a RandomNumberGenerator take
// and return a value convertible from and to, respectively,
// iterator::difference_type

template <class DifferenceType>
struct Convertible
{
    Convertible (DifferenceType val): val_ (val) { }
    operator DifferenceType () /* non-const */ { return val_; }

private:
    const DifferenceType val_;

    template <class T>
    Convertible (T);
};

#if TEST_RW_PEDANTIC // convertible-to-difference is not the same as supporting operator+
// detect assumptions made by the algorithm about the return value
// from RandomGenerator::operator() being anything but convertible
// to the iterator's difference type
template <class DifferenceType, class T>
void operator+ (Convertible<DifferenceType>, T);

template <class DifferenceType, class T>
void operator+ (T, Convertible<DifferenceType>);
#endif // !TEST_RW_PEDANTIC

template <class DifferenceType>
struct RandomGenerator
{
    // dummy arguments to prevent the class from being default
    // constructible or construtible by conversion from an int
    RandomGenerator (int, int) {
        ncalls_ = 0;
    }

    // non-const in order to detect unwarranted assumptions
    // in the algorithm(s)
    Convertible<DifferenceType>
    operator() (DifferenceType n) /* non-const */ {

        ++ncalls_;

        gen_ = (gen_ << 7) + gen_ % 128;

        // prevent the number from going negative(!)
        if (gen_ < 0)
            gen_ = -gen_;

        return gen_ % n;
    }

    static std::size_t ncalls_;

private:

    template <class T>
    void operator() (T) /* non-const */;

    static DifferenceType gen_;
};

template <class DifferenceType>
std::size_t RandomGenerator<DifferenceType>::ncalls_ = 0;

template <class DifferenceType>
DifferenceType RandomGenerator<DifferenceType>::gen_ = 12345;

/**************************************************************************/

int sequence_start;

int sequence_generator ()
{
    return sequence_start++;
}

/**************************************************************************/

// exercises 25.2.11 -- std::random_shuffle()
template <class T, class Iterator, class RandomNumberGenerator>
void test_random_shuffle (const std::size_t            line,
                          const std::size_t            N,
                          const Iterator              &it, 
                          const RandomNumberGenerator*,
                          const T*,
                          bool                         rnd_gen)
{
    const char* const itname  = type_name (it, (T*)0);
    const char* const fname   = "random_shuffle";
    const char* const funname = rnd_gen ? "random_generator" : "";

    rw_info (0, 0, 0,
             "std::%s (%s, %2$s%{?}, %s%{;})",
             fname, itname, rnd_gen, funname);

    // generate a sequential value for each default-constructed T
    // starting with 0 (i.e., T::val_ will be 0 for the first T)
    sequence_start = 0;
    T::gen_        = sequence_generator;

    T    *buf      = new T [N + 1];
    char *missing  = new char [N + 1];

    for (std::size_t i = 0; i < N; ++i) {

        // create a pair of safe iterator to pass to random_shuffle
        // in order to test that the function doesn't write past
        // the end of the sequence [first, last)
        const Iterator first = make_iter (buf,     buf, buf + i, it);
        const Iterator last  = make_iter (buf + i, buf, buf + i, it);

        /* non-const */ RandomNumberGenerator rnd (0, 0);   // dummy args

        // exercise 25.2.11 - std::random_shuffle<>()
        std::size_t n_op_assign = T::n_total_op_assign_;

        // shuffle buffer elements
        if (rnd_gen)
            std::random_shuffle (first, last, rnd);
        else
            std::random_shuffle (first, last);

        n_op_assign = T::n_total_op_assign_ - n_op_assign;

        // verify 25.2.11, p1
        // iterate over elements of the missing array clearing those
        // at the index given by T::val_ to true to verify that the
        // algorithm didn't drop any elements
        std::size_t j;
        std::memset (missing, 1, N);
        for (j = 0; j != i; ++j) {
            const std::size_t inx = std::size_t (buf [j].data_.val_);
            if (inx < N)
                missing [inx] = 0;
        }

        // find the first missing element (if any)
        const char* const first_missing = (char*)std::memchr (missing, 1, i);

        bool success = 0 == first_missing;

        rw_assert (success, 0, line,
                   "%zu. std::%s<%s%{?}, %s%{;}>(): can't find element "
                   "with value %td: got \"%{X=*.*}\"",
                   i, fname, itname, rnd_gen, funname,
                   first_missing - missing,
                   int (i), -1, buf);


        // verify 25.2.11, p2, complexity:
        // exactly K = ((last - first) - 1) swaps
        // i.e., K * 2 assignments (at 2 assignment per swap
        // plus one copy construction)
        success = n_op_assign == 2 * (i ? i - 1 : i);
        rw_assert (success, 0, line,
                   "%zu. std::%s<%s%{?}, %s%{;}>(): complexity: "
                   "expected %zu swaps, got %zu",
                   i, fname, itname, rnd_gen, funname, 
                   2 * i, n_op_assign);
    }

    delete[] buf;
    delete[] missing;
}

/**************************************************************************/

void test_random_shuffle (const std::size_t line)
{
#ifndef _RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE

    rw_info (0, 0, 0,
             "verify that the randomly shuffled sequence "
             "matches the expected result, this "
             "depends on the implementation of the random "
             "number generator used by random_shuffle()");
    
    int array[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
        0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    };

    std::random_shuffle (array, array + sizeof array / sizeof *array);

    const int result[] = {
        0xdc, 0x98, 0x6c, 0xcc, 0x1c, 0xb1, 0x2f, 0x13,
        0xa4, 0x8b, 0x96, 0x87, 0x95, 0xd8, 0x5e, 0xd5,
        0xb6, 0xee, 0xc3, 0x07, 0x68, 0xe4, 0x4e, 0xc0,
        0xb7, 0x40, 0x15, 0x55, 0x31, 0x5f, 0xaf, 0xce,
        0x01, 0x51, 0x09, 0x4f, 0x0b, 0x80, 0x28, 0x0f,
        0xec, 0x43, 0x8a, 0x14, 0x90, 0x9a, 0x7b, 0x3e,
        0x81, 0x1e, 0xab, 0xff, 0xed, 0xf6, 0xca, 0x0c,
        0x6a, 0x9c, 0x91, 0x60, 0x27, 0xb2, 0x04, 0xd3,
        0x72, 0xd7, 0x08, 0x8c, 0x8e, 0x1b, 0x35, 0x49,
        0xba, 0x7a, 0xfd, 0x53, 0x22, 0xaa, 0xf5, 0x93,
        0xf2, 0x63, 0x05, 0xb4, 0x33, 0x9b, 0xea, 0xc8,
        0xc5, 0x39, 0xfc, 0x5d, 0x56, 0x50, 0xe2, 0xf4,
        0xc1, 0xa9, 0x76, 0x99, 0xe7, 0xd0, 0xc6, 0x83,
        0x11, 0x77, 0x70, 0xa6, 0x46, 0x38, 0x1d, 0x74,
        0x6b, 0xdf, 0x30, 0xcf, 0xd1, 0x29, 0x7f, 0x4b,
        0xe8, 0x1f, 0xc4, 0xa1, 0x75, 0xae, 0x3f, 0xf1,
        0xad, 0x42, 0x2d, 0xfe, 0x62, 0x61, 0x6d, 0x3d,
        0xa7, 0x0a, 0x6e, 0xde, 0x16, 0x36, 0xd2, 0xcb,
        0x5c, 0xc2, 0x7c, 0xd6, 0x37, 0xa0, 0x20, 0xb5,
        0xf3, 0xac, 0x71, 0x64, 0x26, 0x66, 0x88, 0x23,
        0x52, 0xdb, 0x2e, 0x10, 0x84, 0x0e, 0x6f, 0x89,
        0xb3, 0x86, 0xe5, 0xe6, 0x25, 0x78, 0xd4, 0x2a,
        0xa8, 0x5b, 0xc9, 0x44, 0x24, 0x17, 0x1a, 0xf0,
        0x69, 0x7e, 0xbf, 0x8d, 0x5a, 0x82, 0x9e, 0xf9,
        0xc7, 0xf7, 0x4c, 0x92, 0x9d, 0x34, 0x02, 0x45,
        0x59, 0xe1, 0x48, 0x7d, 0xb9, 0x94, 0x41, 0x47,
        0x54, 0x00, 0x3c, 0x65, 0xf8, 0xb8, 0xcd, 0x0d,
        0xe9, 0x9f, 0x8f, 0x79, 0x3a, 0xda, 0x4a, 0x97,
        0xef, 0xa2, 0x21, 0x58, 0x67, 0x32, 0xb0, 0x3b,
        0xe0, 0x85, 0x19, 0x12, 0x2c, 0xbc, 0x4d, 0xe3,
        0x2b, 0x03, 0xbe, 0xfb, 0xbd, 0x57, 0xeb, 0x06,
        0xfa, 0xbb, 0xdd, 0xa5, 0xa3, 0x73, 0x18, 0xd9
    };

    for (std::size_t i = 0; i != sizeof array / sizeof *array; ++i) {
        const bool success = array [i] == result [i];
        if (!success) {
            rw_assert (0, 0, line, 
                       "randomly shuffled sequence failed to match "
                       "the expected result (data portability failure) "
                       "%d != %d at %zu",
                       array [i], result [i], i + 1);
        }
    }

#else
    rw_note (0, 0, 0,
             "_RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE is defined, "
             "no random_test available");
#endif   // _RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 1024;          // --nloops=#
/* extern */ int rw_opt_no_random;              // --no-random
/* extern */ int rw_opt_no_random_generator;    // --no-random_generator
/* extern */ int rw_opt_no_rand_test;           // --no-rand_test


static int run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    typedef RandomAccessIter<UserClass>::difference_type DiffT;
    typedef RandomGenerator<DiffT>               RandGenT;

    if (rw_opt_no_random) {
        rw_note (0, __FILE__, __LINE__, "std::random test disabled");
    }
    else {
        rw_info (0, 0, 0,
                 "template <class %s> "
                 "std::random_shuffle (%1$s, %1$s)",
                 "RandomAccessIterator");

        test_random_shuffle (__LINE__, N, RandomAccessIter<UserClass>(),
                             (RandGenT*)0, (UserClass*)0, false);
    }

    if (rw_opt_no_rand_test) {
        rw_note (0, __FILE__, __LINE__, "random algorithm test disabled");
    }
    else {
        test_random_shuffle (__LINE__);
    }

    if (rw_opt_no_random_generator) {
        rw_note (0, __FILE__, __LINE__,  
                 "std::random with random_generator test disabled");
    }
    else {
        rw_info (0, 0, 0,
                 "template <class %s, class %s> "
                 "std::random_shuffle (%1$s, %1$s, %2$s)",
                 "RandomAccessIterator", "RandomNumberGenerator");

        test_random_shuffle (__LINE__, N, RandomAccessIter<UserClass>(),
                             (RandGenT*)0, (UserClass*)0, true);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.random_shuffle",
                    0 /* no comment */, run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-random# "
                    "|-no-random_generator# "
                    "|-no-rand_test", 
                    &rw_opt_nloops,
                    &rw_opt_no_random,
                    &rw_opt_no_random_generator,
                    &rw_opt_no_rand_test);
}
