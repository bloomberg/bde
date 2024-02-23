// bslmf_negation.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_NEGATION
#define INCLUDED_BSLMF_NEGATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the logical negation (NOT) for type traits.
//
//@CLASSES:
//  bsl::negation: forms the logical NOT of the specified type trait
//  bsl::negation_v: the result value of the 'negation' meta-function
//
//@DESCRIPTION: This component makes available the functionality provided by
// the 'std::negation' meta-function in all C++ language modes -
// 'std::negation' is available only starting from C++17.

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

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

using std::negation;
using std::negation_v;

#else

                         // ===============
                         // struct negation
                         // ===============

template <class B>
struct negation : integral_constant<bool, !bool(B::value)> {
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class B>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool negation_v = negation<B>::value;
#endif

#endif

}  // close namespace bsl

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
