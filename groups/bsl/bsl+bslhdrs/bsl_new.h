// bsl_new.h                                                          -*-C++-*-
#ifndef INCLUDED_BSL_NEW
#define INCLUDED_BSL_NEW

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

#include <new>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::bad_alloc;
    using std::new_handler;
    using std::nothrow;
    using std::nothrow_t;
    using std::set_new_handler;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    using std::bad_array_new_length;
    using std::get_new_handler;
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
    using std::bad_exception;
    using std::exception;
    using std::set_terminate;
    using std::set_unexpected;
    using std::terminate;
    using std::terminate_handler;
    using std::uncaught_exception;
    using std::unexpected;
    using std::unexpected_handler;
#  endif // MSVC, or C++2017
# endif // BSLS_PLATFORM_CMP_IBM or BSLS_PLATFORM_CMP_SUN
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::align_val_t;
//  As of Apr-2022, no one (libc++, libstdc++, MSVC) has implemented these.
//     using std::hardware_constructive_interference_size;
//     using std::hardware_destructive_interference_size;
    using std::launder;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

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
