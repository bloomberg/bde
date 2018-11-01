// bsl_exception.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_EXCEPTION
#define INCLUDED_BSL_EXCEPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#include <exception>

namespace bsl {
    // Import selected symbols into bsl namespace

    using native_std::bad_exception;
    using native_std::exception;
    using native_std::set_terminate;
    using native_std::terminate;
    using native_std::terminate_handler;
    using native_std::uncaught_exception;

#if __cplusplus < 201703L                 \
 &&!(defined(BSLS_PLATFORM_CMP_MSVC)   && \
     BSLS_PLATFORM_CMP_VERSION >= 1910 && \
     !_HAS_AUTO_PTR_ETC)
    // These names are removed by C++17
    using native_std::set_unexpected;
    using native_std::unexpected;
    using native_std::unexpected_handler;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using native_std::current_exception;
    using native_std::exception_ptr;
    using native_std::rethrow_exception;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    using native_std::make_exception_ptr;
    using native_std::nested_exception;
    using native_std::rethrow_if_nested;
    using native_std::throw_with_nested;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
# if __cplusplus < 201703L                 \
 &&!(defined(BSLS_PLATFORM_CMP_MSVC)   && \
     BSLS_PLATFORM_CMP_VERSION >= 1910 && \
     !_HAS_AUTO_PTR_ETC)
    using native_std::get_unexpected;
# endif
    using native_std::get_terminate;
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
