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

#include <iostream>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::terminate_handler;
    using native_std::unexpected_handler;
    using native_std::bad_exception;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::bidirectional_iterator_tag;
    using native_std::boolalpha;
    using native_std::cerr;
    using native_std::cin;
    using native_std::clog;
    using native_std::cout;
    using native_std::ctype;
    using native_std::ctype_base;
    using native_std::ctype_byname;
    using native_std::dec;
    using native_std::endl;
    using native_std::ends;
    using native_std::exception;
    using native_std::fixed;
    using native_std::flush;
    using native_std::forward_iterator_tag;
    using native_std::hex;
    using native_std::input_iterator_tag;
    using native_std::internal;
    using native_std::ios_base;
    using native_std::iostream;
    using native_std::istream;
    using native_std::istreambuf_iterator;
    using native_std::iterator;
    using native_std::left;
    using native_std::locale;
    using native_std::noboolalpha;
    using native_std::noshowbase;
    using native_std::noshowpoint;
    using native_std::noshowpos;
    using native_std::noskipws;
    using native_std::nounitbuf;
    using native_std::nouppercase;
    using native_std::num_get;
    using native_std::numpunct;
    using native_std::numpunct_byname;
    using native_std::oct;
    using native_std::ostream;
    using native_std::ostreambuf_iterator;
    using native_std::output_iterator_tag;
    using native_std::random_access_iterator_tag;
    using native_std::right;
    using native_std::scientific;
    using native_std::set_terminate;
    using native_std::set_unexpected;
    using native_std::showbase;
    using native_std::showpoint;
    using native_std::showpos;
    using native_std::skipws;
    using native_std::swap;
    using native_std::terminate;
    using native_std::uncaught_exception;
    using native_std::unexpected;
    using native_std::unitbuf;
    using native_std::uppercase;
    using native_std::use_facet;
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
