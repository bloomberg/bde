/***************************************************************************
 *
 * 20.operators.cpp - test exercising [lib.operators]
 *
 * $Id: 20.operators.cpp 448928 2006-09-22 13:43:18Z faridz $
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

// The test exercises the ability to specialize various components of
// the library (algorithms and containers in particular) on user-defined
// iterator types in the presence of using directives.

#include <rw/_config.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959)

#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif

#if 0 // def _MSC_VER
// disabled (warnings may be meaningful)
#  pragma warning (disable: 4800)
#  pragma warning (disable: 4805)
#endif   // _MSC_VER


#include <algorithm>
#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <cstddef>  // for std::size_t

#include <driver.h>

/**************************************************************************/

_USING (namespace std);
_USING (namespace std::rel_ops);


#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

// explicitly instantiate containers
template class
std::deque<int, std::allocator<int> >;

template class
std::list<int,std::allocator<int> >;

template class
std::map<int, int, std::less<int>, std::allocator<std::pair<const int, int> > >;

template class
std::set<int>;

template class
std::basic_string<int, std::char_traits<int>, std::allocator<int> >;

template class
std::vector<int, std::allocator<int> >;

#endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION

/**************************************************************************/

#if !defined(BSLS_PLATFORM__OS_AIX) && \
                                (!defined (__SUNPRO_CC) || __SUNPRO_CC > 0x530)
#  define FUN(ignore, result, name, arg_list) do {  \
          result (*pf) arg_list = &name;            \
          _RWSTD_UNUSED (pf);                       \
      } while (0)
#else
#if 0
   // working around a SunPro 5.3 bug (see PR #25972) that prevents it
   // from taking the address of a function template in template code
#  define FUN(T, result, name, arg_list) do {                 \
          typedef typename T::iterator       Iterator;        \
          typedef typename T::const_iterator ConstIterator;   \
          const Iterator      *pi  = 0;                       \
          const ConstIterator *pci = 0;                       \
          name (pi, pi);                                      \
          name (pci, pci);                                    \
      } while (0)
#endif

   // working around a SunPro 5.3 bug (see PR #25972) that prevents it
   // from taking the address of a function template in template code
#  define FUN(T, result, name, arg_list) do {                 \
          const T::iterator       *pi  = 0;                   \
          const T::const_iterator *pci = 0;                   \
          name (pi, pi);                                      \
      } while (0)
#endif   // SunPro 5.3


#define TEST_INEQUALITY(T) do {                                 \
        T::iterator       qi;                                   \
        const T::iterator *pi = &qi;                            \
        T::const_iterator qci;                                  \
        const T::const_iterator *pci = &qci;                    \
                                                                \
        pi != pi;                                               \
        pci != pci;                                             \
    } while(0)

#if 0
#define TEST_INEQUALITY(T) do {                                     \
        FUN (T, bool, std::rel_ops::operator!=,                     \
             (const T::iterator&, const T::iterator&));             \
        FUN (T, bool, std::rel_ops::operator!=,                     \
             (const T::const_iterator&, const T::const_iterator&))
#endif

#define TEST_OPERATORS(T) do {                                  \
        T::iterator       qi;                                   \
        const T::iterator *pi = &qi;                            \
        T::const_iterator qci;                                  \
        const T::const_iterator *pci = &qci;                    \
                                                                \
        pi != pi;                                               \
        pci != pci;                                             \
        pi < pi;                                                \
        pci < pci;                                              \
        pi > pi;                                                \
        pci > pci;                                              \
        pi <= pi;                                               \
        pci <= pci;                                             \
        pi >= pi;                                               \
        pci >= pci;                                             \
    } while(0)

#if 0
#define TEST_OPERATORS(T)                                             \
        TEST_INEQUALITY (T);                                          \
        FUN (T, bool, std::rel_ops::operator>,                        \
             (const T::iterator&, const T::iterator&));               \
        FUN (T, bool, std::rel_ops::operator<=,                       \
             (const T::iterator&, const T::iterator&));               \
        FUN (T, bool, std::rel_ops::operator>=,                       \
             (const T::iterator&, const T::iterator&));               \
        FUN (T, bool, std::rel_ops::operator>,                        \
             (const T::const_iterator&, const T::const_iterator&));   \
        FUN (T, bool, std::rel_ops::operator<=,                       \
             (const T::const_iterator&, const T::const_iterator&));   \
        FUN (T, bool, std::rel_ops::operator>=,                       \
             (const T::const_iterator&, const T::const_iterator&))
