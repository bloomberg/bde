// bsl_ranges.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_RANGES
#define INCLUDED_BSL_RANGES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

#include <ranges>

namespace bsl {

namespace ranges {

    // Range access
    using std::ranges::begin;
    using std::ranges::end;
    using std::ranges::cbegin;
    using std::ranges::cend;
    using std::ranges::rbegin;
    using std::ranges::rend;
    using std::ranges::crbegin;
    using std::ranges::crend;
    using std::ranges::size;
    using std::ranges::ssize;
    using std::ranges::empty;
    using std::ranges::data;
    using std::ranges::cdata;

    // Range primitives
    using std::ranges::iterator_t;
    using std::ranges::sentinel_t;
    using std::ranges::range_size_t;
    using std::ranges::range_difference_t;
    using std::ranges::range_value_t;
    using std::ranges::range_reference_t;
    using std::ranges::range_rvalue_reference_t;

    // Not yet added
    // https://cplusplus.github.io/LWG/issue386
    // using std::ranges::range_common_reference_t;

    // Range concepts
    using std::ranges::range;
    using std::ranges::borrowed_range;
    using std::ranges::enable_borrowed_range;
    using std::ranges::sized_range;
    using std::ranges::disable_sized_range;
    using std::ranges::view;
    using std::ranges::enable_view;
    using std::ranges::view_base;

    using std::ranges::input_range;
    using std::ranges::output_range;
    using std::ranges::forward_range;
    using std::ranges::bidirectional_range;
    using std::ranges::random_access_range;
    using std::ranges::contiguous_range;
    using std::ranges::common_range;
    using std::ranges::viewable_range;

    // Views
    using std::ranges::view_interface;
    using std::ranges::subrange;

    // Dangling iterator handling
    using std::ranges::dangling;
    using std::ranges::borrowed_iterator_t;
    using std::ranges::borrowed_subrange_t;

    // Factories
    using std::ranges::empty_view;
    using std::ranges::single_view;
    using std::ranges::iota_view;
    using std::ranges::basic_istream_view;
    using std::ranges::istream_view;
    using std::ranges::wistream_view;

namespace views {

    using std::ranges::views::empty;
    using std::ranges::views::single;
    using std::ranges::views::iota;
    using std::ranges::views::istream;

}  // close namespace views

    // Adaptors
    using std::ranges::ref_view;
    using std::ranges::owning_view;
    using std::ranges::filter_view;
    using std::ranges::transform_view;
    using std::ranges::take_view;
    using std::ranges::take_while_view;
    using std::ranges::drop_view;
    using std::ranges::drop_while_view;
    using std::ranges::join_view;
    using std::ranges::lazy_split_view;
    using std::ranges::split_view;
    using std::ranges::common_view;
    using std::ranges::reverse_view;
    using std::ranges::elements_view;
    using std::ranges::keys_view;
    using std::ranges::values_view;

namespace views {

    using std::ranges::views::all_t;
    using std::ranges::views::all;
    using std::ranges::views::counted;
    using std::ranges::views::filter;
    using std::ranges::views::transform;
    using std::ranges::views::take;
    using std::ranges::views::take_while;
    using std::ranges::views::drop;
    using std::ranges::views::drop_while;
    using std::ranges::views::join;
    using std::ranges::views::lazy_split;
    using std::ranges::views::split;
    using std::ranges::views::common;
    using std::ranges::views::reverse;
    using std::ranges::views::elements;
    using std::ranges::views::keys;
    using std::ranges::views::values;

}  // close namespace views

    // Enumerations
    using std::ranges::subrange_kind;

}  // close namespace ranges

    // Namespace alias
    namespace views = ranges::views;

}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
