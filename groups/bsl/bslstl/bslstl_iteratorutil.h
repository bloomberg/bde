// bslstl_iteratorutil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_ITERATORUTIL
#define INCLUDED_BSLSTL_ITERATORUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities operating on iterators and iterator ranges.
//
//@CLASSES:
//  bslstl::IteratorUtil:
//
//@SEE_ALSO: bslstl_hashtable
//
//@DESCRIPTION: This component provides a namespace, `bslstl::IteratorUtil`,
// containing utility functions for iterator types.  In particular, this
// component includes a function `insertDistance` that returns the number of
// elements that should be accounted for when range-inserting in a container,
// given a pair of iterators `a` and `b` describing a half-open range
// `[a .. b)`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Finding the Distance Between Two Random Access Iterators
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to find the number of elements between two random access
// iterators.
//
// First, we create an array of integer values and two pointers (which are
// considered random access iterators) referring to the beginning and end of a
// range within that array:
// ```
// int values[] = { 1, 2, 3, 4, 5 };
// int *begin = &values[0];
// int *end   = &values[3];
// ```
// Now, we use the `IteratorUtil::insertDistance` class method to calculate the
// distance of the open range `[begin .. end)`:
// ```
// std::size_t distance = IteratorUtil::insertDistance(begin, end);
// assert(3 == distance);
// ```

#include <bslscm_version.h>

#include <bslstl_iterator.h>  // iterator tags, distance
#include <bslstl_pair.h>      // pair
#include <bslstl_ranges.h>

#include <bslmf_addconst.h>
#include <bslmf_removeconst.h>

#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslstl {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    template <typename t_ITERATOR>
    concept IteratorUtil_ModernIterator =
        requires { typename t_ITERATOR::iterator_concept; };
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

                // ========================================
                // struct IteratorUtil_LegacyInsertDistance
                // ========================================

/// This supporting utility type is used for type-based dispatch as part of the
/// implementation of the similarly structured functions `insertDistance` and
/// `canCalculateInsertDistance` from `IteratorUtil`.  Selection between the
/// primary template and secondary (much simpler) one is performed based on the
/// defaulted third parameter that checks if the (template parameter) type
/// `t_SENTINEL` is convertible to the (template parameter) type
/// `t_INPUT_ITERATOR`.  When the primary template is selected, the decision
/// to delegate distance calculation to `bsl::distance` is performed using
/// tag dispatch.
template <class t_INPUT_ITERATOR,
          class t_SENTINEL,
          bool  t_MATCHING_SENTINEL =
              bsl::is_convertible<t_SENTINEL, t_INPUT_ITERATOR>::value>
struct IteratorUtil_LegacyInsertDistance
{
private:
    // PRIVATE CLASS METHODS

    /// Return `0` if the last (unnamed) parameter indicates that the
    /// iterator category is not a forward iterator, otherwise return
    /// `bsl::distance(first, last)`.
    static
    typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
    doCalc(const t_INPUT_ITERATOR&,
           const t_INPUT_ITERATOR&,
           const std::input_iterator_tag&);
    static
    typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
    doCalc(const t_INPUT_ITERATOR& first,
           const t_INPUT_ITERATOR& last,
           const std::forward_iterator_tag&);

public:
    // CLASS METHODS

    /// Return `true` if the (template parameter) type `t_INPUT_ITERATOR` has
    /// a forward iterator category and the (template parameter) type
    /// `t_SENTINEL` is convertible to `t_INPUT_ITERATOR`.
    BSLS_KEYWORD_CONSTEXPR static bool canCalculate();

    /// Return the number of elements in the range `[first, last)` if
    /// the (template parameter) type `t_SENTINEL` is convertible to the
    /// (template parameter) type `t_INPUT_ITERATOR` and `t_INPUT_ITERATOR`
    /// has a forward iterator category; otherwise return 0.
    static
    typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
    insertDistance(const t_INPUT_ITERATOR& first, const t_SENTINEL& last);
};

/// This partial specialization handles the case where the sentinel type is not
/// convertible to the iterator type and thus we do not support distance
/// calculation.  Note that this choice primarily applies on legacy platforms
/// or when iterators do not properly model the modern iterator concepts.
template <class t_INPUT_ITERATOR, class t_SENTINEL>
struct IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR, t_SENTINEL, false>
{
public:
    // CLASS METHODS

    BSLS_KEYWORD_CONSTEXPR static bool canCalculate();

    static
    typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
    insertDistance(const t_INPUT_ITERATOR& first, const t_SENTINEL& last);
};

                        // ===================
                        // struct IteratorUtil
                        // ===================

