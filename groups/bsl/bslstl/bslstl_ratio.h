// bslstl_ratio.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_RATIO
#define INCLUDED_BSLSTL_RATIO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@CANONICAL_HEADER: bsl_ratio.h
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <ratio>

namespace bsl {

    using std::ratio;
    using std::ratio_add;
    using std::ratio_subtract;
    using std::ratio_multiply;
    using std::ratio_divide;
    using std::ratio_equal;
    using std::ratio_not_equal;
    using std::ratio_less;
    using std::ratio_less_equal;
    using std::ratio_greater;
    using std::ratio_greater_equal;
    using std::atto;
    using std::femto;
    using std::pico;
    using std::nano;
    using std::micro;
    using std::milli;
    using std::centi;
    using std::deci;
    using std::deca;
    using std::hecto;
    using std::kilo;
    using std::mega;
    using std::giga;
    using std::tera;
    using std::peta;
    using std::exa;
#ifdef BSL_RATIO_SUPPORTS_EXTENDED_SI_TYPEDEFS
    using std::yocto;
    using std::zepto;
    using std::zetta;
    using std::yotta;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::ratio_equal_v;
    using std::ratio_not_equal_v;
    using std::ratio_less_v;
    using std::ratio_less_equal_v;
    using std::ratio_greater_v;
    using std::ratio_greater_equal_v;
#elif defined BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    template <class R1, class R2> constexpr bool ratio_equal_v
                                                  = ratio_equal<R1, R2>::value;
        // This template variable represents the result value of the
        // 'std::ratio_equal' meta-function.

    template <class R1, class R2> constexpr bool ratio_not_equal_v
                                              = ratio_not_equal<R1, R2>::value;
        // This template variable represents the result value of the
        // 'std::ratio_not_equal' meta-function.

    template <class R1, class R2> constexpr bool ratio_less_v
                                                   = ratio_less<R1, R2>::value;
        // This template variable represents the result value of the
        // 'std::ratio_less' meta-function.

    template <class R1, class R2> constexpr bool ratio_less_equal_v
                                             = ratio_less_equal<R1, R2>::value;
        // This template variable represents the result value of the
        // 'std::ratio_less_equal' meta-function.

    template <class R1, class R2> constexpr bool ratio_greater_v
                                                = ratio_greater<R1, R2>::value;
        // This template variable represents the result value of the
        // 'std::ratio_greater' meta-function.

    template <class R1, class R2> constexpr bool ratio_greater_equal_v
                                          = ratio_greater_equal<R1, R2>::value;
        // This template variable represents the result value of the
        // 'std::ratio_greater_equal' meta-function.

#endif
}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
