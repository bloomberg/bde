// bsl_complex.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_COMPLEX
#define INCLUDED_BSL_COMPLEX

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

#include <complex>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::abs;
    using native_std::arg;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::complex;
    using native_std::conj;
    using native_std::cos;
    using native_std::cosh;
    using native_std::exp;
    using native_std::imag;
    using native_std::ios_base;
    using native_std::istreambuf_iterator;
    using native_std::locale;
    using native_std::log10;
    using native_std::log;
    using native_std::norm;
    using native_std::ostreambuf_iterator;
    using native_std::polar;
    using native_std::pow;
    using native_std::real;
    using native_std::sin;
    using native_std::sinh;
    using native_std::sqrt;
    using native_std::tan;
    using native_std::tanh;
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
