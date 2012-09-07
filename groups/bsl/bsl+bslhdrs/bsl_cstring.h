// bsl_cstring.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CSTRING
#define INCLUDED_BSL_CSTRING

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

#include <cstring>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::size_t;
    using native_std::memmove;
    using native_std::memcpy;
    using native_std::memchr;
    using native_std::strchr;
    using native_std::strpbrk;
    using native_std::strrchr;
    using native_std::strstr;
    using native_std::memcmp;
    using native_std::memset;
    using native_std::strcat;
    using native_std::strcmp;
    using native_std::strcoll;
    using native_std::strcpy;
    using native_std::strcspn;
    using native_std::strerror;
    using native_std::strlen;
    using native_std::strncat;
    using native_std::strncmp;
    using native_std::strncpy;
    using native_std::strspn;
    using native_std::strtok;
    using native_std::strxfrm;
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
