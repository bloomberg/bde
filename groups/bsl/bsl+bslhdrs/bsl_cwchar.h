// bsl_cwchar.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CWCHAR
#define INCLUDED_BSL_CWCHAR

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

#include <cwchar>

namespace bsl
{
    // Import selected symbols into bsl namespace

    using native_std::btowc;
    using native_std::fgetwc;
    using native_std::fgetws;
    using native_std::fputwc;
    using native_std::fputws;
    using native_std::fwide;
    using native_std::fwprintf;
    using native_std::fwscanf;
    using native_std::getwc;
    using native_std::getwchar;
    using native_std::mbrlen;
    using native_std::mbrtowc;
    using native_std::mbsinit;
    using native_std::mbsrtowcs;
    using native_std::mbstate_t;
    using native_std::putwc;
    using native_std::putwchar;
    using native_std::size_t;
    using native_std::swprintf;
    using native_std::swscanf;
    using native_std::ungetwc;
    using native_std::vfwprintf;
    using native_std::vswprintf;
    using native_std::vwprintf;
    using native_std::wcrtomb;
    using native_std::wcscat;
    using native_std::wcschr;
    using native_std::wcscmp;
    using native_std::wcscoll;
    using native_std::wcscpy;
    using native_std::wcscspn;
    using native_std::wcsftime;
    using native_std::wcslen;
    using native_std::wcsncat;
    using native_std::wcsncmp;
    using native_std::wcsncpy;
    using native_std::wcspbrk;
    using native_std::wcsrchr;
    using native_std::wcsrtombs;
    using native_std::wcsspn;
    using native_std::wcsstr;
    using native_std::wcstod;
    using native_std::wcstok;
    using native_std::wcstol;
    using native_std::wcsxfrm;
    using native_std::wctob;
    using native_std::wint_t;
    using native_std::wmemchr;
    using native_std::wmemcmp;
    using native_std::wmemcpy;
    using native_std::wmemmove;
    using native_std::wmemset;
    using native_std::wprintf;
    using native_std::wscanf;
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
