// bsl_cstdio.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CSTDIO
#define INCLUDED_BSL_CSTDIO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>

#include <cstdio>

#if(!defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF)     \
    ||  2 == BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF)

# if defined(BSLS_PLATFORM_OS_AIX)                      \
  || defined(BSLS_PLATFORM_OS_LINUX)                    \
  || defined(BSLS_PLATFORM_OS_SOLARIS)
#    define BSL_CSTDIO_USE_OS_FOR_99 1
#    include <stdio.h>
# endif
#endif

namespace bsl {
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
    using native_std::perror;
    using native_std::printf;
    using native_std::putc;
    using native_std::putchar;
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

#ifdef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    using native_std::gets;
#endif  // BSLS_LIBRARYFEATURES_HAS_C90_GETS

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    using native_std::vfscanf;
    using native_std::vscanf;
    using native_std::vsnprintf;
    using native_std::vsscanf;
#endif  // BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF) \
    && 2 != BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    using native_std::snprintf;
#endif  // BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
}  // close package namespace

#ifdef  BSL_CSTDIO_USE_OS_FOR_99
# undef BSL_CSTDIO_USE_OS_FOR_99
namespace bsl {

# if !defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF) \
     ||  2 == BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    using ::snprintf;
#   define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF 2
# endif // BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
}  // close package namespace
#endif // BSL_CSTDIO_USE_OS_FOR_99

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
