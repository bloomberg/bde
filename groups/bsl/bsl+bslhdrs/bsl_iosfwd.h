// bsl_iosfwd.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_IOSFWD
#define INCLUDED_BSL_IOSFWD

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

#include <iosfwd>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::basic_filebuf;
    using native_std::basic_fstream;
    using native_std::basic_ifstream;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ofstream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::char_traits;
    using native_std::filebuf;
    using native_std::fpos;
    using native_std::fstream;
    using native_std::ifstream;
    using native_std::ios;
    using native_std::iostream;
    using native_std::istream;
    using native_std::istreambuf_iterator;
    using native_std::ofstream;
    using native_std::ostream;
    using native_std::ostreambuf_iterator;
    using native_std::streambuf;
    using native_std::streamoff;
    using native_std::streampos;
    using native_std::wfilebuf;
    using native_std::wfstream;
    using native_std::wifstream;
    using native_std::wios;
    using native_std::wiostream;
    using native_std::wistream;
    using native_std::wofstream;
    using native_std::wostream;
    using native_std::wstreambuf;
    using native_std::wstreampos;
}

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslstp_iosfwd.h>
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
