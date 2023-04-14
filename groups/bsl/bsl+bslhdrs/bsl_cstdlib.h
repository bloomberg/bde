// bsl_cstdlib.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CSTDLIB
#define INCLUDED_BSL_CSTDLIB

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

#include <cstdlib>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::abort;
    using std::abs;
    using std::atexit;
    using std::atof;
    using std::atoi;
    using std::atol;
    using std::bsearch;
    using std::calloc;
    using std::div;
    using std::div_t;
    using std::exit;
    using std::free;
    using std::getenv;
    using std::labs;
    using std::ldiv;
    using std::ldiv_t;
    using std::malloc;
    using std::mblen;
    using std::mbstowcs;
    using std::mbtowc;
    using std::qsort;
    using std::rand;
    using std::realloc;
    using std::size_t;
    using std::srand;
    using std::strtod;
    using std::strtol;
    using std::strtoul;
    using std::system;
    using std::wcstombs;
    using std::wctomb;

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    using std::atoll;
    using std::llabs;
    using std::lldiv;
    using std::lldiv_t;
    using std::strtof;
    using std::strtold;
    using std::strtoll;
    using std::strtoull;
#endif  // BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    using std::_Exit;
    using std::at_quick_exit;
    using std::quick_exit;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
    using std::aligned_alloc;
#endif
}
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
