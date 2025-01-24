// bslmf_disjunction.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_DISJUNCTION
#define INCLUDED_BSLMF_DISJUNCTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the logical disjunction (OR) for type traits.
//
//@CLASSES:
//  bsl::disjunction: forms the logical OR of the specified type traits
//  bsl::disjunction_v: the result value of the `disjunction` meta-function
//
//@DESCRIPTION: This component makes available the functionality provided by
// the `std::disjunction` meta-function in all C++ language modes -
// `std::disjunction` is available only starting from C++17.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <type_traits>
#else
#include <bslmf_conditional.h>
#include <bslmf_integralconstant.h>
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Mon Jan 13 08:31:25 2025
// Command line: sim_cpp11_features.pl bslmf_disjunction.h

# define COMPILING_BSLMF_DISJUNCTION_H
# include <bslmf_disjunction_cpp03.h>
# undef COMPILING_BSLMF_DISJUNCTION_H

// clang-format on
#else

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

using std::disjunction;
using std::disjunction_v;

#else

                         // ==================
                         // struct disjunction
                         // ==================

// forward declaration (required for C++03)
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=10
template <class... B>
struct disjunction;
#endif

// 0 args specialization
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <class...>
struct disjunction : false_type {
};
#else
template <>
struct disjunction<> : false_type {
};
#endif

// 1 arg specialization
template <class B1>
struct disjunction<B1> : B1 {
};

// other cases
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $local-var-args=8
template <class B1, class B2, class... Bn>
struct disjunction<B1, B2, Bn...> :
    conditional<bool(B1::value), B1, disjunction<B2, Bn...> >::type {
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class... B>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool disjunction_v = disjunction<B...>::value;
#endif

#endif

}  // close namespace bsl

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
