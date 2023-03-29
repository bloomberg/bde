// bslstl_algorithm.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALGORITHM
#define INCLUDED_BSLSTL_ALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations for algorithms not in the system library.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_algorithm.h
//
//@SEE_ALSO: bsl+bslhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_algorithm.h>' instead.  This component provides a namespace for
// implementations for standard algorithms that are not provided by the
// underlying standard library implementation.  For example, 'any_of' is a
// C++11 algorithm, and it is provided here for code using C++03.
//
///Usage
///-----
// This component is for use by the 'bsl+bslhdrs' package.  Use
// 'bsl_algorithm.h' directly.

#include <bslscm_version.h>
#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_iterator.h>  // iterator tags
#include <bslstl_pair.h>

#include <algorithm>
#include <functional> // less

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslstl {

struct AlgorithmUtil {
    // Provide a namespace for implementing helper routines for algorithm
    // implementations.

    // CLASS FUNCTIONS
    template <class CONTAINER, class PREDICATE>
    static
    typename CONTAINER::size_type
    containerEraseIf(CONTAINER& container, PREDICATE predicate);
        // Erase all the elements in the specified container 'container' that
        // satisfy the specified predicate 'predicate'.  Return the number of
        // elements erased.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

    // Import selected symbols into bsl namespace
    using std::adjacent_find;
    using std::binary_search;
    using std::copy;
    using std::copy_backward;

    using std::equal;
    using std::equal_range;
    using std::fill;
    using std::fill_n;
    using std::find;
    using std::find_end;
    using std::find_first_of;
    using std::find_if;
    using std::for_each;
    using std::generate;
    using std::generate_n;
    using std::includes;
    using std::inplace_merge;
    using std::iter_swap;
    using std::lexicographical_compare;
    using std::lower_bound;

    using std::make_heap;
    using std::max;
    using std::max_element;
    using std::merge;
    using std::min;
    using std::min_element;
    using std::mismatch;
    using std::next_permutation;
    using std::nth_element;
    using std::partial_sort;
    using std::partial_sort_copy;
    using std::partition;
    using std::pop_heap;
    using std::prev_permutation;
    using std::push_heap;
    using std::remove;
    using std::remove_copy;
    using std::remove_copy_if;
    using std::remove_if;
    using std::replace;
    using std::replace_copy;
    using std::replace_copy_if;
    using std::replace_if;
    using std::reverse;
    using std::reverse_copy;
    using std::rotate;
    using std::rotate_copy;
    using std::search;
    using std::search_n;
    using std::set_difference;
    using std::set_intersection;
    using std::set_new_handler;
    using std::set_symmetric_difference;
    using std::set_union;
    using std::sort;
    using std::sort_heap;
    using std::stable_partition;
    using std::stable_sort;
    using std::swap;
    using std::swap_ranges;
    using std::transform;
    using std::unique;
    using std::unique_copy;
    using std::upper_bound;

#if ! defined BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    // These names are removed by C++17
    using std::random_shuffle;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::all_of;
    using std::any_of;
    using std::copy_if;

    // 'copy_n' is implemented separately in 'bslstp_exalgorithm' for backwards
    // compatibility with the previous STLport definition (which differs from
    // the platform implementation).
    //
    // using std::(copy_n);

    using std::find_if_not;
    using std::is_heap;
    using std::is_heap_until;
    using std::is_partitioned;
    using std::is_permutation;
    using std::is_sorted;
    using std::is_sorted_until;
    using std::minmax;
    using std::minmax_element;
    using std::move;
    using std::move_backward;
    using std::none_of;
    using std::partition_copy;
    using std::partition_point;
    using std::shuffle;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

// C++14 Algorithms
//   We get these via the "using std::equal" above
//   equal
//   mismatch

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::clamp;
    using std::for_each_n;
    using std::sample;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    using std::lexicographical_compare_three_way;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
namespace ranges {

    // Non-modifying sequence operations

    using std::ranges::all_of;
    using std::ranges::any_of;
    using std::ranges::none_of;
    using std::ranges::for_each;
    using std::ranges::for_each_result;
    using std::ranges::for_each_n;
    using std::ranges::for_each_n_result;
    using std::ranges::count;
    using std::ranges::count_if;
    using std::ranges::mismatch;
    using std::ranges::mismatch_result;
    using std::ranges::equal;
    using std::ranges::lexicographical_compare;
    using std::ranges::find;
    using std::ranges::find_if;
    using std::ranges::find_if_not;
    using std::ranges::find_end;
    using std::ranges::find_first_of;
    using std::ranges::adjacent_find;
    using std::ranges::search;
    using std::ranges::search_n;

    // Modifying sequence operations

    using std::ranges::copy;
    using std::ranges::copy_result;
    using std::ranges::copy_if;
    using std::ranges::copy_if_result;
    using std::ranges::copy_n;
    using std::ranges::copy_n_result;
    using std::ranges::copy_backward;
    using std::ranges::copy_backward_result;
    using std::ranges::move;
    using std::ranges::move_result;
    using std::ranges::move_backward;
    using std::ranges::move_backward_result;
    using std::ranges::fill;
    using std::ranges::fill_n;
    using std::ranges::transform;
    using std::ranges::unary_transform_result;
    using std::ranges::binary_transform_result;
    using std::ranges::generate;
    using std::ranges::generate_n;
    using std::ranges::remove;
    using std::ranges::remove_if;
    using std::ranges::remove_copy;
    using std::ranges::remove_copy_result;
    using std::ranges::remove_copy_if;
    using std::ranges::remove_copy_if_result;
    using std::ranges::replace;
    using std::ranges::replace_if;
    using std::ranges::replace_copy;
    using std::ranges::replace_copy_result;
    using std::ranges::replace_copy_if;
    using std::ranges::replace_copy_if_result;
    using std::ranges::swap_ranges;
    using std::ranges::swap_ranges_result;
    using std::ranges::reverse;
    using std::ranges::reverse_copy;
    using std::ranges::reverse_copy_result;
    using std::ranges::rotate;
    using std::ranges::rotate_copy;
    using std::ranges::rotate_copy_result;
    using std::ranges::shuffle;
    using std::ranges::sample;
    using std::ranges::unique;
    using std::ranges::unique_copy;
    using std::ranges::unique_copy_result;

    // Partitioning operations

    using std::ranges::is_partitioned;
    using std::ranges::partition;
    using std::ranges::partition_copy;
    using std::ranges::partition_copy_result;
    using std::ranges::stable_partition;
    using std::ranges::partition_point;

    // Sorting operations

    using std::ranges::is_sorted;
    using std::ranges::is_sorted_until;
    using std::ranges::sort;
    using std::ranges::partial_sort;
    using std::ranges::partial_sort_copy;
    using std::ranges::stable_sort;
    using std::ranges::nth_element;

    // Binary search operations (on sorted ranges)

    using std::ranges::lower_bound;
    using std::ranges::upper_bound;
    using std::ranges::binary_search;
    using std::ranges::equal_range;

    // Other operations (on sorted ranges)

    using std::ranges::merge;
    using std::ranges::merge_result;
    using std::ranges::inplace_merge;

    // Set operations (on sorted ranges)

    using std::ranges::includes;
    using std::ranges::set_difference;
    using std::ranges::set_difference_result;
    using std::ranges::set_intersection;
    using std::ranges::set_intersection_result;
    using std::ranges::set_symmetric_difference;
    using std::ranges::set_symmetric_difference_result;
    using std::ranges::set_union;
    using std::ranges::set_union_result;

    // Heap operations

    using std::ranges::is_heap;
    using std::ranges::is_heap_until;
    using std::ranges::make_heap;
    using std::ranges::push_heap;
    using std::ranges::pop_heap;
    using std::ranges::sort_heap;

    // Minimum/maximum operations

    using std::ranges::max;
    using std::ranges::max_element;
    using std::ranges::min;
    using std::ranges::min_element;
    using std::ranges::minmax;
    using std::ranges::minmax_result;
    using std::ranges::minmax_element;
    using std::ranges::minmax_element_result;
    using std::ranges::clamp;
    using std::ranges::is_permutation;
    using std::ranges::next_permutation;
    using std::ranges::next_permutation_result;
    using std::ranges::prev_permutation;
    using std::ranges::prev_permutation_result;

    // Return types

    using std::ranges::in_fun_result;
    using std::ranges::in_in_result;
    using std::ranges::in_out_result;
    using std::ranges::in_in_out_result;
    using std::ranges::in_out_out_result;
    using std::ranges::min_max_result;
    using std::ranges::in_found_result;
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Import additional names expected by existing code, but not mandated by
    // the standard header.
    using std::advance;
    using std::bad_alloc;
    using std::bidirectional_iterator_tag;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
#if !defined(BSLS_PLATFORM_CMP_MSVC) &&                                       \
    (BSLS_COMPILERFEATURES_CPLUSPLUS <= 201703L)
    using std::iterator;
#endif
    using std::new_handler;
    using std::nothrow;
    using std::nothrow_t;
    using std::output_iterator_tag;
    using std::random_access_iterator_tag;

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

#ifndef BSLSTL_ITERATOR_PROVIDE_SUN_CPP98_FIXES
    // Use the compiler vendor supplied version of 'count' and 'count_if'.
    using std::count;
    using std::count_if;
#else
    // Sun-specific fixes
    template <class InputIter, class TYPE>
    typename iterator_traits<InputIter>::difference_type
    count(InputIter first, InputIter last, const TYPE& value);
        // Provide an override for 'count' since Sun only provides a 4 argument
        // version while the C++ standard requires a 3 argument version.

    template <class InputIter, class PREDICATE>
    typename iterator_traits<InputIter>::difference_type
    count_if(InputIter first, InputIter last, PREDICATE pred);
        // Provide an override for 'count_if' since Sun only provides a 4
        // argument version while the C++ standard requires a 3 argument
        // version.
#endif  // BSLSTL_ITERATOR_PROVIDE_SUN_CPP98_FIXES


#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class INPUT_ITERATOR, class PREDICATE>
    bool all_of(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred);
        // Return 'true' if, for the specified '[first, last)' range and the
        // specified predicate 'pred', the range is either empty or 'pred(*i)'
        // is 'true' for every iterator 'i' in the range, and 'false'
        // otherwise.  Note that at most 'last - first' applications of the
        // predicate are performed.

    template <class INPUT_ITERATOR, class PREDICATE>
    bool any_of(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred);
        // Return 'false' if, for the specified '[first, last)' range and the
        // specified predicate 'pred', the range is either empty or 'pred(*i)'
        // is 'false' for every iterator 'i' in the range, and 'true'
        // otherwise.  Note that at most 'last - first' applications of the
        // predicate are performed.

    template <class INPUT_ITERATOR, class PREDICATE>
    bool none_of(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred);
        // Return 'true' if, for the specified '[first, last)' range and the
        // specified predicate 'pred', the range is either empty or 'pred(*i)'
        // is 'false' for every iterator 'i' in the range, and 'false'
        // otherwise.  Note that at most 'last - first' applications of the
        // predicate are performed.

# if defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC)
    // Visual Studio (the versions we support) provides 'copy_if'.
    using std::copy_if;
# else
# define BSLSTL_ALGORITHMWORKAROUND_IMPLEMENTS_COPY_IF                        1
    // C++03 standard libraries do not provide 'std::copy_if' (as it was not
    // part of the C++03 standard), but it is actually implementable in C++03,
    // so we inject it here.

    template <class INPUT_ITERATOR, class OUTPUT_ITERATOR, class PREDICATE>
    OUTPUT_ITERATOR
    copy_if(INPUT_ITERATOR  first,
            INPUT_ITERATOR  last,
            OUTPUT_ITERATOR result,
            PREDICATE       pred);
        // Copy all elements in the half-open range of the specified 'first',
        // and 'last' ('[first, last)') input iterators for which the specified
        // 'pred' unary predicate is 'true' to the specified 'result' output
        // iterator, incrementing result after each copied element, keeping the
        // element order stable.  The behavior is undefined if the ranges
        // '[first, last)' and
        // '[result, advance(result, distance(first, last)))' overlap.  The
        // behavior is also undefined if 'pred' attempts to invoke any
        // non-constant functions of its argument.  See also [alg.copy] in the
        // C++11 standard.
# endif  // !BSLS_LIBRARYFEATURES_STDCPP_MSVC
#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    template<class TYPE, class COMPARE>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    const TYPE&
    clamp(const TYPE& value, const TYPE& low, const TYPE& high, COMPARE comp);
        // Return the specified 'value' adjusted so that it is in the range
        // ['low', 'high'), using the specified comparison predicate 'comp'.

    template<class TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    const TYPE&
    clamp(const TYPE& value, const TYPE& low, const TYPE& high);
        // Return the specified 'value' adjusted so that it is in the range
        // ['low', 'high').
#endif

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    template<class FORWARD_ITERATOR, class SEARCHER>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    FORWARD_ITERATOR search(FORWARD_ITERATOR first,
                            FORWARD_ITERATOR last,
                            const SEARCHER&  searcher);
        // Return the position in the specified range '[first, last)' of the
        // first occurrence of the pattern sought by the specified 'searcher'
        // if found, and 'last' otherwise.  See [alg.search].
#endif  //  BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD

}  // close namespace bsl

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // --------------------
                        // struct AlgorithmUtil
                        // --------------------

template <class CONTAINER, class PREDICATE>
inline
typename CONTAINER::size_type
BloombergLP::bslstl::AlgorithmUtil::containerEraseIf(CONTAINER& container,
                                                     PREDICATE  predicate)
{
    typename CONTAINER::size_type oldSize = container.size();
    for (typename CONTAINER::iterator it  = container.begin();
                                      it != container.end();) {
        if (predicate(*it)) {
            it = container.erase(it);
        }
        else {
            ++it;
        }
    }
    return oldSize - container.size();
}

#ifdef BSLSTL_ITERATOR_PROVIDE_SUN_CPP98_FIXES
template <class INPUT_ITERATOR, class TYPE>
inline
typename bsl::iterator_traits<INPUT_ITERATOR>::difference_type
bsl::count(INPUT_ITERATOR first, INPUT_ITERATOR last, const TYPE& value)
{
    typename bsl::iterator_traits<INPUT_ITERATOR>::difference_type ret = 0;
    std::count(first, last, value, ret);
    return ret;
}

template <class INPUT_ITERATOR, class PREDICATE>
inline
typename bsl::iterator_traits<INPUT_ITERATOR>::difference_type
bsl::count_if(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred)
{
    typename bsl::iterator_traits<INPUT_ITERATOR>::difference_type ret = 0;
    std::count_if(first, last, pred, ret);
    return ret;
}
#endif  // BSLSTL_ITERATOR_PROVIDE_SUN_CPP98_FIXES


#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

template <class INPUT_ITERATOR, class PREDICATE>
bool bsl::all_of(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred)
{
    for (; first != last; ++first) {
        if (!pred(*first)) {
            return false;                                             // RETURN
        }
     }
    return true;
}

template <class INPUT_ITERATOR, class PREDICATE>
bool bsl::any_of(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred)
{
    for (; first != last; ++first) {
        if (pred(*first)) {
            return true;                                              // RETURN
        }
    }
    return false;
}

template <class INPUT_ITERATOR, class PREDICATE>
bool bsl::none_of(INPUT_ITERATOR first, INPUT_ITERATOR last, PREDICATE pred)
{
    for (; first != last; ++first) {
        if (pred(*first)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

# ifdef BSLSTL_ALGORITHMWORKAROUND_IMPLEMENTS_COPY_IF
template <class INPUT_ITERATOR, class OUTPUT_ITERATOR, class PREDICATE>
inline OUTPUT_ITERATOR
bsl::copy_if(INPUT_ITERATOR  first,
             INPUT_ITERATOR  last,
             OUTPUT_ITERATOR result,
             PREDICATE       pred)
{
    while(first != last) {
        if (pred(*first)) {
            *result++ = *first;
        }
        ++first;
    }
    return result;
}
# endif  // BSLSTL_ALGORITHMWORKAROUND_IMPLEMENTS_COPY_IF

#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY


#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template<class TYPE, class COMPARE>
BSLS_KEYWORD_CONSTEXPR_CPP14
inline const TYPE&
bsl::clamp(const TYPE& value, const TYPE& low, const TYPE& high, COMPARE comp)
{
    BSLS_ASSERT(!comp(high, low));
    return comp(value, low) ? low : comp(high, value) ? high : value;
}

template<class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14
inline const TYPE&
bsl::clamp(const TYPE& value, const TYPE& low, const TYPE& high)
{
    return bsl::clamp(value, low, high, std::less<TYPE>());
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY


#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
template<class FORWARD_ITERATOR, class SEARCHER>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 FORWARD_ITERATOR
bsl::search(FORWARD_ITERATOR first,
            FORWARD_ITERATOR last,
            const SEARCHER&  searcher)
{
    bsl::pair<FORWARD_ITERATOR, FORWARD_ITERATOR> res = searcher(first, last);
    return res.first;
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD


#endif  // INCLUDED_BSLSTL_ALGORITHM

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
