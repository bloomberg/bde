// bsl_cuchar.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CUCHAR
#define INCLUDED_BSL_CUCHAR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.

#include <bsls_libraryfeatures.h>

#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// In C++11 and later, libstdc++ provides `<cuchar>` even if `<uchar.h>` is not
// provided by the C library.  In that case, including it has no effect.
// However, all our supported platforms do provide `<uchar.h>`, except for
// Solaris.

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY) && \
    !defined(BSLS_PLATFORM_OS_SOLARIS)
#include <cuchar>

namespace bsl {

    using std::c16rtomb;
    using std::c32rtomb;
    using std::mbrtoc16;
    using std::mbrtoc32;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
    using std::mbrtoc8;
    using std::c8rtomb;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

}  // close package namespace
#endif  // C99

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
