// bsl_locale.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_LOCALE
#define INCLUDED_BSL_LOCALE

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

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif 

#include <locale>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::codecvt;
    using native_std::codecvt_base;
    using native_std::codecvt_byname;
    using native_std::collate;
    using native_std::collate_byname;
    using native_std::has_facet;
    using native_std::messages;
    using native_std::messages_base;
    using native_std::messages_byname;
    using native_std::moneypunct;
    using native_std::moneypunct_byname;
    using native_std::money_base;
    using native_std::money_get;
    using native_std::money_put;
    using native_std::num_put;
    using native_std::time_base;
    using native_std::time_get;
    using native_std::time_get_byname;
    using native_std::time_put;
    using native_std::time_put_byname;
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