/// This utility struct provides a namespace for functions on iterators and
/// iterator ranges.
struct IteratorUtil {
    /// Return `true` if the calculation of `insertDistance` is supported for
    /// iterators of the specified types, `false` otherwise.
    template <class t_INPUT_ITERATOR, class t_SENTINEL>
    static BSLS_KEYWORD_CONSTEXPR bool canCalculateInsertDistance();

    /// Return 0 if the (template parameter) type `t_INPUT_ITERATOR` is limited
    /// to the standard input-iterator category, otherwise return the number
    /// of elements reachable from the specified `first` to (but not including)
    /// the specified `last`.  This function has a constant-time complexity
    /// if the iterator category of `t_INPUT_ITERATOR` is strictly a standard
    /// input iterator, or is a random access iterator, otherwise it is linear
    /// in the length of the range `[first .. last)`.  The behavior is
    /// undefined unless `last` is reachable from `first`.
    template <class t_INPUT_ITERATOR, class t_SENTINEL>
    static typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
    insertDistance(const t_INPUT_ITERATOR& first,
                   const t_SENTINEL&      last);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    // These template aliases are "convenience aliases" defined in the standard
    // as "exposition-only" to simplify the specification of the deduction
    // guides for the associative containers.

    /// Return the `value_type` of the specified iterator type.
    template <class INPUT_ITER>  // aka iter-val-type
    using IterVal_t = typename bsl::iterator_traits<INPUT_ITER>::value_type;

    /// Return the `key-type` of the specified iterator type, which is
    /// expected to refer to a `pair<KEY_TYPE, MAPPED_TYPE>`.
    template <class INPUT_ITER> // aka iter-key-type
    using IterKey_t = bsl::remove_const_t<
        typename bsl::iterator_traits<INPUT_ITER>::value_type::first_type>;

    /// Return the `mapped-type` of the specified iterator type, which is
    /// expected to refer to a `pair<KEY_TYPE, MAPPED_TYPE>`.
    template <class INPUT_ITER> // aka iter-mapped-type
    using IterMapped_t =
        typename bsl::iterator_traits<INPUT_ITER>::value_type::second_type;

    /// Return the type that is actually stored in a map.  The supplied
    /// iterator type is expected to refer to a
    /// `pair<KEY_TYPE, MAPPED_TYPE>`.
    template <class INPUT_ITER>  // aka iter-to-alloc-type
    using IterToAlloc_t = bsl::pair<
        bsl::add_const_t<
            typename bsl::iterator_traits<INPUT_ITER>::value_type::first_type>,
            typename bsl::iterator_traits<INPUT_ITER>::value_type::second_type
        >;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
};

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // ---------------------------------------
                  // class IteratorUtil_LegacyInsertDistance
                  // ---------------------------------------

template <class t_INPUT_ITERATOR,
          class t_SENTINEL,
          bool  t_MATCHING_SENTINEL>
BSLS_KEYWORD_CONSTEXPR bool
IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR,
                                  t_SENTINEL,
                                  t_MATCHING_SENTINEL>::canCalculate()
{
    typedef typename bsl::iterator_traits<t_INPUT_ITERATOR>::iterator_category
                                                                  IterCategory;
    if BSLS_KEYWORD_CONSTEXPR_CPP17 (
        bsl::is_convertible<IterCategory,
                            bsl::forward_iterator_tag>::value) {
        return true;                                                  // RETURN
    }
    else {
        return false;                                                 // RETURN
    }
}

template <class t_INPUT_ITERATOR,
          class t_SENTINEL,
          bool  t_MATCHING_SENTINEL>
typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR,
                                  t_SENTINEL,
                                  t_MATCHING_SENTINEL>::doCalc(
                                      const t_INPUT_ITERATOR&,
                                      const t_INPUT_ITERATOR&,
                                      const std::input_iterator_tag&)
{
    return 0;
}

template <class t_INPUT_ITERATOR,
          class t_SENTINEL,
          bool  t_MATCHING_SENTINEL>
typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR,
                                  t_SENTINEL,
                                  t_MATCHING_SENTINEL>::doCalc(
                                      const t_INPUT_ITERATOR& first,
                                      const t_INPUT_ITERATOR& last,
                                      const std::forward_iterator_tag&)
{
    return bsl::distance(first, last);
}

template <class t_INPUT_ITERATOR,
          class t_SENTINEL,
          bool  t_MATCHING_SENTINEL>
typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR,
                                  t_SENTINEL,
                                  t_MATCHING_SENTINEL>::insertDistance(
                                      const t_INPUT_ITERATOR& first,
                                      const t_SENTINEL&       last)
{
    typedef typename bsl::iterator_traits<t_INPUT_ITERATOR>::iterator_category
                                                                  IterCategory;
    return doCalc(first,
                  static_cast<const t_INPUT_ITERATOR&>(last),
                  IterCategory());                                    // RETURN
}

