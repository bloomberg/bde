// bsl_cwchar.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CWCHAR
#define INCLUDED_BSL_CWCHAR

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

#include <cwchar>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::btowc;
    using std::fgetwc;
    using std::fgetws;
    using std::fputwc;
    using std::fputws;
    using std::fwide;
    using std::fwprintf;
    using std::fwscanf;
    using std::getwc;
    using std::getwchar;
    using std::mbrlen;
    using std::mbrtowc;
    using std::mbsinit;
    using std::mbsrtowcs;
    using std::mbstate_t;
    using std::putwc;
    using std::putwchar;
    using std::size_t;
    using std::swprintf;
    using std::swscanf;
    using std::ungetwc;
    using std::vfwprintf;
    using std::vswprintf;
    using std::vwprintf;
    using std::wcrtomb;
    using std::wcscat;
    using std::wcschr;
    using std::wcscmp;
    using std::wcscoll;
    using std::wcscpy;
    using std::wcscspn;
    using std::wcsftime;
    using std::wcslen;
    using std::wcsncat;
    using std::wcsncmp;
    using std::wcsncpy;
    using std::wcspbrk;
    using std::wcsrchr;
    using std::wcsrtombs;
    using std::wcsspn;
    using std::wcsstr;
    using std::wcstod;
    using std::wcstok;
    using std::wcstol;
    using std::wcstoul;
    using std::wcsxfrm;
    using std::wctob;
    using std::wint_t;
    using std::wmemchr;
    using std::wmemcmp;
    using std::wmemcpy;
    using std::wmemmove;
    using std::wmemset;
    using std::wprintf;
    using std::wscanf;

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    using std::vfwscanf;
    using std::vswscanf;
    using std::vwscanf;
    using std::wcstof;
    using std::wcstold;
    using std::wcstoll;
    using std::wcstoull;
#endif  // BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
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
