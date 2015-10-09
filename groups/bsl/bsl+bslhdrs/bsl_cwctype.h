// bsl_cwctype.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CWCTYPE
#define INCLUDED_BSL_CWCTYPE

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

#include <cwctype>

namespace bsl
{
    // Import selected symbols into bsl namespace

    using native_std::iswalnum;
    using native_std::iswalpha;
    using native_std::iswcntrl;
    using native_std::iswctype;
    using native_std::iswdigit;
    using native_std::iswgraph;
    using native_std::iswlower;
    using native_std::iswprint;
    using native_std::iswpunct;
    using native_std::iswspace;
    using native_std::iswupper;
    using native_std::iswxdigit;
    using native_std::towctrans;
    using native_std::towlower;
    using native_std::towupper;
    using native_std::wctrans;
    using native_std::wctrans_t;
    using native_std::wctype;
    using native_std::wctype_t;
    using native_std::wint_t;
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
