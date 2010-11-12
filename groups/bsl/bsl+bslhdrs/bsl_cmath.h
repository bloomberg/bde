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