#endif


template <class Container, class RandomAccessIterator>
void test_iterator (Container, RandomAccessIterator)
{
    TEST_OPERATORS (typename Container);
}


template <class Container>
void test_iterator (Container, int*)
{
    // cannot specialize std::rel_ops::operators on native types
    // or pointers to such things
}

/**************************************************************************/

template <class T>
struct UnaryPredicate
{
    bool operator() (const T&) const {
        return true;
    }
};


template <class T>
struct BinaryPredicate
{
    bool operator() (const T&, const T&) const {
        return true;
    }
};


template <class T>
struct RandomNumberGenerator
{
    T operator() (int) const {
        return T ();
    }
};


template <class T>
struct Generator
{
    T operator() () const {
        return T ();
    }
};

template <class T>
struct UnaryFunction
{
    T operator() (const T &t) const {
        return t;
    }
};


template <class T>
struct BinaryFunction
{
    T operator() (const T &t, const T&) const {
        return t;
    }
};

/**************************************************************************/

template <class T, class InputIterator>
void test_input_iterators (T, InputIterator)
{
    // do not run (compile only), prevent warnings about unreachable code
    static int count = 0;

    if (++count)
        return;

    typedef InputIterator I;

    std::for_each (I (), I (), UnaryFunction<T>());
    std::find (I (), I (), T ());
    std::find_if (I (), I (), UnaryPredicate<T>());

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
    std::count (I (), I (), T ());
    std::count_if (I (), I (), UnaryPredicate<T>());
#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)
    std::size_t n = 0;
    std::count (I (), I (), T (), n);
    std::count_if (I (), I (), UnaryPredicate<T>(), n);
#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC

    std::mismatch (I (), I (), I ());
    std::mismatch (I (), I (), I (), BinaryPredicate<T>());
    std::equal (I (), I (), I ());
    std::equal (I (), I (), I (), BinaryPredicate<T>());

    std::includes (I (), I (), I (), I ());
    std::includes (I (), I (), I (), I (), BinaryPredicate<T>());

    std::lexicographical_compare (I (), I (), I (), I ());
    std::lexicographical_compare (I (), I (), I (), I (), BinaryPredicate<T>());
}

/**************************************************************************/

template <class T, class OutputIterator>
void test_output_iterators (T, OutputIterator)
{
    // do not run (compile only), prevent warnings about unreachable code
    static int count = 0;

    if (++count)
        return;

    typedef OutputIterator I;

    std::copy (I (), I (), I ());
    std::copy_backward (I (), I (), I ());

    std::transform  (I (), I (), I (), UnaryFunction<T>());
    std::transform  (I (), I (), I (), I (), BinaryFunction<T>());

    std::replace_copy (I (), I (), I (), T (), T ());
    std::replace_copy_if (I (), I (), I (), UnaryPredicate<T>(), T ());

    std::merge (I (), I (), I (), I (), I ());
    std::merge (I (), I (), I (), I (), I (), BinaryPredicate<T>());

    std::set_union (I (), I (), I (), I (), I ());
    std::set_union (I (), I (), I (), I (), I (), BinaryPredicate<T>());

    std::set_intersection (I (), I (), I (), I (), I ());
    std::set_intersection (I (), I (), I (), I (), I (), BinaryPredicate<T>());

    std::set_difference (I (), I (), I (), I (), I ());
    std::set_difference (I (), I (), I (), I (), I (), BinaryPredicate<T>());

    std::set_symmetric_difference (I (), I (), I (), I (), I ());
    std::set_symmetric_difference (I (), I (), I (), I (), I (),
                                   BinaryPredicate<T>());

    std::fill_n (I (), 0, T ());

    std::generate_n (I (), 0, Generator<T>());

    std::remove_copy (I (), I (), I (), T ());
    std::remove_copy_if (I (), I (), I (), UnaryPredicate<T>());

    std::unique_copy (I (), I (), I ());
    std::unique_copy (I (), I (), I (), BinaryPredicate<T>());

    std::reverse_copy (I (), I (), I ());

    std::rotate_copy (I (), I (), I (), I ());
}

/**************************************************************************/

