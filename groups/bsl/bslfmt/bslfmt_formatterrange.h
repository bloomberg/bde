// bslfmt_formatterrange.h                                            -*-C++-*-
#ifndef INCLUDED_BSLFMT_FORMATTERRANGE
#define INCLUDED_BSLFMT_FORMATTERRANGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for ranges.
//
//@CLASSES:
//  bsl::formatter<t_TYPE[N], t_CHAR>: specialization for C arrays
//  bsl::formatter<bsl::array<t_TYPE, N>, t_CHAR>: specialization for `array`
//  bsl::formatter<bsl::deque, t_CHAR>: specialization for `deque`
//  bsl::formatter<bsl::list, t_CHAR>: specialization for `list`
//  bsl::formatter<bsl::map, t_CHAR>: specialization for `map`
//  bsl::formatter<bsl::multimap, t_CHAR>: specialization for `multimap`
//  bsl::formatter<bsl::multiset, t_CHAR>: specialization for `multiset`
//  bsl::formatter<bsl::set, t_CHAR>: specialization for `set`
//  bsl::formatter<bsl::span, t_CHAR>: specialization for `span`
//  bsl::formatter<bsl::unordered_map, t_CHAR>: for `unordered_map`
//  bsl::formatter<bsl::unordered_multimap, t_CHAR>: for `unordered_multimap`
//  bsl::formatter<bsl::unordered_multiset, t_CHAR>: for `unordered_multiset`
//  bsl::formatter<bsl::unordered_set, t_CHAR>: for `unordered_set`
//  bsl::formatter<bsl::vector, t_CHAR>: specialization for `vector`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@SEE_ALSO: bslfmt_formatterpair, bslfmt_rangeformatter
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` for the standard container types, implementing the range
// formatting of [format.range.formatter].  The punctuation depends on the kind
// of container:
// ```
//  sequences (vector, deque, list, array, span, C arrays)  ->  [v1, v2]
//  sets (set, multiset, unordered_set, unordered_multiset) ->  {v1, v2}
//  maps (map, multimap, unordered_map, unordered_multimap) ->  {k1: v1}
// ```
// Elements are rendered in the escaped ("debug") presentation by default, so a
// container of strings appears as `["a", "b"]` rather than as `[a, b]`.
//
// Note that `bsl::basic_string` and `bsl::basic_string_view` are not covered
// here: their formatters, provided by `bslfmt_formatterstring`, render them as
// text rather than as a sequence of characters.
//
// This header is not intended to be included directly.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Formatting A Container
///- - - - - - - - - - - - - - - - -
// Suppose we wish to log the contents of a container.
//
// First, we create a vector of strings:
// ```
//  bsl::vector<bsl::string> tickers;
//  tickers.push_back("IBM");
//  tickers.push_back("MSFT");
// ```
// Now, we render it with its default format:
// ```
//  bslfmt::MockParseContext<char> parseContext("}", 0);
//
//  bsl::formatter<bsl::vector<bsl::string>, char> formatter;
//  formatter.parse(parseContext);
//
//  bslfmt::MockFormatContext<char> formatContext(0);
//  formatContext.advance_to(formatter.format(tickers, formatContext));
// ```
// Finally, we observe that the container is bracketed and comma-separated, and
// that its elements are quoted:
// ```
//  assert("[\"IBM\", \"MSFT\"]" == formatContext.finalString());
// ```

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#include <bslfmt_formatterbase.h>
#include <bslfmt_rangeformatter.h>

#include <bslstl_array.h>
#include <bslstl_deque.h>
#include <bslstl_list.h>
#include <bslstl_map.h>
#include <bslstl_multimap.h>
#include <bslstl_multiset.h>
#include <bslstl_set.h>
#include <bslstl_span.h>
#include <bslstl_unorderedmap.h>
#include <bslstl_unorderedmultimap.h>
#include <bslstl_unorderedmultiset.h>
#include <bslstl_unorderedset.h>
#include <bslstl_vector.h>

#include <cstddef>  // `std::size_t`

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// Partial specialization of the `bsl::formatter` template for C arrays.
template <class t_TYPE, std::size_t k_SIZE, class t_CHAR>
struct formatter<t_TYPE[k_SIZE], t_CHAR>
                       : BloombergLP::bslfmt::range_formatter<t_TYPE, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::array`.
template <class t_TYPE, std::size_t k_SIZE, class t_CHAR>
struct formatter<bsl::array<t_TYPE, k_SIZE>, t_CHAR>
                       : BloombergLP::bslfmt::range_formatter<t_TYPE, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::deque`.
template <class t_TYPE, class t_ALLOCATOR, class t_CHAR>
struct formatter<bsl::deque<t_TYPE, t_ALLOCATOR>, t_CHAR>
                       : BloombergLP::bslfmt::range_formatter<t_TYPE, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::list`.
template <class t_TYPE, class t_ALLOCATOR, class t_CHAR>
struct formatter<bsl::list<t_TYPE, t_ALLOCATOR>, t_CHAR>
                       : BloombergLP::bslfmt::range_formatter<t_TYPE, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::map`.
