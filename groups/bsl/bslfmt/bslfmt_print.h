// bslfmt_print.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLFMT_PRINT
#define INCLUDED_BSLFMT_PRINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `print(FILE)` implementation.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_print.h
//
//@SEE_ALSO: ISO C++ Standard, <print>
//
//@DESCRIPTION: This component will provide, in the `bsl` namespace, wrappers
// around the functions exposed by the standard <print> header, where they are
// available, otherwise aliases to the `bslfmt` implementation.
//
// Where a BDE implementation is provided, functionality is limited to that
// provided by C++23 and excludes the following features:
//
// * Unicode support for Windows console
// * Support for `enable_nonlocking_formatter_optimization`
//
// Also, when using it, all limitations of the underlying `bslfmt`
// implementation apply.

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
# ifdef BSLS_PLATFORM_CMP_MSVC
#   include <cstdio>  // `stdout`
# endif
# include <print>
#else
# include <bslfmt_print_imp.h>
#endif

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT

using std::print;
using std::println;
using std::vprint_nonunicode;
using std::vprint_nonunicode_buffered;
using std::vprint_unicode;
using std::vprint_unicode_buffered;

#ifdef BSLS_PLATFORM_CMP_MSVC
// MSVC++ 18.1 (VS 2026) does not provide these overloads
inline
void vprint_nonunicode(std::string_view fmt, std::format_args args)
{
    return std::vprint_nonunicode(stdout, fmt, args);
}

inline
void vprint_unicode(std::string_view fmt, std::format_args args)
{
    return std::vprint_unicode(stdout, fmt, args);
}
#endif  // BSLS_PLATFORM_CMP_MSVC

#else

using BloombergLP::bslfmt::print;
using BloombergLP::bslfmt::println;
using BloombergLP::bslfmt::vprint_nonunicode;
using BloombergLP::bslfmt::vprint_nonunicode_buffered;
using BloombergLP::bslfmt::vprint_unicode;
using BloombergLP::bslfmt::vprint_unicode_buffered;

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT

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
