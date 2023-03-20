// bsl_cinttypes.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_CINTTYPES
#define INCLUDED_BSL_CINTTYPES

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

#include <cinttypes>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    using std::imaxabs;
    using std::imaxdiv;
    using std::strtoimax;
    using std::strtoumax;
    using std::wcstoimax;
    using std::wcstoumax;
    using std::imaxdiv_t;

    // ISO C++ Standard)says: *if* and *only* if the type 'intmax_t' designates
    // an extended integer type (6.7.1), the following function signatures are
    // added:
    //
    //: o intmax_t abs(intmax_t);
    //: o imaxdiv_t div(intmax_t, intmax_t);
    //
    // which shall have the same semantics as the function signatures
    // 'intmax_t imaxabs(intmax_t)' and
    // 'imaxdiv_t imaxdiv(intmax_t, intmax_t)', respectively.
    //
    // None of the current platforms BDE supports have an 'std::intmax_t' that
    // isn't a fundamental signed integer type, so those 'abs' and 'div'
    // overloads above should never be present (errorously, on some plaftorms,
    // some functions with the 'abs' and 'div' name are defined by system
    // headers, but on newner platforms they are being removed.)

}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
