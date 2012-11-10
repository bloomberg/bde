// bsl_cstdlib.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CSTDLIB
#define INCLUDED_BSL_CSTDLIB

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

#include <cstdlib>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::div_t;
    using native_std::ldiv_t;
    using native_std::size_t;
    using native_std::abort;
    using native_std::atexit;
    using native_std::exit;
    using native_std::getenv;
    using native_std::calloc;
    using native_std::free;
    using native_std::malloc;
    using native_std::realloc;
    using native_std::atof;
    using native_std::atoi;
    using native_std::atol;
    using native_std::mblen;
    using native_std::mbstowcs;
    using native_std::mbtowc;
    using native_std::strtod;
    using native_std::strtol;
    using native_std::strtoul;
    using native_std::system;
    using native_std::wcstombs;
    using native_std::wctomb;
    using native_std::bsearch;
    using native_std::qsort;
    using native_std::abs;
    using native_std::div;
    using native_std::labs;
    using native_std::ldiv;
    using native_std::rand;
    using native_std::srand;
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
