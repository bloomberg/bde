// bsl_new.h                                                          -*-C++-*-
#ifndef INCLUDED_BSL_NEW
#define INCLUDED_BSL_NEW

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>
#include <bsls_platform.h>

#include <new>

namespace bsl {
    // Import selected symbols into bsl namespace

    using native_std::bad_alloc;
    using native_std::new_handler;
    using native_std::nothrow;
    using native_std::nothrow_t;
    using native_std::set_new_handler;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    using native_std::bad_array_new_length;
    using native_std::get_new_handler;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Import additional names expected by existing code, but not mandated by
    // the standard header.  This may get tricky if some standard library
    // happens to not require any of these names for its native implementation
    // of the <memory> header.
# if defined(BSLS_PLATFORM_CMP_IBM) || defined(BSLS_PLATFORM_CMP_SUN)
#  if __cplusplus < 201103L
    // We limit these non-standard exports to legacy C++2003 compilation on AIX
    // and Sun. Note that some newer compilers require an additional include of
    // <exception> for these, and also that some of these have been removed
    // from C++17.
    using native_std::bad_exception;
    using native_std::exception;
    using native_std::set_terminate;
    using native_std::set_unexpected;
    using native_std::terminate;
    using native_std::terminate_handler;
    using native_std::uncaught_exception;
    using native_std::unexpected;
    using native_std::unexpected_handler;
#  endif // MSVC, or C++2017
# endif // BSLS_PLATFORM_CMP_IBM or BSLS_PLATFORM_CMP_SUN
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
