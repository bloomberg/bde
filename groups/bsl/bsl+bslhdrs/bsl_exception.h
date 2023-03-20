// bsl_exception.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_EXCEPTION
#define INCLUDED_BSL_EXCEPTION

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
#include <bsls_platform.h>

#include <exception>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::bad_exception;
    using std::exception;
    using std::set_terminate;
    using std::terminate;
    using std::terminate_handler;

#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED)
    // This name is removed by C++20

    using std::uncaught_exception;
#endif

#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED)
    // These names are removed by C++17

    using std::set_unexpected;
    using std::unexpected;
    using std::unexpected_handler;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // Not defined until C++17

    using std::uncaught_exceptions;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::current_exception;
    using std::exception_ptr;
    using std::rethrow_exception;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    using std::make_exception_ptr;
    using std::nested_exception;
    using std::rethrow_if_nested;
    using std::throw_with_nested;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
#if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED)
    using std::get_unexpected;
# endif
    using std::get_terminate;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
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
