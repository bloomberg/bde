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
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <cstring>

namespace bsl
{
    // Import selected symbols into bsl namespace

    using native_std::memchr;
    using native_std::memcmp;
    using native_std::memcpy;
    using native_std::memmove;
    using native_std::memset;
    using native_std::size_t;
    using native_std::strcat;
    using native_std::strchr;
    using native_std::strcmp;
    using native_std::strcoll;
    using native_std::strcpy;
    using native_std::strcspn;
    using native_std::strerror;
    using native_std::strlen;
    using native_std::strncat;
    using native_std::strncmp;
    using native_std::strncpy;
    using native_std::strpbrk;
    using native_std::strrchr;
    using native_std::strspn;
    using native_std::strstr;
    using native_std::strtok;
    using native_std::strxfrm;
}  // close package namespace

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
