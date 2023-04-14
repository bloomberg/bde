// bsl_functional.h                                                   -*-C++-*-
#ifndef INCLUDED_BSL_FUNCTIONAL
#define INCLUDED_BSL_FUNCTIONAL

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
#include <bsls_platform.h>

#include <functional>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

    // Import selected symbols into bsl namespace
    using std::divides;
    using std::greater;
    using std::greater_equal;
    using std::less;
    using std::less_equal;
    using std::logical_and;
    using std::logical_not;
    using std::logical_or;
    using std::minus;
    using std::modulus;
    using std::multiplies;
    using std::negate;
    using std::not_equal_to;
    using std::plus;

#if ! defined BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    // These names are removed by C++17
    using std::binary_function;
    using std::bind1st;
    using std::bind2nd;
    using std::binder1st;
    using std::binder2nd;
    using std::const_mem_fun1_ref_t;
    using std::const_mem_fun1_t;
    using std::const_mem_fun_ref_t;
    using std::const_mem_fun_t;
    using std::mem_fun1_ref_t;
    using std::mem_fun1_t;
    using std::mem_fun;
    using std::mem_fun_ref;
    using std::mem_fun_ref_t;
    using std::mem_fun_t;
    using std::pointer_to_binary_function;
    using std::pointer_to_unary_function;
    using std::ptr_fun;
    using std::unary_function;
#endif

#if ! defined BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED
    // These names are removed by C++20
    using std::binary_negate;
    using std::not1;
    using std::not2;
    using std::unary_negate;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    namespace placeholders = std::placeholders;

    using std::bind;
    using std::bit_and;
    using std::bit_or;
    using std::bit_xor;
    using std::is_bind_expression;
    using std::is_placeholder;
    using std::mem_fn;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    using std::bit_not;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::invoke;
    using std::is_bind_expression_v;
    using std::is_placeholder_v;
    using std::not_fn;
#elif defined BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    template <class TYPE>
    constexpr bool is_bind_expression_v =
                                   std::is_bind_expression<TYPE>::value;
        // This template variable represents the result value of the
        // 'std::is_bind_expression' meta-function.

    template <class TYPE>
    constexpr bool is_placeholder_v = std::is_placeholder<TYPE>::value;
        // This template variable represents the result value of the
        // 'std::is_placeholder' meta-function.

#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
    using std::default_searcher;
    using std::boyer_moore_searcher;
    using std::boyer_moore_horspool_searcher;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    using std::bind_front;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

}  // close package namespace

// Include Bloomberg's implementation.
#ifndef BDE_OPENSOURCE_PUBLICATION // STP
#include <bslstp_exfunctional.h>
#endif  // BDE_OPENSOURCE_PUBLICATION -- STP
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#include <bslstl_defaultsearcher.h>
#include <bslstl_boyermoorehorspoolsearcher.h>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#include <bslstl_equalto.h>
#include <bslstl_function.h>
#include <bslstl_hash.h>
#include <bslstl_referencewrapper.h>


#endif // INCLUDED_BSL_FUNCTIONAL

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
