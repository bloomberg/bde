// bsl_execution.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_EXECUTION
#define INCLUDED_BSL_EXECUTION

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
#include <bsls_nativestd.h>

#if BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS

#include <execution>

namespace bsl {

    using native_std::is_execution_policy;

    namespace execution {
        // Import selected symbols into bsl namespace
        using native_std::execution::par;
        using native_std::execution::par_unseq;
        using native_std::execution::parallel_policy;
        using native_std::execution::parallel_unsequenced_policy;
        using native_std::execution::seq;
        using native_std::execution::sequenced_policy;
    }  // close execution namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool is_execution_policy_v =
                       native_std::is_execution_policy<TYPE>::value;

#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
