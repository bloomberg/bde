// bsl_iostream.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_IOSTREAM
#define INCLUDED_BSL_IOSTREAM

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

#include <iostream>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::cerr;
    using native_std::cin;
    using native_std::clog;
    using native_std::cout;
    using native_std::endl;
    using native_std::ends;
    using native_std::flush;
    using native_std::iostream;
    using native_std::istream;
    using native_std::ostream;
    using native_std::wcerr;
    using native_std::wcin;
    using native_std::wcout;
    using native_std::wclog;
    using native_std::ws;
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
