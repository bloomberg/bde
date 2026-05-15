// bslstl_concepts.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_CONCEPTS
#define INCLUDED_BSLSTL_CONCEPTS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@CANONICAL_HEADER: bsl_concepts.h
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.

#include <bsls_libraryfeatures.h>

/// To support code that worked with earlier releases of BDE, we test directly
/// for the presence of the native platform library concepts rather than for
/// the `BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS` macro that has a dependency
/// on `BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE`.  The library features header
/// performs the necessary include magic to ensure that the native library
/// feature macros are available so we deliberately avoid repeating that
/// non-trivial logic here, which is technically relying on a transitive
/// header dependency.
#if defined(__cpp_lib_concepts) && __cpp_lib_concepts >= 202002L

#include <concepts>

namespace bsl {
    // Import selected symbols into bsl namespace

    // language-related concepts
    using std::same_as;
    using std::derived_from;
    using std::convertible_to;
    using std::common_reference_with;
    using std::common_with;
    using std::integral;
    using std::signed_integral;
    using std::unsigned_integral;
    using std::floating_point;
    using std::assignable_from;
    using std::swappable;
    using std::swappable_with;
    using std::destructible;
    using std::constructible_from;
    using std::default_initializable;
    using std::move_constructible;
    using std::copy_constructible;

    // comparison concepts
    using std::equality_comparable;
    using std::equality_comparable_with;
    using std::totally_ordered;
    using std::totally_ordered_with;

    // object concepts
    using std::movable;
    using std::copyable;
    using std::semiregular;
    using std::regular;

    // callable concepts
    using std::invocable;
    using std::regular_invocable;
    using std::predicate;
    using std::relation;
    using std::equivalence_relation;
    using std::strict_weak_order;

}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
