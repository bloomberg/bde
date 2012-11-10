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

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <cmath>


namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::abs;
    using native_std::acos;
    using native_std::asin;
    using native_std::atan2;
    using native_std::atan;
    using native_std::ceil;
    using native_std::cos;
    using native_std::cosh;
    using native_std::exp;
    using native_std::fabs;
    using native_std::floor;
    using native_std::fmod;
    using native_std::frexp;
    using native_std::ldexp;
    using native_std::log10;
    using native_std::log;
    using native_std::modf;
    using native_std::pow;
    using native_std::sin;
    using native_std::sinh;
    using native_std::sqrt;
    using native_std::tan;
    using native_std::tanh;

// C99 math functions are available in namespace std
#if defined(BSLS_PLATFORM_CMP_GNU) \
    && defined(_GLIBCXX_USE_C99_MATH) \
    && !defined(_GLIBCXX_USE_C99_FP_MACROS_DYNAMIC)

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
}

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
