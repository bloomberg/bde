// bslmf_booleantestable.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_BOOLEANTESTABLE
#define INCLUDED_BSLMF_BOOLEANTESTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exposition-only concept 'boolean-testable'.
//
//@CLASSES:
//  bslmf::BooleanTestable: standard exposition-only 'boolean-testable' concept
//
//@DESCRIPTION: This component provides a concept, 'bslmf::BooleanTestable',
// that specifies the requirements on expressions that are convertible to
// 'bool' and for which the logical operators have the conventional semantics.
// For more details, please refer to [concept.booleantestable] in the ISO C++20
// Standard.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Concept for a Predicate (Boolean) Expression
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use 'bslmf::BooleanTestable' to implement a
// concept to determine whether applying the '<' operator to a given type
// produces an 'if'-testable result.
//..
//  template <class t_TYPE>
//  concept LessComparable =
//      requires(t_TYPE v) {
//          { v < v } -> bslmf::BooleanTestable;
//      };
//..
// Now types can be tested using this concept:
//..
//  static_assert(LessComparable<int>);
//
//  struct NonLessComparable { void operator<(NonLessComparable) {} };
//  static_assert(!LessComparable<NonLessComparable>);
//..

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

#include <concepts>

namespace BloombergLP {
namespace bslmf {

template <class t_TYPE>
concept BooleanTestable_Impl = std::convertible_to<t_TYPE, bool>;

template <class t_TYPE>
concept BooleanTestable =
    // Specify the requirements on expressions that are convertible to 'bool'
    // and for which the logical operators have the conventional semantics.
    // Note that this is an implementation of the exposition-only concept
    // 'boolean-testable' defined by the ISO C++20 Standard
    // [concept.booleantestable].
    BooleanTestable_Impl<t_TYPE> &&
    requires(t_TYPE&& t) {
        { !static_cast<t_TYPE&&>(t) } -> BooleanTestable_Impl;
    };

}  // close package namespace
}  // close enterprise namespace
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
