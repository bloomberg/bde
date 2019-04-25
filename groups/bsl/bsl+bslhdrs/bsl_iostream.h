// bsl_iostream.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_IOSTREAM
#define INCLUDED_BSL_IOSTREAM

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

#include <bsl_ios.h>
#include <bsl_istream.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>

#include <bsls_nativestd.h>

#include <iostream>

namespace bsl {
    // Import selected symbols into bsl namespace

    using native_std::cerr;
    using native_std::cin;
    using native_std::clog;
    using native_std::cout;
    using native_std::wcerr;
    using native_std::wcin;
    using native_std::wclog;
    using native_std::wcout;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Export additional names, leaked to support transitive dependencies in
    // higher level (non BDE) Bloomberg code.
# if !defined(BSLS_PLATFORM_CMP_MSVC) && __cplusplus < 201703L
    // As some of these names are removed from C++17, take a sledgehammer to
    // crack this nut, and remove all non-standard exports.
    using native_std::bad_exception;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::bidirectional_iterator_tag;
    using native_std::ctype;
    using native_std::ctype_base;
    using native_std::ctype_byname;
    using native_std::endl;
    using native_std::ends;
    using native_std::exception;
    using native_std::flush;
    using native_std::forward_iterator_tag;
    using native_std::input_iterator_tag;
    using native_std::ios_base;
    using native_std::iostream;
    using native_std::istream;
    using native_std::istreambuf_iterator;
    using native_std::iterator;
    using native_std::locale;
    using native_std::num_get;
    using native_std::numpunct;
    using native_std::numpunct_byname;
    using native_std::ostream;
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
    using native_std::ws;
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
