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
//@AUTHOR: Arthur Chiu (achiu21)
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
