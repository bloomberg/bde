// bslstl_ios.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_IOS
#define INCLUDED_BSLSTL_IOS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@CANONICAL_HEADER: bsl_ios.h
//
//@SEE_ALSO: bsl+bslhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <bslstl_iosfwd.h>

#include <ios>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::basic_ios;
    using std::fpos;
    using std::ios;
    using std::ios_base;
    using std::wios;
    using std::streamoff;
    using std::streamsize;
    using std::streampos;
    using std::wstreampos;

    using std::boolalpha;
    using std::dec;
    using std::exception;
    using std::fixed;
    using std::hex;
    using std::internal;
    using std::left;
    using std::oct;
    using std::right;
    using std::scientific;
    using std::showbase;
    using std::showpoint;
    using std::showpos;
    using std::skipws;
    using std::unitbuf;
    using std::uppercase;
    using std::noboolalpha;
    using std::noshowbase;
    using std::noshowpoint;
    using std::noshowpos;
    using std::noskipws;
    using std::nounitbuf;
    using std::nouppercase;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // C++11 extensions
    using std::defaultfloat;
    using std::hexfloat;
    using std::io_errc;
    using std::is_error_code_enum;
    using std::iostream_category;
    using std::make_error_code;
    using std::make_error_condition;
#else
    // Define this in C++03 mode as well
    inline
    ios_base& defaultfloat(ios_base& ios)
    {
        ios.unsetf(ios_base::floatfield);
        return ios;
    }
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Additional symbols leaked to support transitive dependencies in higher
    // level (non BDE) Bloomberg code.
# if !defined(BSLS_PLATFORM_CMP_MSVC) && __cplusplus < 201703L
    // As some of these names are removed from C++17, take a sledgehammer to
    // crack this nut, and remove all non-standard exports.
    using std::bad_exception;
    using std::basic_streambuf;
    using std::bidirectional_iterator_tag;
    using std::ctype;
    using std::ctype_base;
    using std::ctype_byname;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::istreambuf_iterator;
    using std::iterator;
    using std::locale;

# ifndef BSLS_PLATFORM_OS_DARWIN
// This alias isn't available from <ios> in Maverick (OS X 10.9).  See DRQS
// 54913937.
    using std::num_get;
# endif

    using std::numpunct;
    using std::numpunct_byname;
    using std::ostreambuf_iterator;
    using std::output_iterator_tag;
    using std::random_access_iterator_tag;
    using std::set_terminate;
    using std::set_unexpected;
    using std::swap;
    using std::terminate;
    using std::terminate_handler;
    using std::uncaught_exception;
    using std::unexpected;
    using std::unexpected_handler;
    using std::use_facet;
# endif // MSVC, or C++2017
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
