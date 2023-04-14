// bsl_cstdio.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CSTDIO
#define INCLUDED_BSL_CSTDIO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <cstdio>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

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

    using std::FILE;
    using std::fpos_t;
    using std::size_t;

    using std::clearerr;
    using std::fclose;
    using std::feof;
    using std::ferror;
    using std::fflush;
    using std::fgetc;
    using std::fgetpos;
    using std::fgets;
    using std::fopen;
    using std::fprintf;
    using std::fputc;
    using std::fputs;
    using std::fread;
    using std::freopen;
    using std::fscanf;
    using std::fseek;
    using std::fsetpos;
    using std::ftell;
    using std::fwrite;
    using std::getc;
    using std::getchar;
    using std::perror;
    using std::printf;
    using std::putc;
    using std::putchar;
    using std::puts;
    using std::remove;
    using std::rename;
    using std::rewind;
    using std::scanf;
    using std::setbuf;
    using std::setvbuf;
    using std::sprintf;
    using std::sscanf;
    using std::tmpfile;
    using std::tmpnam;
    using std::ungetc;
    using std::vfprintf;
    using std::vprintf;
    using std::vsprintf;

#ifdef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    using std::gets;
#endif  // BSLS_LIBRARYFEATURES_HAS_C90_GETS

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    using std::vfscanf;
    using std::vscanf;
    using std::vsnprintf;
    using std::vsscanf;
#endif  // BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF) \
    && 2 != BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    using std::snprintf;
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
