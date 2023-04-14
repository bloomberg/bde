// bsl_utility.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_UTILITY
#define INCLUDED_BSL_UTILITY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bslmf_integersequence.h>
#include <bslmf_makeintegersequence.h>

#include <bsls_libraryfeatures.h>

#include <bslstl_inplace.h>
#include <bslstl_utility.h>

#include <utility>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace.  Note that 'pair' is
    // provided by 'bslstl_pair.h' (but 'make_pair' currently is not).

    using std::make_pair;

    namespace rel_ops = std::rel_ops;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::declval;
    using std::forward;
    using std::move;
    using std::move_if_noexcept;
    using std::swap;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    using std::piecewise_construct;
    using std::piecewise_construct_t;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    using std::exchange;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    using std::integer_sequence;
    using std::index_sequence;
    using std::make_integer_sequence;
    using std::make_index_sequence;
    using std::index_sequence_for;
#else
    template <class T, T ...INTS>
    using integer_sequence = BloombergLP::bslmf::IntegerSequence<T, INTS...>;

    template <std::size_t... INTS>
    using index_sequence = integer_sequence<std::size_t, INTS...>;

    template <class T, T N>
    using make_integer_sequence =
                                 BloombergLP::bslmf::MakeIntegerSequence<T, N>;

    template <std::size_t N>
    using make_index_sequence = make_integer_sequence<std::size_t, N>;

    template <class... T>
    using index_sequence_for = make_index_sequence<sizeof...(T)>;
#endif
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    using std::cmp_equal;
    using std::cmp_not_equal;
    using std::cmp_less;
    using std::cmp_greater;
    using std::cmp_less_equal;
    using std::cmp_greater_equal;
    using std::in_range;
#endif
}  // close package namespace

// Include Bloomberg's implementation.
#include <bslstl_pair.h>

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
