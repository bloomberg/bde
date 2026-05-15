// bsl_spanstream.h                                                   -*-C++-*-
#ifndef INCLUDED_BSL_SPANSTREAM
#define INCLUDED_BSL_SPANSTREAM

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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM)
// The library features header tests both the header is present and the
// platform feature macros claim that `spanstream`s are implemented.
# include <spanstream>

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::basic_ispanstream;
    using std::basic_ospanstream;
    using std::basic_spanbuf;
    using std::basic_spanstream;
    using std::ispanstream;
    using std::ospanstream;
    using std::spanbuf;
    using std::spanstream;
    using std::wspanbuf;
    using std::wispanstream;
    using std::wospanstream;
    using std::wspanstream;

}  // close package namespace
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