template <class t_KEY,
          class t_VALUE,
          class t_COMPARATOR,
          class t_ALLOCATOR,
          class t_CHAR>
struct formatter<bsl::map<t_KEY, t_VALUE, t_COMPARATOR, t_ALLOCATOR>, t_CHAR>
    : BloombergLP::bslfmt::MapLikeFormatter<typename
               bsl::map<t_KEY, t_VALUE, t_COMPARATOR, t_ALLOCATOR>::value_type,
               t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for
/// `bsl::multimap`.
template <class t_KEY,
          class t_VALUE,
          class t_COMPARATOR,
          class t_ALLOCATOR,
          class t_CHAR>
struct formatter<bsl::multimap<t_KEY, t_VALUE, t_COMPARATOR, t_ALLOCATOR>,
                 t_CHAR>
    : BloombergLP::bslfmt::MapLikeFormatter<typename
          bsl::multimap<t_KEY, t_VALUE, t_COMPARATOR, t_ALLOCATOR>::value_type,
          t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for
/// `bsl::multiset`.
template <class t_KEY, class t_COMPARATOR, class t_ALLOCATOR, class t_CHAR>
struct formatter<bsl::multiset<t_KEY, t_COMPARATOR, t_ALLOCATOR>, t_CHAR>
    : BloombergLP::bslfmt::SetLikeFormatter<typename
                   bsl::multiset<t_KEY, t_COMPARATOR, t_ALLOCATOR>::value_type,
                   t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::set`.
template <class t_KEY, class t_COMPARATOR, class t_ALLOCATOR, class t_CHAR>
struct formatter<bsl::set<t_KEY, t_COMPARATOR, t_ALLOCATOR>, t_CHAR>
    : BloombergLP::bslfmt::SetLikeFormatter<typename
                        bsl::set<t_KEY, t_COMPARATOR, t_ALLOCATOR>::value_type,
                        t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::span`.
template <class t_TYPE, std::size_t k_EXTENT, class t_CHAR>
struct formatter<bsl::span<t_TYPE, k_EXTENT>, t_CHAR>
                       : BloombergLP::bslfmt::range_formatter<t_TYPE, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for
/// `bsl::unordered_map`.
template <class t_KEY,
          class t_VALUE,
          class t_HASH,
          class t_EQUAL,
          class t_ALLOCATOR,
          class t_CHAR>
struct formatter<bsl::unordered_map<t_KEY,
                                    t_VALUE,
                                    t_HASH,
                                    t_EQUAL,
                                    t_ALLOCATOR>,
                 t_CHAR>
    : BloombergLP::bslfmt::MapLikeFormatter<typename
           bsl::unordered_map<t_KEY,
                              t_VALUE,
                              t_HASH,
                              t_EQUAL,
                              t_ALLOCATOR>::value_type,
           t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for
/// `bsl::unordered_multimap`.
template <class t_KEY,
          class t_VALUE,
          class t_HASH,
          class t_EQUAL,
          class t_ALLOCATOR,
          class t_CHAR>
struct formatter<bsl::unordered_multimap<t_KEY,
                                         t_VALUE,
                                         t_HASH,
                                         t_EQUAL,
                                         t_ALLOCATOR>,
                 t_CHAR>
    : BloombergLP::bslfmt::MapLikeFormatter<typename
           bsl::unordered_multimap<t_KEY,
                                   t_VALUE,
                                   t_HASH,
                                   t_EQUAL,
                                   t_ALLOCATOR>::value_type,
           t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for
/// `bsl::unordered_multiset`.
template <class t_KEY,
          class t_HASH,
          class t_EQUAL,
          class t_ALLOCATOR,
          class t_CHAR>
struct formatter<bsl::unordered_multiset<t_KEY, t_HASH, t_EQUAL, t_ALLOCATOR>,
                 t_CHAR>
    : BloombergLP::bslfmt::SetLikeFormatter<typename
           bsl::unordered_multiset<t_KEY,
                                   t_HASH,
                                   t_EQUAL,
                                   t_ALLOCATOR>::value_type,
           t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for
/// `bsl::unordered_set`.
template <class t_KEY,
          class t_HASH,
          class t_EQUAL,
          class t_ALLOCATOR,
          class t_CHAR>
struct formatter<bsl::unordered_set<t_KEY, t_HASH, t_EQUAL, t_ALLOCATOR>,
                 t_CHAR>
    : BloombergLP::bslfmt::SetLikeFormatter<typename
           bsl::unordered_set<t_KEY, t_HASH, t_EQUAL, t_ALLOCATOR>::value_type,
           t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for `bsl::vector`.
template <class t_TYPE, class t_ALLOCATOR, class t_CHAR>
struct formatter<bsl::vector<t_TYPE, t_ALLOCATOR>, t_CHAR>
                       : BloombergLP::bslfmt::range_formatter<t_TYPE, t_CHAR> {
};

}  // close namespace bsl

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
