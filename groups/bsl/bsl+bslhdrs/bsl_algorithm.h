// bsl_algorithm.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_ALGORITHM
#define INCLUDED_BSL_ALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <algorithm>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

    // Import selected symbols into bsl namespace
    using std::adjacent_find;
    using std::binary_search;
    using std::copy;
    using std::copy_backward;

    // 'count' and 'count_if' are provided in 'bslstl_algorithmworkaround' in
    // order to work-around the Sun standard library, libCstd.
    //
    // using std::count;
    // using std::count_if;

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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::clamp;
    using std::for_each_n;
    using std::sample;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Import additional names expected by existing code, but not mandated by
    // the standard header.
    using std::advance;
    using std::bad_alloc;
    using std::bidirectional_iterator_tag;
    using std::forward_iterator_tag;
    using std::get_temporary_buffer;
    using std::input_iterator_tag;
    using std::iterator;
    using std::new_handler;
    using std::nothrow;
    using std::nothrow_t;
    using std::output_iterator_tag;
    using std::random_access_iterator_tag;
    using std::return_temporary_buffer;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#include <bslstl_algorithm.h>
#include <bslstl_algorithmworkaround.h>

#ifndef BDE_OPENSOURCE_PUBLICATION // STP
#include <bslstp_exalgorithm.h>
#endif  // BDE_OPENSOURCE_PUBLICATION -- STP

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
