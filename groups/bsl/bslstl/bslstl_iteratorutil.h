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
//@DESCRIPTION: This component provides a namespace, 'bslstl::IteratorUtil',
// containing utility functions for iterator types.  In particular, this
// component includes a function 'insertDistance' that returns the number of
// elements that should be accounted for when range-inserting in a container,
// given a pair of iterator 'a' and 'b' describing a half-open range
// '[a .. b)'.
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
//..
//  int values[] = { 1, 2, 3, 4, 5 };
//  int *begin = &values[0];
//  int *end   = &values[3];
//..
// Now, we use the 'IteratorUtil::insertDistance' class method to calculate the
// distance of the open range '[begin .. end)':
//..
//  std::size_t distance = IteratorUtil::insertDistance(begin, end);
//  assert(3 == distance);
//..

#include <bslscm_version.h>

#include <bslstl_iterator.h>  // iterator tags, distance
#include <bslstl_pair.h>      // pair

#include <bslmf_addconst.h>
#include <bslmf_removeconst.h>

#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslstl {

                        // ===================
                        // struct IteratorUtil
                        // ===================

struct IteratorUtil {
    // This utility struct provides a namespace for functions on iterators and
    // iterator ranges.

    template <class InputIterator>
    static typename bsl::iterator_traits<InputIterator>::difference_type
    insertDistance(InputIterator first, InputIterator last);
        // Return 0 if the (template parameter) type 'InputIterator' is limited
        // to the standard input-iterator category, otherwise return the number
        // of elements that is reachable from the specified 'first' to (but not
        // including) the specified 'last'.  This function has a constant-time
        // complexity if the iterator category of 'InputIterator' is a strictly
        // a standard input iterator, or is a random access iterator, otherwise
        // it is linear in the length of the range '[first .. last)'.  The
        // behavior is undefined unless 'last' is reachable from 'first'.  Note
        // that this function always returns 0 when compiled with the Sun
        // compiler, while we work around issues in the Sun standard library.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    // These template aliases are 'convenience alises' defined in the standard
    // as 'exposition-only' to simplify the specification of the deduction
    // guides for the associative containers.

    template <class INPUT_ITER>  // aka iter-val-type
    using IterVal_t = typename bsl::iterator_traits<INPUT_ITER>::value_type;
        // returns the 'value_type' of the specified iterator type.

    template <class INPUT_ITER> // aka iter-key-type
    using IterKey_t = bsl::remove_const_t<
        typename bsl::iterator_traits<INPUT_ITER>::value_type::first_type>;
        // returns the 'key-type' of the specified iterator type, which is
        // expected to refer to a 'pair<KEY_TYPE, MAPPED_TYPE>'

    template <class INPUT_ITER> // aka iter-mapped-type
    using IterMapped_t =
        typename bsl::iterator_traits<INPUT_ITER>::value_type::second_type;
        // returns the 'mapped-type' of the specified iterator type, which is
        // expected to refer to a 'pair<KEY_TYPE, MAPPED_TYPE>'

    template <class INPUT_ITER>  // aka iter-to-alloc-type
    using IterToAlloc_t = bsl::pair<
        bsl::add_const_t<
            typename bsl::iterator_traits<INPUT_ITER>::value_type::first_type>,
            typename bsl::iterator_traits<INPUT_ITER>::value_type::second_type
        >;
        // returns the type that is actually stored in a map.  The supplied
        // iterator type is expected to refer to a
        // 'pair<KEY_TYPE, MAPPED_TYPE>'.
#endif

};

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------
                    // class IteratorUtil
                    // ------------------

template <class InputIterator>
typename bsl::iterator_traits<InputIterator>::difference_type
IteratorUtil::insertDistance(InputIterator first, InputIterator last)
{
    struct impl {
        // This local class provides a utility to estimate the maximum
        // number of elements that may be inserted by a range-insert
        // operation on a standard container, by performing tag dispatch
        // on the iterator's category type.

        static
        typename bsl::iterator_traits<InputIterator>::difference_type
        calc(InputIterator, InputIterator, std::input_iterator_tag)
        {
            return 0;
        }

        static
        typename bsl::iterator_traits<InputIterator>::difference_type
        calc(InputIterator first,
             InputIterator last,
             std::forward_iterator_tag)
        {
            return bsl::distance(first, last);
        }
    };

    typedef typename bsl::iterator_traits<InputIterator>::iterator_category
                                                                  IterCategory;
    return impl::calc(first, last, IterCategory());
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