template <class T, class ForwardIterator>
void test_forward_iterators (T, ForwardIterator)
{
    // do not run (compile only), prevent warnings about unreachable code
    static int count = 0;

    if (++count)
        return;

    typedef ForwardIterator I;

    std::find_end (I (), I (), I (), I ());
    std::find_end (I (), I (), I (), I (), BinaryPredicate<T>());

    std::find_first_of (I (), I (), I (), I ());
    std::find_first_of (I (), I (), I (), I (), BinaryPredicate<T>());

    std::adjacent_find (I (), I ());
    std::adjacent_find (I (), I (), BinaryPredicate<T>());

    std::search (I (), I (), I (), I ());
    std::search (I (), I (), I (), I (), BinaryPredicate<T>());

    std::search_n (I (), I (), 0, T ());
    std::search_n (I (), I (), 0, T (), BinaryPredicate<T>());

    std::swap_ranges (I (), I (), I ());
    std::iter_swap (I (), I ());

    std::replace (I (), I (), T (), T ());
    std::replace_if (I (), I (), UnaryPredicate<T>(), T ());

    std::equal_range (I (), I (), T ());
    std::equal_range (I (), I (), T (), BinaryPredicate<T>());

    std::binary_search (I (), I (), T ());
    std::binary_search (I (), I (), T (), BinaryPredicate<T>());

    std::min_element (I (), I ());
    std::min_element (I (), I (), BinaryPredicate<T>());

    std::max_element (I (), I ());
    std::max_element (I (), I (), BinaryPredicate<T>());

    std::fill (I (), I (), T ());

    std::generate (I (), I (), Generator<T>());

    std::remove (I (), I (), T ());
    std::remove_if (I (), I (), UnaryPredicate<T>());

    std::unique (I (), I ());
    std::unique (I (), I (), BinaryPredicate<T>());

    std::reverse (I (), I ());

    std::rotate (I (), I (), I ());
}

/**************************************************************************/

template <class T, class BidirectionalIterator>
void test_bidirectional_iterators (T, BidirectionalIterator)
{
    // do not run (compile only), prevent warnings about unreachable code
    static int count = 0;

    if (++count)
        return;

    typedef BidirectionalIterator I;

    std::partition (I (), I (), UnaryPredicate<T>());
    std::stable_partition (I (), I (), UnaryPredicate<T>());

    std::lower_bound (I (), I (), T ());
    std::lower_bound (I (), I (), T (), BinaryPredicate<T>());

    std::upper_bound (I (), I (), T ());
    std::upper_bound (I (), I (), T (), BinaryPredicate<T>());

    std::inplace_merge (I (), I (), I ());
    std::inplace_merge (I (), I (), I (), BinaryPredicate<T>());

    std::next_permutation (I (), I ());
    std::next_permutation (I (), I (), BinaryPredicate<T>());

    std::prev_permutation (I (), I ());
    std::prev_permutation (I (), I (), BinaryPredicate<T>());
}

/**************************************************************************/

template <class T, class RandomAccessIterator>
void test_random_access_iterators (T, RandomAccessIterator)
{
    // do not run (compile only), prevent warnings about unreachable code
    static int count = 0;

    if (++count)
        return;

    typedef RandomAccessIterator I;

    RandomNumberGenerator<T> rndgen;

    std::random_shuffle (I (), I ());
    std::random_shuffle (I (), I (), rndgen);

    std::sort (I (), I ());
    std::sort (I (), I (), BinaryPredicate<T>());

    std::stable_sort (I (), I ());
    std::stable_sort (I (), I (), BinaryPredicate<T>());

    std::partial_sort (I (), I (), I ());
    std::partial_sort (I (), I (), I (), BinaryPredicate<T>());

    std::partial_sort_copy (I (), I (), I (), I ());
    std::partial_sort_copy (I (), I (), I (), I (), BinaryPredicate<T>());

    std::nth_element (I (), I (), I ());
    std::nth_element (I (), I (), I (), BinaryPredicate<T>());

    std::push_heap (I (), I (), BinaryPredicate<T>());
    std::pop_heap (I (), I (), BinaryPredicate<T>());
    std::make_heap (I (), I (), BinaryPredicate<T>());
    std::sort_heap (I (), I (), BinaryPredicate<T>());
}

/**************************************************************************/

