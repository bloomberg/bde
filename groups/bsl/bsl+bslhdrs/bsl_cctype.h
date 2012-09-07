// bsl_cctype.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CCTYPE
#define INCLUDED_BSL_CCTYPE

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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <cctype>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::isalnum;
    using native_std::isalpha;
    using native_std::iscntrl;
    using native_std::isdigit;
    using native_std::isgraph;
    using native_std::islower;
    using native_std::isprint;
    using native_std::ispunct;
    using native_std::isspace;
    using native_std::isupper;
    using native_std::isxdigit;
    using native_std::tolower;
    using native_std::toupper;
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
