// bsl_cmath.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_CMATH
#define INCLUDED_BSL_CMATH

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

#ifndef INCLUDED_BSLS_LIBRARYFEATURES
#include <bsls_libraryfeatures.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <cmath>


namespace bsl {
    // Import selected symbols into bsl namespace

    using native_std::abs;
    using native_std::acos;
    using native_std::asin;
    using native_std::atan;
    using native_std::atan2;
    using native_std::ceil;
    using native_std::cos;
    using native_std::cosh;
    using native_std::exp;
    using native_std::fabs;
    using native_std::floor;
    using native_std::fmod;
    using native_std::frexp;
    using native_std::ldexp;
    using native_std::log;
    using native_std::log10;
    using native_std::modf;
    using native_std::pow;
    using native_std::sin;
    using native_std::sinh;
    using native_std::sqrt;
    using native_std::tan;
    using native_std::tanh;

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    using native_std::fpclassify;
    using native_std::isfinite;
    using native_std::isinf;
    using native_std::isnan;
    using native_std::isnormal;
    using native_std::signbit;
    using native_std::isgreater;
    using native_std::isgreaterequal;
    using native_std::isless;
    using native_std::islessequal;
    using native_std::islessgreater;
    using native_std::isunordered;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    using native_std::double_t;
    using native_std::float_t;
    using native_std::acosh;
    using native_std::asinh;
    using native_std::atanh;
    using native_std::cbrt;
    using native_std::copysign;
    using native_std::erf;
    using native_std::erfc;
    using native_std::exp2;
    using native_std::expm1;
    using native_std::fdim;
    using native_std::fma;
    using native_std::fmax;
    using native_std::fmin;
    using native_std::hypot;
    using native_std::ilogb;
    using native_std::lgamma;
    using native_std::llrint;
    using native_std::log1p;
    using native_std::log2;
    using native_std::logb;
    using native_std::lrint;
    using native_std::lround;
    using native_std::llround;
    using native_std::nan;
    using native_std::nanl;
    using native_std::nanf;
    using native_std::nearbyint;
    using native_std::nextafter;
    using native_std::nexttoward;
    using native_std::remainder;
    using native_std::remquo;
    using native_std::rint;
    using native_std::round;
    using native_std::scalbln;
    using native_std::scalbn;
    using native_std::tgamma;
    using native_std::trunc;
#endif  //  BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
