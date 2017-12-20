// bsl_ratio.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_RATIO
#define INCLUDED_BSL_RATIO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <ratio>

namespace bsl {

    using native_std::ratio;
    using native_std::ratio_add;
    using native_std::ratio_subtract;
    using native_std::ratio_multiply;
    using native_std::ratio_divide;
    using native_std::ratio_equal;
    using native_std::ratio_not_equal;
    using native_std::ratio_less;
    using native_std::ratio_less_equal;
    using native_std::ratio_greater;
    using native_std::ratio_greater_equal;
    using native_std::atto;
    using native_std::femto;
    using native_std::pico;
    using native_std::nano;
    using native_std::micro;
    using native_std::milli;
    using native_std::centi;
    using native_std::deci;
    using native_std::deca;
    using native_std::hecto;
    using native_std::kilo;
    using native_std::mega;
    using native_std::giga;
    using native_std::tera;
    using native_std::peta;
    using native_std::exa;
#ifdef BSL_RATIO_SUPPORTS_EXTENDED_SI_TYPEDEFS
    using native_std::yocto;
    using native_std::zepto;
    using native_std::zetta;
    using native_std::yotta;
#endif

}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
