// bsl_cstring.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CSTRING
#define INCLUDED_BSL_CSTRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <cstring>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::memchr;
    using std::memcmp;
    using std::memcpy;
    using std::memmove;
    using std::memset;
    using std::size_t;
    using std::strcat;
    using std::strchr;
    using std::strcmp;
    using std::strcoll;
    using std::strcpy;
    using std::strcspn;
    using std::strerror;
    using std::strlen;
    using std::strncat;
    using std::strncmp;
    using std::strncpy;
    using std::strpbrk;
    using std::strrchr;
    using std::strspn;
    using std::strstr;
    using std::strtok;
    using std::strxfrm;
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
