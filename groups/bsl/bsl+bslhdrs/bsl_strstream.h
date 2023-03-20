// bsl_strstream.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_STRSTREAM
#define INCLUDED_BSL_STRSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_platform.h>

#include <bsl_ios.h>

#include <strstream>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace
    using std::istrstream;
    using std::ostrstream;
    using std::strstream;
    using std::strstreambuf;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Export additional names, leaked to support transitive dependencies in
    // higher level (non BDE) Bloomberg code.
# if !defined(BSLS_PLATFORM_CMP_MSVC) && __cplusplus < 201703L
    // As some of these names are removed from C++17, take a sledgehammer to
    // crack this nut, and remove all non-standard exports.
    using std::bad_exception;
    using std::basic_ios;
    using std::basic_iostream;
    using std::basic_istream;
    using std::basic_ostream;
    using std::basic_streambuf;
    using std::bidirectional_iterator_tag;
    using std::ctype;
    using std::ctype_base;
    using std::ctype_byname;
    using std::endl;
    using std::ends;
    using std::exception;
    using std::flush;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::ios_base;
    using std::istreambuf_iterator;
    using std::iterator;
    using std::locale;
    using std::num_get;
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
    using std::ws;
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
