// bsl_initializer_list.h                                             -*-C++-*-
#ifndef INCLUDED_BSL_INITIALIZER_LIST
#define INCLUDED_BSL_INITIALIZER_LIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>

namespace bsl {

using std::initializer_list;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::begin;
    using std::end;
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

}  // close namespace bsl
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

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
