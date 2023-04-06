// bsl_cmath.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_CMATH
#define INCLUDED_BSL_CMATH

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

#include <cmath>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#if !defined(BDE_DONT_ALLOW_TRANSITIVE_INCLUDES) && \
     defined(BDE_BUILD_TARGET_STLPORT)

// Code in Robo depends on <math.h> included transitively with <cmath> and it
// fails to build otherwise in the stlport4 mode on Sun.

#include <math.h>

#endif
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::abs;
    using std::acos;
    using std::asin;
    using std::atan;
    using std::atan2;
    using std::ceil;
    using std::cos;
    using std::cosh;
    using std::exp;
    using std::fabs;
    using std::floor;
    using std::fmod;
    using std::frexp;
    using std::ldexp;
    using std::log;
    using std::log10;
    using std::modf;
    using std::pow;
    using std::sin;
    using std::sinh;
    using std::sqrt;
    using std::tan;
    using std::tanh;

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    using std::fpclassify;
    using std::isfinite;
    using std::isinf;
    using std::isnan;
    using std::isnormal;
    using std::signbit;
    using std::isgreater;
    using std::isgreaterequal;
    using std::isless;
    using std::islessequal;
    using std::islessgreater;
    using std::isunordered;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    using std::double_t;
    using std::float_t;
    using std::acosh;
    using std::asinh;
    using std::atanh;
    using std::cbrt;
    using std::copysign;
    using std::erf;
    using std::erfc;
    using std::exp2;
    using std::expm1;
    using std::fdim;
    using std::fma;
    using std::fmax;
    using std::fmin;
    using std::hypot;
    using std::ilogb;
    using std::lgamma;
    using std::llrint;
    using std::log1p;
    using std::log2;
    using std::logb;
    using std::lrint;
    using std::lround;
    using std::llround;
    using std::nan;
    using std::nanl;
    using std::nanf;
    using std::nearbyint;
    using std::nextafter;
    using std::nexttoward;
    using std::remainder;
    using std::remquo;
    using std::rint;
    using std::round;
    using std::scalbln;
    using std::scalbn;
    using std::tgamma;
    using std::trunc;
#endif  //  BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS
    using std::assoc_laguerre;
    using std::assoc_laguerref;
    using std::assoc_laguerrel;
    using std::assoc_legendre;
    using std::assoc_legendref;
    using std::assoc_legendrel;
    using std::beta;
    using std::betaf;
    using std::betal;
    using std::comp_ellint_1;
    using std::comp_ellint_1f;
    using std::comp_ellint_1l;
    using std::comp_ellint_2;
    using std::comp_ellint_2f;
    using std::comp_ellint_2l;
    using std::comp_ellint_3;
    using std::comp_ellint_3f;
    using std::comp_ellint_3l;
    using std::cyl_bessel_i;
    using std::cyl_bessel_if;
    using std::cyl_bessel_il;
    using std::cyl_bessel_j;
    using std::cyl_bessel_jf;
    using std::cyl_bessel_jl;
    using std::cyl_bessel_k;
    using std::cyl_bessel_kf;
    using std::cyl_bessel_kl;
    using std::cyl_neumann;
    using std::cyl_neumannf;
    using std::cyl_neumannl;
    using std::ellint_1;
    using std::ellint_1f;
    using std::ellint_1l;
    using std::ellint_2;
    using std::ellint_2f;
    using std::ellint_2l;
    using std::ellint_3;
    using std::ellint_3f;
    using std::ellint_3l;
    using std::expint;
    using std::expintf;
    using std::expintl;
    using std::hermite;
    using std::hermitef;
    using std::hermitel;
    using std::legendre;
    using std::legendref;
    using std::legendrel;
    using std::laguerre;
    using std::laguerref;
    using std::laguerrel;
    using std::riemann_zeta;
    using std::riemann_zetaf;
    using std::riemann_zetal;
    using std::sph_bessel;
    using std::sph_besself;
    using std::sph_bessell;
    using std::sph_legendre;
    using std::sph_legendref;
    using std::sph_legendrel;
    using std::sph_neumann;
    using std::sph_neumannf;
    using std::sph_neumannl;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    using std::lerp;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

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
