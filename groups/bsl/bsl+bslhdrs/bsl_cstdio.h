// bsl_cstdio.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CSTDIO
#define INCLUDED_BSL_CSTDIO

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

#include <cstdio>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::FILE;
    using native_std::fpos_t;
    using native_std::size_t;
    using native_std::clearerr;
    using native_std::fclose;
    using native_std::feof;
    using native_std::ferror;
    using native_std::fflush;
    using native_std::fgetc;
    using native_std::fgetpos;
    using native_std::fgets;
    using native_std::fopen;
    using native_std::fprintf;
    using native_std::fputc;
    using native_std::fputs;
    using native_std::fread;
    using native_std::freopen;
    using native_std::fscanf;
    using native_std::fseek;
    using native_std::fsetpos;
    using native_std::ftell;
    using native_std::fwrite;
    using native_std::getc;
    using native_std::getchar;
    using native_std::putc;
    using native_std::putchar;
    using native_std::gets;
    using native_std::perror;
    using native_std::printf;
    using native_std::puts;
    using native_std::remove;
    using native_std::rename;
    using native_std::rewind;
    using native_std::scanf;
    using native_std::setbuf;
    using native_std::setvbuf;
    using native_std::sprintf;
    using native_std::sscanf;
    using native_std::tmpfile;
    using native_std::tmpnam;
    using native_std::ungetc;
    using native_std::vfprintf;
    using native_std::vprintf;
    using native_std::vsprintf;
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
