// bsl_concepts.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_CONCEPTS
#define INCLUDED_BSL_CONCEPTS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.

#include <bsls_libraryfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

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
