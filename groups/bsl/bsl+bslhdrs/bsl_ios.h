// bsl_ios.h                                                          -*-C++-*-
#ifndef INCLUDED_BSL_IOS
#define INCLUDED_BSL_IOS

#include <bsls_ident.h>
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

#include <bsls_nativestd.h>

#include <bsl_iosfwd.h>

#include <bsls_platform.h>

#include <ios>

namespace bsl {
    // Import selected symbols into bsl namespace

    using native_std::basic_ios;
    using native_std::fpos;
    using native_std::ios;
    using native_std::ios_base;
    using native_std::wios;
    using native_std::streamoff;
    using native_std::streamsize;
    using native_std::streampos;
    using native_std::wstreampos;

    using native_std::boolalpha;
    using native_std::dec;
    using native_std::fixed;
    using native_std::hex;
    using native_std::internal;
    using native_std::left;
    using native_std::oct;
    using native_std::right;
    using native_std::scientific;
    using native_std::showbase;
    using native_std::showpoint;
    using native_std::showpos;
    using native_std::skipws;
    using native_std::unitbuf;
    using native_std::uppercase;
    using native_std::noboolalpha;
    using native_std::noshowbase;
    using native_std::noshowpoint;
    using native_std::noshowpos;
    using native_std::noskipws;
    using native_std::nounitbuf;
    using native_std::nouppercase;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // C++11 extensions
    using native_std::hexfloat;
    using native_std::io_errc;
    using native_std::is_error_code_enum;
    using native_std::iostream_category;
    using native_std::make_error_code;
    using native_std::make_error_condition;
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Additional symbols leaked to support transitive dependencies in higher
    // level (non BDE) Bloomberg code.
# if !defined(BSLS_PLATFORM_CMP_MSVC) && __cplusplus < 201703L
    // As some of these names are removed from C++17, take a sledgehammer to
    // crack this nut, and remove all non-standard exports.
    using native_std::bad_exception;
    using native_std::basic_streambuf;
    using native_std::bidirectional_iterator_tag;
    using native_std::ctype;
    using native_std::ctype_base;
    using native_std::ctype_byname;
    using native_std::exception;
    using native_std::forward_iterator_tag;
    using native_std::input_iterator_tag;
    using native_std::istreambuf_iterator;
    using native_std::iterator;
    using native_std::locale;

# ifndef BSLS_PLATFORM_OS_DARWIN
// This alias isn't available from <ios> in Maverick (OS X 10.9).  See DRQS
// 54913937.
    using native_std::num_get;
# endif

    using native_std::numpunct;
    using native_std::numpunct_byname;
    using native_std::ostreambuf_iterator;
    using native_std::output_iterator_tag;
    using native_std::random_access_iterator_tag;
    using native_std::set_terminate;
    using native_std::set_unexpected;
    using native_std::swap;
    using native_std::terminate;
    using native_std::terminate_handler;
    using native_std::uncaught_exception;
    using native_std::unexpected;
    using native_std::unexpected_handler;
    using native_std::use_facet;
# endif // MSVC, or C++2017
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
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