template <class t_INPUT_ITERATOR,
          class t_SENTINEL>
BSLS_KEYWORD_CONSTEXPR bool
IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR,
                                  t_SENTINEL,
                                  false>::canCalculate()
{
    return false;                                                     // RETURN
}

template <class t_INPUT_ITERATOR,
          class t_SENTINEL>
typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
IteratorUtil_LegacyInsertDistance<t_INPUT_ITERATOR,
                                  t_SENTINEL,
                                  false>::insertDistance(
                                      const t_INPUT_ITERATOR&,
                                      const t_SENTINEL&)
{
    return 0;                                                         // RETURN
}

                     // ------------------
                     // class IteratorUtil
                     // ------------------

template <class t_INPUT_ITERATOR, class t_SENTINEL>
BSLS_KEYWORD_CONSTEXPR bool IteratorUtil::canCalculateInsertDistance()
{
    // Note that this function, `insertDistance`, and the test function
    // `insertDistanceClassification` should all have the same structure
    // for both correctness and maintainability.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    typedef typename bsl::iterator_traits<t_INPUT_ITERATOR>::iterator_category
                                                                  IterCategory;

    // If we look like a modern iterator, or if the iterator and sentinel
    // types are different, we should see if `ranges::distance` is usable
    // before falling back to legacy approaches.
    if constexpr (IteratorUtil_ModernIterator<t_INPUT_ITERATOR>
               || !bsl::is_convertible_v<t_SENTINEL, t_INPUT_ITERATOR>)
    {
        if constexpr (bsl::input_iterator<t_INPUT_ITERATOR>
                   && bsl::sentinel_for<t_SENTINEL, t_INPUT_ITERATOR>)
        {
            if constexpr (bsl::forward_iterator<t_INPUT_ITERATOR>
                       && bsl::is_convertible_v<IterCategory,
                                                bsl::forward_iterator_tag>) {
                return true;                                          // RETURN
            }
            else {
                return false;                                         // RETURN
            }
        }
        else {
            return false;                                             // RETURN
        }
    }
    else {
#endif

        return IteratorUtil_LegacyInsertDistance<
            t_INPUT_ITERATOR,
            t_SENTINEL>::canCalculate();                              // RETURN

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    }  // close of `if constexpr`
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
}

template <class t_INPUT_ITERATOR, class t_SENTINEL>
typename bsl::iterator_traits<t_INPUT_ITERATOR>::difference_type
IteratorUtil::insertDistance(const t_INPUT_ITERATOR& first,
                             const t_SENTINEL&       last)
{
    // Note that this function, `canCalculateInsertDistance`, and the test
    // function `insertDistanceClassification` should all have the same
    // structure for both correctness and maintainability.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    typedef typename bsl::iterator_traits<t_INPUT_ITERATOR>::iterator_category
                                                                  IterCategory;

    if constexpr (IteratorUtil_ModernIterator<t_INPUT_ITERATOR>
               || !bsl::is_convertible_v<t_SENTINEL, t_INPUT_ITERATOR>)
    {
        // If we can use `ranges::distance` we should.
        if constexpr (bsl::input_iterator<t_INPUT_ITERATOR>
                   && bsl::sentinel_for<t_SENTINEL, t_INPUT_ITERATOR>)
        {
            // The iterator meets the requirements for calling
            // `bsl::ranges::distance`, but we only want to check that distance
            // if the iterator in question is a forward iterator.  A C++20
            // iterator will have an iterator category that looks like a
            // forward iterator if it is copyable, even if its
            // `iterator_concept` is `input_iterator_tag`.  A classic C++17
            // forward iterator might not meet the requirements of being a
            // C++20 forward iterator for seemingly silly reasons like a
            // mismatched return type on `operator++`.  To minimize impact on
            // legacy iterators, we therefore fall back to `bsl::distance` as
            // long as the iterator doesn't have a nested `iterator_concept`
            // member.
            if constexpr (bsl::forward_iterator<t_INPUT_ITERATOR>
                       && bsl::is_convertible_v<IterCategory,
                                                bsl::forward_iterator_tag>) {
                return bsl::ranges::distance(first, last);            // RETURN
            }
            else {
                return 0;                                             // RETURN
            }
        }
        else {
            return 0;                                                 // RETURN
        }
    }
    else {
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

        return IteratorUtil_LegacyInsertDistance<
            t_INPUT_ITERATOR,
            t_SENTINEL>::insertDistance(first, last);                 // RETURN

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
    }  // close of `if constexpr`
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