static int
run_test (int, char**)
{
    typedef std::map<int, int>      Map;
    typedef std::multimap<int, int> MultiMap;

    // verify that rel_ops operators can be instantiated
    // on iterators of the containers below
#if !defined(BSLS_PLATFORM__OS_AIX)
    TEST_OPERATORS (std::deque<int>);
#endif

    TEST_INEQUALITY (std::list<int>);
    TEST_INEQUALITY (Map);
    TEST_INEQUALITY (MultiMap);
    TEST_INEQUALITY (std::set<int>);
    TEST_INEQUALITY (std::multiset<int>);

#if !defined (_MSC_VER) || _MSC_VER > 1300
    // prevent from testing with the braindead MSVC 6 and 7
    // as a workaround for compiler bugs (PR #16828, 22268)

    // prevent attempts to specialize rel_ops operators on
    // native types (or pointers to such things)
    test_iterator (std::basic_string<int>(),
                   std::basic_string<int>::iterator ());

    test_iterator (std::vector<int>(),
                   std::vector<int>::iterator ());
#endif   // MSVC > 7.0

    TEST_OPERATORS (std::vector<int>);
#if !defined(BSLS_PLATFORM__OS_AIX)
    TEST_OPERATORS (std::vector<bool>);
#endif

#define TEST_INPUT_ITERATORS(T)                                     \
    test_input_iterators (T (), std::deque<T>::iterator ());        \
    test_input_iterators (T (), std::list<T>::iterator ());         \
    test_input_iterators (std::map<T, T>::value_type (),            \
                          std::map<T, T>::iterator ());             \
    test_input_iterators (std::multimap<T, T>::value_type (),       \
                          std::multimap<T, T>::iterator ());        \
    test_input_iterators (T (), std::set<T>::iterator ());          \
    test_input_iterators (T (), std::multiset<T, T>::iterator ());  \
    test_input_iterators (T (), std::basic_string<T>::iterator ()); \
    test_input_iterators (T (), std::vector<T>::iterator ())

#define TEST_OUTPUT_ITERATORS(T)                                     \
    test_output_iterators (T (), std::deque<T>::iterator ());        \
    test_output_iterators (T (), std::list<T>::iterator ());         \
    test_output_iterators (T (), std::basic_string<T>::iterator ()); \
    test_output_iterators (T (), std::vector<T>::iterator ())

#define TEST_FORWARD_ITERATORS(T)                                     \
    test_forward_iterators (T (), std::deque<T>::iterator ());        \
    test_forward_iterators (T (), std::list<T>::iterator ());         \
    test_forward_iterators (T (), std::basic_string<T>::iterator ()); \
    test_forward_iterators (T (), std::vector<T>::iterator ())

#define TEST_BIDIRECTIONAL_ITERATORS(T)                                     \
    test_bidirectional_iterators (T (), std::deque<T>::iterator ());        \
    test_bidirectional_iterators (T (), std::list<T>::iterator ());         \
    test_bidirectional_iterators (T (), std::basic_string<T>::iterator ()); \
    test_bidirectional_iterators (T (), std::vector<T>::iterator ())

#define TEST_RANDOM_ACCESS_ITERATORS(T)                                     \
    test_random_access_iterators (T (), std::deque<T>::iterator ());        \
    test_random_access_iterators (T (), std::basic_string<T>::iterator ()); \
    test_random_access_iterators (T (), std::vector<T>::iterator ());       \

    // verify that algorithms can be specialized on container
    // iterators without causing ambiguities with rel_ops
    TEST_INPUT_ITERATORS (int);
    TEST_OUTPUT_ITERATORS (int);
    TEST_FORWARD_ITERATORS (int);
    TEST_BIDIRECTIONAL_ITERATORS (int);
    TEST_RANDOM_ACCESS_ITERATORS (int);

#if !defined (__HP_aCC) || _RWSTD_HP_aCC_MINOR > 3600

    // working around an HP aCC bug (PR #28331)
    TEST_INPUT_ITERATORS (bool);
    TEST_OUTPUT_ITERATORS (bool);
    TEST_FORWARD_ITERATORS (bool);
    TEST_BIDIRECTIONAL_ITERATORS (bool);
    TEST_RANDOM_ACCESS_ITERATORS (bool);

#endif   // HP aCC > x.36

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.operators",
                    "interactions with the rest of the implementation",
                    run_test,
                    0 /* no command line options */);
}
