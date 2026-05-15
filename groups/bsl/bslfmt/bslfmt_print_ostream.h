// bslfmt_print_ostream.h                                             -*-C++-*-
#ifndef INCLUDED_BSLFMT_PRINT_OSTREAM
#define INCLUDED_BSLFMT_PRINT_OSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `print(ostream)` implementation.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_print_ostream.h
//
//@SEE_ALSO: ISO C++ Standard, <ostream>, <print>
//
//@DESCRIPTION: This component will provide, in the `bsl` namespace, wrappers
// around the `print` and related functions exposed by the standard `<ostream>`
// header, where they are available, otherwise aliases to the `bslfmt`
// implementation.  In this implementation, these functions are only provided
// via the `bsl_print_ostream.h` header, and are not include in `<ostream>` or
// `bsl_print.h` to avoid overhead for users that do not need them.  Including
// this functionality by default causes too much debug information growth in
// existing large systems that do not use `print`.

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
#include <ostream>
#else
#include <bslfmt_print_ostream_imp.h>
#endif

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT

using std::print;
using std::println;
using std::vprint_nonunicode;
using std::vprint_unicode;

#else

using BloombergLP::bslfmt::print;
using BloombergLP::bslfmt::println;
using BloombergLP::bslfmt::vprint_nonunicode;
using BloombergLP::bslfmt::vprint_unicode;

#endif

}  // close namespace bsl

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
