// bsl_fstream.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_FSTREAM
#define INCLUDED_BSL_FSTREAM

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

#include <fstream>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::basic_filebuf;
    using native_std::basic_fstream;
    using native_std::basic_ifstream;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ofstream;
    using native_std::basic_ostream;
    using native_std::codecvt;
    using native_std::codecvt_base;
    using native_std::codecvt_byname;
    using native_std::endl;
    using native_std::ends;
    using native_std::filebuf;
    using native_std::flush;
    using native_std::fstream;
    using native_std::ifstream;
    using native_std::ofstream;
    using native_std::streamsize;
    using native_std::wfilebuf;
    using native_std::wfstream;
    using native_std::wifstream;
    using native_std::wofstream;
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
