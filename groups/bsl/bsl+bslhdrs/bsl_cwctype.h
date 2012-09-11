// bsl_cwctype.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CWCTYPE
#define INCLUDED_BSL_CWCTYPE

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

#include <cwctype>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::wctype_t;
    using native_std::wint_t;
    using native_std::wctrans_t;
    using native_std::towctrans;
    using native_std::wctrans;
    using native_std::wctype;
    using native_std::iswctype;
    using native_std::iswalnum;
    using native_std::iswalpha;
    using native_std::iswcntrl;
    using native_std::iswdigit;
    using native_std::iswgraph;
    using native_std::iswlower;
    using native_std::iswprint;
    using native_std::iswpunct;
    using native_std::iswspace;
    using native_std::iswupper;
    using native_std::iswxdigit;
    using native_std::towlower;
    using native_std::towupper;
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
