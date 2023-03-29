// bslstl_compare.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_COMPARE
#define INCLUDED_BSLSTL_COMPARE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_compilerfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
#include <compare>

namespace bsl {
// Import selected symbols into bsl namespace

// Types returned by 'operator<=>'
using std::strong_ordering;
using std::weak_ordering;
using std::partial_ordering;

// Concepts
using std::three_way_comparable;
using std::three_way_comparable_with;

// Meta-functions
using std::common_comparison_category;
using std::common_comparison_category_t;
using std::compare_three_way_result;
using std::compare_three_way_result_t;

// Customization point objects
using std::strong_order;
using std::weak_order;
using std::partial_order;
using std::compare_strong_order_fallback;
using std::compare_weak_order_fallback;
using std::compare_partial_order_fallback;

// Functors
using std::compare_three_way;

// Utility functions
using std::is_eq;
using std::is_neq;
using std::is_lt;
using std::is_lteq;
using std::is_gt;
using std::is_gteq;

}  // close package namespace

#endif // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

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
